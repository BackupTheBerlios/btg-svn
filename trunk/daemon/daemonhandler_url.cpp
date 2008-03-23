/*
 * btg Copyright (C) 2008 Michael Wojciechowski.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * $Id$
 */

#include "daemonhandler.h"
#include <bcore/command/context_create.h>
#include <bcore/command/context_create_url.h>
#include <bcore/verbose.h>

#include "modulelog.h"
#include "filetrack.h"

namespace btg
{
   namespace daemon
   {
      const std::string moduleName("hdl");
      const t_uint max_url_age = 30;

      using namespace btg::core;

      std::vector<UrlIdSessionMapping>::iterator daemonHandler::getMapping(t_uint _hid)
      {
         std::vector<UrlIdSessionMapping>::iterator i;
         for (i = UrlIdSessions.begin();
              i != UrlIdSessions.end();
              i++)
            {
               if (i->hid == _hid)
                  {
                     return i;
                  }
            }

         return UrlIdSessions.end();
      }

      void daemonHandler::handleUrlDownload(t_uint _hid)
      {
         std::vector<UrlIdSessionMapping>::iterator i = getMapping(_hid);

         if (i != UrlIdSessions.end())
            {
               if (i->valid)
                  {
                     addUrl(*i);
                     UrlIdSessions.erase(i);
                  }
            }
      }

      void daemonHandler::handleUrlDownloads()
      {
         std::vector<UrlIdSessionMapping>::iterator i;
         for (i = UrlIdSessions.begin();
              i != UrlIdSessions.end();
              i++)
            {
               if ((i->age > max_url_age) || (!i->valid))
                  {
                     continue;
                  }

               i->age++;

               btg::daemon::http::httpInterface::Status s = httpmgr.getStatus(i->hid);
               if (s == btg::daemon::http::httpInterface::FINISH)
                  {
                     // Dl finished, now create the context.
                     addUrl(*i);
                  }

               if (s == btg::daemon::http::httpInterface::ERROR)
                  {
                     // Unable to download torrent, clean up.
                     removeUrl(*i);

                     dd_->filetrack->remove(i->userdir,
                                            i->filename);
                     // Expire this download.
                     i->valid = false;
                  }
            }

         // Remove expired downloads.
         i = UrlIdSessions.begin();
         while (i != UrlIdSessions.end())
            {
               if (i->age > max_url_age)
                  {
                     httpmgr.Terminate(i->hid);
                     dd_->filetrack->remove(i->userdir,
                                            i->filename);
                     i = UrlIdSessions.erase(i);
                  }
               else
                  {
                     i++;
                  }
            }
      }

      void daemonHandler::removeUrl(UrlIdSessionMapping & _mapping)
      {
         dd_->filetrack->remove(_mapping.userdir,
                                _mapping.filename);
         // Expire this download.
         _mapping.valid = false;
      }

      void daemonHandler::addUrl(UrlIdSessionMapping & _mapping)
      {
         // Find the required event handler.
         eventHandler* eh = 0;
         if (!sessionlist_.get(_mapping.session, eh))
            {
               // No session, expire.
               _mapping.valid = false;
               return;
            }

         btg::core::sBuffer sbuf;
         if (!sbuf.read(_mapping.userdir + GPD->sPATH_SEPARATOR() + _mapping.filename))
            {
               // No data, expire.
               _mapping.valid = false;
               return;
            }

         // Remove the file from filetrack, as it will be added when
         // creating the torrent.
         dd_->filetrack->remove(_mapping.userdir,
                                _mapping.filename);

         btg::core::contextCreateWithDataCommand* ccwdc = 
            new btg::core::contextCreateWithDataCommand(_mapping.filename, sbuf, _mapping.start);

         // Do not use a connection id.
         if (!eh->createWithData(ccwdc))
            {
               BTG_MERROR(logWrapper(), 
                          "Adding '" << _mapping.filename << "' from URL failed.");
               _mapping.status = UCS_CREATE_FAILED;
               _mapping.valid  = false;
            }
         else
            {
               _mapping.status = UCS_CREATED;
               _mapping.valid  = false;
               MVERBOSE_LOG(logWrapper(), 
                            verboseFlag_, "Added '" << _mapping.filename << "' from URL.");
            }
         
         delete ccwdc;
         ccwdc = 0;
      }

      void daemonHandler::handle_CN_CURLSTATUS(eventHandler* _eventhandler, 
                                               btg::core::Command* _command)
      {
         contextUrlStatusCommand* cusc = dynamic_cast<contextUrlStatusCommand*>(_command);
         t_uint hid = cusc->id();

         std::vector<UrlIdSessionMapping>::iterator mapping = getMapping(hid);

         if (mapping == UrlIdSessions.end())
            {
               sendError(_command->getType(), "Unknown URL id.");
               return;
            }

         btg::daemon::http::httpInterface::Status httpstat = httpmgr.getStatus(hid);
         
         btg::core::urlStatus urlstat = URLS_UNDEF;
         
         switch (httpstat)
            {
            case btg::daemon::http::httpInterface::ERROR:
               {
                  removeUrl(*mapping);
                  urlstat = URLS_ERROR;
                  break;
               }
            case btg::daemon::http::httpInterface::INIT:
               urlstat = URLS_WORKING;
               break;
            case btg::daemon::http::httpInterface::WAIT:
               urlstat = URLS_WORKING;
               break;
            case btg::daemon::http::httpInterface::FINISH:
               {
                  urlstat = URLS_FINISHED;

                  // The http download finished, add the torrent.
                  handleUrlDownload(hid);

                  if (mapping->status == UCS_CREATED)
                     {
                        urlstat = URLS_CREATE;
                     }

                  if (mapping->status == UCS_CREATE_FAILED)
                     {
                        urlstat = URLS_CREATE_ERR;
                     }

                  break;
               }
            }
         
         sendCommand(dd_->externalization,
                     dd_->transport,
                     connectionID_,
                     new contextUrlStatusResponseCommand(hid, urlstat));
      }

      void daemonHandler::handle_CN_CCREATEFROMURL(eventHandler* _eventhandler, 
                                                   btg::core::Command* _command)
      {
         contextCreateFromUrlCommand* ccful = dynamic_cast<contextCreateFromUrlCommand*>(_command);

         std::string userdir  = _eventhandler->getTempDir();
         std::string filename = ccful->getFilename();

         if (!dd_->filetrack->add(userdir,
                                  filename))
            {
               // File already exists.
               sendError(_command->getType(), "Torrent file already exists.");
               return;
            }

         bool unique = true;
         std::vector<UrlIdSessionMapping>::iterator i;
         for (i = UrlIdSessions.begin();
              i != UrlIdSessions.end();
              i++)
            {
               if ((i->valid) && 
                   (i->filename == filename) && 
                   (i->userdir == userdir))
                  {
                     unique = false;
                     break;
                  }
            }

         if (!unique)
            {
               dd_->filetrack->remove(userdir,
                                      filename);

               sendError(_command->getType(), "Torrent file already exists.");
               return;
            }

         t_uint hid = httpmgr.Fetch(ccful->getUrl(), 
                                    userdir + GPD->sPATH_SEPARATOR() + filename);
         UrlIdSessionMapping usmap(hid, _eventhandler->getSession(), userdir, filename, ccful->getStart());
         UrlIdSessions.push_back(usmap);
         
         BTG_MNOTICE(logWrapper(), "Added mapping: HID " << usmap.hid << " -> " << usmap.session << " -> " << usmap.filename);
         
         sendCommand(dd_->externalization,
                     dd_->transport,
                     connectionID_,
                     new contextCreateFromUrlResponseCommand(hid));
      }

   } // namespace daemon
} // namespace btg

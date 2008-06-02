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

#include "urlmgr.h"

#include <bcore/verbose.h>
#include "modulelog.h"
#include "filetrack.h"
#include "session_list.h"

#include <bcore/command/context_create.h>

namespace btg
{
   namespace daemon
   {
      using namespace btg::core;

      const std::string moduleName("umgr");
      const t_uint max_url_age = 30;

      UrlIdSessionMapping::UrlIdSessionMapping(t_uint _id, 
                                               long _session, 
                                               std::string const& _userdir,
                                               std::string const& _filename,
                                               bool const _start)
         : id(_id),
           valid(true),
           session(_session),
           userdir(_userdir),
           filename(_filename),
           start(_start),
           status(UCS_UNDEF),
           age(0)
      {
      }
      
      /* */

      urlManager::urlManager(btg::core::LogWrapperType _logwrapper,
                             bool const _verboseFlag,
                             fileTrack* _filetrack,
                             sessionList* _sessionlist)
         : btg::core::Logable(_logwrapper),
           verboseFlag(_verboseFlag),
           filetrack(_filetrack),
           sessionlist(_sessionlist),
           httpmgr(_logwrapper),
           urlIdSessions()
      {

      }

      std::vector<UrlIdSessionMapping>::iterator urlManager::getUrlMapping(t_uint _id)
      {
         std::vector<UrlIdSessionMapping>::iterator i;
         for (i = urlIdSessions.begin();
              i != urlIdSessions.end();
              i++)
            {
               if (i->id == _id)
                  {
                     return i;
                  }
            }

         return urlIdSessions.end();
      }

      bool urlManager::abort(const t_uint _id)
      {
         std::vector<UrlIdSessionMapping>::iterator iter = getUrlMapping(_id);

         if (iter == urlIdSessions.end())
            {
               return false;
            }

         if (iter->status == UCS_UNDEF)
            {
               // The context was not added.
               httpmgr.Terminate(_id);
               removeUrl(*iter);
               
               return true;
            }

         return false;
      }

      t_uint urlManager::size() const
      {
         return urlIdSessions.size();
      }

      void urlManager::checkUrlDownloads()
      {
         std::vector<UrlIdSessionMapping>::iterator i;
         for (i = urlIdSessions.begin();
              i != urlIdSessions.end();
              i++)
            {
               if ((i->age > max_url_age) || (!i->valid))
                  {
                     continue;
                  }

               i->age++;

               btg::daemon::http::httpInterface::Status s = httpmgr.getStatus(i->id);
               if (s == btg::daemon::http::httpInterface::FINISH)
                  {
                     // Dl finished, now create the context.
                     addUrl(*i);
                  }

               if (s == btg::daemon::http::httpInterface::ERROR)
                  {
                     // Unable to download torrent, clean up.
                     removeUrl(*i);

                     filetrack->remove(i->userdir, i->filename);
                     // Expire this download.
                     i->valid = false;
                  }
            }

         // Remove expired downloads.
         i = urlIdSessions.begin();
         while (i != urlIdSessions.end())
            {
               if (i->age > max_url_age)
                  {
                     httpmgr.Terminate(i->id);
                     filetrack->remove(i->userdir,
                                       i->filename);
                     i = urlIdSessions.erase(i);
                  }
               else
                  {
                     i++;
                  }
            }
      }

      void urlManager::removeUrl(UrlIdSessionMapping & _mapping)
      {
         filetrack->remove(_mapping.userdir,
                           _mapping.filename);
         // Expire this download.
         _mapping.valid = false;
      }

      void urlManager::addUrl(t_uint _id)
      {
         std::vector<UrlIdSessionMapping>::iterator i = getUrlMapping(_id);

         if (i != urlIdSessions.end())
            {
               if (i->valid)
                  {
                     addUrl(*i);
                     urlIdSessions.erase(i);
                  }
            }
      }

      void urlManager::addUrl(UrlIdSessionMapping & _mapping)
      {
         // Find the required event handler.
         eventHandler* eh = 0;
         if (!sessionlist->get(_mapping.session, eh))
            {
               // No session, expire.
               _mapping.valid = false;
               return;
            }

         btg::core::sBuffer sbuf;
         if (!sbuf.read(_mapping.userdir + projectDefaults::sPATH_SEPARATOR() + _mapping.filename))
            {
               // No data, expire.
               _mapping.valid = false;
               return;
            }

         // Remove the file from filetrack, as it will be added when
         // creating the torrent.
         filetrack->remove(_mapping.userdir,
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
                            verboseFlag, "Added '" << _mapping.filename << "' from URL.");
            }
         
         delete ccwdc;
         ccwdc = 0;
      }

      bool urlManager::unique(std::string const & _filename, 
                              std::string const & _userdir) const
      {
         bool u = true;
 
         std::vector<UrlIdSessionMapping>::const_iterator i;
         for (i = urlIdSessions.begin();
              i != urlIdSessions.end();
              i++)
            {
               if ((i->valid) && 
                   (i->filename == _filename) && 
                   (i->userdir == _userdir))
                  {
                     u = false;
                     break;
                  }
            }

         return u;
      }

      bool urlManager::getStatus(const t_uint _id, btg::core::urlStatus & _urlstat)
      {
         std::vector<UrlIdSessionMapping>::iterator mapping = getUrlMapping(_id);

         if (mapping == urlIdSessions.end())
            {
               return false;
            }

         btg::daemon::http::httpInterface::Status httpstat = httpmgr.getStatus(_id);
         
         switch (httpstat)
            {
            case btg::daemon::http::httpInterface::ERROR:
               {
                  removeUrl(*mapping);
                  _urlstat = URLS_ERROR;
                  break;
               }
            case btg::daemon::http::httpInterface::INIT:
               _urlstat = URLS_WORKING;
               break;
            case btg::daemon::http::httpInterface::WAIT:
               _urlstat = URLS_WORKING;
               break;
            case btg::daemon::http::httpInterface::FINISH:
               {
                  _urlstat = URLS_FINISHED;

                  // The http download finished, add the torrent.
                  addUrl(_id);

                  if (mapping->status == UCS_CREATED)
                     {
                        _urlstat = URLS_CREATE;
                     }

                  if (mapping->status == UCS_CREATE_FAILED)
                     {
                        _urlstat = URLS_CREATE_ERR;
                     }

                  break;
               }
            case btg::daemon::http::httpInterface::ABORTED:
               {
                  _urlstat = URLS_ABORTED;
                  break;
               }
            }
         
         return true;
      }

      bool urlManager::getDlProgress(const t_uint _id, t_uint & _dltotal, t_uint & _dlnow, t_uint & _dlspeed)
      {
         std::vector<UrlIdSessionMapping>::iterator mapping = getUrlMapping(_id);

         if (mapping == urlIdSessions.end())
            {
               return false;
            }
         
         return httpmgr.getDlProgress(_id, _dltotal, _dlnow, _dlspeed);
      }

      t_uint urlManager::addMapping(std::string const& _url, 
                                    std::string const& _userdir,
                                    std::string const& _filename,
                                    t_long const _session,
                                    bool const _start)
      {
         t_uint id = httpmgr.Fetch(_url, _userdir + projectDefaults::sPATH_SEPARATOR() + _filename);

         UrlIdSessionMapping usmap(id, _session, _userdir, _filename, _start);
         urlIdSessions.push_back(usmap);

         BTG_MNOTICE(logWrapper(), "Added mapping: HID " << id << " -> " << 
                     _session << " -> " << _filename);

         return id;
      }

      urlManager::~urlManager()
      {
         urlIdSessions.clear();
      }

   } // namespace daemon
} // namespace btg

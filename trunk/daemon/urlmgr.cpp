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
#include <bcore/opstatus.h>

namespace btg
{
   namespace daemon
   {
      using namespace btg::core;

      const std::string moduleName("umgr");
      const t_uint max_url_age = 30;

      UrlIdSessionMapping::UrlIdSessionMapping(t_uint _id, 
                                               t_long _session, 
                                               std::string const& _userdir,
                                               std::string const& _filename,
                                               std::string const& _uri,
                                               bool const _start,
                                               bool const _is_magnet_uri)
         : id(_id),
           valid(true),
           session(_session),
           userdir(_userdir),
           filename(_filename),
           URI(_uri),
           start(_start),
           status(UCS_UNDEF),
           age(0),
           magnet_uri(_is_magnet_uri)
      {
      }
      
      /* */

      urlManager::urlManager(btg::core::LogWrapperType _logwrapper,
                             bool const _verboseFlag,
                             fileTrack* _filetrack,
                             sessionList* _sessionlist,
                             btg::daemon::opId & _opid)
         : btg::core::Logable(_logwrapper),
           verboseFlag(_verboseFlag),
           filetrack(_filetrack),
           sessionlist(_sessionlist),
           httpmgr(_logwrapper, _opid),
           urlIdSessions(),
           magnetmgr(_logwrapper,
                     _verboseFlag,
                     _filetrack,
                     _sessionlist,
                     _opid)
      {

      }

      std::vector<UrlIdSessionMapping>::const_iterator urlManager::getUrlMapping(t_uint _id) const
      {
         std::vector<UrlIdSessionMapping>::const_iterator i;
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
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         if (belongs(_id))
            {
               return abortImpl(_id);
            }
         else if (magnetmgr.belongs(_id))
            {
               return magnetmgr.abort(_id);
            }
         else
            {
               return false;
            }
      }

      bool urlManager::abortImpl(const t_uint _id)
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

      t_uint urlManager::size()
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         t_uint s = urlIdSessions.size() + magnetmgr.size();
         return s;
      }
      
      void urlManager::checkUrlDownloads()
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         BTG_MNOTICE(logWrapper(), "checkUrlDownloads");

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
               
               if (belongs(i->id))
                  {
                     checkUrlDownloadsImpl(*i);
                  }
               else if (magnetmgr.belongs(i->id))
                  {
                     checkMagnetDownloadsImpl(*i);
                  }
            }

         // Remove expired downloads.
         i = urlIdSessions.begin();
         while (i != urlIdSessions.end())
            {
               if (i->age > max_url_age)
                  {

                     if (belongs(i->id))
                        {
                           expireUrl(i);
                        }
                     else if (magnetmgr.belongs(i->id))
                        {
                           expireMagnet(i);
                        }
                  }
               else
                  {
                     i++;
                  }
            }

         magnetmgr.checkMagnetDownloads();
      }

      void urlManager::expireUrl(std::vector<UrlIdSessionMapping>::iterator & _iter)
      {
         httpmgr.Terminate(_iter->id);
         filetrack->remove(_iter->userdir,
                           _iter->filename);
         _iter = urlIdSessions.erase(_iter);

      }

      void urlManager::expireMagnet(std::vector<UrlIdSessionMapping>::iterator & _iter)
      {
         magnetmgr.abort(_iter->id);
         filetrack->remove(_iter->userdir,
                           _iter->filename);
         _iter = urlIdSessions.erase(_iter);
      }

      void urlManager::checkMagnetDownloadsImpl(UrlIdSessionMapping & _mapping)
      {
         BTG_MNOTICE(logWrapper(), "checkMagnetDownloadsImpl");

         t_uint urlstat = OP_UNDEF;
         if (!magnetmgr.getStatus(_mapping.id, urlstat))
            {
               return;
            }

         if (urlstat == btg::core::OP_FINISHED)
            {
               // Dl finished, now create the context.
               if (addUrl(_mapping))
                  {
                     magnetmgr.setState(_mapping.id, MAG_CREATED);
                  }
               else
                  {
                     magnetmgr.setState(_mapping.id, MAG_CREATE_FAILED);
                  }
            }

         if (urlstat == btg::core::OP_ERROR)
            {
               // Unable to download torrent, clean up.
               removeUrl(_mapping);
            }
      }

      void urlManager::checkUrlDownloadsImpl(UrlIdSessionMapping & _mapping)
      {
         BTG_MNOTICE(logWrapper(), "checkUrlDownloadsImpl");

         btg::daemon::http::httpInterface::Status s = httpmgr.getStatus(_mapping.id);
         if (s == btg::daemon::http::httpInterface::FINISH)
            {
               // Dl finished, now create the context.
               addUrl(_mapping);
            }
         
         if (s == btg::daemon::http::httpInterface::ERROR)
            {
               // Unable to download torrent, clean up.
               removeUrl(_mapping);
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

      bool urlManager::addUrl(UrlIdSessionMapping & _mapping)
      {
         bool status = false;

         // Find the required event handler.
         eventHandler* eh = 0;
         if (!sessionlist->get(_mapping.session, eh))
            {
               // No session, expire.
               _mapping.valid = false;
               return status;
            }

         btg::core::sBuffer sbuf;
         if (!sbuf.read(_mapping.userdir + projectDefaults::sPATH_SEPARATOR() + _mapping.filename))
            {
               // No data, expire.
               _mapping.valid = false;
               return status;
            }

         // Remove the file from filetrack, as it will be added when
         // creating the torrent.
         filetrack->remove(_mapping.userdir,
                           _mapping.filename);
         
         // Downloaded file.
         btg::core::contextCreateWithDataCommand* ccwdc = 
            new btg::core::contextCreateWithDataCommand(_mapping.filename, sbuf, _mapping.start);
         
         _mapping.valid  = false;

         // Do not use a connection id.
         if (!eh->createWithData(ccwdc))
            {
               BTG_MERROR(logWrapper(), 
                          "Adding '" << _mapping.filename << "' from URL failed.");
               _mapping.status = UCS_CREATE_FAILED;
               status = false;
            }
         else
            {
               MVERBOSE_LOG(logWrapper(), 
                            verboseFlag, "Added '" << _mapping.filename << "' from URL.");
               _mapping.status = UCS_CREATED;
               status = true;
            }
         
         delete ccwdc;
         ccwdc = 0;

         return status;
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

      bool urlManager::belongs(const t_uint _id) const
      {
         std::vector<UrlIdSessionMapping>::const_iterator mapping = getUrlMapping(_id);

         if (mapping->magnet_uri)
            {
               return false;
            }

         return true;
      }

      bool urlManager::getStatus(const t_uint _id, t_uint & _urlstat)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         BTG_MNOTICE(logWrapper(), "urlManager::getStatus");

         if (belongs(_id))
            {
               return getStatusImpl(_id, _urlstat);
            }
         else if (magnetmgr.belongs(_id))
            {
               return magnetmgr.getStatus(_id, _urlstat);
            }
         else
            {
               return false;
            }
      }

      bool urlManager::getStatusImpl(const t_uint _id, t_uint & _urlstat)
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
                  _urlstat = btg::core::OP_ERROR;
                  break;
               }
            case btg::daemon::http::httpInterface::INIT:
               _urlstat = btg::core::OP_WORKING;
               break;
            case btg::daemon::http::httpInterface::WAIT:
               _urlstat = btg::core::OP_WORKING;
               break;
            case btg::daemon::http::httpInterface::FINISH:
               {
                  _urlstat = btg::core::OP_FINISHED;

                  // The http download finished, add the torrent.
                  addUrl(_id);

                  if (mapping->status == UCS_CREATED)
                     {
                        _urlstat = btg::core::OP_CREATE;
                     }

                  if (mapping->status == UCS_CREATE_FAILED)
                     {
                        _urlstat = btg::core::OP_CREATE_ERR;
                     }

                  break;
               }
            case btg::daemon::http::httpInterface::ABORTED:
               {
                  _urlstat = btg::core::OP_ABORTED;
                  break;
               }
            }
         
         return true;
      }

      bool urlManager::getDlProgress(const t_uint _id, t_uint & _dltotal, t_uint & _dlnow, t_uint & _dlspeed)
      {
         if (belongs(_id))
            {
               return getDlProgressImpl(_id, _dltotal, _dlnow, _dlspeed);
            }
         else if (magnetmgr.belongs(_id))
            {
               return magnetmgr.getDlProgress(_id, _dltotal, _dlnow, _dlspeed);
            }
         else
            {
               return false;
            }
      }
      bool urlManager::getDlProgressImpl(const t_uint _id, t_uint & _dltotal, t_uint & _dlnow, t_uint & _dlspeed)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::vector<UrlIdSessionMapping>::iterator mapping = getUrlMapping(_id);

         if (mapping == urlIdSessions.end())
            {
               return false;
            }
         
         return httpmgr.getDlProgress(_id, _dltotal, _dlnow, _dlspeed);
      }

      bool urlManager::isMagnetURI(std::string const& s) const
      {
         bool magnet = false;
         std::string const begin("magnet:?xt=");

         if (s.find(begin) == 0)
            {
               magnet = true;
            }

         return magnet;
      }

      t_uint urlManager::addMapping(std::string const& _url, 
                                    std::string const& _userdir,
                                    std::string const& _filename,
                                    t_long const _session,
                                    bool const _start)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         bool is_magnet_uri = isMagnetURI(_url);

         t_uint id = 0;

         if (is_magnet_uri)
            {
               id = magnetmgr.add(_session, _userdir, _filename, _url, _start);
            }
         else
            {
               id = httpmgr.Fetch(_url, _userdir + projectDefaults::sPATH_SEPARATOR() + _filename);
            }

         UrlIdSessionMapping usmap(id, _session, _userdir, _filename, _url, _start, is_magnet_uri);
         urlIdSessions.push_back(usmap);

         BTG_MNOTICE(logWrapper(), "Added mapping: HID " << id << " -> " << 
                     _session << " -> " << _filename << " (magnet:" << is_magnet_uri << ")");

         return id;
      }

      urlManager::~urlManager()
      {
         urlIdSessions.clear();
      }

   } // namespace daemon
} // namespace btg

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

#include "magmgr.h"

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

      const std::string moduleName("mmgr");
      const t_uint max_url_age = 30;

      MagnetIdSessionMapping::MagnetIdSessionMapping(t_uint _id, 
                                                     t_long const _session,
                                                     std::string const& _userdir,
                                                     std::string const& _filename, 
                                                     std::string const& _URI,
                                                     bool const _start)
         : id(_id),
           session(_session),
           userdir(_userdir),
           filename(_filename),
           URI(_URI),
           start(_start),
           handle(0),
           status(MAG_UNDEF),
           age(0),
           tempTorrentId(-1)
      {
      }
      
      /* */

      magnetManager::magnetManager(btg::core::LogWrapperType _logwrapper,
                                   bool const _verboseFlag,
                                   fileTrack* _filetrack,
                                   sessionList* _sessionlist,
                                   btg::daemon::opId & _opid)
         : btg::core::Logable(_logwrapper),
           verboseFlag(_verboseFlag),
           filetrack(_filetrack),
           sessionlist(_sessionlist),
           opid(_opid),
           magnetIdSessions()
      {

      }

      std::vector<MagnetIdSessionMapping>::iterator magnetManager::getUrlMapping(t_uint _id)
      {
         std::vector<MagnetIdSessionMapping>::iterator i;
         for (i = magnetIdSessions.begin();
              i != magnetIdSessions.end();
              i++)
            {
               if (i->id == _id)
                  {
                     return i;
                  }
            }

         return magnetIdSessions.end();
      }

      bool magnetManager::getDlProgress(const t_uint _id, t_uint & _dltotal, t_uint & _dlnow, t_uint & _dlspeed)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::vector<MagnetIdSessionMapping>::iterator mapping = getUrlMapping(_id);

         if (mapping == magnetIdSessions.end())
            {
               return false;
            }
         
         _dltotal = 0;
         _dlnow   = 0;
         _dlspeed = 0;

         return true;
      }

      bool magnetManager::getStatus(const t_uint _id, t_uint & _urlstat)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::vector<MagnetIdSessionMapping>::iterator mapping = getUrlMapping(_id);

         if (mapping == magnetIdSessions.end())
            {
               return false;
            }

         switch (mapping->status)
            {

            case MAG_UNDEF:
            case MAG_NO_META:
               {
                  _urlstat = btg::core::OP_WORKING;
                  break;
               }
            case MAG_META_FAILED:
               {
                  _urlstat = btg::core::OP_ERROR;
                  break;
               }
            case MAG_DOWNLOADED:
               {
                  _urlstat = btg::core::OP_FINISHED;
                  break;
               }
            case MAG_CREATED:
               {
                  _urlstat = btg::core::OP_CREATE;
                  break;
               }
            case MAG_CREATE_FAILED:
               {
                  _urlstat = btg::core::OP_CREATE_ERR;
                  break;
               }
            case MAG_ABORTED:
               {
                  _urlstat = btg::core::OP_ABORTED;
                  break;
               }
            }

         return true;
      }

      t_uint magnetManager::add(t_long const _session,
                                std::string const& _userdir,
                                std::string const& _filename, 
                                std::string const& _URI,
                                bool const _start)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         t_uint id = opid.id();

         MagnetIdSessionMapping m(id, 
                                  _session,
                                  _userdir,
                                  _filename, 
                                  _URI,
                                  _start);

         magnetIdSessions.push_back(m);

         return id;
      }

      bool magnetManager::belongs(const t_uint _id)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::vector<MagnetIdSessionMapping>::iterator mapping = getUrlMapping(_id);

         if (mapping == magnetIdSessions.end())
            {
               return false;
            }

         return true;
      }

      t_uint magnetManager::size()
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         return magnetIdSessions.size();
      }

      void magnetManager::setState(const t_uint _id, const MagnetCreateStatus _state)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::vector<MagnetIdSessionMapping>::iterator mapping = getUrlMapping(_id);

         if (mapping == magnetIdSessions.end())
            {
               return;
            }
         
         switch (mapping->status)
            {
            case MAG_DOWNLOADED:
               {
                  switch (_state)
                     {
                     case MAG_CREATED:
                     case MAG_CREATE_FAILED:
                        mapping->status = _state;
                        break;
                     default:
                        break;
                     }
                  break;
               }
            default:
               break;
            }
      }

      void magnetManager::stopTorrentDownload(MagnetIdSessionMapping & _mapping)
      {
         eventHandler* eh = 0;
         if (!sessionlist->get(_mapping.session, eh))
            {
               BTG_MNOTICE(logWrapper(),
                           "magnetManager::stopTorrentDownload unknown session '" << _mapping.session << ".");
               return;
            }

         eh->stopMagnetDownload(_mapping.tempTorrentId);
      }

      bool magnetManager::startTorrentDownload(MagnetIdSessionMapping & _mapping)
      {
         eventHandler* eh = 0;
         if (!sessionlist->get(_mapping.session, eh))
            {
               BTG_MNOTICE(logWrapper(),
                           "magnetManager::startTorrentDownload unknown session '" << _mapping.session << ".");

               filetrack->remove(_mapping.userdir, _mapping.filename);
               return false;
            }

         
         if (!eh->createFromMagnet(_mapping.filename, 
                                  _mapping.URI,
                                  true,
                                   _mapping.tempTorrentId))
            {
               filetrack->remove(_mapping.userdir, _mapping.filename);
               return false;
            }

         return true;
      }

      void magnetManager::handle_MAG_UNDEF(MagnetIdSessionMapping & _mapping)
      {
         // Start torrent to download a .torrent file from
         // peers.

         _mapping.status =  MAG_NO_META;
         if (!startTorrentDownload(_mapping))
            {
               _mapping.status = MAG_META_FAILED;
            }
      }

      void magnetManager::handle_MAG_NO_META(MagnetIdSessionMapping & _mapping)
      {
         eventHandler* eh = 0;
         if (sessionlist->get(_mapping.session, eh))
            {
               if (eh->gotTorrent(_mapping.tempTorrentId))
                  {
                     stopTorrentDownload(_mapping);
                     
                     // Downloaded torrent. Ready for creation.
                     _mapping.status =  MAG_DOWNLOADED;

                     eh->removeInitialMagnetDlData(_mapping.tempTorrentId);
                  }
            }
      }

      void magnetManager::checkState(MagnetIdSessionMapping & _mapping)
      {
         switch (_mapping.status)
            {
            case MAG_UNDEF:
               {
                  handle_MAG_UNDEF(_mapping);
                  break;
               }
            case MAG_NO_META:
               {
                  handle_MAG_NO_META(_mapping);
                  break;
               }
            case MAG_META_FAILED:
               {
                  break;
               }
            case MAG_DOWNLOADED:
               {
                  break;
               }
            case MAG_CREATED:
               {
                  break;
               }
            case MAG_CREATE_FAILED:
               {
                  break;
               }
            }
      }

      void magnetManager::checkMagnetDownloads()
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::vector<MagnetIdSessionMapping>::iterator i;
         for (i = magnetIdSessions.begin();
              i != magnetIdSessions.end();
              i++)
            {
               if (i->age > max_url_age)
                  {
                     continue;
                  }

               checkState(*i);
               i->age++;
            }

         // Remove expired downloads.
         i = magnetIdSessions.begin();
         while (i != magnetIdSessions.end())
            {
               if (i->age > max_url_age)
                  {
                     abort(*i);
                     i = magnetIdSessions.erase(i);
                  }
               else
                  {
                     i++;
                  }
            }
      }

      bool magnetManager::abort(const t_uint _id)
      {
         /* !!! BOLLOX implement this. */
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         return false;
      }

      void magnetManager::abort(MagnetIdSessionMapping & mapping)
      {
         /* Abort downloading or creation. */
         
      }

      magnetManager::~magnetManager()
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::vector<MagnetIdSessionMapping>::iterator i = magnetIdSessions.begin();
         while (i != magnetIdSessions.end())
            {
               abort(*i);
            }

         magnetIdSessions.clear();
      }

   } // namespace daemon
} // namespace btg

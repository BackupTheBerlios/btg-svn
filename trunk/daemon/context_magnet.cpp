/*
 * btg Copyright (C) 2005 Michael Wojciechowski.
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

#include "context.h"

#include <bcore/logmacro.h>
#include "modulelog.h"
#include "filetrack.h"

#include "lt_version.h"
#include <libtorrent/magnet_uri.hpp>

#if BTG_OPTION_EVENTCALLBACK
#  include "callbackmgr.h"
#endif // BTG_OPTION_EVENTCALLBACK

namespace btg
{
   namespace daemon
   {
      using namespace btg::core;
      
      const std::string moduleName("ctn");
      
      /* Assumes that filetracker knows this torrent. */
      Context::addResult Context::addFromMagnet(std::string const& _torrent_filename, 
                                                std::string const& _magnet_URI, 
                                                t_int & _handle_id)
      {
         Context::addResult status = Context::ERR_UNDEFINED;
         
         BTG_MENTER(logWrapper(), "addFromMagnet", "_torrent_filename=" << _torrent_filename << ", URI " << _magnet_URI);
         
         std::string fileTrackFilename = _torrent_filename;

         // No idea if this torrent has any data at this point.
         boost::filesystem::path dataPath = boost::filesystem::path(tempDir_, boost::filesystem::native);
         torrentStorage ts                = tsTemp;

         libtorrent::add_torrent_params atp;
         atp.name         = 0; // "default name";
         //atp.ti.swap(tinfo);
         atp.save_path    = dataPath;
         atp.storage_mode = allocation_mode_;
         atp.auto_managed = false;
         atp.paused       = false;
         atp.duplicate_is_error = true;

         bool torrent_created = false;
         libtorrent::torrent_handle magnet_handle;
         try
            {
               magnet_handle = libtorrent::add_magnet_uri(*torrent_session, _magnet_URI, atp);
               torrent_created = true;
            }
         catch (std::exception& e)
            {
               torrent_created = false;
               BTG_ERROR_LOG(logWrapper(), "libtorrent exception: " << e.what() );
            }

         if (!torrent_created)
            {
               status = Context::ERR_LIBTORRENT;
               // Stop tracking the torrent file.
               filetrack_->remove(tempDir_, fileTrackFilename);
               BTG_MEXIT(logWrapper(), "addFromMagnet", status);
               return status;
            }

         // Got handle. 

         t_int const handle_id = global_counter;
         global_counter++;

         torrentInfo* ti = new torrentInfo(false, false);
         ti->handle      = magnet_handle;
         ti->filename    = _torrent_filename;
         ti->limitUpld   = default_torrent_uploadLimit_;
         ti->limitDwnld  = default_torrent_downloadLimit_;
         ti->seedLimit   = default_torrent_seedLimit_;
         ti->seedTimeout = default_torrent_seedTimeout_;

         torrents[handle_id] = ti;
         torrent_ids.push_back(handle_id);
         torrent_storage[handle_id] = ts;
         
         // Updated handle ID in arguments.
         _handle_id          = handle_id;
         
         status = Context::ERR_OK;
         BTG_MEXIT(logWrapper(), "addFromMagnet", toString(status));
         return status;
      }

      void Context::stopMagnetDownload(t_int const _torrent_id)
      {
         BTG_MENTER(logWrapper(), "stopMagnetDownload", "id = " << _torrent_id);

         torrentInfo* ti;
         if ((ti = getTorrentInfo(_torrent_id)) == 0)
            {
               BTG_MEXIT(logWrapper(), "stopMagnetDownload", false);
               return;
            }

         if (ti->ready)
            {
               BTG_MEXIT(logWrapper(), "stopMagnetDownload (no need to stop id " << _torrent_id << ")", false);
               return;
            }

         torrent_session->remove_torrent(ti->handle, libtorrent::session::delete_files);
         BTG_MEXIT(logWrapper(), "stopMagnetDownload", true);
      }

      void Context::removeInitialMagnetDlData(t_int const _torrent_id)
      {
         BTG_MENTER(logWrapper(), "removeInitialMagnetDlData", "id = " << _torrent_id);

         torrentInfo* ti;
         if ((ti = getTorrentInfo(_torrent_id)) == 0)
            {
               BTG_MEXIT(logWrapper(), "removeInitialMagnetDlData", false);
               return;
            }

         progress_.remove(_torrent_id);

         torrents.erase(_torrent_id);
         delete ti;
         ti = 0;
         
         // Remove storage data
         torrent_storage.erase(_torrent_id);

         // Remove the context id:
         t_intListI ii;
         for (ii=torrent_ids.begin(); ii != torrent_ids.end(); ii++)
            {
               if (*ii == _torrent_id)
                  {
                     torrent_ids.erase(ii);
                     break;
                  }
            }
         BTG_MEXIT(logWrapper(), "removeInitialMagnetDlData", true);
      }

   } // namespace daemon
} // namespace btg

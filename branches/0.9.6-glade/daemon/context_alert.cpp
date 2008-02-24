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

#include <bcore/trackerstatus.h>
#include <bcore/logmacro.h>

#if BTG_OPTION_EVENTCALLBACK
#  include "callbackmgr.h"
#endif // BTG_OPTION_EVENTCALLBACK

#include <bcore/verbose.h>

namespace btg
{
   namespace daemon
   {
      using namespace btg::core;


      void Context::handleBannedHost(libtorrent::peer_ban_alert* _alert)
      {
         // A peer was banned.
         asio::ip::basic_endpoint<asio::ip::tcp> endp = _alert->ip;
         asio::ip::address_v4 banned_ip = endp.address().to_v4();

         BTG_NOTICE("Banned host: " << banned_ip.to_string() << ".");

         VERBOSE_LOG(verboseFlag_, "Banned host: " << banned_ip.to_string() << ".");
      }

      void Context::handleFinishedTorrent(libtorrent::torrent_finished_alert* _alert)
      {
         // A torrent finnished.

         // Resolve the torrent handle to a torrent id.
         torrentInfo* ti = 0;
         std::map<t_int, torrentInfo*>::iterator tii;
         t_int torrent_id = -1;
         for (tii = torrents.begin(); tii != torrents.end(); tii++)
            {
               if (tii->second->handle == _alert->handle)
                  {
                     torrent_id = tii->first;
                     ti = tii->second;
                     break;
                  }
            }

         // Unknown id?
         if (torrent_id == -1 || !ti)
            {
               BTG_NOTICE("Context::handleAlerts(), got torrent_finished_alert for unknown torrent handle " << &(_alert->handle));
               return;
            }

         // Move the output to the seeding dir.

         if (!this->moveToSeedingDir(torrent_id))
            {
               BTG_NOTICE("Context::handleAlerts(), unable to move to the seeding directory.");
            }

         // -/- -/- -/- -/- -/- -/- -/- -/- -/-

#if BTG_OPTION_EVENTCALLBACK
         // Call event callback.
         std::string filename;
         this->getFilename(torrent_id, filename);

         std::vector<std::string> cbm_arguments;
         cbm_arguments.push_back(filename);
         cbm_->event(username_,
                     callbackManager::CBM_SEED,
                     cbm_arguments);
#endif // BTG_OPTION_EVENTCALLBACK

         // Known id, set a timestamp on the finish.
         // Make sure we dont already have a timestamp
         // here, we dont want to reset finished time on
         // restart of daemon.
         if (ti->finished_timestamp.is_not_a_date_time())
            {
               // Get the current time.
               ti->finished_timestamp = boost::posix_time::ptime(boost::posix_time::second_clock::local_time());
               BTG_NOTICE("Torrent " << torrent_id
                          << " finished at "
                          << boost::posix_time::to_simple_string(ti->finished_timestamp));
            }
      }

      void Context::handlePeerError(libtorrent::peer_error_alert* _alert)
      {
         // A peer generates errors.
         asio::ip::basic_endpoint<asio::ip::tcp> endp = _alert->ip;
         asio::ip::address_v4 banned_ip = endp.address().to_v4();
         BTG_NOTICE("Errors from peer: " << banned_ip.to_string() << ".");
      }

      void Context::handleTrackerAlert(libtorrent::tracker_alert* _alert)
      {
         libtorrent::torrent_handle const& handle = _alert->handle;

         t_int torrent_id;
         torrentInfo *ti;
         std::string filename;

         if (getIdFromHandle(handle, torrent_id, ti))
            {
	      getFilename(torrent_id, filename);

	       VERBOSE_LOG(verboseFlag_, "Tracker alert: filename '" << 
			   filename << "', status = " << 
			   _alert->status_code << ", message '" << 
			   _alert->msg() << "'");
	       ti->trackerStatus.invalidate();
	       // Handle alerts which have some undefined status code.
	       if (_alert->status_code != trackerStatus::undefined)
		 {
		   ti->trackerStatus.setStatus(_alert->status_code);
		 }
	       else
		 {
		   ti->trackerStatus.setStatus(trackerStatus::warning);
		 }
               ti->trackerStatus.setSerial(ti->serial);
	       ti->trackerStatus.setMessage(_alert->msg());
               ti->serial++;

               if (ti->serial > trackerStatus::MAX_SERIAL)
                  {
                     ti->serial = trackerStatus::MIN_SERIAL;
                  }
            }
      }

      void Context::handleTrackerReplyAlert(libtorrent::tracker_reply_alert* _alert)
      {
         libtorrent::torrent_handle const& handle = _alert->handle;

         t_int torrent_id;
         torrentInfo *ti;
	 std::string filename;

         if (getIdFromHandle(handle, torrent_id, ti))
            {
	      getFilename(torrent_id, filename);

               VERBOSE_LOG(verboseFlag_, "Tracker reply alert: filename '" << 
			   filename << "', status = 200 OK");
	       ti->trackerStatus.invalidate();
               ti->trackerStatus.setStatus(200);
               ti->trackerStatus.setSerial(ti->serial);
               ti->serial++;

               if (ti->serial > trackerStatus::MAX_SERIAL)
                  {
                     ti->serial = trackerStatus::MIN_SERIAL;
                  }
            }
      }

     void Context::handleTrackerWarningAlert(libtorrent::tracker_warning_alert* _alert)
     {
       // Tracker wants to tell the client about something.

       libtorrent::torrent_handle const& handle = _alert->handle;

       t_int torrent_id;
       torrentInfo *ti;
       std::string filename;

       if (getIdFromHandle(handle, torrent_id, ti))
	 {
	   getFilename(torrent_id, filename);

	   VERBOSE_LOG(verboseFlag_, "Tracker warning alert: filename '" << 
		       filename << "', message '" << _alert->msg() << "'");
	       ti->trackerStatus.invalidate();
               ti->trackerStatus.setStatus(trackerStatus::warning);
               ti->trackerStatus.setSerial(ti->serial);
               ti->trackerStatus.setMessage(_alert->msg());
               ti->serial++;

               if (ti->serial > trackerStatus::MAX_SERIAL)
                  {
                     ti->serial = trackerStatus::MIN_SERIAL;
                  }
            }
     }

      void Context::handleAlerts()
      {
         std::auto_ptr<libtorrent::alert> sp_alert = torrent_session->pop_alert();
         libtorrent::alert* alert = sp_alert.get();

         if (alert != 0)
            {
               // peer_ban_alert
               // peer_error_alert
               if (typeid(*alert) == typeid(libtorrent::peer_ban_alert))
                  {
                     handleBannedHost(dynamic_cast<libtorrent::peer_ban_alert*>(alert));
                  }
               else if (typeid(*alert) == typeid(libtorrent::torrent_finished_alert))
                  {
                     handleFinishedTorrent(dynamic_cast<libtorrent::torrent_finished_alert*>(alert));
                  }
               else if (typeid(*alert) == typeid(libtorrent::peer_error_alert))
                  {
                     handlePeerError(dynamic_cast<libtorrent::peer_error_alert*>(alert));
                  }
               else if (typeid(*alert) == typeid(libtorrent::tracker_alert))
                  {
                     handleTrackerAlert(dynamic_cast<libtorrent::tracker_alert*>(alert));
                  }
               else if (typeid(*alert) == typeid(libtorrent::tracker_reply_alert))
                  {
                     handleTrackerReplyAlert(dynamic_cast<libtorrent::tracker_reply_alert*>(alert));
                  }
               else if (typeid(*alert) == typeid(libtorrent::tracker_warning_alert))
                  {
		    handleTrackerWarningAlert(dynamic_cast<libtorrent::tracker_warning_alert*>(alert));
                  }
               else
                  {
                     // Log other alerts.
                     BTG_NOTICE("Alert: " << alert->msg());
                  }
            }
      }

   } // namespace daemon
} // namespace btg


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

#include <boost/asio/ip/address_v4.hpp>

//#include <boost/date_time/posix_time/posix_time.hpp>
#include <libtorrent/time.hpp>

#if BTG_OPTION_EVENTCALLBACK
#  include "callbackmgr.h"
#endif // BTG_OPTION_EVENTCALLBACK

#include <bcore/verbose.h>
#include "lt_version.h"

#include "filetrack.h"
#include <bcore/btg_assert.h>

#include <boost/filesystem.hpp>

namespace btg
{
   namespace daemon
   {
      using namespace btg::core;


      void Context::handleBannedHost(libtorrent::peer_ban_alert* _alert)
      {
         // A peer was banned.
         boost::asio::ip::basic_endpoint<boost::asio::ip::tcp> endp = _alert->ip;
         boost::asio::ip::address_v4 banned_ip = endp.address().to_v4();
         BTG_NOTICE(logWrapper(), "Banned host: " << banned_ip.to_string() << ".");

         VERBOSE_LOG(logWrapper(), verboseFlag_, "Banned host: " << banned_ip.to_string() << ".");
      }

      void Context::handleFinishedTorrent(libtorrent::torrent_finished_alert* _alert)
      {
         // A torrent finnished.
         
         t_int torrent_id;
         torrentInfo* ti;
         
         if (!getIdFromHandle(_alert->handle, torrent_id, ti))
            {
               // Unknown id?
               BTG_NOTICE(logWrapper(), "Context::handleAlerts(), got torrent_finished_alert for unknown torrent handle " << &(_alert->handle));
               return;
            }

         BTG_NOTICE(logWrapper(), "handleFinishedTorrent torrent_id=" << torrent_id \
            << ", ti=0x" << std::hex << ti << std::dec );

         // Move the output to the seeding dir.

         if (!moveToSeedingDir(torrent_id))
            {
               BTG_NOTICE(logWrapper(), "Context::handleAlerts(), unable to move to the seeding directory.");
            }

         // -/- -/- -/- -/- -/- -/- -/- -/- -/-

#if BTG_OPTION_EVENTCALLBACK
         // Call event callback.
         std::string filename;
         getFilename(torrent_id, filename);

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
               BTG_NOTICE(logWrapper(),
                          "Torrent " << torrent_id
                          << " finished at "
                          << boost::posix_time::to_simple_string(ti->finished_timestamp));
            }
      }

      void Context::handlePeerError(libtorrent::peer_error_alert* _alert)
      {
         // A peer generates errors.
         boost::asio::ip::basic_endpoint<boost::asio::ip::tcp> endp = _alert->ip;
         boost::asio::ip::address_v4 banned_ip = endp.address().to_v4();
         BTG_NOTICE(logWrapper(), "Errors from peer: " << banned_ip.to_string() << ".");
      }
      void Context::handleTrackerAlert(libtorrent::tracker_error_alert* _alert)
      {
         t_int torrent_id;
         torrentInfo *ti;
         std::string filename;

         if (getIdFromHandle(_alert->handle, torrent_id, ti))
            {
               getFilename(torrent_id, filename);
               VERBOSE_LOG(logWrapper(),
                           verboseFlag_, "Tracker alert: filename '" << 
                           filename << "', status = " << 
                           _alert->status_code << ", message '" << 
                           _alert->message()
                           << "'");

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
               ti->trackerStatus.setMessage(
                                            _alert->message()
               );

               ti->serial++;

               if (ti->serial > trackerStatus::MAX_SERIAL)
                  {
                     ti->serial = trackerStatus::MIN_SERIAL;
                  }
            }
      }

      void Context::handleTrackerReplyAlert(libtorrent::tracker_reply_alert* _alert)
      {
         t_int torrent_id;
         torrentInfo *ti;
         std::string filename;

         if (getIdFromHandle(_alert->handle, torrent_id, ti))
            {
               getFilename(torrent_id, filename);

               VERBOSE_LOG(logWrapper(),
                           verboseFlag_, "Tracker reply alert: filename '" << 
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

         t_int torrent_id;
         torrentInfo *ti;
         std::string filename;

         if (getIdFromHandle(_alert->handle, torrent_id, ti))
            {
               getFilename(torrent_id, filename);
               VERBOSE_LOG(logWrapper(), verboseFlag_, "Tracker warning alert: filename '" << 
                           filename << "', message '" << 
                           _alert->message()
                           << "'");

               ti->trackerStatus.invalidate();
               ti->trackerStatus.setStatus(trackerStatus::warning);
               ti->trackerStatus.setSerial(ti->serial);
               ti->trackerStatus.setMessage(
               _alert->message()
               );

               ti->serial++;

               if (ti->serial > trackerStatus::MAX_SERIAL)
                  {
                     ti->serial = trackerStatus::MIN_SERIAL;
                  }
            }
      }

      void Context::handleResumeDataAlert(libtorrent::save_resume_data_alert* _alert)
      {
         libtorrent::torrent_handle th = _alert->handle;
         t_int tid = -1;
         torrentInfo *ti = NULL;

         if (!getIdFromHandle(th, tid, ti)) // else skip it - probably deleted before alert arrived
         {
            BTG_ERROR_LOG(logWrapper(), "Context::handleResumeDataAlert, torrent is not found.");
            return;
         }

         if (!ti->ready)
            {
               BTG_NOTICE(logWrapper(), "Not writting resume data for '" << ti->filename << "'.");
               return;
            }

         // The name to which the resume data is saved to.
         std::string filename;
         filename = tempDir_ + projectDefaults::sPATH_SEPARATOR() + ti->filename +
            fastResumeFileNameEnd;

         // Output file.
         std::ofstream out;

#if HAVE_IOS_BASE
         out.open(filename.c_str(), std::ios_base::out);
#else
         out.open(filename.c_str(), std::ios::out);
#endif

         if (!out.is_open())
            {
               BTG_ERROR_LOG(logWrapper(), "Context::handleResumeDataAlert, unable to open file " << filename << " for writing.");
               return;
            }

         try
            {
               // Encode to file.
               libtorrent::bencode(
                                   std::ostream_iterator<char>(out),
                                   *_alert->resume_data
                                   );
            }
         catch (const libtorrent::invalid_encoding & e)
            {
               BTG_ERROR_LOG(logWrapper(), "libtorrent exception: " << e.what() );
               return;
            }

         out.close();
         BTG_NOTICE(logWrapper(), "wrote fast resume data for '" << filename << "'");
      }

      void Context::handleResumeDataFailedAlert(libtorrent::save_resume_data_failed_alert* _alert)
      {
         BTG_NOTICE(logWrapper(), "Failed to write fast resume data: '" << _alert->msg << "'");
      }

      void Context::handleMetadataFailedAlert(libtorrent::metadata_failed_alert* _alert)
      {
         libtorrent::torrent_handle th = _alert->handle;
         t_int tid = -1;
         torrentInfo* ti = 0;

         if (!getIdFromHandle(th, tid, ti)) // else skip it - probably deleted before alert arrived
         {
            BTG_ERROR_LOG(logWrapper(), "Context::handleMetadataFailedAlert, torrent is not found.");
            return;
         }
         
         BTG_NOTICE(logWrapper(), "Failed download metadata for context " << tid << ".");
      }

      void Context::handleTorrentDeletedAlert(libtorrent::torrent_deleted_alert* _alert)
      {
         
      }

      void Context::handleMetadataReceivedAlert(libtorrent::metadata_received_alert* _alert)
      {
         libtorrent::torrent_handle th = _alert->handle;
         t_int tid = -1;
         torrentInfo* ti = 0;

         if (!getIdFromHandle(th, tid, ti)) // else skip it - probably deleted before alert arrived
         {
            BTG_ERROR_LOG(logWrapper(), "Context::handleMetadataReceivedAlert, torrent is not found.");
            return;
         }
         
         BTG_NOTICE(logWrapper(), "handleMetadataReceivedAlert, id=" << tid << ".");

         std::string torrent_filename   = ti->filename;
         libtorrent::torrent_info tinfo = ti->handle.get_torrent_info();

         // Save meta data for future use.
         libtorrent::entry t(libtorrent::entry::dictionary_t);
         boost::shared_array<char> meta = tinfo.metadata();
         libtorrent::entry meta_entry = libtorrent::bdecode(meta.get(), meta.get() + tinfo.metadata_size());
         std::vector<libtorrent::announce_entry> tracker = ti->handle.trackers();
         if (tracker.size())
            { 
               t["announce"]= tracker.front().url;
            }
         t["info"] = meta_entry;
         

         std::string fullFilename = tempDir_ + projectDefaults::sPATH_SEPARATOR() + ti->filename; // + ".frommagnet";
         boost::filesystem::ofstream outfile(fullFilename, std::ios_base::binary);

         if (!outfile)
            {
               BTG_NOTICE(logWrapper(), "handleMetadataReceivedAlert, id=" << tid << ", unable to write to file '" << fullFilename << "'.");
               return;
            }
         
         std::vector<char> encodedData;
         libtorrent::bencode(std::back_inserter(encodedData), t);
         
         //libtorrent::bencode(std::ostream_iterator<char>(outfile), t);

         BTG_NOTICE(logWrapper(), "handleMetadataReceivedAlert, data size = " << encodedData.size());

         outfile.write(&encodedData[0], encodedData.size());

         outfile.close();

         ti->got_torrent = true;

         BTG_NOTICE(logWrapper(), "handleMetadataReceivedAlert, id=" << tid << ", wrote to file '" << fullFilename << "'.");
      }

      void Context::handleStateChangeAlert(libtorrent::state_changed_alert* _alert)
      {
         libtorrent::torrent_handle th = _alert->handle;
         t_int tid = -1;
         torrentInfo *ti = NULL;

         if (!getIdFromHandle(th, tid, ti)) // else skip it - probably deleted before alert arrived
         {
            BTG_ERROR_LOG(logWrapper(), "Context::handleStateChangeAlert, torrent is not found.");
            return;
         }

         // Saving resume data
         switch (_alert->state)
         {
         case libtorrent::torrent_status::downloading:
         {
            writeResumeData(tid);
            break;
         }
         case libtorrent::torrent_status::finished:
         case libtorrent::torrent_status::seeding:
            {
               if (!moveToSeedingDir(tid))
                  {
                     VERBOSE_LOG(logWrapper(), verboseFlag_, "Unable to move files belonging to context " << tid << " to seed dir.");
                  }
               writeResumeData(tid);
               break;
            }
         default:
            {
               break;
            }
         }
      }

      void Context::handleAlert(libtorrent::alert* _alert)
      {
         if (!_alert)
            {
               // no more alerts in queue
               //break;
               return;
            }
            
         libtorrent::torrent_alert* alert = dynamic_cast<libtorrent::torrent_alert*>(_alert);
         // we aren't interested in alerts that doesn't contain a
         // torrent_handle.
         if (alert != 0 )
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
               else if (typeid(*alert) == typeid(libtorrent::tracker_error_alert))
                  {
                     handleTrackerAlert(dynamic_cast<libtorrent::tracker_error_alert*>(alert));
                  }
               else if (typeid(*alert) == typeid(libtorrent::tracker_reply_alert))
                  {
                     handleTrackerReplyAlert(dynamic_cast<libtorrent::tracker_reply_alert*>(alert));
                  }
               else if (typeid(*alert) == typeid(libtorrent::tracker_warning_alert))
                  {
                     handleTrackerWarningAlert(dynamic_cast<libtorrent::tracker_warning_alert*>(alert));
                  }
               else if (typeid(*alert) == typeid(libtorrent::save_resume_data_alert))
                  {
                     handleResumeDataAlert(dynamic_cast<libtorrent::save_resume_data_alert*>(alert));
                  }
               else if (typeid(*alert) == typeid(libtorrent::save_resume_data_failed_alert))
                  {
                     handleResumeDataFailedAlert(dynamic_cast<libtorrent::save_resume_data_failed_alert*>(alert));
                  }
               else if (typeid(*alert) == typeid(libtorrent::state_changed_alert))
                  {
                     handleStateChangeAlert(dynamic_cast<libtorrent::state_changed_alert*>(alert));
                  }
               else if (typeid(*alert) == typeid(libtorrent::metadata_received_alert))
                  {
                     handleMetadataReceivedAlert(dynamic_cast<libtorrent::metadata_received_alert*>(alert));
                  }
               else if (typeid(*alert) == typeid(libtorrent::metadata_failed_alert))
                  {
                     handleMetadataFailedAlert(dynamic_cast<libtorrent::metadata_failed_alert*>(alert));
                  }
               else if (typeid(*alert) == typeid(libtorrent::torrent_deleted_alert))
                  {
                     handleTorrentDeletedAlert(dynamic_cast<libtorrent::torrent_deleted_alert*>(alert));
                  }
               else
                  {
                     // Log other alerts.
                     BTG_NOTICE(logWrapper(), "Alert: " << 
                                alert->message()
                                << " (" << typeid(*alert).name() << ")");
                  }
            }
      }
      
      void Context::handleSavedAlerts()
      {
         std::vector<libtorrent::torrent_alert*> saved_alerts;
         {
            boost::mutex::scoped_lock interface_lock(interfaceMutex_);

            // Handle all stored alerts.
            saved_alerts = saved_alerts_;
            saved_alerts_.clear();
         }

         std::vector<libtorrent::torrent_alert*>::iterator iter;
         for (iter = saved_alerts.begin();
              iter != saved_alerts.end();
              iter++)
            {
               libtorrent::alert* a = *iter;
               {
                  boost::mutex::scoped_lock interface_lock(interfaceMutex_);
                  handleAlert(a); 
               }
               delete a;
            }
         saved_alerts.clear();
      }

      void Context::handleAlerts()
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         // Handle this amount of alerts at one time.
         t_uint alertCounterMax = 255;
         t_uint alertCounter    = 0;

         // Handle all available alerts.
         for(;;)
         {
            libtorrent::time_duration td = libtorrent::milliseconds(100);
            if (torrent_session->wait_for_alert(td) == 0)
               {
                  // No alerts to pop.
                  break;
               }
            
            // Handle a single alert.
            std::auto_ptr<libtorrent::alert> sp_alert = torrent_session->pop_alert();
            libtorrent::alert* raw_alert = sp_alert.get();
            
            handleAlert(raw_alert);

            alertCounter++;

            if (alertCounter > alertCounterMax)
            {
               VERBOSE_LOG(logWrapper(), verboseFlag_, "Stopped handling alerts, max (" << alertCounterMax << ") reached.");
               break;
            }
         }
      }

   } // namespace daemon
} // namespace btg

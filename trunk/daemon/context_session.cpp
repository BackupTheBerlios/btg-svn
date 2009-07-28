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

#if BTG_OPTION_SAVESESSIONS
#  include <sstream>
#  include <bcore/t_string.h>
#endif // BTG_OPTION_SAVESESSIONS

#include "modulelog.h"
#include <bcore/verbose.h>
#include "lt_version.h"

namespace btg
{
   namespace daemon
   {
      using namespace btg::core;

      const std::string moduleName("ctn");

#if BTG_OPTION_SAVESESSIONS
      bool Context::serialize(btg::core::externalization::Externalization* _e, 
                              bool const _dumpFastResume)
      {
         // Dump timestamp, used so we can relate finished_timestamp
         // to something.
         std::string isoTimestamp;
         isoTimestamp = boost::posix_time::to_iso_string(boost::posix_time::ptime(boost::posix_time::second_clock::universal_time()));
         _e->stringToBytes(&isoTimestamp);

         t_int count = torrent_ids.size();
         _e->intToBytes(&count);
         t_intListCI iter;

         if(_dumpFastResume)
            {
               writeResumeData();
            }

         for (iter = torrent_ids.begin();
              iter != torrent_ids.end();
              iter++)
            {
               t_int id = *iter;
               torrentInfo* ti = torrents[id];
               libtorrent::torrent_status status = ti->handle.status();

               // Calculate new summed stats. For torrents initiated since we started, all tc fields will be 0.
               // For torrents auto-initiated from earlier state file, the counters written will equal to the last time
               // btgdaemon quit plus the number of bytes we've transfered since we started.
               // Note that we should never modify torrent_counter here, since this function might be called
               // multiple times (regular saves to "prevent" crashes etc).
               t_ulong total_download         = ti->total_download + status.total_download;
               t_ulong total_upload           = ti->total_upload + status.total_upload;
               t_ulong total_payload_download = ti->total_payload_download + status.total_payload_download;
               t_ulong total_payload_upload   = ti->total_payload_upload + status.total_payload_upload;

               _e->stringToBytes(&(ti->filename));
               _e->boolToBytes(status.paused);
               _e->uLongToBytes(&total_download);
               _e->uLongToBytes(&total_upload);
               _e->uLongToBytes(&total_payload_download);
               _e->uLongToBytes(&total_payload_upload);

               _e->intToBytes(&(ti->limitUpld));
               _e->intToBytes(&(ti->limitDwnld));
               _e->intToBytes(&(ti->seedLimit));
               _e->longToBytes(&(ti->seedTimeout));

               _e->boolToBytes(ti->seedingAutoStopped);

               // Download counter.
               _e->uLongToBytes(&(ti->download_counter));

               // Seed counter.
               _e->uLongToBytes(&(ti->seed_counter));

               // Finished timestamp
               isoTimestamp = boost::posix_time::to_iso_string(ti->finished_timestamp);
               _e->stringToBytes(&isoTimestamp);

               // Add .torrent file
               btg::core::sBuffer sbuf;
               std::string fullFilename;
               fullFilename = tempDir_ + projectDefaults::sPATH_SEPARATOR() + ti->filename;
               if (!sbuf.read(fullFilename))
                  {
                     BTG_MNOTICE(logWrapper(), "failed to read .torrent file from '" << fullFilename << "' (serializing)");
                     return false;
                  }

               sbuf.serialize(_e);

               // Add .fast file
               fullFilename = fullFilename + fastResumeFileNameEnd;
               if (!sbuf.read(fullFilename))
                  {
                     BTG_MNOTICE(logWrapper(), "failed to read .fast file from '" << fullFilename << "' (serializing)");
                  }

               sbuf.serialize(_e);

               // Save selected files.
               ti->selected_files.serialize(_e);

               // Peer ID.
               libtorrent::peer_id pid;
#if BTG_LT_0_12
               pid = 0;
#elif (BTG_LT_0_13 || BTG_LT_0_14)
               pid = torrent_session->id();
#endif
               std::string peerid = btg::core::convertToString<libtorrent::peer_id>(pid);

               MVERBOSE_LOG(logWrapper(), verboseFlag_, "Peer ID used: '" << peerid << "'.");
               _e->stringToBytes(&peerid);
            }

         // Global settings.
         // Save DHT status.
         _e->boolToBytes(useDHT_);
         
         if (useDHT_)
            {
               std::vector<t_byte> buf;
               libtorrent::bencode(std::back_inserter(buf), torrent_session->dht_state());
               _e->addBytes(&buf[0], buf.size());
            }

         // Save encryption status.
         _e->boolToBytes(useEncryption_);
         
         return true;
      }

#define BTG_CDC(i, m) { if ((i)) { BTG_ERROR_LOG(logWrapper(), "Context::deserialize(), failed to deserialize context, " << m); return false; } }

      bool Context::deserialize(btg::core::externalization::Externalization* _e, t_uint _version)
      {
         BTG_MENTER(logWrapper(), "deserialize", "version = " << std::hex << _version << std::dec);
         bool status = true;
         std::string filename;
         bool paused;
         t_ulong total_download;
         t_ulong total_upload;
         t_ulong total_payload_download;
         t_ulong total_payload_upload;
         t_int limit_upload;
         t_int limit_download;
         t_int limit_seedLimit;
         t_long limit_seedTimeout;
         bool seedingAutoStopped;
         t_ulong download_counter;
         t_ulong seed_counter;

         // Fetch timestamp from dump.
         //
         // We use this timestamp to calculate a time_diff, how long
         // between the dump of the file (when the daemon exited), and
         // the current time. We can then add this to the
         // finish_timestamp we get from each finished torrent, so we
         // can calculate when to REALLY stop the torrent (if its
         // seeding).
         std::string isoTimestamp;
         BTG_CDC(!_e->bytesToString(&isoTimestamp), "dump timestamp");
         boost::posix_time::time_duration time_diff;
         try
            {
               boost::posix_time::ptime dumped_timestamp = boost::posix_time::from_iso_string(isoTimestamp);
               time_diff = boost::posix_time::ptime(boost::posix_time::second_clock::universal_time()) - dumped_timestamp;
            }
         catch(...)
            {
               BTG_MNOTICE(logWrapper(), 
                           "dumped timestamp is not a valid timestamp: '" <<
                           isoTimestamp << "'");
               return false;
            }

         t_int count = 0;
         BTG_CDC(!_e->bytesToInt(&count), "number of sessions");
         BTG_MNOTICE(logWrapper(), "deserialize(), Found " << count << " contexts in session");
         for (t_int x = 0; x < count; x++)
            {
               BTG_MNOTICE(logWrapper(), "deserizlize() context no " << x);
               BTG_CDC(!_e->bytesToString(&filename), "filename");
               BTG_CDC(!_e->bytesToBool(paused), "paused flag");
               BTG_CDC(!_e->bytesToULong(&total_download), "total download");
               BTG_CDC(!_e->bytesToULong(&total_upload), "total upload");
               BTG_CDC(!_e->bytesToULong(&total_payload_download), "total payload download");
               BTG_CDC(!_e->bytesToULong(&total_payload_upload), "total payload upload");
               BTG_CDC(!_e->bytesToInt(&limit_upload), "upload limit");
               BTG_CDC(!_e->bytesToInt(&limit_download), "download limit");
               BTG_CDC(!_e->bytesToInt(&limit_seedLimit), "seed limit");
               BTG_CDC(!_e->bytesToLong(&limit_seedTimeout), "seed timeout");
               BTG_CDC(!_e->bytesToBool(seedingAutoStopped), "seeding autostopped");

               BTG_CDC(!_e->bytesToULong(&download_counter), "download counter");

               BTG_CDC(!_e->bytesToULong(&seed_counter), "seed counter");

               // Finished timestamp
               isoTimestamp.clear();
               BTG_CDC(!_e->bytesToString(&isoTimestamp), "finished timestamp");

               // Deserialize .torrent file
               btg::core::sBuffer sbuf;
               BTG_CDC(!sbuf.deserialize(_e), "torrent file");

               std::string fullFilename;
               fullFilename = tempDir_ + projectDefaults::sPATH_SEPARATOR() + filename;
               if (!sbuf.write(fullFilename))
                  {
                     BTG_MNOTICE(logWrapper(), "failed to write .torrent file '" << fullFilename << "' after deserialization");
                     return false;
                  }

               // Deserialize .fast file
               BTG_CDC(!sbuf.deserialize(_e), "fast file");
               fullFilename = tempDir_ + projectDefaults::sPATH_SEPARATOR() + filename + fastResumeFileNameEnd;
               if (!sbuf.write(fullFilename))
                  {
                     BTG_MNOTICE(logWrapper(), "failed to write .fast file '" << fullFilename << "' after deserialization");
                     return false;
                  }

					if (_version >= 0x97 && _version < 0x9a)
					{
						// Deserialize DHT flag and list of nodes.
						BTG_MNOTICE(logWrapper(), "deserialize, reading DHT information");

						BTG_CDC(!_e->bytesToBool(useDHT_), "DHT flag");

						if (useDHT_)
							{

								BTG_MNOTICE(logWrapper(), "deserialize, session is using DHT");

								t_uint dhtStatusLength;
								t_byteP buf;
								BTG_CDC(!_e->getBytes(&buf,dhtStatusLength), "DHT status");
								
								libtorrent::entry dhtState = libtorrent::bdecode(buf, buf+dhtStatusLength);
						
								delete buf;
								enableDHT(dhtState);
							}
					}

					if (_version >= 0x98 && _version < 0x9a)
					{
						// Encryption:
						BTG_CDC(!_e->bytesToBool(useEncryption_), "encryption flag");
               }

               selectedFileEntryList aSelectedFileEntryList;
               if (_version >= 0x98)
                  {
                     // Selected files.
                     BTG_CDC(!aSelectedFileEntryList.deserialize(_e), "list of selected files");
                  }

               if (_version >= 0x99)
                  {
                     // Peer ID.
                     std::string peerid;

                     BTG_CDC(!_e->bytesToString(&peerid), "Peer ID");

                     MVERBOSE_LOG(logWrapper(), verboseFlag_, "Using peer ID: '" << peerid << "'.");

                     libtorrent::peer_id pid = btg::core::convertStringTo<libtorrent::peer_id>(peerid);
                     torrent_session->set_peer_id(pid);
                  }

               BTG_MNOTICE(logWrapper(), "deserialize() adding torrent " << filename);
               // If we got this far we're OK.
               t_int handle_id;
               if (add(filename, handle_id) == Context::ERR_OK)
                  {
                     // Get ID, add just added 1 to global_counter
                     t_uint handle_id = global_counter - 1;

                     // Set counter
                     torrentInfo *ti            = torrents[handle_id];
                     ti->total_download         = total_download;
                     ti->total_upload           = total_upload;
                     ti->total_payload_download = total_payload_download;
                     ti->total_payload_upload   = total_payload_upload;
							if (_version >= 0x89)
							{
                        bool asfr = applySelectedFiles(ti,aSelectedFileEntryList);
#if (BTG_LT_0_13 || BTG_LT_0_14)
								BTG_CDC(!asfr, "call applySelectedFiles");
#elif BTG_LT_0_12
                        // Selecting files is not supported by 0.12.x, so ignore this.
#endif
							}

                     // Restore state
                     if (paused)
                        {
                           stop(handle_id);
                        }

                     // Restore limits
                     limit(handle_id, limit_upload, limit_download, limit_seedLimit, limit_seedTimeout);
                     ti->seedingAutoStopped = seedingAutoStopped;

                     ti->download_counter = download_counter;
                     ti->seed_counter     = seed_counter;

                     try
                        {
                           ti->finished_timestamp = boost::posix_time::from_iso_string(isoTimestamp) + time_diff;
                        }
                     catch(...)
                        {
                           // Most likely a not-a-date-time-string, in
                           // other words the torrent is not finished
                           // yet.
                        }
                  } // if add()
               else
                  {
                     BTG_ERROR_LOG(logWrapper(), "Falied to add " << filename << ".");
                  }
            } // for

         if (_version >= 0x9a)
            {
               // Per session settings.

               // DHT flag and list of nodes.
               BTG_MNOTICE(logWrapper(), "deserialize, reading DHT information");
               
               BTG_CDC(!_e->bytesToBool(useDHT_), "DHT flag");
               
               if (useDHT_)
                  {
                     
                     BTG_MNOTICE(logWrapper(), "deserialize, session is using DHT");
                     
                     t_uint dhtStatusLength;
                     t_byteP buf;
                     BTG_CDC(!_e->getBytes(&buf,dhtStatusLength), "DHT status");
							
                     libtorrent::entry dhtState = libtorrent::bdecode(buf, buf+dhtStatusLength);
                     
                     delete buf;
                     enableDHT(dhtState);
                  }
               else
                  {
                     BTG_MNOTICE(logWrapper(), "deserialize, session is not using DHT");
                  }

               // Encryption.
               BTG_CDC(!_e->bytesToBool(useEncryption_), "encryption flag");
               if (useEncryption_)
                  {
                     BTG_MNOTICE(logWrapper(), "deserialize, session is using encryption");
                     enableEncryption();
                  }
               else
                  {
                     BTG_MNOTICE(logWrapper(), "deserialize, session is not using encryption");
                  }
            }
         BTG_MEXIT(logWrapper(), "deserialize", status);
         return status;
      }
#endif // BTG_OPTION_SAVESESSIONS

   } // namespace daemon
} // namespace btg

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

#include <libtorrent/identify_client.hpp>

#include <bcore/command/limit_base.h>
#include <bcore/util.h>
#include <bcore/sbuf.h>
#include <bcore/trackerstatus.h>
#include <bcore/logmacro.h>
#include <bcore/os/fileop.h>

#include "context.h"
#include "filetrack.h"
#include "ipfilter/ipfilterif.h"

#include <fstream>
#include <ostream>
#include <iostream>
#if BTG_OPTION_SAVESESSIONS
#  include <sstream>
#endif // BTG_OPTION_SAVESESSIONS

#if BTG_OPTION_EVENTCALLBACK
#  include "callbackmgr.h"
#endif // BTG_OPTION_EVENTCALLBACK

#include "modulelog.h"
#include <bcore/verbose.h>

#include "portmgr.h"
#include "limitmgr.h"

#include "dconfig.h"

#include <bcore/t_string.h>
#include <bcore/btg_assert.h>

#include "lt_version.h"

#if (BTG_LT_0_13 || BTG_LT_0_14)
#include <libtorrent/extensions/metadata_transfer.hpp>
#include <libtorrent/extensions/ut_pex.hpp>
#endif

namespace btg
{
   namespace daemon
   {
      using namespace btg::core;

      const std::string moduleName("ctn");

      torrentInfo::torrentInfo():
         handle(),
         filename(""),
         limitUpld(btg::core::limitBase::LIMIT_DISABLED),
         limitDwnld(btg::core::limitBase::LIMIT_DISABLED),
         seedLimit(btg::core::limitBase::LIMIT_DISABLED),
         seedTimeout(btg::core::limitBase::LIMIT_DISABLED),
         download_counter(0),
         seed_counter(0),
         finished_timestamp(),
         seedingAutoStopped(false),
         serial(0),
         trackerStatus(),
         selected_files()
#if BTG_OPTION_SAVESESSIONS
         , total_download(0),
         total_upload(0),
         total_payload_download(0),
         total_payload_upload(0)
#endif // BTG_OPTION_SAVESESSIONS
      {  
      }

      bool Context::set_boost_native_paths = true;

      Context::Context(btg::core::LogWrapperType _logwrapper,
                       const daemonConfiguration* _config,
                       bool const _verboseFlag,
                       std::string const& _username,
                       std::string const& _tempDir,
                       std::string const& _workDir,
                       std::string const& _seedDir,
                       std::string const& _outputDir,
                       bool _interface_used,
                       std::string const& _interface,
                       portManager* _portMgr,
                       limitManager* _limitMgr,
                       fileTrack* _filetrack,
                       IpFilterIf* _filter,
                       bool const _useTorrentName,
#if BTG_OPTION_EVENTCALLBACK
                       callbackManager* _cbm,
#endif // BTG_OPTION_EVENTCALLBACK
                       bool _clientAttached
                       )
         : btg::core::Logable(_logwrapper),
           verboseFlag_(_verboseFlag),
           config_(_config),
           username_(_username),
           tempDir_(_tempDir),
           workDir_(_workDir),
           seedDir_(_seedDir),
           outputDir_(_outputDir),
           interface_used_(_interface_used),
           interface_(_interface),
           portMgr(_portMgr),
           limitMgr_(_limitMgr),
           configured(false),
           default_torrent_uploadLimit_(btg::core::limitBase::LIMIT_DISABLED),
           default_torrent_downloadLimit_(btg::core::limitBase::LIMIT_DISABLED),
           default_torrent_seedLimit_(btg::core::limitBase::LIMIT_DISABLED),
           default_torrent_seedTimeout_(btg::core::limitBase::LIMIT_DISABLED),
           listen_port_range_(0,0),
           useDHT_(false),
           enabledDHT_(false),
           DHTport_(0),
           useEncryption_(false),
           torrent_session(0),
           // Identify this client to its peers.
           global_counter(0),
           torrent_ids(0),
           torrents(),
           clientAttached(_clientAttached),
           fastResumeFileNameEnd(".fast"),
           session_settings_(),
           filetrack_(_filetrack),
           filter_(_filter)
#if BTG_OPTION_EVENTCALLBACK
         , cbm_(_cbm)
#endif // BTG_OPTION_EVENTCALLBACK
         , progress_(),
           useTorrentName_(_useTorrentName)
      {
         BTG_MNOTICE(logWrapper(),
                     "using libtorrent version: " << LIBTORRENT_VERSION);

         BTG_MENTER(logWrapper(),
                    "Constructor", "_clientAttached = " << _clientAttached);

         portMgr->get(listen_port_range_);

         bool lt_context_created = true;

         try
            {
               torrent_session = new libtorrent::session(
                                                         libtorrent::fingerprint(
                                                                                 "BG",
                                                                                 projectDefaults::iMAJORVERSION(),
                                                                                 projectDefaults::iMINORVERSION(),
                                                                                 projectDefaults::iREVISIONVERSION(),
                                                                                 0
                                                                                 )
                                                         );
            }
         catch (boost::thread_resource_error & _error)
            {
               lt_context_created = false;
            }

         if (!lt_context_created)
            {
               // Libtorrent was not initialized.
               // Attempt to clean up.

               delete torrent_session;
               torrent_session = 0;
            }
         else
            {
               // Libtorrent created its threads.
               //
#if (BTG_LT_0_13 || BTG_LT_0_14)
               // Add the available extensions.
               torrent_session->add_extension(&libtorrent::create_ut_pex_plugin);
#endif

               if (filter_)
                  {
                     filter_->set(*torrent_session);
                  }

               // To enable system specific paths.
               // But only once.
               if (Context::set_boost_native_paths)
                  {
                     try
                        {
                           boost::filesystem::path::default_name_check(boost::filesystem::native);
                           Context::set_boost_native_paths = false;
                        }
                     catch (std::exception& e)
                        {
                           Context::set_boost_native_paths = true;
                           BTG_ERROR_LOG(logWrapper(), "libtorrent exception: " << e.what() );
                        }
                  }

               this->setNormalHttpSettings();

               // From the libtorrent manual.
               // Events that can be considered normal, but still deserves
               // an event. This could be a piece hash that fails.
               torrent_session->set_severity_level(libtorrent::alert::info);

               setPeerIdFromConfig();
            }
      }

      void Context::setPeerIdFromConfig()
      {
         // Read a Peer ID from config.
         // Convert it to a string in which each character is a hex pair.
         // This is a representation which is used by libtorrent.
         // Pad with zeros or truncate the string if its more than 20 characters.
         const t_uint minPeerIdSize = 4;
         const t_uint maxPeerIdSize = 20;

         std::string peerid = config_->getPeerId();

         if (peerid.size() == 0)
            {
               return;
            }

         if (peerid.size() < minPeerIdSize)
            {
               MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                            "Peer ID: '" << peerid << "' must be at least " << 
                            minPeerIdSize << " characters.");
               return;
            }

         if (peerid.size() > maxPeerIdSize)
            {
               peerid = peerid.substr(0, maxPeerIdSize);
            }

         std::ostringstream oss;
         std::string::const_iterator iter;
         t_uint i = 0;
         for (iter = peerid.begin();
              iter != peerid.end();
              iter++)
            {
               i = *iter;
               oss << std::hex << std::setw(2) << std::setfill('0') << i;
            }

         if (peerid.size() < maxPeerIdSize)
            {
               i = 0;
               for (t_uint counter = 0;
                    counter < (maxPeerIdSize - peerid.size());
                    counter++)
                  {
                     oss << std::hex << std::setw(2) << std::setfill('0') << i;
                  }
            }
         
         oss << std::dec << std::setfill(' ');

         libtorrent::peer_id pid = btg::core::convertStringTo<libtorrent::peer_id>(oss.str());

         MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                      "Overriding libtorrent peer ID with: '" << peerid << "'.");
         torrent_session->set_peer_id(pid);
      }

      t_intList Context::getContexts() const
      {
         t_intList i;

         // Return an empty vector if this context is not configured.
         if (!configured)
            {
               return i;
            }

         i = torrent_ids;

         return i;
      }

      bool Context::getFilename(t_int const _torrent_id, std::string& _destination)
      {
         if (!configured)
            {
               return false;
            }

         torrentInfo *ti;
         if ((ti = getTorrentInfo(_torrent_id)) == 0)
            {
               return false;
            }

         _destination = ti->filename;
         return true;
      }

      bool Context::getFile(t_int const _torrent_id,
                            std::string & _filename, 
                            btg::core::sBuffer & _buffer) const
      {
         torrentInfo *ti;
         if ((ti = getTorrentInfo(_torrent_id)) == 0)
            {
               return false;
            }

         std::string fullFilename = tempDir_ + projectDefaults::sPATH_SEPARATOR() + ti->filename;
         if (!_buffer.read(fullFilename))
            {
               BTG_MNOTICE(logWrapper(), 
                           "failed to read .torrent file from '" << fullFilename << "'");
               return false;
            }

         _filename = ti->filename;
         return true;
      }

      bool Context::isConfigured() const
      {
         return configured;
      }

      bool Context::setup(btg::core::requiredSetupData const& _rsd)
      {
         BTG_MENTER(logWrapper(), "setup", "workDir=" << workDir_ << ", outputDir=" << outputDir_);

         if (torrent_session == 0)
            {
               // The constructor catched an exception thrown by the
               // boost thread class used by libtorrent.
               configured = false;

               return false;
            }

         default_torrent_uploadLimit_   = btg::core::limitBase::LIMIT_DISABLED;
         default_torrent_downloadLimit_ = btg::core::limitBase::LIMIT_DISABLED;
         default_torrent_seedLimit_     = _rsd.getSeedLimit();
         default_torrent_seedTimeout_   = _rsd.getSeedTimeout();
         useDHT_                        = _rsd.useDHT();
         useEncryption_                 = _rsd.useEncryption();
         configured                     = true;

         try
            {
               boost::filesystem::create_directory(boost::filesystem::path(workDir_));
            }
         catch (std::exception& e)
            {
               BTG_ERROR_LOG(logWrapper(), ": context setup failed to create '" << workDir_ << "':" << e.what() );
               configured = false;
            }

         if (!configured)
            {
               return configured;
            }

         try
            {
               boost::filesystem::create_directory(boost::filesystem::path(seedDir_));
            }
         catch (std::exception& e)
            {
               BTG_ERROR_LOG(logWrapper(), ": context setup failed to create '" << seedDir_ << "':" << e.what() );
               configured = false;
            }

         if (!configured)
            {
               return configured;
            }

         try
            {
               boost::filesystem::create_directory(boost::filesystem::path(outputDir_));
            }
         catch (std::exception& e)
            {
               BTG_ERROR_LOG(logWrapper(), ": context setup failed to create '" << outputDir_ << "':" << e.what() );
               configured = false;
            }

         if (!configured)
            {
               return configured;
            }

         if (configured)
            {
               try
                  {
                     if (interface_used_)
                        {
                           VERBOSE_LOG(logWrapper(), verboseFlag_, "Libtorrent binding to interface '" << interface_ << "'.");
                           torrent_session->listen_on(listen_port_range_, interface_.c_str());
                        }
                     else
                        {
                           torrent_session->listen_on(listen_port_range_);
                        }

                     // Just start DHT, if enabled for this session.
                     //
                     // If any nodes were saved using session saving,
                     // they will be added
                     enableDHT();
                  }
               catch (std::exception& e)
                  {
                     BTG_ERROR_LOG(logWrapper(), "libtorrent exception: " << e.what() );
                     configured = false;
                  }
            }

#if (BTG_LT_0_13 || BTG_LT_0_14)
         // Enable encryption.
         if (useEncryption_)
            {
               enableEncryption();
            }
#endif // BTG_LT_0_13

         // At this point a libtorrent session was created, and it
         // needs to be limited.
         //
         limitMgr_->add(torrent_session);

         BTG_MEXIT(logWrapper(), "setup", configured);
         return configured;
      }

      Context::addResult Context::add(std::string const& _torrent_filename,
                                      t_int & _handle_id)
      {
         BTG_MENTER(logWrapper(), "add", "_torrent_filename=" << _torrent_filename);

         if (!configured)
            {
               return Context::ERR_UNDEFINED;
            }

         Context::addResult status = Context::ERR_UNDEFINED;

         // Filename used as key for the file tracker.
         // tempDir_ + projectDefaults::sPATH_SEPARATOR() + 
         std::string fileTrackFilename = _torrent_filename;

         // Add torrent name to filetracker.
         if (!filetrack_->add(tempDir_, fileTrackFilename))
            {
               status = Context::ERR_EXISTS;
               BTG_MNOTICE(logWrapper(),
                           "add, file '" << fileTrackFilename << 
                           "' already exists in filetrack");
               BTG_MEXIT(logWrapper(), "add", status);
               return status;
            }

         libtorrent::entry torrent_entry;

         // Prepend tempDir_ to filename.
         std::string targetPath = tempDir_ + 
            projectDefaults::sPATH_SEPARATOR() + 
            _torrent_filename;

#if BTG_LT_0_12 || BTG_LT_0_13
         // Read the file:
         try
            {
               std::ifstream in(targetPath.c_str(), std::ios_base::binary);
               in.unsetf(std::ios_base::skipws);
               torrent_entry = libtorrent::bdecode(
                                                   std::istream_iterator<char>(in),
                                                   std::istream_iterator<char>()
                                                   );
               status = Context::ERR_OK;
            }
         catch (std::exception& e)
            {
               
               status = Context::ERR_LIBTORRENT;
            }
         if (status != Context::ERR_OK)
            {
               // remove it from filetrack
               filetrack_->remove(tempDir_, fileTrackFilename);
               BTG_MEXIT(logWrapper(), "add", status);
               return status;
            }
#elif BTG_LT_0_14
         status = Context::ERR_OK;
#endif

#if BTG_LT_0_12
         libtorrent::torrent_info tinfo;
         if (!entryToInfo(torrent_entry, tinfo))
#elif BTG_LT_0_13
         boost::intrusive_ptr<libtorrent::torrent_info> tinfo(new libtorrent::torrent_info);
         if (!entryToInfo(torrent_entry, *tinfo))
#elif BTG_LT_0_14
         boost::intrusive_ptr<libtorrent::torrent_info> tinfo;
         bool gotInfo = true;
         try
            {
               boost::filesystem::path targetFilename(targetPath);
               BTG_MNOTICE(logWrapper(), "Attempt to get info from '" << targetPath << "'");
               tinfo.reset(new libtorrent::torrent_info(targetFilename));
            }
         catch (std::exception& e)
            {
               BTG_MNOTICE(logWrapper(), "libtorrent exception (torrent_info constructor): " << e.what());
               gotInfo = false;
            }
         
         if (!gotInfo)
#endif
            {
               // Unable to convert the entry to a torrent info.
               // Adding files cannot continue.
               
               filetrack_->remove(tempDir_, fileTrackFilename);
               status = Context::ERR_LIBTORRENT;
               BTG_MEXIT(logWrapper(), "add", status);
               return status;
            }

         std::vector<std::string> contained_files;
#if BTG_LT_0_12 || BTG_LT_0_13
         if (!entryToFiles(torrent_entry, contained_files))
            {
               filetrack_->remove(tempDir_, fileTrackFilename);
               status = Context::ERR_LIBTORRENT;
               BTG_MEXIT(logWrapper(), "add", status);
               return status;
            }
#elif BTG_LT_0_14
         if (!torrentInfoToFiles(*tinfo, contained_files))
            {
               filetrack_->remove(tempDir_, fileTrackFilename);
               status = Context::ERR_LIBTORRENT;
               BTG_MEXIT(logWrapper(), "add", status);
               return status;
            }
#endif
         btg_assert(contained_files.size() >= 1, 
                    logWrapper(),
                    "entryToFiles must return at least one file");

         // A torrent was read without failing, now add the files to
         // the file tracker.
         if (!filetrack_->setFiles(tempDir_, fileTrackFilename, contained_files))
            {
               // The torrent file is unique, but some of the contents
               // is not.
               BTG_MNOTICE(logWrapper(), "add, file '" << fileTrackFilename << "': file collision");
               filetrack_->remove(tempDir_, fileTrackFilename);
               status = Context::ERR_EXISTS;
               BTG_MEXIT(logWrapper(), "add", status);
               return status;
            }

         boost::filesystem::path dataPath;

         // Check the torrent file against the seed dir. Attempt to
         // find out if the data resides in the seed directory.
         torrentStorage ts = tsWork;
#if BTG_LT_0_12
         if (dataPresentInSeedDir(tinfo))
#elif (BTG_LT_0_13 || BTG_LT_0_14)
         if (dataPresentInSeedDir(*tinfo))
#endif
            {
               dataPath =  boost::filesystem::path(seedDir_, boost::filesystem::native);
               ts = tsSeed;
            }
         else
            {
               dataPath = boost::filesystem::path(workDir_, boost::filesystem::native);
               ts = tsWork;
            }

         libtorrent::torrent_handle handle;
         try
            {
               if (this->hasFastResumeData(_torrent_filename))
                  {
                     // Get fast resume data.
                     std::string fastResumeFileName = tempDir_ + projectDefaults::sPATH_SEPARATOR() + _torrent_filename + fastResumeFileNameEnd;
                     std::ifstream in(fastResumeFileName.c_str(), std::ios_base::binary);
                     in.unsetf(std::ios_base::skipws);
#if (BTG_LT_0_12 || BTG_LT_0_13)
                     libtorrent::entry fastResumeEntry;
#elif BTG_LT_0_14
                     std::vector<char>* resumeData       = new std::vector<char>;
#endif
                     try
                        {
#if (BTG_LT_0_12 || BTG_LT_0_13)
                           fastResumeEntry = libtorrent::bdecode(
                                                                 std::istream_iterator<char>(in),
                                                                 std::istream_iterator<char>()
                                                                 );
#elif BTG_LT_0_14
                           std::copy(std::istream_iterator<char>(in), 
                                     std::istream_iterator<char>(),
                                     std::back_inserter(*resumeData));
#endif
                        }
                     catch (std::exception& e)
                        {
                           BTG_MNOTICE(logWrapper(), "libtorrent exception: " << e.what());
                           status = Context::ERR_LIBTORRENT;
                        }

                     if (status == Context::ERR_OK)
                        {
                           BTG_MNOTICE(logWrapper(), "using fast resume for '" << _torrent_filename << "'");
#if BTG_LT_0_12
                           handle = torrent_session->add_torrent(torrent_entry, dataPath, fastResumeEntry);         
#elif BTG_LT_0_13
                           handle = torrent_session->add_torrent(tinfo, dataPath, fastResumeEntry);
#elif BTG_LT_0_14
                           libtorrent::add_torrent_params atp;
                           atp.name        = 0; // "default name";
                           atp.ti.swap(tinfo);
                           atp.save_path   = dataPath;
                           atp.resume_data = resumeData;

                           handle          = torrent_session->add_torrent(atp);
#endif

                        }
                  }
               else
                  {
#if BTG_LT_0_12
                     handle = torrent_session->add_torrent(torrent_entry, dataPath);
#elif BTG_LT_0_13
                     handle = torrent_session->add_torrent(tinfo, dataPath);
#elif BTG_LT_0_14
                     libtorrent::add_torrent_params atp;
                     atp.name      = 0; // "default name";
                     atp.ti.swap(tinfo);
                     atp.save_path = dataPath;

                     handle        = torrent_session->add_torrent(atp);
#endif
                  }
            }
         catch (std::exception& e)
            {
               BTG_ERROR_LOG(logWrapper(), "libtorrent exception: " << e.what() );
               status = Context::ERR_LIBTORRENT;
            }

         if (status == Context::ERR_OK)
            {
               t_int const handle_id = global_counter;
               global_counter++;

               torrentInfo *ti = new torrentInfo();
               ti->handle      = handle;
               ti->filename    = _torrent_filename;
               ti->limitUpld   = default_torrent_uploadLimit_;
               ti->limitDwnld  = default_torrent_downloadLimit_;
               ti->seedLimit   = default_torrent_seedLimit_;
               ti->seedTimeout = default_torrent_seedTimeout_;

               // Set the files which are present.
               // Initially all files are selected.
               libtorrent::torrent_info t_i = ti->handle.get_torrent_info();
               libtorrent::torrent_info::file_iterator file_iter;

               for (file_iter = t_i.begin_files();
                    file_iter != t_i.end_files();
                    file_iter++)
                  {
                     std::string filename  = file_iter->path.native_file_string();
                     ti->selected_files.add(filename, true);
                  }

               torrents[handle_id] = ti;
               torrent_ids.push_back(handle_id);
               torrent_storage[handle_id] = ts;

               // Updated handle ID in arguments.
               _handle_id          = handle_id;
#if BTG_OPTION_EVENTCALLBACK
               // Call event callback.
               std::vector<std::string> cbm_arguments;
               cbm_arguments.push_back(_torrent_filename);
               cbm_->event(username_,
                           callbackManager::CBM_ADD,
                           cbm_arguments);
#endif // BTG_OPTION_EVENTCALLBACK
            }
         else
            {
               // Failed to add the file? Remove it from tracker.
               filetrack_->remove(tempDir_, fileTrackFilename);
            }

         BTG_MEXIT(logWrapper(), "add", "");
         return status;
      }

      Context::addResult Context::add(std::string const& _torrent_filename, 
                                      btg::core::sBuffer const& _buffer,
                                      t_int & _handle_id)
      {
         BTG_MENTER(logWrapper(), "add", "_torrent_filename=" << _torrent_filename << ", size=" << _buffer.size());
         Context::addResult status = Context::ERR_UNDEFINED;

         // Check torrent does not exist before we overwrite it...
         std::string fileTrackFilename = _torrent_filename;
         if (filetrack_->exists(tempDir_, fileTrackFilename))
            {
               status = Context::ERR_EXISTS;
               BTG_MNOTICE(logWrapper(), "add, file '" << fileTrackFilename << "' already exists in filetrack");
               BTG_MEXIT(logWrapper(), "add", status);
               return status;
            }

         // Convert _buffer into a temporary file and call the other add.
         std::string targetPath = tempDir_ + projectDefaults::sPATH_SEPARATOR() + _torrent_filename;
         if (_buffer.write(targetPath))
            {
               status = add(_torrent_filename, _handle_id);
            }
         else
            {
               BTG_ERROR_LOG(logWrapper(), "Attempt to write '" << targetPath << "' failed");
            }

         BTG_MEXIT(logWrapper(), "add", status);
         return status;
      }

      bool Context::remove(t_int const _torrent_id,
                           bool const _eraseData
#if BTG_OPTION_EVENTCALLBACK
                           , bool _callCallback
#endif // BTG_OPTION_EVENTCALLBACK
                           )
      {
         BTG_MENTER(logWrapper(), "remove", "id = " << _torrent_id << ", _eraseData = " << _eraseData);

         if (!configured)
            {
               return false;
            }

         torrentInfo *ti;
         if ((ti = getTorrentInfo(_torrent_id)) == 0)
            {
               return false;
            }

         // Remove fast resume data, if any (is serialized before
         // remove() if we're quiting and sessionsaving is in use)
         this->removeFastResumeData(_torrent_id);

         // Remove .torrent file (this is also serialized if
         // sessionsaving is in use)
         this->removeTorrentFile(_torrent_id);

         // Remove this id from progress tracking.
         progress_.remove(_torrent_id);

         bool                     foundEntities = false;
         std::vector<std::string> entitiesToErase;
         // Find out what directories and files were created by this
         // torrent, so it can be erased, the requested by the user.
         if (_eraseData)
            {
               // The files which are to be erased can be either in
               // the work dir or in the seed dir.
               foundEntities = this->getListOfEntities(_torrent_id, workDir_, entitiesToErase);

               if (!foundEntities)
                  {
                     foundEntities = this->getListOfEntities(_torrent_id, seedDir_, entitiesToErase);
                  }

               BTG_MNOTICE(logWrapper(), "remove, foundEntities = " << foundEntities);
            }

         // Stop keeping track of this filename;
         std::string filename;

         if (getFilename(_torrent_id, filename))
            {
               std::string fileTrackFilename = filename;
               filetrack_->remove(tempDir_, fileTrackFilename);
            }

#if BTG_OPTION_EVENTCALLBACK
         if (_callCallback)
            {
               // Call event callback.
               std::vector<std::string> cbm_arguments;
               cbm_arguments.push_back(filename);
               cbm_->event(username_,
                           callbackManager::CBM_REM,
                           cbm_arguments);
            }
#endif // BTG_OPTION_EVENTCALLBACK

         libtorrent::torrent_handle handle = ti->handle;
         torrent_session->remove_torrent(handle);

         // Remove the information structure:
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

         if (_eraseData)
            {
               if (foundEntities)
                  {
                     BTG_MNOTICE(logWrapper(), "remove, removing data");
                     std::vector<std::string>::const_iterator iter;
                     for (iter = entitiesToErase.begin();
                          iter != entitiesToErase.end();
                          iter++)
                        {
                           BTG_MNOTICE(logWrapper(), "remove, erasing '" << *iter << "'");

                           if (!btg::core::os::fileOperation::remove(*iter))
                              {
                                 BTG_MNOTICE(logWrapper(), "remove, failed to erase '" << *iter << "'");
                              }
                        }
                  }
            }

         BTG_MEXIT(logWrapper(), "remove", true);
         return true;
      }

      bool Context::removeAll(bool const _eraseData)
      {
         BTG_MENTER(logWrapper(), "removeAll", "");

         bool result = true;
         t_intList contexts = this->getContexts();
         if (contexts.size() == 0)
            {
               result = false;
            }

         for (t_intListI i=contexts.begin(); i!= contexts.end(); i++)
            {
               if (!this->remove(*i, _eraseData))
                  {
                     result = false;
                  }
            }

         BTG_MEXIT(logWrapper(), "removeAll", result);
         return result;
      }

      bool Context::start(t_int const _torrent_id)
      {
         BTG_MENTER(logWrapper(), "start", "id = " << _torrent_id);

         torrentInfo *ti;
         if ((ti = getTorrentInfo(_torrent_id)) == 0)
            {
               return false;
            }

         // Allready running.
         if (!ti->handle.is_paused())
            {
               BTG_MNOTICE(logWrapper(), "start - already started");
               return true;
            }

         ti->handle.resume();

         BTG_MEXIT(logWrapper(), "start", true);

         return true;
      }

      bool Context::startAll()
      {
         BTG_MENTER(logWrapper(), "startAll", "");

         bool result = true;
         t_intList contexts = this->getContexts();
         if (contexts.size() == 0)
            {
               result = false;
            }

         for (t_intListI i=contexts.begin(); i!= contexts.end(); i++)
            {
               if (!this->start(*i))
                  {
                     result = false;
                  }
            }

         BTG_MEXIT(logWrapper(), "startAll", result);
         return result;
      }

      bool Context::stop(t_int const _torrent_id)
      {
         BTG_MENTER(logWrapper(), "stop", "id = " << _torrent_id);

         torrentInfo *ti;
         if ((ti = getTorrentInfo(_torrent_id)) == 0)
            {
               return false;
            }

         // if allready paused:
         if (ti->handle.is_paused())
            {
               return true;
            }

         // Not allready paused, so pause it.
         ti->handle.pause();

         BTG_MEXIT(logWrapper(), "stop", true);
         return true;
      }

      bool Context::stopAll()
      {
         BTG_MENTER(logWrapper(), "stopAll", "");

         bool result          = true;
         t_intList contexts = this->getContexts();
         if (contexts.size() == 0)
            {
               result = false;
            }

         for (t_intListI i=contexts.begin(); i!= contexts.end(); i++)
            {
               if (!this->stop(*i))
                  {
                     result = false;
                  }
            }

         BTG_MEXIT(logWrapper(), "stopAll", result);
         return result;
      }

      bool Context::limit(t_int const _torrent_id, 
                          t_int const _limitUpld, 
                          const t_int _limitDwnld, 
                          const t_int _seedLimit, 
                          const t_long _seedTimeout)
      {
         BTG_MENTER(logWrapper(), "limit", "id = " << _torrent_id << ", _limitUpld = " << 
                    _limitUpld << ", _limitDwnld = " << 
                    _limitDwnld << ", seedLimit = " << 
                    _seedLimit << ", seedTimeout = " << 
                    _seedTimeout);

         torrentInfo *ti;
         if ((ti = getTorrentInfo(_torrent_id)) == 0)
            {
               return false;
            }

         ti->handle.set_upload_limit(_limitUpld);
         ti->handle.set_download_limit(_limitDwnld);

         ti->limitUpld   = _limitUpld;
         ti->limitDwnld  = _limitDwnld;
         ti->seedLimit   = _seedLimit;
         ti->seedTimeout = _seedTimeout;

         BTG_MEXIT(logWrapper(), "limit", true);
         return true;
      }

      bool Context::limitAll(t_int const _limitUpld, 
                             const t_int _limitDwnld, 
                             const t_int _seedLimit, 
                             const t_long _seedTimeout)
      {
         BTG_MENTER(logWrapper(), "limitAll", "");

         bool result          = true;
         t_intList contexts   = this->getContexts();
         if (contexts.size() == 0)
            {
               result = false;
            }

         for (t_intListI i=contexts.begin(); i!= contexts.end(); i++)
            {
               if (!this->limit(*i, _limitUpld, _limitDwnld, _seedLimit, _seedTimeout))
                  {
                     result = false;
                  }
            }

         BTG_MEXIT(logWrapper(), "limitAll", result);
         return result;
      }

      bool Context::removeLimit(t_int const _torrent_id, 
                                bool const _upld, 
                                bool const _dwnld, 
                                bool const _seedLimit, 
                                bool const _seedTimeout)
      {
         BTG_MENTER(logWrapper(), "removeLimit", "id = " << _torrent_id << ", _upld = " << 
                    _upld<< ", _dwnld = " << _dwnld << ", seedLimit = " <<
                    _seedLimit << ", seedTimeout = " << _seedTimeout);

         torrentInfo *ti;
         if ((ti = getTorrentInfo(_torrent_id)) == 0)
            {
               return false;
            }

         if (_upld)
            {
               ti->handle.set_upload_limit(btg::core::limitBase::LIMIT_DISABLED);
               ti->limitUpld = btg::core::limitBase::LIMIT_DISABLED;
            }

         if (_dwnld)
            {
               ti->handle.set_download_limit(btg::core::limitBase::LIMIT_DISABLED);
               ti->limitDwnld = btg::core::limitBase::LIMIT_DISABLED;
            }

         if (_seedLimit)
            {
               ti->seedLimit= btg::core::limitBase::LIMIT_DISABLED;
            }

         if (_seedTimeout)
            {
               ti->seedTimeout = btg::core::limitBase::LIMIT_DISABLED;
            }

         BTG_MEXIT(logWrapper(), "removeLimit", true);
         return true;
      }

      bool Context::removeLimitAll(bool const _upld, 
                                   bool const _dwnld, 
                                   bool const _seedLimit, 
                                   bool const _seedTimeout)
      {
         BTG_MENTER(logWrapper(), "removeLimitAll", "");

         bool result          = true;
         t_intList contexts   = this->getContexts();

         if (contexts.size() == 0)
            {
               result = false;
            }

         for (t_intListI i=contexts.begin(); i!= contexts.end(); i++)
            {
               if (!this->removeLimit(*i, _upld, _dwnld, _seedLimit, _seedTimeout))
                  {
                     result = false;
                  }
            }

         BTG_MEXIT(logWrapper(), "removeLimitAll", result);
         return result;
      }

      bool Context::getLimit(t_int const _torrent_id, 
                             t_int & _limitUpld, 
                             t_int & _limitDwnld, 
                             t_int & _seedLimit, 
                             t_long & _seedTimeout)
      {
         BTG_MENTER(logWrapper(), "getLimit", "id = " << _torrent_id);

         torrentInfo *ti;
         if ((ti = getTorrentInfo(_torrent_id)) == 0)
            {
               return false;
            }

         _limitUpld   = ti->limitUpld;
         _limitDwnld  = ti->limitDwnld;
         _seedLimit   = ti->seedLimit;
         _seedTimeout = ti->seedTimeout;

         BTG_MENTER(logWrapper(), "getLimit", "_limitUpld = " << 
                    _limitUpld << ", _limitDwnld = " << 
                    _limitDwnld << ", _seedLimit = " << 
                    _seedLimit << ", _seedTimeout = " << 
                    _seedTimeout);
         return true;
      }

      bool Context::getStatus(t_int const _torrent_id, 
                              Status & _destination)
      {
         BTG_MENTER(logWrapper(), "getStatus", "");

         torrentInfo *ti;
         if ((ti = getTorrentInfo(_torrent_id)) == 0)
            {
               return false;
            }

         // Get the status from libtorrent:
         libtorrent::torrent_status status = ti->handle.status();

         std::string filename;
         if (useTorrentName_)
            {
               // Use torrent name.
               filename = ti->handle.get_torrent_info().name();
            }
         else
            {
               // Use file name.
               getFilename(_torrent_id, filename);
            }

         // Our status:
         Status::torrent_status ts;

         switch (status.state)
            {
            case libtorrent::torrent_status::queued_for_checking:
               ts = Status::ts_queued;
               break;
            case libtorrent::torrent_status::checking_files:
               ts = Status::ts_checking;
               break;
            case libtorrent::torrent_status::connecting_to_tracker:
               ts = Status::ts_connecting;
               break;
            case libtorrent::torrent_status::downloading:
               ts = Status::ts_downloading;
               break;
            case libtorrent::torrent_status::seeding:
               ts = Status::ts_seeding;
               break;
            case libtorrent::torrent_status::finished:
               // FINISHED = (not all files selected) && (all selected files downloaded)
               // we show FINISHED when (not all files selected) && (all selected files downloaded) && (torrent stopped)
               // so here we show SEEDING
               ts = Status::ts_seeding;
               break;
            default:
               ts = Status::ts_undefined;
            }

         if (ti->handle.is_paused())
            {
               if (status.progress == 1)
                  {
                     // Finished and stopped.
                     ts = Status::ts_finished;
                  }
               else
                  {
                     // Not finished, just stopped.
                     ts = Status::ts_stopped;
                  }
            }

         t_ulong dn_total     = status.total_payload_download;
         t_ulong ul_total     = status.total_payload_upload;
#if BTG_OPTION_SAVESESSIONS
         dn_total+= torrents[_torrent_id]->total_payload_download;
         ul_total+= torrents[_torrent_id]->total_payload_upload;
#endif // BTG_OPTION_SAVESESSIONS

         // Not used, should be removed.
         t_ulong failed_bytes = 0;

         t_ulong dn_rate        = 0;
         t_ulong ul_rate        = 0;

         if ((ts == Status::ts_downloading) || (ts == Status::ts_seeding))
            {
               dn_rate        = static_cast<t_ulong>(status.download_payload_rate);
               ul_rate        = static_cast<t_ulong>(status.upload_payload_rate);
            }

         // Percent:
         t_int done         = static_cast<t_ulong>(status.progress * 100);

         // Get the filesize:
         t_ulong filesize      = ti->handle.get_torrent_info().total_size();

         // Get the number of leeches and seeders.

         t_int seeders = status.num_seeds;
         t_int leeches = status.num_peers - seeders;

         if (status.num_incomplete != -1)
            {
               leeches = status.num_incomplete;
            }

         if (status.num_complete != -1)
            {
               seeders = status.num_complete;
            }

         // Time left:
         if (!progress_.exists(_torrent_id))
            {
               progress_.add(_torrent_id, filesize);
            }

         progress_.update(_torrent_id, dn_rate);
         timeSpecification timespec = progress_.getTime(_torrent_id, done);

         t_int   day = 0;
         t_ulong hour   = 0;
         t_int   minute = 0;
         t_int   second = 0;

         if (!timespec.isUndefined())
            {
               day = timespec.day();
               hour   = timespec.hour();
               minute = timespec.minute();
               second = timespec.second();
            }

         t_ulong counter = 0;

         if (ts == Status::ts_downloading)
            {
               counter = ti->download_counter;
            }

         if (ts == Status::ts_seeding)
            {
               counter = ti->seed_counter;
            }

         BTG_MNOTICE(logWrapper(), 
                     "sending progress info: " <<
                     hour << ":" << minute << ":" << second <<
                     ". Trackerstatus is " << ti->trackerStatus.toString());

         _destination.set(_torrent_id,
                          filename,
                          ts,
                          dn_total,
                          ul_total,
                          failed_bytes,
                          dn_rate,
                          ul_rate,
                          done,
                          filesize,
                          leeches,
                          seeders,
                          day,
                          hour,
                          minute,
                          second,
                          ti->trackerStatus,
                          counter
                          );

         BTG_MEXIT(logWrapper(), "getStatus", true);
         return true;
      }

      bool Context::getStatus(t_intList const& _contexts, 
                              t_statusList & _status)
      {
         BTG_MENTER(logWrapper(), "getStatus(list)", "");

         bool   result   = true;
         Status status;

         if (_contexts.size() == 0)
            {
               result = false;
               BTG_MEXIT(logWrapper(), "getStatus(list)", result);
               return result;
            }

         for (t_intListCI i=_contexts.begin(); 
              i!= _contexts.end();
              i++)
            {
               if (this->getStatus(*i, status))
                  {
                     _status.push_back(status);
                  }
               else
                  {
                     result = false;
                  }
            }

         BTG_MEXIT(logWrapper(), "getStatus(list)", result);
         return result;
      }

      bool Context::getStatusAll(t_statusList & _vstatus)
      {
         BTG_MENTER(logWrapper(), "getStatusAll", "");

         bool           result   = true;
         t_intList      contexts = this->getContexts();
         Status         status;

         if (contexts.size() == 0)
            {
               result = false;
            }

         for (t_intListI i=contexts.begin(); i!= contexts.end(); i++)
            {
               if (this->getStatus(*i, status))
                  {
                     _vstatus.push_back(status);
                  }
               else
                  {
                     result = false;
                  }
            }

         BTG_MEXIT(logWrapper(), "getStatusAll", result);
         return result;
      }

      bool Context::getFileInfo(t_int const _torrent_id, 
                                t_fileInfoList & _vfileinfo)
      {
         BTG_MENTER(logWrapper(), "getFileInfo", "id = " << _torrent_id);

         bool result = true;

         torrentInfo* ti = getTorrentInfo(_torrent_id);
         if (ti == 0)
            {
               BTG_MEXIT(logWrapper(), "getFileInfo", false);
               return false;
            }

         if (!safeForFileInfo(_torrent_id))
            {
               // The torrent is in a state where fetching file info
               // is not possible.
               BTG_MEXIT(logWrapper(), "getFileInfo", false);
               return false;
            }

         // Torrent is seeding or finished, so all files are downloaded.
         libtorrent::torrent_status tstatus = ti->handle.status();

         if ((tstatus.state == libtorrent::torrent_status::seeding)
             || 
             (tstatus.state == libtorrent::torrent_status::finished))
            {
               libtorrent::torrent_info t_i = ti->handle.get_torrent_info();
               libtorrent::torrent_info::file_iterator file_iter;

               t_ulong piece_len = t_i.piece_length();

               for (file_iter = t_i.begin_files();
                    file_iter != t_i.end_files();
                    file_iter++)
                  {
                     std::string filename  = file_iter->path.native_file_string();
                     t_ulong file_size     = file_iter->size;

                     if (!ti->selected_files.selected(filename))
                        {
                           // Skip this file, as it is not selected.
                           continue;
                        }

                     fileInformation file_info(filename, piece_len, file_size);
                     _vfileinfo.push_back(file_info);
                  }

               BTG_MEXIT(logWrapper(), "getFileInfo", true);
               return true;
            }
         

         /*
          * By using the list of files and the piece-size from the
          * torrent_info object, you can fairly easy map each piece to a
          * file (since a piece can span file boundries I guess you
          * should be able to map partial pieces to files too). Then you
          * can use the torrent_status::pieces vector to see which pieces
          * has been downloaded, and map them to the files to see how
          * large parts of each file is downloaded.
          */

         libtorrent::torrent_status status = ti->handle.status();
         libtorrent::torrent_info t_i      = ti->handle.get_torrent_info();
#if (BTG_LT_0_12 || BTG_LT_0_13)
         const std::vector<bool>* pieces   = status.pieces;
#elif BTG_LT_0_14
         /// !!!
         std::vector<bool> piecevector;
         bitfieldToVector(status.pieces, piecevector);
         const std::vector<bool>* pieces   = &piecevector;
#endif
         if (pieces->size() > 0)
            {
               t_ulong piece_len        = t_i.piece_length();
               t_int         num_pieces = t_i.num_pieces();

               t_fileInfoList fileinfolist;

               t_int piece_counter = 0;

               std::vector<bool>::const_iterator piece_iter = pieces->begin();

               libtorrent::torrent_info::file_iterator file_iter;
               for (file_iter = t_i.begin_files();
                    file_iter != t_i.end_files();
                    file_iter++)
                  {
                     std::string filename  = file_iter->path.native_file_string();
                     t_ulong file_size     = file_iter->size;

                     if (!ti->selected_files.selected(filename))
                        {
                           // Skip this file, as it is not selected.
                           continue;
                        }

                     t_bitVector filepieces;
                     t_ulong current_size = 0;
                     do
                        {
                           if (piece_counter < num_pieces)
                              {
                                 filepieces.push_back(*piece_iter);
                                 current_size += t_i.piece_size(piece_counter);

                                 piece_counter++;
                                 piece_iter++;
                              }
                           else
                              {
                                 break;
                              }
                        }
                     while (current_size < file_size);

                     if (current_size != file_size)
                        {
                           piece_counter--;
                           piece_iter--;
                        }
                     BTG_MNOTICE(logWrapper(), 
                                 "creating a fileInformation object (filename = " <<
                                 filename << ", number of pieces = " << filepieces.size() <<
                                 ", piece size = " << piece_len << ")");

                     fileInformation file_info(filename, filepieces, piece_len, file_size);
                     fileinfolist.push_back(file_info);
                  }
               _vfileinfo = fileinfolist;
            }
         else
            {
               result = false;
            }

         BTG_MEXIT(logWrapper(), "getFileInfo", result);
         return result;
      }

      bool Context::getPeers(t_int const _torrent_id, 
                             t_peerList & _peerlist,
                             t_uint * _peerExOffset,
                             t_uint * _peerExCount,
                             t_peerExList * _peerExList)
      {
         BTG_MENTER(logWrapper(), "getPeers", "id = " << _torrent_id
            << ", offset = " << (_peerExOffset ? *_peerExOffset : (t_uint)-1)
            << ", count = " << (_peerExCount ? *_peerExCount : (t_uint)-1));

         torrentInfo *ti;
         if ((ti = getTorrentInfo(_torrent_id)) == 0)
            {
               return false;
            }

         bool status = true;
         std::vector<libtorrent::peer_info> peerinfolist;

         try
            {
               ti->handle.get_peer_info(peerinfolist);
            }
         catch(libtorrent::invalid_handle)
            {
               status = false;
            }

         if (status)
            {
               std::vector<libtorrent::peer_info>::const_iterator iter;
               for (iter = peerinfolist.begin(); iter != peerinfolist.end(); iter++)
                  {
                     // Convert the peer ip from the libtorrent implementation to 4 bytes.
#if BTG_LT_0_14
                     boost::asio::ip::basic_endpoint<boost::asio::ip::tcp> endp = iter->ip;
                     boost::asio::ip::address_v4 addr = endp.address().to_v4();
#else
                     asio::ip::basic_endpoint<asio::ip::tcp> endp = iter->ip;
                     asio::ip::address_v4 addr = endp.address().to_v4();
#endif
                     boost::array<unsigned char, 4> bytes = addr.to_bytes();

                     peerAddress pa(
                                    bytes.at(0),
                                    bytes.at(1),
                                    bytes.at(2),
                                    bytes.at(3)
                                    );

                     std::string client_identification = libtorrent::identify_client(iter->pid);
#if BTG_LT_0_12
                     Peer peer(pa, iter->seed, client_identification);
#else                    
                     Peer peer(pa, iter->flags & libtorrent::peer_info::seed, client_identification);
#endif

                     _peerlist.push_back(peer);
                  }

               if (_peerlist.size() == 0)
                  {
                     // Do not return an empty peer list.
                     status = false;
                  }
               else
               {
                  if (_peerExOffset && _peerExCount && _peerExList)
                  {
                     _peerExList->clear();
                     if (*_peerExOffset + *_peerExCount > peerinfolist.size())
                     {
                        if (*_peerExCount > peerinfolist.size())
                           {
                              *_peerExCount = peerinfolist.size();
                           }
                        *_peerExOffset = peerinfolist.size() - *_peerExCount;
                     }
                     for(t_uint i = *_peerExOffset, endi = *_peerExOffset + *_peerExCount; i < endi; ++i)
                     {
#if BTG_LT_0_14
                        std::vector<bool> fpieces;
                        bitfieldToVector(peerinfolist[i].pieces, fpieces);
#endif

                        PeerEx peerEx(
                           peerinfolist[i].flags,
#if BTG_LT_0_13 || BTG_LT_0_14
                           peerinfolist[i].source,
#elif BTG_LT_0_12
                           0,
#endif
                           (t_uint)peerinfolist[i].down_speed, 
                           (t_uint)peerinfolist[i].up_speed,
                           (t_uint)peerinfolist[i].payload_down_speed, 
                           (t_uint)peerinfolist[i].payload_up_speed,
                           peerinfolist[i].total_download, 
                           peerinfolist[i].total_upload,
#if BTG_LT_0_12 || BTG_LT_0_13
                           peerinfolist[i].pieces,
#elif BTG_LT_0_14
                           fpieces,
#endif
                           peerinfolist[i].download_limit, 
                           peerinfolist[i].upload_limit,
#ifndef TORRENT_DISABLE_RESOLVE_COUNTRIES
                           peerinfolist[i].country,
#else
                           0,
#endif
                           peerinfolist[i].load_balancing,
                           peerinfolist[i].download_queue_length, 
                           peerinfolist[i].upload_queue_length,
                           peerinfolist[i].downloading_piece_index, 
                           peerinfolist[i].downloading_block_index,
                           peerinfolist[i].downloading_progress, 
                           peerinfolist[i].downloading_total,
                           peerinfolist[i].client,
                           peerinfolist[i].connection_type,
#if BTG_LT_0_13 || BTG_LT_0_14
                           libtorrent::total_seconds(peerinfolist[i].last_request), 
                           libtorrent::total_seconds(peerinfolist[i].last_active),
                           peerinfolist[i].num_hashfails, 
                           peerinfolist[i].failcount,
                           peerinfolist[i].target_dl_queue_length,
                           peerinfolist[i].remote_dl_rate
#elif BTG_LT_0_12
                           (t_uint)-1, (t_uint)-1,
                           (t_uint)-1, (t_uint)-1,
                           (t_uint)-1,
                           (t_uint)-1
#endif
                           );
                        _peerExList->push_back(peerEx);
                     }
                  }
               }
            }

         BTG_MEXIT(logWrapper(), "getPeers", "return = " << status
            << ", offset = " << (_peerExOffset ? *_peerExOffset : (t_uint)-1)
            << ", count = " << (_peerExList ? _peerExList->size() : (t_uint)-1));
         return status;
      }

      bool Context::getTrackers(t_int const _torrent_id, t_strList & _trackers)
      {
         BTG_MENTER(logWrapper(), "getTrackers", "id = " << _torrent_id);

         torrentInfo *ti;
         if ((ti = getTorrentInfo(_torrent_id)) == 0)
            {
               return false;
            }

         bool status = true;

         std::vector<libtorrent::announce_entry> trackers;

         try
            {
               trackers = ti->handle.trackers();
            }
         catch(libtorrent::invalid_handle)
            {
               status = false;
            }

         if (status)
            {
               for (std::vector<libtorrent::announce_entry>::const_iterator iter = trackers.begin();
                    iter != trackers.end();
                    iter++)
                  {
                     _trackers.push_back(iter->url);
                  }

               if (_trackers.size() == 0)
                  {
                     // Do not return an empty peer list.
                     status = false;
                  }
            }

         BTG_MEXIT(logWrapper(), "getTrackers", status);
         return status;
      }

      bool Context::getSelectedFiles(t_int const _torrent_id,
                                     selectedFileEntryList & _file_list)
      {
         BTG_MENTER(logWrapper(), "getSelectedFiles", "id = " << _torrent_id);

         torrentInfo *ti;
         if ((ti = getTorrentInfo(_torrent_id)) == 0)
            {
               return false;
            }
         
         _file_list = ti->selected_files;

         BTG_MEXIT(logWrapper(), "getSelectedFiles", true);
         return true;
      }

      bool Context::setSelectedFiles(t_int const _torrent_id,
                                     btg::core::selectedFileEntryList const& _file_list)
      {
         BTG_MENTER(logWrapper(), "setSelectedFiles", "id = " << _torrent_id);

         torrentInfo* ti = getTorrentInfo(_torrent_id);
         if (ti == 0)
            {
               return false;
            }
         
         btg::core::selectedFileEntryList file_list;

         if (_file_list.size() < ti->selected_files.size())
            {
               // The message only contains some changes, not the
               // whole list.

               BTG_MNOTICE(logWrapper(), "Setting files to download, not all files received (" << _file_list.size() << ").");

               file_list = ti->selected_files;

               std::vector<selectedFileEntry> const & newfiles = _file_list.files();
               std::vector<selectedFileEntry> currentfiles     = file_list.files();

               for (std::vector<selectedFileEntry>::const_iterator  newiter = newfiles.begin();
                    newiter != newfiles.end();
                    newiter++)
                  {
                     for (std::vector<selectedFileEntry>::iterator currentiter = currentfiles.begin();
                          currentiter != currentfiles.end();
                          currentiter++)
                        {
                           if (currentiter->filename() == newiter->filename())
                              {
                                 *currentiter = *newiter;
                              }
                        }  
                  }

               file_list.setFiles(currentfiles);
            }
         else
            {
               // Same size.
               file_list.setFiles(_file_list.files());
            }

         BTG_MNOTICE(logWrapper(), "Setting files to download");
         std::vector<selectedFileEntry> const& files = file_list.files();
         if (files.size() == 0)
            {
               BTG_MEXIT(logWrapper(), "setSelectedFiles", false);
               return false;
            }

         std::vector<selectedFileEntry>::const_iterator iter;
         for (iter = files.begin();
              iter != files.end();
              iter++)
            {
               BTG_MNOTICE(logWrapper(), "fn:" + iter->filename());
            }

         if (!checkSelectedFiles(ti->selected_files, file_list))
            {
               BTG_MEXIT(logWrapper(), "setSelectedFiles", false);
               return false;
            }
         
         if (!applySelectedFiles(ti, file_list))
            {
               BTG_MEXIT(logWrapper(), "setSelectedFiles", false);
               return false;
            }
         
         // after handle.prioritize_files torrent status is recalculated, so we can use it to
         // switch storage dir
         switch (ti->handle.status().state)
            {
            case libtorrent::torrent_status::queued_for_checking:
            case libtorrent::torrent_status::checking_files:
            case libtorrent::torrent_status::connecting_to_tracker:
            case libtorrent::torrent_status::downloading:
               if (!moveToWorkingDir(_torrent_id))
                  {
                     BTG_MNOTICE(logWrapper(), "moveToWorkingDir failed. state:" << ti->handle.status().state);
                  }
               break;
            case libtorrent::torrent_status::seeding:
            case libtorrent::torrent_status::finished:
               if (!moveToSeedingDir(_torrent_id))
                  {
                     BTG_MNOTICE(logWrapper(), "moveToSeedingDir failed. state:" << ti->handle.status().state);
                  }
               break;
            default:
               ; // do nothing
            }
         
         BTG_MEXIT(logWrapper(), "setSelectedFiles", true);
         return true;
      }
      
      bool Context::checkSelectedFiles(btg::core::selectedFileEntryList const& _l1,
                                       btg::core::selectedFileEntryList const& _l2) const
      {
         BTG_MENTER(logWrapper(), "checkSelectedFiles", "file list");

         if (_l1.size() != _l2.size())
            {
               BTG_MEXIT(logWrapper(), "checkSelectedFiles", false);
               return false;
            }

         std::vector<selectedFileEntry> l1_files = _l1.files();
         std::vector<selectedFileEntry> l2_files = _l2.files();

         std::vector<selectedFileEntry>::const_iterator l1_iter = l1_files.begin();
         std::vector<selectedFileEntry>::const_iterator l1_end  = l1_files.end();
         std::vector<selectedFileEntry>::const_iterator l2_iter = l2_files.begin();

         do
            {
               if (l1_iter->filename() != l2_iter->filename())
                  {
                     BTG_MEXIT(logWrapper(), "checkSelectedFiles", false);
                     return false;
                  }
               
               l1_iter++; 
               l2_iter++;
            } while (l1_iter != l1_end);

         BTG_MEXIT(logWrapper(), "checkSelectedFiles", true);
         return true;
      }
      
      bool Context::applySelectedFiles(torrentInfo* ti,
                                       btg::core::selectedFileEntryList const& _input)
      {
         BTG_MENTER(logWrapper(), "applySelectedFiles", "torrent info");
#if (BTG_LT_0_13 || BTG_LT_0_14)
         enum lt_file_priority
         {
            LTF_NO  = 0,
            LTF_YES = 1
         };

         std::vector<selectedFileEntry> const& files = _input.files();

         if (ti->handle.get_torrent_info().num_files() != static_cast<t_int>(files.size()))
            {
               BTG_MEXIT(logWrapper(), "applySelectedFiles (get_torrent_info().num_files() != files.size())", false);
               return false;
            }

         std::vector<int> prio_files;

         std::vector<selectedFileEntry>::const_iterator iter;
         for (iter = files.begin();
              iter != files.end();
              iter++)
            {
               if (iter->selected())
                  {
                     prio_files.push_back(LTF_YES);
                  }
               else
                  {
                     prio_files.push_back(LTF_NO);
                  }
            }

#if BTG_DEBUG
         std::string debugPriorityString;

         for (std::vector<int>::const_iterator prio_files_iter = prio_files.begin();
              prio_files_iter != prio_files.end();
              prio_files_iter++)
            {
               if (*prio_files_iter == LTF_YES)
                  {
                     debugPriorityString += "X";
                  }
               else
                  {
                     debugPriorityString += "_";
                  }
            }
         
         BTG_MNOTICE(logWrapper(), "Setting file priorities: " + debugPriorityString);
#endif // BTG_DEBUG

         ti->handle.prioritize_files(prio_files);

         ti->selected_files = _input;

         BTG_MEXIT(logWrapper(), "applySelectedFiles", true);
         return true;
#elif BTG_LT_0_12
         BTG_MNOTICE(logWrapper(), "Setting file priorities is not implemented in libtorrent 0.12.x");
         BTG_MEXIT(logWrapper(), "applySelectedFiles", false);
         return false;
#endif
      }

      torrentInfo* Context::getTorrentInfo(t_int const _torrent_id) const
      {
         std::map<t_int, torrentInfo*>::const_iterator tii = torrents.find(_torrent_id);

         if (tii == torrents.end())
            {
               BTG_MEXIT(logWrapper(), "getTorrentInfo", false);
               return 0;
            }

         return tii->second;
      }

      bool Context::safeForFileInfo(t_int const _torrent_id)
      {
         bool result = false;

         torrentInfo* ti = getTorrentInfo(_torrent_id);
         if (ti == 0)
            {
               return false;
            }

         // Get the status from libtorrent:
         libtorrent::torrent_status status = ti->handle.status();

         if ((status.state == libtorrent::torrent_status::connecting_to_tracker)
             ||
             (status.state == libtorrent::torrent_status::downloading)
             ||
             (status.state == libtorrent::torrent_status::seeding)
             || 
             (status.state == libtorrent::torrent_status::finished))
            {
               result = true;
            }

         if (!result)
            {
               VERBOSE_LOG(logWrapper(), verboseFlag_, "Encryption settings:");
            }
         return result;
      }

      void Context::setClientAttached(bool const _clientAttached)
      {
         clientAttached = _clientAttached;
      }

      bool Context::isClientAttached() const
      {
         return clientAttached;
      }

      void Context::checkSeedLimits()
      {
         std::map<t_int, torrentInfo*>::iterator tii;

         for (tii = torrents.begin();
              tii != torrents.end();
              tii++)
            {
               torrentInfo* ti = tii->second;
               libtorrent::torrent_status status = ti->handle.status();
#if BTG_OPTION_EVENTCALLBACK
               bool callback_stopped_seeding = false;
               // If the above is true, an event callback will be
               // called.
#endif // BTG_OPTION_EVENTCALLBACK
               if ((status.state == libtorrent::torrent_status::seeding) && (!ti->handle.is_paused()))
                  {
                     // Limit or timeout is set, doesnt realy mather which we check first...
                     // Check if we've seeded enough
                     // If it has already been stopped once by limits, do not check any more...
                     if ((!ti->seedingAutoStopped) && (ti->seedLimit >= 0))
                        {
                           // Calculate how much we've seeded the torrent.
                           // Mainly we check for downloaded vs uploaded data,
                           // but in case downloaded is less than filesize, and were finished, then
                           // we have to check filesize vs uploaded (right?)
                           t_int percent = 0;

                           t_ulong dn_total = status.total_payload_download;
                           t_ulong ul_total = status.total_payload_upload;
#if BTG_OPTION_SAVESESSIONS
                           // Get the accumulated counters from earlier:
                           dn_total += ti->total_payload_download;
                           ul_total += ti->total_payload_upload;
#endif // BTG_OPTION_SAVESESSIONS
                           float ratio                = 0;
                           t_ulong torrent_total_size = ti->handle.get_torrent_info().total_size();

                           if (dn_total < torrent_total_size)
                              {
                                 ratio = (1.0f*ul_total) / (1.0f*ti->handle.get_torrent_info().total_size());
                              }
                           else
                              {
                                 ratio = (1.0f*ul_total) / (1.0f*dn_total);
                              }

                           percent = static_cast<t_int>(100 * ratio);

                           if (percent >= ti->seedLimit)
                              {
                                 BTG_MNOTICE(logWrapper(), "stopIfSeeding(), stopping torrent '" << tii->first << "', has seeded " << percent << "%");
                                 ti->handle.pause();
                                 ti->seedingAutoStopped   = true;
#if BTG_OPTION_EVENTCALLBACK
                                 // Call event callback.
                                 callback_stopped_seeding = true;
#endif // BTG_OPTION_EVENTCALLBACK
                              }
                        }

                     // Check if we've seeded enough time
                     if(!ti->seedingAutoStopped && ti->seedTimeout >= 0)
                        {
                           boost::posix_time::time_duration time_seeded = boost::posix_time::ptime(boost::posix_time::second_clock::universal_time()) - ti->finished_timestamp;
                           // Easier for clients to set second timeout
                           // with a ulong instead and convert here...
                           boost::posix_time::time_duration timeout(boost::posix_time::seconds(ti->seedTimeout));
                           if(time_seeded > timeout)
                              {
                                 BTG_MNOTICE(logWrapper(), "stopIfSeeding(), stopping torrent " <<
                                             tii->first << ", has seeded " <<
                                             boost::posix_time::to_simple_string(time_seeded));

                                 ti->handle.pause();
                                 ti->seedingAutoStopped   = true;
#if BTG_OPTION_EVENTCALLBACK
                                 // Call event callback.
                                 callback_stopped_seeding = true;
#endif // BTG_OPTION_EVENTCALLBACK
                              }
                        }
                  }
#if BTG_OPTION_EVENTCALLBACK
               if (callback_stopped_seeding)
                  {
                     // Call event callback.
                     std::string filename;
                     this->getFilename(tii->first, filename);

                     std::vector<std::string> cbm_arguments;
                     cbm_arguments.push_back(filename);
                     cbm_->event(username_,
                                 callbackManager::CBM_FINISHED,
                                 cbm_arguments);
                  }
#endif // BTG_OPTION_EVENTCALLBACK
            }
      }

      void Context::updateElapsedOrSeedCounter()
      {
         BTG_MNOTICE(logWrapper(), "Updating elapsed/seed counter.");

         std::map<t_int, torrentInfo*>::iterator tii;

         for (tii = torrents.begin();
              tii != torrents.end();
              tii++)
            {
               torrentInfo* ti = tii->second;
               libtorrent::torrent_status status = ti->handle.status();
       
               if (status.state == libtorrent::torrent_status::downloading)
                  {
                     ti->download_counter++;
                  }
               else if (status.state == libtorrent::torrent_status::seeding)
                  {
                     ti->seed_counter++;
                  }
            }
      }

      bool Context::clean(t_int _contextID, 
                          t_strList & _files, 
                          t_intList & _contextIDs)
      {
         BTG_MENTER(logWrapper(), "clean", "Cleaning contextID=" << _contextID);

         bool op_status = false;

         torrentInfo *ti;
         if ((ti = getTorrentInfo(_contextID)) == 0)
            {
               return false;
            }

         // Check status:
         libtorrent::torrent_status status = ti->handle.status();

         if (status.state == libtorrent::torrent_status::seeding || status.progress == 1.0)
            {
               std::string filename;
               getFilename(_contextID, filename);

               _files.push_back(filename);
               _contextIDs.push_back(_contextID);

               BTG_MNOTICE(logWrapper(), "removing torrent with ID " << _contextID);
               op_status = true;

               // Attempt to move data to the destination directory.
               BTG_MNOTICE(logWrapper(), 
                           "moving ID " << _contextID << " from '" <<
                           workDir_ << "' to '" << outputDir_ << "'");
               this->moveToDestinationDir(_contextID);

               BTG_MNOTICE(logWrapper(), "removing torrent with ID = " << _contextID);
               this->remove(_contextID, false
#if BTG_OPTION_EVENTCALLBACK
                            , false
#endif // BTG_OPTION_EVENTCALLBACK
                            );
               ti = 0;
            }

         BTG_MEXIT(logWrapper(), "clean", op_status);
         return op_status;
      }

      bool Context::clean(t_strList & _files, t_intList & _contextIDs)
      {
         BTG_MENTER(logWrapper(), "clean", "");

         bool op_status = false;
         std::map<t_int, torrentInfo*>::const_iterator tii;
         for (tii = torrents.begin(); tii != torrents.end(); tii++)
            {
               t_int torrent_id      = tii->first;
               const torrentInfo *ti = tii->second;
               // Check status:
               libtorrent::torrent_status status = ti->handle.status();

               if ((status.state == libtorrent::torrent_status::seeding) || (status.progress == 1.0))
                  {
                     _files.push_back(ti->filename);
                     _contextIDs.push_back(torrent_id);

                     BTG_MNOTICE(logWrapper(), 
                                 "marking torrent with ID = " << torrent_id << " for removal");
                     op_status = true;
                  }
            }

         if (_contextIDs.size() >= 1)
            {
#if BTG_OPTION_EVENTCALLBACK
               t_strListCI filename_iter = _files.begin();
#endif // BTG_OPTION_EVENTCALLBACK
               t_intListCI iter;
               for (iter=_contextIDs.begin(); iter!=_contextIDs.end(); iter++)
                  {

#if BTG_OPTION_EVENTCALLBACK
                     t_fileInfoList fileinfolist;
                     getFileInfo(*iter, fileinfolist);
                     // Only list top dirs.
                     t_strList simplified_list;
                     removeFileInfoDetails(fileinfolist, simplified_list);
                     fileinfolist.clear();
#endif // BTG_OPTION_EVENTCALLBACK

                     // Attempt to move this file to the destination directory.
                     BTG_MNOTICE(logWrapper(), 
                                 "moving '" << *iter << "' from '" <<
                                 workDir_ << "' to '" << outputDir_ << "'");
                     this->moveToDestinationDir(*iter);

                     BTG_MNOTICE(logWrapper(), "removing torrent with ID " << *iter);
                     this->remove(*iter, false
#if BTG_OPTION_EVENTCALLBACK
                                  , false
#endif // BTG_OPTION_EVENTCALLBACK
                                  );

#if BTG_OPTION_EVENTCALLBACK
                     // Call the callback for each file cleaned.
                     std::vector<std::string> cbm_arguments;

                     BTG_MNOTICE(logWrapper(), "adding '" << *filename_iter << "'");

                     // Add the torrent file name.
                     cbm_arguments.push_back(*filename_iter);

                     // Add the files which were moved.
                     t_strListCI file_iter;
                     for (file_iter = simplified_list.begin();
                          file_iter != simplified_list.end();
                          file_iter++)
                        {
                           cbm_arguments.push_back(*file_iter);
                        }

                     cbm_->event(username_,
                                 callbackManager::CBM_CLEAN,
                                 cbm_arguments);

                     filename_iter++;
#endif // BTG_OPTION_EVENTCALLBACK
                  }
            }

         BTG_MEXIT(logWrapper(), "clean", op_status);
         return op_status;
      }

      void Context::setNormalHttpSettings()
      {
         BTG_MNOTICE(logWrapper(), "setting session settings: normal");
         session_settings_.tracker_completion_timeout      = 512;
         session_settings_.tracker_receive_timeout         = 512;
         session_settings_.stop_tracker_timeout            = 512;
         session_settings_.peer_timeout                    = 512;
         session_settings_.urlseed_timeout                 = 512;
         session_settings_.tracker_maximum_response_length = 1024 * 1024 * 1024;
         std::string userAgent = config_->getUserAgent();
         if (userAgent.size() > 0)
            {
               MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                            "Overriding user agent with: '" << userAgent << "'.");
               session_settings_.user_agent = userAgent;
            }
         else
            {
               session_settings_.user_agent = projectDefaults::sDAEMON() + " " + projectDefaults::sVERSION();
            }
         BTG_MNOTICE(logWrapper(), "user agent string: " << session_settings_.user_agent);

         torrent_session->set_settings(session_settings_);
      }

      void Context::setProxyHttpSettings(btg::core::addressPort const& _proxy)
      {
         BTG_MNOTICE(logWrapper(), "setting session settings: proxy");
#if (BTG_LT_0_13 || BTG_LT_0_14)
         libtorrent::proxy_settings ps;
         ps.hostname = _proxy.getIp();
         ps.port     = _proxy.getPort();
         torrent_session->set_peer_proxy(ps);
         torrent_session->set_web_seed_proxy(ps);
         torrent_session->set_tracker_proxy(ps);
#elif (BTG_LT_0_12)
         session_settings_.proxy_ip   = _proxy.getIp();
         session_settings_.proxy_port = _proxy.getPort();

         torrent_session->set_settings(session_settings_);
#endif // libtorrent version.
      }

      void Context::setDestructionlHttpSettings()
      {
         BTG_MNOTICE(logWrapper(), "setting session settings: destruction");

         session_settings_.tracker_completion_timeout = 1;
         session_settings_.tracker_receive_timeout    = 1;
         session_settings_.stop_tracker_timeout       = 1;
         session_settings_.peer_timeout               = 1;
         session_settings_.urlseed_timeout            = 1;

         torrent_session->set_settings(session_settings_);
      }

      bool Context::getIdFromHandle(libtorrent::torrent_handle const& _handle,
                                    t_int & _torrent_id,
                                    torrentInfo * & _ti)
      {
         bool result = false;

         // Resolve the torrent handle to a torrent id.
         std::map<t_int, torrentInfo*>::iterator tii;

         for (tii = torrents.begin();
              tii != torrents.end();
              ++tii)
            {
               if (tii->second->handle == _handle)
                  {
                     _torrent_id = tii->first;
                     _ti         = tii->second;
                     result      = true;
                     break;
                  }
            }

         return result;
      }

      void Context::shutdown()
      {
         BTG_MENTER(logWrapper(), "shutdown", "");
         this->setDestructionlHttpSettings();

         this->stopAll();

         this->writeResumeData();

         this->removeAll(false /* Do not remove data. */);

         disableDHT();

         limitMgr_->remove(torrent_session);

         this->torrent_session_proxy = torrent_session->abort();

         delete torrent_session;
         torrent_session = 0;

         BTG_MEXIT(logWrapper(), "shutdown", "");
      }

      void Context::enableDHT(libtorrent::entry const& _dht_state)
      {
         if ((useDHT_) && (!enabledDHT_))
            {
               BTG_MNOTICE(logWrapper(), "enableDHT, DHT configuration");

               std::pair<t_uint, t_uint> dht_port_range;
               portMgr->get(dht_port_range);

               DHTport_ = dht_port_range.first;

               libtorrent::dht_settings dht_settings;
               dht_settings.service_port = DHTport_;
               torrent_session->set_dht_settings(dht_settings);

               BTG_MNOTICE(logWrapper(), "enableDHT, Enabling DHT");
               torrent_session->start_dht(_dht_state);

               // DHT nodes for bootstrapping.
               torrent_session->add_dht_router(std::make_pair(std::string("router.bittorrent.com")
                                                              , 6881));
               torrent_session->add_dht_router(std::make_pair(std::string("router.utorrent.com")
                                                              , 6881));
               torrent_session->add_dht_router(std::make_pair(std::string("router.bitcomet.com")
                                                              , 6881));

               enabledDHT_ = true;
            }
      }

      void Context::disableDHT()
      {
         if (useDHT_ && enabledDHT_)
            {
               BTG_MNOTICE(logWrapper(), "disableDHT, disabling DHT");
               torrent_session->stop_dht();

               std::pair<t_uint, t_uint> dht_port_range(DHTport_, DHTport_);
               portMgr->giveBack(dht_port_range);

               enabledDHT_ = false;
            }
      }

      bool Context::dhtEnabled() const
      {
         return enabledDHT_;
      }

      bool Context::encryptionEnabled() const
      {
         return useEncryption_;
      }

      void Context::enableEncryption()
      {
#if (BTG_LT_0_13 || BTG_LT_0_14)
         try
            {
               libtorrent::pe_settings enc_settings;
                     
               daemonConfiguration::encryption_policy in;
               daemonConfiguration::encryption_policy out;
               daemonConfiguration::encryption_level level;
               bool preferRc4;
               config_->getEncryptionSettings(in, out, level, preferRc4);

               std::string verboseEncryption;

               switch (in)
                  {
                  case daemonConfiguration::forced:
                     enc_settings.in_enc_policy     = libtorrent::pe_settings::forced;
                     verboseEncryption             += "in: forced";
                     break;
                  case daemonConfiguration::enabled:
                     enc_settings.in_enc_policy     = libtorrent::pe_settings::enabled;
                     verboseEncryption             += "in: enabled";
                     break;
                  case daemonConfiguration::disabled:
                     enc_settings.in_enc_policy     = libtorrent::pe_settings::disabled;
                     verboseEncryption             += "in: disabled";
                     break;
                  }

               switch (out)
                  {
                  case daemonConfiguration::forced:
                     enc_settings.out_enc_policy    = libtorrent::pe_settings::forced;
                     verboseEncryption             += " out: forced";
                     break;
                  case daemonConfiguration::enabled:
                     enc_settings.out_enc_policy    = libtorrent::pe_settings::enabled;
                     verboseEncryption             += " out: enabled";
                     break;
                  case daemonConfiguration::disabled:
                     enc_settings.out_enc_policy    = libtorrent::pe_settings::disabled;
                     verboseEncryption             += " out: disabled";
                     break;
                  }

               switch(level)
                  {
                  case daemonConfiguration::plaintext:
                     enc_settings.allowed_enc_level = libtorrent::pe_settings::plaintext;
                     verboseEncryption             += " lvl: plaintext";
                     break;
                  case daemonConfiguration::rc4:
                     enc_settings.allowed_enc_level = libtorrent::pe_settings::rc4;
                     verboseEncryption             += " lvl: rc4";
                     break;
                  case daemonConfiguration::both:
                     enc_settings.allowed_enc_level = libtorrent::pe_settings::both;
                     verboseEncryption             += " lvl: both";
                     break;
                  }

               enc_settings.prefer_rc4 = preferRc4;

               if (preferRc4)
                  {
                     verboseEncryption += " prefer: on";
                  }
               else
                  {
                     verboseEncryption += " prefer: off";
                  }

               VERBOSE_LOG(logWrapper(), verboseFlag_, "Encryption settings:");
               VERBOSE_LOG(logWrapper(), verboseFlag_, verboseEncryption << ".");

               torrent_session->set_pe_settings(enc_settings);
               BTG_MNOTICE(logWrapper(), "Encryption settings set");
            }
         catch (std::exception& e)
            {
               BTG_ERROR_LOG(logWrapper(), "libtorrent exception: " << e.what() );
            }
#endif
      }

      std::string Context::getTempDir() const
      {
         return tempDir_;
      }

      void Context::updateFilter(IpFilterIf* _filter)
      {
         filter_ = _filter;

         if (!torrent_session)
            {
               return;
            }

         if (!filter_)
            {
               return;
            }

         filter_->set(*torrent_session);
      }

      Context::~Context()
      {
         portMgr->giveBack(listen_port_range_);
      }

   } // namespace daemon
} // namespace btg

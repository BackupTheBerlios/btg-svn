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

#ifndef CONTEXT_H
#define CONTEXT_H

#include "lt_version.h"

#include <libtorrent/entry.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/session.hpp>

#include <libtorrent/session_settings.hpp>
#include <libtorrent/alert_types.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include <string>
#include <map>
#include <vector>

#include <bcore/type.h>
#include <bcore/type_btg.h>

#include <bcore/addrport.h>
#include <bcore/command_factory.h>

#include <bcore/file_info.h>

#include "progress.h"

#include <bcore/command/setup.h>

#include <bcore/sbuf.h>
#include <bcore/addrport.h>

namespace btg
{
   namespace core
   {
      class Status;
      class trackerStatus;
   }
}

namespace btg
{
   namespace daemon
      {

#if BTG_OPTION_EVENTCALLBACK
         class callbackManager;
#endif // BTG_OPTION_EVENTCALLBACK

         /**
          * \addtogroup daemon
          */
         /** @{ */

         /// Used to keep information about a torrent
         class torrentInfo
            {
            public:
               torrentInfo();

               /// Torrent handle
               libtorrent::torrent_handle handle;

               /// Filename
               std::string filename;

               // Limits
               /// Bandwith limit on upload.
               t_int limitUpld;

               /// Bandwith limit on download.
               t_int limitDwnld;

               /// Seed limit.
               t_int seedLimit;

               /// Seed timeout.
               t_long seedTimeout;

               /// Counter, torrent has been downloading for this
               /// amount of time. In minutes.
               t_ulong download_counter;

               /// Counter, torrent has been seeding for this amount
               /// of time. In minutes.
               t_ulong seed_counter;

               /// Finished timestamp, used for seedTimeout.
               boost::posix_time::ptime finished_timestamp;

               /// Indicates if this torrent has been stopped by the
               /// seed-limits. In case the user starts the torrent
               /// again we dont want to stop it.
               bool seedingAutoStopped;

               /// Current serial, used for sending tracker status
               /// messages.
               t_int serial;

               /// Tracker Status.
               btg::core::trackerStatus trackerStatus;

               /// List of files in a torrent, 
               /// selected files can be extracted from it.
               btg::core::selectedFileEntryList selected_files;
#if BTG_OPTION_SAVESESSIONS
               /// Session saving: total download in bytes.
               libtorrent::size_type total_download;
               /// Session saving: total upload in bytes.
               libtorrent::size_type total_upload;

               /// Session saving: total payload download in bytes.
               libtorrent::size_type total_payload_download;
               /// Session saving: total payload upload in bytes.
               libtorrent::size_type total_payload_upload;
#endif // BTG_OPTION_SAVESESSIONS
            };

         class fileTrack;
         class IpFilterIf;
         class portManager;
         class limitManager;
         class daemonConfiguration;
         
         /// An abstraction which is used to communicate with the thread that
         /// runs the bittorrent client.
         class Context: public btg::core::Logable
            {
            public:

               /// Result of adding a torrent file.
               enum addResult
                  {
                     ERR_UNDEFINED = -1,  //!< An undefined error occured.
                     ERR_OK = 0,          //!< No error.
                     ERR_EXISTS = 1,      //!< Torrent already exists.
                     ERR_LIBTORRENT = 2   //!< A libtorrent error occured.
                  };

               /// Constructor.
               /// @param [in] _logwrapper     Pointer used to send logs to.
               /// @param [in] _config         Pointer to the daemon configuration.
               /// @param [in] _verboseFlag    Indicates that verbose logging should be done.
               /// @param [in] _username       Username.
               /// @param [in] _tempDir        Temporary directory.
               /// @param [in] _workDir        Working directory.
               /// @param [in] _seedDir        Seeding directory.
               /// @param [in] _outputDir      Output directory.
               /// @param [in] _interface_used Indicates if the session shall bind to a specific interface.
               /// @param [in] _interface      Interface to bind to.

               /// @param [in] _portMgr        Pointer to port manager.
               /// @param [in] _limitMgr       Pointer to limit manager.
               /// @param [in] _filetrack      Pointer to the class keeping track of added/removed torrents.
               /// @param [in] _filter         Pointer to a filter, used to ban peers.
               /// @param [in] _useTorrentName Enable/disable using the torrent name from the tracker.
               /// @param [in] _cbm            Pointer to callback manager.
               /// @param [in] _clientAttached If true, a client is attached.
               Context(btg::core::LogWrapperType _logwrapper,
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
                       bool _clientAttached = true
                       );

               /// Returns true if this context is configured.
               bool isConfigured() const;

               /// Setup this context.
               /// \note The interface parameter is not used.
               bool setup(btg::core::requiredSetupData const& _rsd);

               /// Set the DHT nodes used by libtorrent.
               /// @param [in] _nodes List of nodes.
               /// @return True - success, false otherwise.
               bool setDHTNodes(std::vector<std::pair<std::string, t_int> > const& _nodes);

               /// Add a torrent file to this context.
               /// @param [in] _torrent_filename  Filename of the torrent file to add (expected to be in tempDir).
               /// @param [in] _buffer            The contents of the file.
               /// @param [out] _handle_id        The ID of the added context. Defined only if the torrent was added. Check the returned status.
               /// @return One of the addResult enums
               addResult add(std::string const& _torrent_filename, 
                             btg::core::sBuffer const& _buffer,
                             t_int & _handle_id);

               /// Remove a torrent file from this context.
               /// @param [in] _torrent_id   Context ID of the torrent to remove.
               /// @param [in] _eraseData    Indicates that data should be erased.
               /// @param [in] _callCallback Indicates that a callback should be called.
               /// @return True if operation was successful, false if not.
               bool remove(t_int const _torrent_id,
                           bool const _eraseData
#if BTG_OPTION_EVENTCALLBACK
                           , bool _callCallback = true
#endif // BTG_OPTION_EVENTCALLBACK
                           );

               /// Remove all torrents from this context.
               /// @return True if operation was successful, false if not.
               bool removeAll(bool const _eraseData);

               /// Start the torrent.
               /// @param [in] _torrent_id Context ID of the torrent to start.
               /// @return True if operation was successful, false if not.
               bool start(t_int const _torrent_id);

               /// Start all torrents.
               /// @return True if operation was successful, false if not.
               bool startAll();

               /// Stop the torrent.
               /// @param [in] _torrent_id Context ID of the torrent to stop.
               /// @return True if operation was successful, false if not.
               bool stop(t_int const _torrent_id);

               /// Stop all torrents.
               /// @return True if operation was successful, false if not.
               bool stopAll();

               /// Set limits a context uses.
               /// @param [in]  _torrent_id  Context ID of a torrent.
               /// @param [in]  _limitUpld   Number of B per second.
               /// @param [in]  _limitDwnld  Number of B per second.
               /// @param [in]  _seedLimit   How many % we should seed before stopping.
               /// @param [in]  _seedTimeout How many seocnds we should seed before stopping.
               bool limit(t_int const _torrent_id,
                          t_int const _limitUpld, 
                          const t_int _limitDwnld, 
                          const t_int _seedLimit, 
                          const t_long _seedTimeout);

               /// Set limits ussed by all contexts.
               /// @param [in]  _limitUpld  Number of B per second.
               /// @param [in]  _limitDwnld Number of B per second.
               /// @param [in]  _seedLimit   How many % we should seed before stopping.
               /// @param [in]  _seedTimeout How many seocnds we should seed before stopping.
               bool limitAll(t_int const _limitUpld, 
                             const t_int _limitDwnld, 
                             const t_int _seedLimit, 
                             const t_long _seedTimeout);

               /// Remove the limits of a single context.
               /// @param [in]  _torrent_id     Context ID of a torrent.
               /// @param [in]  _upld           If true, remove upload limit.
               /// @param [in]  _dwnld          If true, remove download limit.
               /// @param [in]  _seedLimit      If true, remove seed limit.
               /// @param [in]  _seedTimeout   If true, remove seed timeout.
               bool removeLimit(t_int const _torrent_id, 
                                bool const _upld = true, 
                                bool const _dwnld = true, 
                                bool const _seedLimit = true, 
                                bool const _seedTimeout = true);

               /// Remove the limits for all contexts.
               /// @param [in]  _upld     If true, remove upload limit.
               /// @param [in]  _dwnld    If true, remove download limit.
               /// @param [in]  _seedLimit      If true, remove seed limit.
               /// @param [in]  _seedTimeout   If true, remove seed timeout.
               bool removeLimitAll(bool const _upld = true, 
                                   bool const _dwnld = true, 
                                   bool const _seedLimit = true, 
                                   bool const _seedTimeout = true);

               /// Get the limits which was set for a context.
               /// If not set, return false.
               /// @param [in]  _torrent_id     Context ID of a torrent.
               /// @param [out]  _limitUpld     Number of B per second.
               /// @param [out]  _limitDwnld    Number of B per second.
               /// @param [out]  _seedLimit     Max seed %.
               /// @param [out]  _seedTimeout   Max seed timeout.
               bool getLimit(t_int const _torrent_id, 
                             t_int & _limitUpld, 
                             t_int & _limitDwnld, 
                             t_int &_seedLimit, 
                             t_long &_seedTimeout);

               /// Get status of a torrent.
               /// @param [in]  _torrent_id  Context ID of the torrent to get status of.
               /// @param [out] _destination Pointer to the object which stores the status information.
               /// @return True if operation was successful, false if not.
               bool getStatus(t_int const _torrent_id, 
                              btg::core::Status & _destination);

               /// Get status of list of torrents.
               /// @return True if operation was successful, false if not.
               bool getStatus(t_intList const& _contexts, 
                              t_statusList & _status);

               /// Get status of all torrents.
               /// @return True if operation was successful, false if not.
               bool getStatusAll(t_statusList & _vstatus);

               /// Get a list of files together with information about
               /// which pieces of the files was downloaded.
               /// @param [in] _torrent_id Context ID of the torrent to get
               /// file information about.
               /// @param [out] _vfileinfo List of btg::core::fileInformation objects.
               /// @return True if operation was successful, false if not.
               bool getFileInfo(t_int const _torrent_id, t_fileInfoList & _vfileinfo);

               /// Get a list of peers.
               /// 
               /// @param _torrent_id   Get peer for context identified by this id.
               /// @param _peerlist     Reference to list of peers.
               /// @param _peerExOffset Extended peer info, offset.
               /// @param _peerExCount  Extended peer info, count.
               /// @param _peerExList   Extended peer info, list.
               /// @return True - success, false - otherwise.
               bool getPeers(t_int const _torrent_id, 
                             t_peerList & _peerlist,
                             t_uint * _peerExOffset = 0, 
                             t_uint * _peerExCount = 0,
                             t_peerExList * _peerExList = 0);

               /// Get a list of trackers belonging to a certain torrent.
               bool getTrackers(t_int const _torrent_id, t_strList & _trackers);

               /// Get the list of selected files.
               bool getSelectedFiles(t_int const _torrent_id,
                                     btg::core::selectedFileEntryList & _file_list);

               /// Set the files to download. See checkSelectedFiles
               /// and applySelectedFiles.
               bool setSelectedFiles(t_int const _torrent_id,
                                     btg::core::selectedFileEntryList const& _file_list);

               /// Check the list of files from a client against the stored list.
               bool checkSelectedFiles(btg::core::selectedFileEntryList const& _l1,
                                       btg::core::selectedFileEntryList const& _l2) const;
               
               /// Update the stored list of selected files.
               bool applySelectedFiles(torrentInfo* ti,
                                       btg::core::selectedFileEntryList const& _input);

               /// Get a list of contexts.
               /// @return List of torrents.
               t_intList getContexts() const;

               /// Write the filename assosiated with the torrent id to the
               /// second parameter.
               /// @param [in]  _torrent_id  Context ID of a torrent.
               /// @param [out] _destination Output filename.
               /// @return True if operation was successful, false if not.
               bool getFilename(t_int const _torrent_id, std::string& _destination);

               /// Get the full filename and the contents of the
               /// torrent file.
               bool getFile(t_int const _torrent_id,
                            std::string & _filename, 
                            btg::core::sBuffer & _buffer) const;

               /// Tell this context if there is a client attached or not.
               void setClientAttached(bool const _clientAttached);

               /// Returns true if a client is attached.
               bool isClientAttached() const;

               /// Stop seeding if the torrent has meet the seeding limits.
               void checkSeedLimits();

               /// Update elapsed or seed counter.
               void updateElapsedOrSeedCounter();

               /// Clean a specific old (seeding or finished and stopped) torrent.
               bool clean(t_int _contextID, t_strList & _files, t_intList & _contextIDs);

               /// Clean old (seeding or finished and stopped) torrents.
               bool clean(t_strList & _files, t_intList & _contextIDs);

               /// Remove fast resume data from temporary storage
               bool removeFastResumeData(t_int const _torrent_id);

               /// Used to remove the .torrent file from the temporary storage
               bool removeTorrentFile(t_int const _torrent_id);

               /// Used to move an uncompleted by some reasons download
               /// to the working directory (workDir_).
               /// @param [in]  _torrent_id  Context ID of a torrent.
               /// @return True if operation was successful, false if not.
               bool moveToWorkingDir(t_int const _torrent_id);

               /// Used to move a finished download to the seeding
               /// directory (seedDir_).
               /// @param [in]  _torrent_id  Context ID of a torrent.
               /// @return True if operation was successful, false if not.
               bool moveToSeedingDir(t_int const _torrent_id);

               /// Used to move a finished download to the destination
               /// directory (outputDir_).
               /// @param [in]  _torrent_id  Context ID of a torrent.
               /// @return True if operation was successful, false if not.
               bool moveToDestinationDir(t_int const _torrent_id);

               /// Handle any alerts produced by libtorrent.
               void handleAlerts();

               /// Handle saved alerts.
               void handleSavedAlerts();

               /// Get the port range use by this context.
               std::pair<t_int, t_int> getPortRange() const;
            private:
               /// Handle a single alert.
               void handleAlert(libtorrent::alert* _alert);
               
               /// Wait for a certain type of alert.
               /// 
               /// Used for blocking until fast file is written to
               /// disk, which is done in an async manner.
               void waitForResumeDataAlert();
            private:
               /// Handle libtorrent alert.
               void handleBannedHost(libtorrent::peer_ban_alert* _alert);

               /// Handle libtorrent alert.
               void handleFinishedTorrent(libtorrent::torrent_finished_alert* _alert);

               /// Handle libtorrent alert.
               void handlePeerError(libtorrent::peer_error_alert* _alert);
#if BTG_LT_0_14
               void handleTrackerAlert(libtorrent::tracker_error_alert* _alert);
#else
               /// Handle libtorrent alert.
               void handleTrackerAlert(libtorrent::tracker_alert* _alert);
#endif
               /// Handle libtorrent alert.
               void handleTrackerReplyAlert(libtorrent::tracker_reply_alert* _alert);

               /// Handle libtorrent alert.
               void handleTrackerWarningAlert(libtorrent::tracker_warning_alert* _alert);

#if BTG_LT_0_14
               /// Handle libtorrent resume data
               void handleResumeDataAlert(libtorrent::save_resume_data_alert* _alert);

               void handleResumeDataFailedAlert(libtorrent::save_resume_data_failed_alert* _alert);
#endif

#if BTG_LT_0_14
               /// Handle torrent state change
               void handleStateChangeAlert(libtorrent::state_changed_alert* _alert);
#endif
            public:
#if BTG_OPTION_SAVESESSIONS
               /// Session saving: serialize the torrents so they can be reloaded later.
               bool serialize(btg::core::externalization::Externalization* _e, 
                              bool const _dumpFastResume);
               /// Session saving: deserialize data into the torrents.
               /// @param [in] _e       Pointer to the externalization interface used.
               /// @param [in] _version Version of the file
               bool deserialize(btg::core::externalization::Externalization* _e, t_uint _version);
#endif // BTG_OPTION_SAVESESSIONS

               /// Shutdown this context.
               ///
               /// See the shutdown method in the session list class
               /// and eventhandler class.
               void shutdown();

               /// Indicates if dht is enabled.
               bool dhtEnabled() const;

               /// Indicates if encryption is enabled.
               bool encryptionEnabled() const;

               /// Get the temp directory used by this context.
               std::string getTempDir() const;

               /// Update the IP filter which is used.
               void updateFilter(IpFilterIf* _filter);

               /// Destructor.
               ~Context();
            private:
               /// Indicates if this class should log verbose
               /// messages.
               bool const verboseFlag_;

               /// Pointer to the daemon configuration.
               const daemonConfiguration* config_;

               /// The username used.
               std::string username_;

               /// The path to where to save temporary files.
               std::string tempDir_;

               /// The path to where to save the torrent files.
               std::string workDir_;

               /// The path to where to torrent files that are seeding are moved to.
               std::string seedDir_;

               /// The path to where to save the finished torrent files.
               std::string outputDir_;

               /// Indicates if the session was bound to a specific interface.
               bool interface_used_;

               /// Session was bound to this specific interface.
               std::string interface_;

               /// Pointer to port manager.
               portManager*  portMgr;
               
               /// Pointer to limit manager.
               limitManager* limitMgr_;

               /// Flag to tell if this instance is configured.
               bool          configured;

               /// Default per-torrent limit: upload.
               t_int default_torrent_uploadLimit_;

               /// Default per-torrent limit: download.
               t_int default_torrent_downloadLimit_;

               /// Default per-torrent limit: seed percent.
               t_int default_torrent_seedLimit_;

               /// Default per-torrent limit: seed time in minutes.
               t_long default_torrent_seedTimeout_;

               /// The port range used for listening for incoming
               /// connections used by this context.
               std::pair<t_uint, t_uint> listen_port_range_;

               /// Indicates if DHT is being used.
               bool                    useDHT_;
               /// Indicates if DHT was enabled.
               bool                    enabledDHT_;
               /// If DHT was enabled, the port used is contained in
               /// this member.
               t_uint                  DHTport_;

               /// Indicates that encryption shall be used.
               bool                   useEncryption_;
               // libtorrent objects:
               /// Pointer to the libtorrent session object.
               libtorrent::session*                      torrent_session;

               /// Proxy used for async shutdown libtorrent sessions.
               libtorrent::session_proxy                 torrent_session_proxy;

               /// Counter used to index torrent sessions.
               t_int                                     global_counter;

               /// IDs of torrent sessions.
               t_intList                                 torrent_ids;

               /// Map of torrent information.
               std::map<t_int, torrentInfo*>             torrents;
               
               /// where storage for torrent is
               enum torrentStorage { tsWork, tsSeed, tsDest };
               
               /// Map of torrent storage
               std::map<t_int, torrentStorage>           torrent_storage;

               /// Flag which tells if a client is attached.
               bool                                      clientAttached;

               /// Indicates if os specific paths were enabled before.
               static bool                               set_boost_native_paths;

               /// String added to a filename to turn it into fast
               /// resume filename.
               std::string const                         fastResumeFileNameEnd;

               /// Libtorrent settings currently used.
               libtorrent::session_settings              session_settings_;

               /// Keep track of added files.
               fileTrack*                                filetrack_;

               /// Pointer to the filter used.
               IpFilterIf*                               filter_;

#if BTG_OPTION_EVENTCALLBACK
               /// Callback manager.
               callbackManager*                          cbm_;
#endif // BTG_OPTION_EVENTCALLBACK

               /// Progress monitor.
               Progress                                  progress_;

               /// Enable/disable using the torrent name from the
               /// tracker.
               bool const                                useTorrentName_;
#if BTG_LT_0_14
               /// Allocation mode used.
               libtorrent::storage_mode_t                allocation_mode_;
#endif
               /// Move the data belonging to a torrent file to another directory.
               /// @return True - file was moved, false otherwise.
               bool moveToDirectory(t_int const _torrent_id, std::string const& _destination_dir);

               /// Add a torrent file to this context. Starts stopped.
               /// @param [in] _torrent_filename   Filename of the torrent file to add (expected to be in tempDir).
               /// @param [out] _handle_id         The ID of the added context. Defined only if the torrent was added. Check the returned status.
               /// @return One of the addResult enums
               addResult add(std::string const& _torrent_filename,
                             t_int & _handle_id);

               /// Check that a handle really exists before trying to use
               /// libtorrent functions on it. If it exists, a pointer to the torrentinfo class is returned.
               /// @param [in] _torrent_id The ID of the libtorrent handle.
               /// @return 0 if not found, else pointer to the torrentinfo.
               torrentInfo* getTorrentInfo(t_int const _torrent_id) const;

               /// Check that file info can be obtained from a handle.
               /// Libtorrent either casts an exception or crashes or
               /// a file info is requested during file checking.
               /// @param [in] _torrent_id The ID of the libtorrent handle.
               /// @return True - success. False - failture.
               bool safeForFileInfo(t_int const _torrent_id);

               /// Set http settings for normal usage.
               void setNormalHttpSettings();

               /// Set proxy.
               void setProxyHttpSettings(btg::core::addressPort const& _proxy);

               /// Called before the destructor, sets timeouts to the
               /// closing of the libtorrent session will be faster.
               void setDestructionlHttpSettings();

               /// Write the fast resume data for all torrents.
               bool writeResumeData();

               /// Write the fast resume data for the torrent.
               /// @param [in] _torrent_id The ID of the libtorrent handle.
               bool writeResumeData(t_int const _torrent_id);

               /// Returns true if the torrent file pointed by the
               /// argument has fast resume data.
               bool hasFastResumeData(std::string const& _torrent_filename) const;

#if BTG_OPTION_EVENTCALLBACK
               /// Simplify the list - only (unique) topdirs and files should be listed.
               void removeFileInfoDetails(t_fileInfoList const& _source,
                                          t_strList & _destination);
#endif // BTG_OPTION_EVENTCALLBACK

               /// Convert an entry into a torrent info.
               /// @return True - converted. False - conversion failed.
#if (BTG_LT_0_12 || BTG_LT_0_13)
               bool entryToInfo(libtorrent::entry const& _input,
                                libtorrent::torrent_info & _output) const;

               /// Convert an entry to a list of contained files.
               /// @return True - converted. False - conversion failed.
               bool entryToFiles(libtorrent::entry const& _input,
                                 std::vector<std::string> & _output) const;
#endif

#if BTG_LT_0_14
               bool torrentInfoToFiles(libtorrent::torrent_info const& _tinfo,
                                       std::vector<std::string> & _output) const;
#endif                                                
               /// Find out if data of a torrent is present in the seed dir.
               /// @param [in] _torrent_info Torrent info.
               /// @return True - data is present, false otherwise.
               bool dataPresentInSeedDir(libtorrent::torrent_info const& _torrent_info);

               /// Get a list which consists of:
               /// 1. Files.
               /// 2. Unique directories - longest directories first.
               bool getListOfEntities(t_int const _torrent_id,
                                      std::string const& _directory,
                                      std::vector<std::string> & _entities);

               /// Given a handle, which is given in libtorrent
               /// alerts, find its corresponding id used by the
               /// daemon and set the pointer to the struct used to
               /// keep information about a torrent.
               /// @param [in]  _handle     Const reference to libtorrent handle.
               /// @param [out] _torrent_id Reference to a context id.
               /// @param [out] _ti         Reference to a pointer to torrent information.
               /// @return True on success, false otherwise.
               bool getIdFromHandle(libtorrent::torrent_handle const& _handle,
                                    t_int & _torrent_id,
                                    torrentInfo* & _ti);
               /// Enable the use of DHT.
               /// Should only be called once, as one port is used for DHT.
               void enableDHT(libtorrent::entry const& _dht_state = libtorrent::entry());

               /// Disable the use of DHT.
               void disableDHT();

               /// Enable encryption.
               void enableEncryption();

               /// Set peer ID (read from configuration, converting it
               /// to a format used by libtorrent).
               void setPeerIdFromConfig();
               
               /// Convert an enum to a string, for debugging.
               std::string toString(addResult const _addresult) const;

#if (BTG_LT_0_14)
               /// Convert a libtorrent bitfield into a vector of bits.
               /// 
               /// Libtorrent 0.14 changed the way a torrent's pieces
               /// are represented.
               void bitfieldToVector(libtorrent::bitfield const& _input, 
                                     std::vector<bool> & _output) const;

               /// Mutex used to control access to the members
               /// of this class from the outside.
               boost::mutex interfaceMutex_;

               std::vector<libtorrent::torrent_alert*> saved_alerts_;
#endif
            private:
               /// Copy constructor.
               Context(Context const& _c);
               /// Assignment operator.
               Context& operator=(Context const& _c);
            };
         /** @} */

      } // namespace daemon
} // namespace btg

#endif

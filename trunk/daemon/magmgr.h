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

#ifndef MAGMGR_H
#define MAGMGR_H

#include <daemon/http/httpmgr.h>
#include <bcore/logable.h>
#include <bcore/urlstatus.h>
#include <daemon/opid.h>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

namespace libtorrent
{
   class torrent_handle;
};

namespace btg
{
   namespace daemon
   {
      /// Creation status when adding torrent from URL.
      enum MagnetCreateStatus
      {
         MAG_UNDEF         = 0, //!< Not defined.
         MAG_NO_META       = 1, //!< Downloading torrent from peers.
         MAG_META_FAILED   = 2, //!< Failed to download torrent.
         MAG_DOWNLOADED    = 3, //!< Downloaded torrent.
         MAG_CREATED       = 4, //!< Created.
         MAG_CREATE_FAILED = 5, //!< Not created.
         MAG_ABORTED       = 6  //!< Aborted.
      };

      /// Mapping between session and URL download.
      struct MagnetIdSessionMapping
      {
         /// Contructor.
         MagnetIdSessionMapping(t_uint _id, 
                                t_long const _session,
                                std::string const& _userdir,
                                std::string const& _filename, 
                                std::string const& _URI,
                                bool const _start
                                );
         
         /// Download id.
         t_uint          id;
         /// Session id.
         long            session;
         std::string     userdir;
         /// The destianation file name.
         std::string     filename;
         /// URI.
         std::string     URI;
         /// Should the context be started after adding.
         bool            start;
         /// Handle used for downloading torrent files from peers.
         libtorrent::torrent_handle* handle;
         /// Status.
         MagnetCreateStatus status;
         /// Age in (x * url timer duration).
         t_uint          age;

         t_int           tempTorrentId;
      };

      class fileTrack;
      class sessionList;

      /// Manages URL downloads in the daemon.
      class magnetManager: public btg::core::Logable
         {
         public:
            /// Constructor.
            magnetManager(btg::core::LogWrapperType _logwrapper,
                          bool const _verboseFlag,
                          fileTrack* _filetrack,
                          sessionList* _sessionlist,
                          btg::daemon::opId & _opid);

            /// Handle downloads - called by timer.
            void checkMagnetDownloads();

            /// Add a magnet URI to a session.
            t_uint add(t_long const _session,
                       std::string const& _userdir,
                       std::string const& _torrent_filename, 
                       std::string const& _magnet_URI,
                       bool const _start);

            bool belongs(const t_uint _id);

            bool getStatus(const t_uint _id, t_uint & _urlstat);

            bool getDlProgress(const t_uint _id, t_uint & _dltotal, t_uint & _dlnow, t_uint & _dlspeed);

            bool abort(const t_uint _id);

            /// Number of requests.
            t_uint size();

            void setState(const t_uint _id, const MagnetCreateStatus _state);

            /// Destructor.
            ~magnetManager();
         protected:
            /// Get a mapping when having an URL id.
            std::vector<MagnetIdSessionMapping>::iterator getUrlMapping(t_uint _id);

            void abort(MagnetIdSessionMapping & mapping);
            void checkState(MagnetIdSessionMapping & mapping);

            bool startTorrentDownload(MagnetIdSessionMapping & _mapping);
            void stopTorrentDownload(MagnetIdSessionMapping & _mapping);
         protected:
            void handle_MAG_UNDEF(MagnetIdSessionMapping & mapping);
            void handle_MAG_NO_META(MagnetIdSessionMapping & mapping);
            void handle_MAG_DOWNLOADED(MagnetIdSessionMapping & mapping);
            void handle_MAG_CREATED(MagnetIdSessionMapping & mapping);
            void handle_MAG_CREATE_FAILED(MagnetIdSessionMapping & mapping);
         protected:
            /// Verbose flag.
            bool         verboseFlag;

            /// Pointer to file tracking object.
            fileTrack*   filetrack;

            /// Pointer to list of sessions.
            sessionList* sessionlist;

            btg::daemon::opId & opid;

            std::vector<MagnetIdSessionMapping> magnetIdSessions;

            /// Mutex used to control access to the members
            /// of this class from the outside.
            boost::mutex     interfaceMutex_;
         };

      /** @} */

   } // namespace daemon
} // namespace btg
            
#endif // MAGMGR_H


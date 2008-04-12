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

#ifndef URLMGR_H
#define URLMGR_H

#include <daemon/http/httpmgr.h>
#include <bcore/logable.h>
#include <bcore/urlstatus.h>

namespace btg
{
   namespace daemon
   {
      /// Creation status when adding torrent from URL.
      enum UrlCreateStatus
      {
         UCS_UNDEF         = 0, //!< Not defined.
         UCS_CREATED       = 1, //!< Created sucessfuly.
         UCS_CREATE_FAILED = 2  //!< Not created.
      };

      /// Mapping between session and URL download.
      struct UrlIdSessionMapping
      {
         /// Contructor.
         UrlIdSessionMapping(t_uint _id, 
                             long _session, 
                             std::string const& _userdir,
                             std::string const& _filename,
                             bool const _start);
         
         /// Download id.
         t_uint          id;
         /// Indicates if this download is valid.
         bool            valid;
         /// Session id.
         long            session;
         /// The destination directory.
         std::string     userdir;
         /// The destianation file name.
         std::string     filename;
         /// Should the context be started after adding.
         bool            start;
         /// Status.
         UrlCreateStatus status;
         /// Age in (x * url timer duration).
         t_uint          age;
      };

      class fileTrack;
      class sessionList;

      class urlManager: public btg::core::Logable
         {
         public:
            urlManager(btg::core::LogWrapperType _logwrapper,
                       bool const _verboseFlag,
                       fileTrack* _filetrack,
                       sessionList* _sessionlist);

            /// Handle downloads - called by timer.
            void checkUrlDownloads();

            t_uint size() const;

            bool unique(std::string const & _filename, 
                        std::string const & _userdir) const;

            bool abort(const t_uint _id);

            bool getStatus(const t_uint _id, 
                           btg::core::urlStatus & _urlstat);

            t_uint addMapping(std::string const& _url, 
                              std::string const& _userdir,
                              std::string const& _filename,
                              t_long const _session,
                              bool const _start);

            ~urlManager();
         protected:
            /// Handle a single download.
            void addUrl(t_uint _id);

            /// Create torrent from the file which was downloaded.
            void addUrl(UrlIdSessionMapping & _mapping);

            /// Clean up, if the torrent could not be downloaded.
            void removeUrl(UrlIdSessionMapping & _mapping);
            
            /// Get a mapping when having an URL id.
            std::vector<UrlIdSessionMapping>::iterator getUrlMapping(t_uint _id);
         protected:
            /// Verbose flag.
            bool         verboseFlag;

            /// Pointer to file tracking object.
            fileTrack*   filetrack;

            /// Pointer to list of sessions.
            const sessionList* sessionlist;

            /// Http manager used for all downloads.
            btg::daemon::http::httpDlManager httpmgr;

            /// Mapping between URL id and session.
            std::vector<UrlIdSessionMapping> urlIdSessions;
         };

      /** @} */

   } // namespace daemon
} // namespace btg
            
#endif // URLMGR_H


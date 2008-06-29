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

#ifndef FILEMGR_H
#define FILEMGR_H

#include <string>
#include <map>

#include <bcore/sbuf.h>
#include <bcore/logable.h>

#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include <daemon/opid.h>

namespace btg
{
   namespace daemon
   {
      /**
       * \addtogroup daemon
       */
      /** @{ */

      /// File state information.
      struct fileData
      {
         /// State.
         enum Status
            {
               UNDEF        = 0, //!< Undefined state.
               INIT         = 1, //!< Added filename, wait for parts.
               WORK         = 2, //!< Received at least one part.
               DONE         = 3, //!< Received all parts.
               DLERROR      = 4, //!< Error receiving required parts.
               CREATED      = 5, //!< Created torrent.
               CREATE_ERROR = 6, //!< Unable to create torrent.
               ABORTED      = 7  //!< Aborted.
            };
         
         /// Constructor.
         fileData(std::string const& _dir,
                  std::string const& _filename, 
                  t_uint _parts,
                  bool   _start);
         
         /// Directory.
         const std::string    dir;
         /// Filename.
         const std::string    filename;
         /// Number of parts.
         const t_uint         parts;
         /// Indicates if the torrent should be started after
         /// uploading (to the daemon).
         const bool           start;
         /// Current part received.
         t_uint               current_part;
         /// List of buffers received.
         std::vector<btg::core::sBuffer> buffers;
         /// State.
         Status               status;

         /// Indicates that this download is valid, used to mark
         /// downloads which can be delted.
         bool                 valid;

         /// Age in (x * file timer duration).
         t_uint               age;
      };

      class fileTrack;

      /// Tracks file uploads in the daemon.
      class fileManager: public btg::core::Logable
         {
         public:
            /// Constructor.
            fileManager(btg::core::LogWrapperType _logwrapper,
                        fileTrack* _filetrack,
                        btg::daemon::opId & _opid);

            /// Add a file.
            t_uint addFile(std::string const& _dir,
                           std::string const& _filename,
                           t_uint const _parts,
                           bool const _start);

            /// Add a piece of a file.
            bool addPiece(const t_uint _id, 
                          const t_uint _num,
                          btg::core::sBuffer const& _data);

            /// Get the id of the next piece.
            bool nextPiece(const t_uint _id, t_uint & _piece) const;

            /// Get the state of a file upload.
            fileData::Status getStatus(const t_uint _id);

            /// Get the whole file after a completed upload.
            bool getResult(const t_uint _id, 
                           btg::core::sBuffer & _buffer);

            /// Invalidate an upload.
            void invalidate(const t_uint _id);

            /// Set state of an upload.
            void setState(const t_uint _id, 
                          fileData::Status const _status);

            /// Update the age of an upload.
            void updateAge();

            /// Abort a download in progress.
            bool abort(const t_uint _id);

            /// Remove stored data for an upload.
            void removeData(const t_uint _id);

            /// Remove dead uploads.
            void removeDead();

            /// Get information about a file.
            void getFileInfo(const t_uint _id,
                             std::string & _dir,
                             std::string & _filename,
                             bool        & _start);

            /// Get the number of active downloads.
            t_uint size() const;

            /// Destructor.
            virtual ~fileManager();
         protected:
            /// File tracker used.
            fileTrack* filetrack;

            /// Used to create unique ids used for tracking status of
            /// commands or aborting the same commands.
            btg::daemon::opId & opid;

            /// Get the current ID - start() returns this ID each time
            /// its called.
            t_uint getCurrentId();
            
            /// Maps ID to file information.
            std::map<t_uint, fileData> files;
         };

      /** @} */

   } // namespace daemon
} // namespace btg

#endif // FILEMGR


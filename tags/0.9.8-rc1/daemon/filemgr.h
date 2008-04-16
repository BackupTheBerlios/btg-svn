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

namespace btg
{
   namespace daemon
   {
      /**
       * \addtogroup daemon
       */
      /** @{ */

      struct fileData
      {
         enum Status
            {
               UNDEF        = 0, // Undefined state.
               INIT         = 1, // Added filename, wait for parts.
               WORK         = 2, // Received at least one part.
               DONE         = 3, // Received all parts.
               DLERROR      = 4, // Error receiving required parts.
               CREATED      = 5, // Created torrent.
               CREATE_ERROR = 6, // Unable to create torrent.
               ABORTED      = 7  // Aborted.
            };
         
         fileData(std::string const& _dir,
                  std::string const& _filename, 
                  t_uint _parts,
                  bool   _start);
         
         const std::string    dir;
         const std::string    filename;
         const t_uint         parts;
         const bool           start;
         t_uint               current_part;
         std::vector<btg::core::sBuffer> buffers;
         Status               status;

         /// Indicates that this download is valid, used to mark
         /// downloads which can be delted.
         bool                 valid;

         /// Age in (x * file timer duration).
         t_uint               age;
      };

      class fileTrack;

      class fileManager: public btg::core::Logable
         {
         public:
            /// Constructor.
            fileManager(btg::core::LogWrapperType _logwrapper,
                        fileTrack* _filetrack);

            t_uint addFile(std::string const& _dir,
                           std::string const& _filename,
                           t_uint const _parts,
                           bool const _start);

            bool addPiece(const t_uint _id, 
                          const t_uint _num,
                          btg::core::sBuffer const& _data);

            bool nextPiece(const t_uint _id, t_uint & _piece) const;

            fileData::Status getStatus(const t_uint _id);

            bool getResult(const t_uint _id, 
                           btg::core::sBuffer & _buffer);

            void invalidate(const t_uint _id);

            void setState(const t_uint _id, 
                          fileData::Status const _status);

            void updateAge();

            /// Abort a download in progress.
            bool abort(const t_uint _id);

            void removeData(const t_uint _id);

            void removeDead();

            void getFileInfo(const t_uint _id,
                             std::string & _dir,
                             std::string & _filename,
                             bool        & _start);

            /// Get the number of active downloads.
            t_uint size() const;

            /// Destructor.
            virtual ~fileManager();
         protected:
            fileTrack* filetrack;
            /// Get the current ID - start() returns this ID each time
            /// its called.
            t_uint getCurrentId();

            /// The current ID.
            t_uint         current_id;
            /// The max ID - used to decide when to wrap.
            const t_uint   max_id;
            
            std::map<t_uint, fileData> files;
         };

      /** @} */

   } // namespace daemon
} // namespace btg

#endif // FILEMGR


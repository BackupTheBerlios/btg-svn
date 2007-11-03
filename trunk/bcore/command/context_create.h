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
 * Id    : $Id$
 */

#ifndef COMMAND_CREATE_CONTEXT_H
#define COMMAND_CREATE_CONTEXT_H

#include "context.h"
#include <bcore/dbuf.h>
#include <bcore/sbuf.h>

#include <string>

namespace btg
{
   namespace core
   {
      /**
       * \addtogroup commands
       */
      /** @{ */

      /// Create a context - tell the daemon to read the included
      /// filename and create it as a context.
      class contextCreateWithDataCommand: public Command
      {
      public:
         /// Constructor.
         contextCreateWithDataCommand();

         /// Constructor.
         /// @param [in] _filename     The filename of the torrent file to create.
         /// @param [in] _torrent_file The contents (raw bytes) of the file to create.
         /// @param [in] _start        Indicates if the torrent will be started after creation.
         contextCreateWithDataCommand(std::string const& _filename,
                                      sBuffer const& _torrent_file,
                                      bool const _start);

         std::string toString() const;
         bool serialize(btg::core::externalization::Externalization* _e) const;
         bool deserialize(btg::core::externalization::Externalization* _e);

         /// Get the contained filename.
         sBuffer getFile() const
         {
            return torrent_file;
         }

         /// Get the contained filename.
         std::string getFilename() const
            {
               return filename;
            }

         /// Indicates if the torrent should be started after
         /// loading.
         bool getStart() const
         {
            return start;
         }

         /// Destructor.
         virtual ~contextCreateWithDataCommand();
      private:
         /// The filename of the torrent file to create.
         std::string filename;

         /// The contents of the file.
         sBuffer torrent_file;

         /// Indicates if the torrent should be started after
         /// creation.
         bool start;
      };

      /** @} */

   } // namespace core
} // namespace btg

#endif

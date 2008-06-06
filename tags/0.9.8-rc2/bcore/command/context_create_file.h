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
 * Id    : $Id: context_create_url.h 222 2008-03-21 00:55:17Z wojci $
 */

#ifndef COMMAND_CREATE_CONTEXT_FILE_H
#define COMMAND_CREATE_CONTEXT_FILE_H

#include <bcore/command/context.h>
#include <bcore/filestatus.h>
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

      /// Upload a file to the daemon.
      class contextCreateFromFileCommand: public Command
      {
      public:
         /// Constructor.
         contextCreateFromFileCommand();

         /// Constructor.
         /// @param [in] _filename      The filename of the torrent file to create.
         /// @param [in] _numberOfParts The file will be sent as this number of parts.
         /// @param [in] _start         Indicates if the torrent will be started after creation.
         contextCreateFromFileCommand(std::string const& _filename,
                                      t_uint _numberOfParts,
                                      bool const _start);

         bool serialize(btg::core::externalization::Externalization* _e) const;
         bool deserialize(btg::core::externalization::Externalization* _e);

         /// Get the contained filename.
         std::string getFilename() const
            {
               return filename;
            }
         
         /// Get the number of parts this file is sent in.
         t_uint numberOfParts() const
         {
            return numberOfParts_;
         }

         /// Indicates if the torrent should be started after
         /// loading.
         bool getStart() const
         {
            return start;
         }

         /// Destructor.
         virtual ~contextCreateFromFileCommand();
      private:
         /// The filename of the torrent file to create.
         std::string filename;

         /// The number of parts this file is sent in.
         t_uint numberOfParts_;

         /// Indicates if the torrent should be started after
         /// creation.
         bool start;
      };

      /// A file is being opened.
      class contextCreateFromFileResponseCommand: public Command
      {
      public:
         /// Constructor.
         contextCreateFromFileResponseCommand();

         /// Constructor.
         /// @param [in] _id The id of the file which is being opened.
         contextCreateFromFileResponseCommand(t_uint const _id);

         bool serialize(btg::core::externalization::Externalization* _e) const;
         bool deserialize(btg::core::externalization::Externalization* _e);

         /// Get the contained id.
         t_uint id() const
         {
            return id_;
         }

         /// Destructor.
         virtual ~contextCreateFromFileResponseCommand();
      private:
         /// The id of the file to load.
         t_uint id_;
      };

      /// Part of a file upload.
      class contextCreateFromFilePartCommand: public Command
      {
      public:
         /// Constructor.
         contextCreateFromFilePartCommand();

         /// Constructor.
         contextCreateFromFilePartCommand(t_uint _id,
                                          t_uint _part,
                                          sBuffer const& _data);

         bool serialize(btg::core::externalization::Externalization* _e) const;
         bool deserialize(btg::core::externalization::Externalization* _e);

         /// Id this part belongs to.
         t_uint id() const
         {
            return id_;
         }

         /// Part number.
         t_uint part() const
         {
            return part_;
         }

         /// Contained data.
         sBuffer const& data() const
         {
            return data_;
         }

         /// Destructor.
         virtual ~contextCreateFromFilePartCommand();
      private:
         /// Id of the upload this part belongs to.
         t_uint  id_;
         /// Part number.
         t_uint  part_;
         /// Data.
         sBuffer data_;
      };

      /// Get the status of a file download.
      class contextFileStatusCommand: public Command
      {
      public:
         /// Constructor.
         contextFileStatusCommand();

         /// Constructor.
         /// @param [in] _id The id of the file the daemon is downloading.
         contextFileStatusCommand(t_uint const _id);

         bool serialize(btg::core::externalization::Externalization* _e) const;
         bool deserialize(btg::core::externalization::Externalization* _e);

         /// Get the contained id.
         t_uint id() const
         {
            return id_;
         }

         /// Destructor.
         virtual ~contextFileStatusCommand();
      private:
         /// The id of the file to load.
         t_uint id_;
      };

      /// Status of a file download.
      class contextFileStatusResponseCommand: public Command
      {
      public:
         /// Constructor.
         contextFileStatusResponseCommand();

         /// Constructor.
         /// @param [in] _id     The ID of the file the daemon is downloading.
         /// @param [in] _status The status.
         contextFileStatusResponseCommand(t_uint _id, 
                                          btg::core::fileStatus const _status);

         bool serialize(btg::core::externalization::Externalization* _e) const;
         bool deserialize(btg::core::externalization::Externalization* _e);

         /// Get the contained id.
         t_uint id() const
         {
            return id_;
         }
         
         /// Get the contained status.
         btg::core::fileStatus status() const
            {
               return status_;
            }

         /// Destructor.
         virtual ~contextFileStatusResponseCommand();
      private:
         /// The id of the file to load.
         t_uint id_;
         /// Status.
         btg::core::fileStatus status_;
      };

      /// Get the status of a URL download.
      class contextFileAbortCommand: public Command
      {
      public:
         /// Constructor.
         contextFileAbortCommand();

         /// Constructor.
         /// @param [in] _id The id of the URL the daemon is downloading.
         contextFileAbortCommand(t_uint const _id);

         bool serialize(btg::core::externalization::Externalization* _e) const;
         bool deserialize(btg::core::externalization::Externalization* _e);

         /// Get the contained id.
         t_uint id() const
         {
            return id_;
         }

         /// Destructor.
         virtual ~contextFileAbortCommand();
      private:
         /// The id of the url to load.
         t_uint id_;
      };

      /** @} */

   } // namespace core
} // namespace btg

#endif // COMMAND_CREATE_CONTEXT_FILE_H


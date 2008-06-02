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
 * Id    : $Id$
 */

#ifndef COMMAND_CREATE_CONTEXT_URL_H
#define COMMAND_CREATE_CONTEXT_URL_H

#include <bcore/command/context.h>
#include <bcore/urlstatus.h>
#include <string>

namespace btg
{
   namespace core
   {
      /**
       * \addtogroup commands
       */
      /** @{ */

      /// Create a context - tell the daemon to read an url and create
      /// it as a context.
      class contextCreateFromUrlCommand: public Command
      {
      public:
         /// Constructor.
         contextCreateFromUrlCommand();

         /// Constructor.
         /// @param [in] _filename     The filename of the torrent file to create.
         /// @param [in] _url          The url of the file to use.
         /// @param [in] _start        Indicates if the torrent will be started after creation.
         contextCreateFromUrlCommand(std::string const& _filename,
                                     std::string const& _url,
                                     bool const _start);

         std::string toString() const;
         bool serialize(btg::core::externalization::Externalization* _e) const;
         bool deserialize(btg::core::externalization::Externalization* _e);

         /// Get the contained url.
         std::string getUrl() const
         {
            return url;
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
         virtual ~contextCreateFromUrlCommand();
      private:
         /// The filename of the torrent file to create.
         std::string filename;

         /// The url.
         std::string url;

         /// Indicates if the torrent should be started after
         /// creation.
         bool start;
      };

      /// The ID of a url which the daemon started to download.  
      class contextCreateFromUrlResponseCommand: public Command
      {
      public:
         /// Constructor.
         contextCreateFromUrlResponseCommand();

         /// Constructor.
         contextCreateFromUrlResponseCommand(t_uint const _id);

         bool serialize(btg::core::externalization::Externalization* _e) const;
         bool deserialize(btg::core::externalization::Externalization* _e);

         /// Get the contained id.
         t_uint id() const
         {
            return id_;
         }

         /// Destructor.
         virtual ~contextCreateFromUrlResponseCommand();
      private:
         /// The id of the url to load.
         t_uint id_;
      };

      /// Get the status of a URL download.
      class contextUrlStatusCommand: public Command
      {
      public:
         /// Constructor.
         contextUrlStatusCommand();

         /// Constructor.
         /// @param [in] _id The id of the URL the daemon is downloading.
         contextUrlStatusCommand(t_uint const _id);

         bool serialize(btg::core::externalization::Externalization* _e) const;
         bool deserialize(btg::core::externalization::Externalization* _e);

         /// Get the contained id.
         t_uint id() const
         {
            return id_;
         }

         /// Destructor.
         virtual ~contextUrlStatusCommand();
      private:
         /// The id of the url to load.
         t_uint id_;
      };

      /// Status of a URL download.
      class contextUrlStatusResponseCommand: public Command
      {
      public:
         /// Constructor.
         contextUrlStatusResponseCommand();

         /// Constructor.
         /// @param [in] _id     The ID of the URL the daemon is downloading.
         /// @param [in] _status The status.
         contextUrlStatusResponseCommand(t_uint _id, 
                                         btg::core::urlStatus const _status);

         bool serialize(btg::core::externalization::Externalization* _e) const;
         bool deserialize(btg::core::externalization::Externalization* _e);

         /// Get the contained id.
         t_uint id() const
         {
            return id_;
         }
         
         /// Get the contained status.
         btg::core::urlStatus status() const
         {
            return status_;
         }
         
         /// Get progress information.
         bool getDlProgress(t_uint &_dltotal, t_uint &_dlnow, t_uint &_dlspeed) const
         {
            _dltotal = dltotal_;
            _dlnow = dlnow_;
            _dlspeed = dlspeed_;
            return bDlProgress_;
         }
         
         /// Set the download progress info
         void setDlProgress(t_uint _dltotal, t_uint _dlnow, t_uint _dlspeed)
         {
            bDlProgress_ = true;
            dltotal_ = _dltotal;
            dlnow_ = _dlnow;
            dlspeed_ = _dlspeed;
         }

         /// Destructor.
         virtual ~contextUrlStatusResponseCommand();
      private:
         /// The id of the url to load.
         t_uint id_;
         /// Status.
         btg::core::urlStatus status_;
         /// Whether download progress info present
         bool bDlProgress_;
         /// Total bytes to go
         t_uint dltotal_;
         /// Bytes already downloaded
         t_uint dlnow_;
         /// download speed
         t_uint dlspeed_;
      };

      /// Get the status of a URL download.
      class contextUrlAbortCommand: public Command
      {
      public:
         /// Constructor.
         contextUrlAbortCommand();

         /// Constructor.
         /// @param [in] _id The id of the URL the daemon is downloading.
         contextUrlAbortCommand(t_uint const _id);

         bool serialize(btg::core::externalization::Externalization* _e) const;
         bool deserialize(btg::core::externalization::Externalization* _e);

         /// Get the contained id.
         t_uint id() const
         {
            return id_;
         }

         /// Destructor.
         virtual ~contextUrlAbortCommand();
      private:
         /// The id of the url to load.
         t_uint id_;
      };

      /** @} */

   } // namespace core
} // namespace btg

#endif // COMMAND_CREATE_CONTEXT_URL_H


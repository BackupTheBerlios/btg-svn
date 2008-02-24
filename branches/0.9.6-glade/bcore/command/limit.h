/*
 * btg Copyright (C) 2005 Jesper Nyholm Jensen.
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

#ifndef COMMAND_LIMIT_H
#define COMMAND_LIMIT_H

#include "context.h"
#include "limit_base.h"

#include <bcore/dbuf.h>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup commands
          */
         /** @{ */

         /// Set global limits.
         class limitCommand: public Command, public limitBase
            {
            public:
               /// Default constructor.
               limitCommand();
               /// Constructor.
               /// @param [in] _limitBytesDwnld The number of
               /// bytes/second the upload limit should be set to; -1
               /// == no limit.
               /// @param [in] _limitBytesUpld The number of
               /// bytes/second the download limit should be set to;
               /// -1 == no limit.
               /// @param [in] _maxUplds Max uploads.
               /// @param [in] _maxConnections Max connections.
               limitCommand(t_int const  _limitBytesUpld,
                            t_int const  _limitBytesDwnld,
                            t_int const  _maxUplds,
                            t_long const _maxConnections);
               /// Get the upload limit.
               /// @return the upload limit.
               t_int getUploadLimit() const;
               /// Get the download limit.
               /// @return the download limit.
               t_int getDownloadLimit() const;

               /// Get the max number of uploads.
               t_int getMaxUplds() const;

               /// Get the max number of connections.
               t_int getMaxConnections() const;

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Destructor.
               virtual ~limitCommand();
            private:
               /// Download bandwidth limit in Bytes/second.
               t_int limitDwnld;
               /// Upload bandwidth limit in Bytes/second.
               t_int limitUpld;

               /// Max number of connected uploads.
               t_int maxUplds;

               /// Max session-global  number connections (when downloading).
               t_int maxConnections;

               /// Copy constructor.
               limitCommand(limitCommand const& _limitcommand);
               /// Assignment operator.
               limitCommand& operator= (limitCommand const& _limitcommand);
            };

         /// Get global limit status.
         class limitStatusCommand: public Command
            {
            public:
               /// Default constructor.
               limitStatusCommand();

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Destructor.
               virtual ~limitStatusCommand();
            private:
               /// Copy constructor.
               limitStatusCommand(limitStatusCommand const& _limitstatuscommand);
               /// Assignment operator.
               limitStatusCommand& operator= (limitStatusCommand const& _limitstatuscommand);
            };

         /// Response to global limit status.
         class limitStatusResponseCommand: public Command, public limitBase
            {
            public:
               /// Default constructor.
               limitStatusResponseCommand();
               /// Constructor.
               /// @param [in] _limitBytesDwnld The number of
               /// bytes/second the upload limit should be set to; -1
               /// == no limit.
               /// @param [in] _limitBytesUpld The number of
               /// bytes/second the download limit should be set to;
               /// -1 == no limit.
               /// @param [in] _maxUplds Max uploads.
               /// @param [in] _maxConnections Max connections.
               limitStatusResponseCommand(t_int const  _limitBytesUpld,
                                          t_int const  _limitBytesDwnld,
                                          t_int const  _maxUplds,
                                          t_long const _maxConnections);
               /// Get the upload limit.
               /// @return the upload limit.
               t_int getUploadLimit() const;
               /// Get the download limit.
               /// @return the download limit.
               t_int getDownloadLimit() const;

               /// Get the max number of uploads.
               t_int getMaxUplds() const;

               /// Get the max number of connections.
               t_int getMaxConnections() const;

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Destructor.
               virtual ~limitStatusResponseCommand();
            private:
               /// Download bandwidth limit in Bytes/second.
               t_int limitDwnld;
               /// Upload bandwidth limit in Bytes/second.
               t_int limitUpld;

               /// Max number of connected uploads.
               t_int maxUplds;

               /// Max session-global  number connections (when downloading).
               t_int maxConnections;

               /// Copy constructor.
               limitStatusResponseCommand(limitStatusResponseCommand const& _limitstatusreplycommand);
               /// Assignment operator.
               limitStatusResponseCommand& operator= (limitStatusResponseCommand const& _limitstatusreplycommand);
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif // COMMAND_LIMIT_H



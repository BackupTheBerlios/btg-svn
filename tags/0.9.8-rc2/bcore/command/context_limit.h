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

#ifndef COMMAND_CONTEXT_LIMIT_H
#define COMMAND_CONTEXT_LIMIT_H

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

         /// Set up-/download bandwidth of a context.
         class contextLimitCommand: public contextCommand, public limitBase
            {
            public:
               /// Default constructor.
               contextLimitCommand();
               /// Constructor.
               /// @param [in] _context_id The context ID of the
               /// context of which a status is required.
               /// @param [in] _limitBytesDwnld The number of
               /// bytes/second the upload limit should be set to; -1
               /// == no limit.
               /// @param [in] _limitBytesUpld The number of
               /// bytes/second the download limit should be set to;
               /// -1 == no limit.
               /// @param [in] _seedLimit How many % we should seed before torrent is stopped. -1 == no limit.
               /// @param [in] _seedTimeout How many seconds we should seed before torrent is stopped. -1 == no timeout.
               /// @param [in] _allContexts If set to true, return
               /// status of all contexts.
               contextLimitCommand(t_int const  _context_id,
                                   t_int const  _limitBytesUpld  = LIMIT_DISABLED,
                                   t_int const  _limitBytesDwnld = LIMIT_DISABLED,
                                   t_int const  _seedLimit   = LIMIT_DISABLED,
                                   t_long const  _seedTimeout = LIMIT_DISABLED,
                                   bool const _allContexts     = false);
               /// Get the upload limit.
               /// @return the upload limit.
               t_int getUploadLimit() const;
               /// Get the download limit.
               /// @return the download limit.
               t_int getDownloadLimit() const;
               /// Get the seed limit.
               /// @return the seed limit.
               t_int getSeedLimit() const;
               /// Get the seed timeout.
               /// @return the seed timeout.
               t_long getSeedTimeout() const;
               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);
               /// Destructor.
               virtual ~contextLimitCommand();
            private:
               /// Download bandwidth limit in Bytes/second.
               t_int limitDwnld;
               /// Upload bandwidth limit in Bytes/second.
               t_int limitUpld;
               /// Seed limit
               t_int seedLimit;
               /// Seed timeout
               t_long seedTimeout;

               /// Copy constructor.
               contextLimitCommand(contextLimitCommand& _contextLimitCommand);
               /// Assignment operator.
               contextLimitCommand& operator= (contextLimitCommand& _contextLimitCommand);
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

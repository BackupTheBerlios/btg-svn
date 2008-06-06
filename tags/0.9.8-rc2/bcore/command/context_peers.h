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

#ifndef COMMAND_PEERS_H
#define COMMAND_PEERS_H

#include "context.h"
#include <bcore/dbuf.h>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup commands
          */
         /** @{ */

         /// Get a list of peers.
         class contextPeersCommand: public contextCommand
            {
            public:
               /// The default constructor. Creates an invalid command per design.
               contextPeersCommand();

               /// Constructs a valid command which requests a list of peers for a context.
               contextPeersCommand(t_int const _context_id,
                                   bool const _allContexts = false);

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);
               
               /// Requests extended info in response, sets the range of this response
               /// @param _offset [in] offset in the server's peer list
               /// @param _count [in] peers count
               void setExRange(t_uint _offset, t_uint _count);
               
               /// Get requested range of extended response data
               /// @param [out] _offset offset from the beginning in the peer lis
               /// @param [out] _count count peers to return
               /// @return whether extended response was requested
               bool getExRange(t_uint& _offset, t_uint& _count) const;
               
               /// The destructor.
               virtual ~contextPeersCommand();
               
            private:
               /// whether limit requested
               bool bExtended_;
               
               /// offset in the server's peer-list
               t_uint offset_;
               
               /// count peers to return
               t_uint count_;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

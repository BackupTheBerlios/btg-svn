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

#ifndef COMMAND_PEERS_RSP_H
#define COMMAND_PEERS_RSP_H

#include "context.h"

#include <bcore/dbuf.h>
#include <bcore/peer.h>
#include <bcore/type_btg.h>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup commands
          */
         /** @{ */

         /// Contains a list of peers for a context.
         class contextPeersResponseCommand: public contextCommand
            {
            public:
               /// Default constructor.
               contextPeersResponseCommand();

               /// Constructor.
               /// @param [in] _context_id      The context ID of the context of which a status is required.
               /// @param [in] _peerList        List of peers.
               contextPeersResponseCommand(t_int const  _context_id,
                                           t_peerList const& _peerList);
               
               /// Set extended peer information
               /// @param [in] offset     Offset of specified peers in the full peer list
               /// @param [in] peerExList The list of extended peers
               void setExList(t_uint offset, t_peerExList const& peerExList);

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Get the contained list of peers.
               const t_peerList & getList() const { return peerList_; }
               
               /// Get extended response flag
               bool isEx() const { return bExtended_; }
               
               /// Get offset of the extended peers in the full peer list
               t_uint getExOffset() const { return offset_; }
               
               /// Get extended peer list
               const t_peerExList & getExList() const { return peerExList_; }

               /// Destructor.
               virtual ~contextPeersResponseCommand();
            private:
               /// List of peers.
               t_peerList peerList_;
               
               /// Whether extended peer list is included in the response
               bool bExtended_;
               
               /// Offset of the extended peer list in the full peer list
               t_uint offset_;
               
               /// List of extended peers.
               t_peerExList peerExList_;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif


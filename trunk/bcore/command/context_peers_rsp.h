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
 * $Id: context_peers_rsp.h,v 1.1.2.6 2006/03/13 18:40:47 jstrom Exp $
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
               /// @param [in] _peerlist        List of peers.
               contextPeersResponseCommand(t_int const  _context_id,
                                           t_peerList const& _peerlist);

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Get the contained list of peers.
               t_peerList getList() const;

               /// Destructor.
               virtual ~contextPeersResponseCommand();
            private:
               /// List of peers.
               t_peerList peerlist_;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif


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
 * $Id: context_peers_rsp.cpp,v 1.1.2.11 2006/08/05 23:21:09 wojci Exp $
 */

#include "context_peers_rsp.h"
#include <bcore/helpermacro.h>

namespace btg
{
   namespace core
   {

      contextPeersResponseCommand::contextPeersResponseCommand()
         : contextCommand(Command::CN_CPEERSRSP, contextCommand::UNDEFINED_CONTEXT, false),
           peerlist_(0)
      {
      }

      contextPeersResponseCommand::contextPeersResponseCommand(t_int const  _context_id,
                                                               t_peerList const& _peerlist)
         : contextCommand(Command::CN_CPEERSRSP, _context_id),
           peerlist_(_peerlist)
      {
      }

      bool contextPeersResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( contextCommand::serialize(_e) );

         // Serialize the list.

         // First write the size of the list.
         t_int list_size = peerlist_.size();
         _e->setParamInfo("size of the list of peers.", true);
         BTG_RCHECK( _e->intToBytes(&list_size) );

         _e->setParamInfo("list of peers.", true);
         // Then serialize the contents of the list.
         t_peerListCI iter;
         for (iter=peerlist_.begin(); iter != peerlist_.end(); iter++)
            {
               BTG_RCHECK( iter->serialize(_e) );
            }

         return true;
      }

      bool contextPeersResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( contextCommand::deserialize(_e) );

         // The size of the list.
         t_int list_size = 0;

         _e->setParamInfo("size of the list of peers.", true);
         BTG_RCHECK( _e->bytesToInt(&list_size) );

         _e->setParamInfo("list of peers.", true);
         // The contents.
         for (t_int list_counter = 0; list_counter < list_size; list_counter++)
            {
               Peer peer;
               BTG_RCHECK( peer.deserialize(_e) );

               peerlist_.push_back(peer);
            }

         return true;
      }

      t_peerList contextPeersResponseCommand::getList() const
      {
         return peerlist_;
      }

      contextPeersResponseCommand::~contextPeersResponseCommand()
      {
         peerlist_.clear();
      }

   } // namespace core
} // namespace btg


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

#include "context_peers_rsp.h"
#include <bcore/helpermacro.h>

namespace btg
{
   namespace core
   {

      contextPeersResponseCommand::contextPeersResponseCommand()
         : contextCommand(Command::CN_CPEERSRSP, contextCommand::UNDEFINED_CONTEXT, false),
           peerList_(), 
           bExtended_(false),
           offset_(),
           peerExList_()
      {
      }

      contextPeersResponseCommand::contextPeersResponseCommand(t_int const  _context_id,
                                                               t_peerList const& _peerList)
         : contextCommand(Command::CN_CPEERSRSP, _context_id),
           peerList_(_peerList),
           bExtended_(false),
           offset_(),
           peerExList_()
      {
      }

      void contextPeersResponseCommand::setExList(t_uint _offset, t_peerExList const& _peerExList)
      {
         bExtended_ = true;
         offset_ = _offset;
         peerExList_ = _peerExList;
      }

      bool contextPeersResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( contextCommand::serialize(_e) );

         // Serialize the list.

         // First write the size of the list.
         t_uint list_size = peerList_.size();
         _e->setParamInfo("size of the list of peers", true);
         BTG_RCHECK( _e->uintToBytes(&list_size) );

         _e->setParamInfo("list of peers", true);
         // Then serialize the contents of the list.
         for (t_peerListCI iter = peerList_.begin(); iter != peerList_.end(); ++iter)
            {
               BTG_RCHECK( iter->serialize(_e) );
            }
         
         _e->setParamInfo("extended peer list flag", true);
         BTG_RCHECK( _e->boolToBytes(bExtended_) );
         
         if (bExtended_)
         {
            _e->setParamInfo("offset in the full peer list", false);
            BTG_RCHECK( _e->uintToBytes(&offset_) );
            
            list_size = peerExList_.size();
            _e->setParamInfo("size of the extended list of peers", false);
            BTG_RCHECK( _e->uintToBytes(&list_size) );

            _e->setParamInfo("extended list of peers", false);
            for (t_peerExListCI iter=peerExList_.begin(); iter != peerExList_.end(); ++iter)
               {
                  BTG_RCHECK( iter->serialize(_e) );
               }
         }

         return true;
      }

      bool contextPeersResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( contextCommand::deserialize(_e) );

         // The size of the list.
         t_uint list_size = 0;

         _e->setParamInfo("size of the list of peers.", true);
         BTG_RCHECK( _e->bytesToUint(&list_size) );

         _e->setParamInfo("list of peers.", true);
         peerList_.clear();
         // The contents.
         for (t_uint list_counter = 0; list_counter < list_size; ++list_counter)
            {
               Peer peer;
               BTG_RCHECK( peer.deserialize(_e) );
               
               peerList_.push_back(peer);
            }

         _e->setParamInfo("Extended list of peers marker (whether the list present or not).", true);
         BTG_RCHECK( _e->bytesToBool(bExtended_) );
         
         if (bExtended_)
         {
            _e->setParamInfo("Offset in the full peer list.", false);
            BTG_RCHECK( _e->bytesToUint(&offset_) );
            
            _e->setParamInfo("size of the extended list of peers.", false);
            BTG_RCHECK( _e->bytesToUint(&list_size) );

            _e->setParamInfo("extended list of peers.", false);
            peerExList_.clear();
            for (t_uint list_counter = 0; list_counter < list_size; ++list_counter)
               {
                  PeerEx peerEx;
                  BTG_RCHECK( peerEx.deserialize(_e) );
                  
                  peerExList_.push_back(peerEx);
               }
         }

         return true;
      }

      contextPeersResponseCommand::~contextPeersResponseCommand()
      {
         
      }

   } // namespace core
} // namespace btg


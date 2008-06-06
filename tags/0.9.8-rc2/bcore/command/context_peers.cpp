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

#include "context_peers.h"
#include <bcore/helpermacro.h>
#include <iostream>

namespace btg
{
   namespace core
   {

      contextPeersCommand::contextPeersCommand() :
         contextCommand(Command::CN_CPEERS, contextCommand::UNDEFINED_CONTEXT, false),
         bExtended_(false),
         offset_(),
         count_()
      {
         
      }

      contextPeersCommand::contextPeersCommand(t_int const _context_id, bool const _allContexts) :
         contextCommand(Command::CN_CPEERS, _context_id, _allContexts),
         bExtended_(false),
         offset_(),
         count_()
      {
         
      }

      bool contextPeersCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         contextCommand::serialize(_e);
         BTG_RCHECK(_e->status());
         
         _e->setParamInfo("extended response requested", true);
         _e->boolToBytes(bExtended_);
         BTG_RCHECK(_e->status());

         if (bExtended_)
         {
            _e->setParamInfo("offset of the extended peers in the complete peer list", false);
            _e->uintToBytes(&offset_);
            BTG_RCHECK(_e->status());
            
            _e->setParamInfo("count of extended peers", false);
            _e->uintToBytes(&count_);
            BTG_RCHECK(_e->status());
         }
         
         return true;
      }

      bool contextPeersCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         contextCommand::deserialize(_e);
         BTG_RCHECK(_e->status());
         
         _e->setParamInfo("extended response requested", true);
         _e->bytesToBool(bExtended_);
         BTG_RCHECK(_e->status());

         if (bExtended_)
         {
            _e->setParamInfo("offset of the extended peers in the complete peer list", false);
            _e->bytesToUint(&offset_);
            BTG_RCHECK(_e->status());
            
            _e->setParamInfo("count of extended peers", false);
            _e->bytesToUint(&count_);
            BTG_RCHECK(_e->status());
         }
         
         return true;
      }
      
      void contextPeersCommand::setExRange(t_uint _offset, t_uint _count)
      {
         bExtended_ = true;
         offset_ = _offset;
         count_ = _count;
      }

      bool contextPeersCommand::getExRange(t_uint& _offset, t_uint& _count) const
      {
         _offset = offset_;
         _count = count_;
         return bExtended_;
      }

      contextPeersCommand::~contextPeersCommand()
      {
         
      }

   } // namespace core
} // namespace btg

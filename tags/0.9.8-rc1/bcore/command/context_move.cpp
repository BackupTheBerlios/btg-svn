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

#include "context_move.h"
#include <bcore/helpermacro.h>

namespace btg
{
   namespace core
   {
      contextMoveToSessionCommand::contextMoveToSessionCommand()
         : contextCommand(Command::CN_CMOVE, 
                          contextCommand::UNDEFINED_CONTEXT),
           session_(Command::INVALID_SESSION)
      {

      }

      contextMoveToSessionCommand::contextMoveToSessionCommand(t_int const _context_id,
                                                               t_long const _session_id, 
                                                               bool const _allContexts)
         : contextCommand(Command::CN_CMOVE,
                          _context_id,
                          _allContexts),
           session_(_session_id)
      {
      }
               
      bool contextMoveToSessionCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( contextCommand::serialize(_e) );

         _e->setParamInfo("session id", true);
         BTG_RCHECK( _e->longToBytes(&session_) );

         return true;
      }

      bool contextMoveToSessionCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( contextCommand::deserialize(_e) );

         _e->setParamInfo("session id", true);
         BTG_RCHECK( _e->bytesToLong(&session_) );

         return true;
      }

      t_long contextMoveToSessionCommand::session() const
      {
         return session_;
      }

      contextMoveToSessionCommand::~contextMoveToSessionCommand()
      {

      }

   } // namespace core
} // namespace btg

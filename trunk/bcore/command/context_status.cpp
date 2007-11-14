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

#include "context_status.h"
#include <bcore/util.h>
#include <bcore/helpermacro.h>

namespace btg
{
   namespace core
   {

      contextStatusCommand::contextStatusCommand()
         : contextCommand(Command::CN_CSTATUS, contextCommand::UNDEFINED_CONTEXT, false)
      {}

      contextStatusCommand::contextStatusCommand(t_int const _context_id, bool const _allContexts)
         : contextCommand(Command::CN_CSTATUS, _context_id, _allContexts)
      {}

      contextStatusCommand::~contextStatusCommand()
      {}

      /* */

      contextMultipleStatusCommand::contextMultipleStatusCommand()
         : contextCommand(Command::CN_CMSTATUS, contextCommand::UNDEFINED_CONTEXT, false),
           context_ids()
      {
         
      }

      contextMultipleStatusCommand::contextMultipleStatusCommand(t_intList const& _context_ids)
         : contextCommand(Command::CN_CMSTATUS, contextCommand::UNDEFINED_CONTEXT, false),
           context_ids(_context_ids)
      {

      }

      bool contextMultipleStatusCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( contextCommand::serialize(_e) );

         _e->setParamInfo("contexts IDs", true);
         BTG_RCHECK( _e->intListToBytes(&context_ids) );

         return true;
      }

      bool contextMultipleStatusCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( contextCommand::deserialize(_e) );

         _e->setParamInfo("context Ids", true);
         BTG_RCHECK( _e->bytesToIntList(&context_ids) );

         return true;
      }

      t_intList contextMultipleStatusCommand::getIds() const
      {
         return context_ids;
      }

      contextMultipleStatusCommand::~contextMultipleStatusCommand()
      {

      }

   } // namespace core
} // namespace btg

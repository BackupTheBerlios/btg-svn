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

#include "context_abort.h"
#include <bcore/util.h>
#include <bcore/helpermacro.h>
#include <iostream>

namespace btg
{
   namespace core
   {

      contextAbortCommand::contextAbortCommand()
         : contextCommand(Command::CN_CABORT, contextCommand::UNDEFINED_CONTEXT, false),
           eraseData_(false)
      {}

      contextAbortCommand::contextAbortCommand(t_int const _context_id, 
                                               bool const _eraseData,
                                               bool const _allContexts)
         : contextCommand(Command::CN_CABORT, _context_id, _allContexts),
           eraseData_(_eraseData)
      {}

      bool contextAbortCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( contextCommand::serialize(_e) );

         _e->setParamInfo("flag: erase data", true);
         _e->boolToBytes(this->eraseData_);
         BTG_RCHECK(_e->status());

         return true;
      }

      bool contextAbortCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         contextCommand::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("flag: erase data", true);
         _e->bytesToBool(this->eraseData_);
         BTG_RCHECK(_e->status());

         return true;
      }

      std::string contextAbortCommand::toString() const
      {
         std::string output = contextCommand::toString();

         if (eraseData_)
            {
               output += " Erase data..";
            }
         return output;
      }

      bool contextAbortCommand::eraseData() const
      {
         return eraseData_;
      }

      contextAbortCommand::~contextAbortCommand()
      {}

   } // namespace core
} // namespace btg

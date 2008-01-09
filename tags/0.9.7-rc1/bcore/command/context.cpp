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

#include "context.h"
#include <bcore/helpermacro.h>
#include <bcore/t_string.h>

namespace btg
{
   namespace core
   {
      using namespace std;

      const t_int contextCommand::UNDEFINED_CONTEXT = -1;

      contextCommand::contextCommand(commandType _type)
         : Command(_type), context_id(contextCommand::UNDEFINED_CONTEXT),
           allContexts(false)
      {}

      contextCommand::contextCommand(commandType _type, t_int const _context_id, bool const _allContexts)
         : Command(_type), context_id(_context_id),
           allContexts(_allContexts)
      {}

      std::string contextCommand::toString() const
      {
         using namespace std;
         string output = Command::toString() + GPD->sSPACE() + "Context_id=" + convertToString<t_long>(this->context_id) + ".";

         if (allContexts)
            {
               output += GPD->sSPACE() + "All contexts flag is set.";
            }
         return output;
      }

      t_int contextCommand::getContextId() const
      {
         return this->context_id;
      }

      bool contextCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         Command::serialize(_e);
         BTG_RCHECK(_e->status());

         // All elements are required.
         _e->setParamInfo("context ID", true);
         _e->intToBytes(&this->context_id);
         BTG_RCHECK(_e->status());

         // All elements are required.
         _e->setParamInfo("flag: for all contexts", true);
         _e->boolToBytes(this->allContexts);
         BTG_RCHECK(_e->status());

         return true;
      }

      bool contextCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         Command::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("context ID", true);
         _e->bytesToInt(&this->context_id);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("flag: for all contexts", true);
         _e->bytesToBool(this->allContexts);
         BTG_RCHECK(_e->status());

         return true;
      }

      void contextCommand::setAllContextsFlag()
      {
         this->allContexts = true;
      }

      bool contextCommand::isAllContextsFlagSet() const
      {
         return this->allContexts;
      }


      contextCommand::~contextCommand()
      {}

   } // namespace core
} // namespace btg

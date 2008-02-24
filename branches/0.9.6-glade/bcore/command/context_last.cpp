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

#include "context_last.h"
#include <bcore/helpermacro.h>

namespace btg
{
   namespace core
   {
      using namespace std;

      lastCIDCommand::lastCIDCommand()
	: Command(Command::CN_CLAST)
      {
      }

      bool lastCIDCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         return true;
      }

      bool lastCIDCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         return true;
      }

      lastCIDCommand::~lastCIDCommand()
      {}

      /*
       *
       */
      lastCIDResponseCommand::lastCIDResponseCommand()
         : Command(Command::CN_CLASTRSP),
           context_id(-1)
      {
      }

      lastCIDResponseCommand::lastCIDResponseCommand(t_int const _cid)
         : Command(Command::CN_CLASTRSP),
           context_id(_cid)
      {

      }

      bool lastCIDResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );
         BTG_RCHECK( _e->intToBytes(&this->context_id) );

         return true;
      }

      bool lastCIDResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );
         BTG_RCHECK( _e->bytesToInt(&this->context_id) );

         return true;
      }

      t_int lastCIDResponseCommand::getContextId() const
      {
         return this->context_id;
      }

      lastCIDResponseCommand::~lastCIDResponseCommand()
      {

      }

   } // namespace core
} // namespace btg

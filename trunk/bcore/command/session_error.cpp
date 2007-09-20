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
 * $Id: session_error.cpp,v 1.1.4.10 2006/03/13 18:40:48 jstrom Exp $
 */

#include "session_error.h"
#include "command.h"
#include <bcore/helpermacro.h>

namespace btg
{
   namespace core
   {

      using namespace std;

      sessionErrorCommand::sessionErrorCommand()
         : Command(Command::CN_SERROR), which_command(Command::CN_UNDEFINED)
      {}

      sessionErrorCommand::sessionErrorCommand(t_int const _which_command)
         : Command(Command::CN_SERROR), which_command(_which_command)
      {}

      bool sessionErrorCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         bool command_present = true;

         if (this->which_command == Command::CN_UNDEFINED)
            {
               command_present = false;
            }

         _e->setParamInfo("flag: indicates that a command ID is present.", true);
         BTG_RCHECK( _e->boolToBytes(command_present) );

         if (command_present)
            {
               _e->setParamInfo("command ID", false);
               BTG_RCHECK( _e->intToBytes(&this->which_command) );
            }

         return true;
      }


      bool sessionErrorCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         this->which_command = Command::CN_UNDEFINED;
         bool command_present = true;

         _e->setParamInfo("flag: indicates that a command ID is present.", true);
         BTG_RCHECK( _e->bytesToBool(command_present) );

         if (command_present)
            {
               _e->setParamInfo("command ID", false);
               BTG_RCHECK( _e->bytesToInt(&this->which_command) );
            }

         return true;
      }

      sessionErrorCommand::~sessionErrorCommand()
      {}

   } // namespace core
} // namespace btg

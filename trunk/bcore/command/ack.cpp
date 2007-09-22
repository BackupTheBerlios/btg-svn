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

#include <iostream>
#include "command.h"
#include "ack.h"
#include <bcore/helpermacro.h>

namespace btg
{
   namespace core
   {

      ackCommand::ackCommand()
         : Command(Command::CN_ACK),
           command_type(Command::CN_UNDEFINED)
      {}

      ackCommand::ackCommand(t_int const _command_type)
         : Command(Command::CN_ACK),
           command_type(_command_type)
      {}

      std::string ackCommand::toString() const
      {
         using namespace std;
         string output = Command::toString() + GPD->sSPACE() + "Ack for " + Command::getName(command_type) + ".";
         return output;
      }

      void ackCommand::setCommandType(t_int const _command_type)
      {
         command_type = _command_type;
      }

      t_int ackCommand::getCommandType() const
      {
         return command_type;
      }

      bool ackCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK(Command::serialize(_e));

         _e->setParamInfo("ack command", true);
         BTG_RCHECK(_e->intToBytes(&this->command_type));
         
         return true;
      }

      bool ackCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK(Command::deserialize(_e));

         _e->setParamInfo("ack command", true);
         BTG_RCHECK(_e->bytesToInt(&this->command_type));

         return true;
      }

      ackCommand::~ackCommand()
      {}

   } // namespace core
} // namespace btg

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
 * $Id: error.cpp,v 1.7.4.11 2006/05/20 18:31:18 wojci Exp $
 */

#include "error.h"
#include "command.h"
#include <bcore/helpermacro.h>

#include <iostream>

namespace btg
{
   namespace core
   {

      using namespace std;

      errorCommand::errorCommand()
         : Command(Command::CN_ERROR), which_command(Command::CN_UNDEFINED), message("none"), isMessagePresent(false)
      {}

      errorCommand::errorCommand(t_int const _which_command, string const& _message)
         : Command(Command::CN_ERROR), which_command(_which_command), message(_message), isMessagePresent(true)
      {}

      std::string errorCommand::toString() const
      {
         using namespace std;
         string output = Command::toString() +
            GPD->sSPACE() + "Failed command=" + Command::getName(this->which_command) + "." +
            GPD->sSPACE() + "Message='" + this->message + "'.";

         return output;
      }

      bool errorCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         _e->setParamInfo("flag, indicates if a command is present", true);
         bool command_present = true;

         if (this->which_command == Command::CN_UNDEFINED)
            {
               command_present = false;
            }

         BTG_RCHECK( _e->boolToBytes(command_present) );

         if (command_present)
            {
               _e->setParamInfo("command ID", false);
               BTG_RCHECK( _e->intToBytes(&this->which_command) );
            }

         _e->setParamInfo("flag, indicates if a message is present", true);
         bool message_present = true;

         if ((this->message.size() == 0) || (!isMessagePresent))
            {
               message_present = false;
            }

         BTG_RCHECK( _e->boolToBytes(message_present) );

         if (message_present)
            {
               _e->setParamInfo("error message", false);
               BTG_RCHECK( _e->stringToBytes(&this->message) );
            }

         return true;
      }

      bool errorCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         this->which_command = Command::CN_UNDEFINED;
         bool command_present = true;

         _e->setParamInfo("flag, indicates if a command is present", true);
         BTG_RCHECK( _e->bytesToBool(command_present) );

         if (command_present)
            {
               _e->setParamInfo("command ID", false);
               BTG_RCHECK( _e->bytesToInt(&this->which_command) );
            }

         bool message_present = true;

         _e->setParamInfo("flag, indicates if a message is present", true);
         BTG_RCHECK( _e->bytesToBool(message_present) );

         if (message_present)
            {
               _e->setParamInfo("error message", false);
               BTG_RCHECK( _e->bytesToString(&this->message) );

               this->isMessagePresent = true;
            }
         else
            {
               this->isMessagePresent = false;
               this->message          = "";
            }

         return true;
      }

      t_int errorCommand::getErrorCommand() const
      {
         return this->which_command;
      }

      void errorCommand::setMessage(string const& _message)
      {
         this->message = _message;
      }

      string errorCommand::getMessage() const
      {
         return message;
      }

      bool errorCommand::messagePresent() const
      {
         return isMessagePresent;
      }

      bool errorCommand::operator== (const errorCommand* _ec)
      {
         bool parent = Command::operator==(dynamic_cast<const Command*>(_ec));

         if (
             (this->isMessagePresent == _ec->messagePresent()) &&
             (this->message          == _ec->getMessage()) &&
             parent                  == true
             )
            {
               return true;
            }

         return false;
      }

      errorCommand::~errorCommand()
      {}

   } // namespace core
} // namespace btg

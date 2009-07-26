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
 * Id    : $Id$
 */

#include "session_name.h"
#include <bcore/helpermacro.h>

namespace btg
{
   namespace core
   {
      sessionNameCommand::sessionNameCommand()
      : Command(Command::CN_SNAME)
      {

      }

      bool sessionNameCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         return true;
      }

      bool sessionNameCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         return true;
      }

      sessionNameCommand::~sessionNameCommand()
      {
         
      }

      sessionNameResponseCommand::sessionNameResponseCommand()
         : Command(Command::CN_SNAMERSP),
           name("")
      {

      }

      sessionNameResponseCommand::sessionNameResponseCommand(std::string _name)
         : Command(Command::CN_SNAMERSP),
           name(_name)
      {

      }

      bool sessionNameResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         _e->setParamInfo("Session name.", true);
         BTG_RCHECK( _e->stringToBytes(&name) );

         return true;
      }

      bool sessionNameResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         _e->setParamInfo("Session name.", true);
         BTG_RCHECK( _e->bytesToString(&name) );
         
         return true;
      }

      std::string sessionNameResponseCommand::getName() const
      {
         return name;
      }

      sessionNameResponseCommand::~sessionNameResponseCommand()
      {

      }

      setSessionNameCommand::setSessionNameCommand()
         : Command(Command::CN_SSETNAME),
           name("")
      {

      }

      setSessionNameCommand::setSessionNameCommand(std::string _name)
         : Command(Command::CN_SSETNAME),
           name(_name)
      {

      }

      bool setSessionNameCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );
         
         _e->setParamInfo("Session name.", true);
         BTG_RCHECK( _e->stringToBytes(&name) );

         return true;
      }

      bool setSessionNameCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         _e->setParamInfo("Session name.", true);
         BTG_RCHECK( _e->bytesToString(&name) );

         return true;
      }

      std::string setSessionNameCommand::getName() const
      {
         return name;
      }

      setSessionNameCommand::~setSessionNameCommand()
      {

      }

   } // namespace core
} // namespace btg


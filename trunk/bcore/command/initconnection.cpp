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
#include "initconnection.h"
#include <bcore/helpermacro.h>
#include <bcore/auth/hash.h>

namespace btg
{
   namespace core
   {

      initConnectionCommand::initConnectionCommand()
         : Command(Command::CN_GINITCONNECTION),
           username_(),
           hash_()
      {}

      initConnectionCommand::initConnectionCommand(std::string const& _username, std::string const& _password)
         : Command(Command::CN_GINITCONNECTION),
           username_(_username),
           hash_()
      {
         hash_.generate(_password);
      }

      initConnectionCommand::initConnectionCommand(std::string const& _username, btg::core::Hash const& _hash)
         : Command(Command::CN_GINITCONNECTION),
           username_(_username),
           hash_(_hash)
      {
      }

      std::string initConnectionCommand::toString() const
      {
         std::string output = Command::toString() + GPD->sSPACE() + "initConnectionCommand.";
         return output;
      }

      std::string initConnectionCommand::getUsername() const
      {
         return username_;
      }

      void initConnectionCommand::setUsername(std::string const _username)
      {
         username_ = _username;
      }

      std::string initConnectionCommand::getPassword() const
      {
         std::string hash;
         hash_.get(hash);
         return hash;
      }

      void initConnectionCommand::setPassword(std::string const _password)
      {
         hash_.generate(_password);
      }

      void initConnectionCommand::setPassword(btg::core::Hash _hash)
      {
         hash_ = _hash;
      }

      bool initConnectionCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         // string, username ;
         _e->setParamInfo("username", true);
         BTG_RCHECK( _e->stringToBytes(&username_) );

         // Serialize hash
         _e->setParamInfo("password hash", true);
         BTG_RCHECK( hash_.serialize(_e) );

         return true;
      }

      bool initConnectionCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         // string, username ;
         BTG_RCHECK( _e->bytesToString(&username_) );

         // Serialize hash
         BTG_RCHECK( hash_.deserialize(_e) );

         return true;
      }

      initConnectionCommand::~initConnectionCommand()
      {}

   } // namespace core
} // namespace btg

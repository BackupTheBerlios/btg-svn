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

#include "session_attach.h"
#include <bcore/helpermacro.h>

#include <iostream>

namespace btg
{
   namespace core
   {

      attachSessionCommand::attachSessionCommand()
         : Command(Command::CN_SATTACH),
           buildID(),
           session(INVALID_SESSION)
      {
      }

      attachSessionCommand::attachSessionCommand(std::string const& _buildID,
                                                 t_long const _session)
         : Command(Command::CN_SATTACH),
           buildID(_buildID),
           session(_session)
      {
      }

      bool attachSessionCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         _e->setParamInfo("build id", true);
         BTG_RCHECK( _e->stringToBytes(&this->buildID) );

         _e->setParamInfo("session ID", true);
         BTG_RCHECK( _e->longToBytes(&this->session) );

         return true;
      }

      bool attachSessionCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         _e->setParamInfo("build id", true);
         BTG_RCHECK( _e->bytesToString(&this->buildID) );

         _e->setParamInfo("session ID", true);
         BTG_RCHECK( _e->bytesToLong(&this->session) );

         return true;
      }

      std::string attachSessionCommand::getBuildID() const
      {
         return buildID;
      }

      t_long attachSessionCommand::getSession() const
      {
         return this->session;
      }

      attachSessionCommand::~attachSessionCommand()
      {

      }

   } // namespace core
} // namespace btg


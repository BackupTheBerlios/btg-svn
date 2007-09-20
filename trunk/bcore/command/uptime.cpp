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
 * $Id: uptime.cpp,v 1.1.2.10 2006/03/13 18:40:48 jstrom Exp $
 */

#include "uptime.h"
#include <bcore/helpermacro.h>

#include <iostream>


namespace btg
{
   namespace core
   {
      uptimeCommand::uptimeCommand()
         : Command(Command::CN_GUPTIME)
      {

      }

      bool uptimeCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         return true;
      }

      bool uptimeCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         return true;
      }

      uptimeCommand::~uptimeCommand()
      {

      }

      uptimeResponseCommand::uptimeResponseCommand()
         : Command(Command::CN_GUPTIMERSP),
           uptime(0)
      {

      }

      uptimeResponseCommand::uptimeResponseCommand(t_ulong const _uptime)
         : Command(Command::CN_GUPTIMERSP),
           uptime(_uptime)
      {

      }

      bool uptimeResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         _e->setParamInfo("uptime", true);
         BTG_RCHECK(_e->uLongToBytes(&this->uptime) );

         return true;
      }

      bool uptimeResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         _e->setParamInfo("uptime", true);
         BTG_RCHECK( _e->bytesToULong(&this->uptime) );

         return true;
      }

      t_ulong uptimeResponseCommand::getUptime() const
      {
         return uptime;
      }

      uptimeResponseCommand::~uptimeResponseCommand()
      {

      }

   } // namespace core
} // namespace btg

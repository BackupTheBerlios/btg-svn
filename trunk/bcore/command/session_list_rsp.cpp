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

#include "session_list_rsp.h"
#include <bcore/helpermacro.h>

#include <iostream>

namespace btg
{
   namespace core
   {
      listSessionResponseCommand::listSessionResponseCommand()
         : Command(Command::CN_SLISTRSP),
           sessions(),
           names()
      {

      }

      listSessionResponseCommand::listSessionResponseCommand(t_longList _sessions,
                                                             t_strList _names)
         : Command(Command::CN_SLISTRSP),
           sessions(_sessions),
           names(_names)
      {

      }

      bool listSessionResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         _e->setParamInfo("list of sessions.", true);
         BTG_RCHECK( _e->longListToBytes(&sessions) );

         _e->setParamInfo("list of session names.", true);
         BTG_RCHECK( _e->stringListToBytes(&names) );

         return true;
      }

      bool listSessionResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         _e->setParamInfo("list of sessions.", true);
         BTG_RCHECK( _e->bytesToLongList(&sessions) );

         _e->setParamInfo("list of session names.", true);
         BTG_RCHECK( _e->bytesToStringList(&names) );

         return true;
      }

      t_longList listSessionResponseCommand::getSessions() const
      {
         return sessions;
      }

      t_strList listSessionResponseCommand::getNames() const
      {
         return names;
      }

      listSessionResponseCommand::~listSessionResponseCommand()
      {

      }

   } // namespace core
} // namespace btg

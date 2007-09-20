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
 * $Id: session_list.cpp,v 1.6.4.6 2006/03/13 18:40:48 jstrom Exp $
 */

#include "session_list.h"
#include <bcore/util.h>
#include <bcore/helpermacro.h>

#include <iostream>

namespace btg
{
   namespace core
   {
      listSessionCommand::listSessionCommand()
         : Command(Command::CN_SLIST)
      {

      }

      bool listSessionCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         return true;
      }

      bool listSessionCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         return true;
      }

      listSessionCommand::~listSessionCommand()
      {

      }

   } // namespace core
} // namespace btg

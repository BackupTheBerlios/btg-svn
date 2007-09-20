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
 * $Id: connectionextrastate.cpp,v 1.1.2.1 2007/04/25 06:03:33 wojci Exp $
 */

#include "connectionextrastate.h"

#include <bcore/command/context.h>

namespace btg
{
   namespace daemon
   {
      ConnectionExtraState::ConnectionExtraState():
         lastCreatedContextId(btg::core::contextCommand::UNDEFINED_CONTEXT)
      {
      }
   }
}

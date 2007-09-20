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
 * $Id: context_status.cpp,v 1.5.4.2 2006/03/13 18:40:47 jstrom Exp $
 */

#include "context_status.h"
#include <bcore/util.h>
#include <iostream>

namespace btg
{
   namespace core
   {

      contextStatusCommand::contextStatusCommand()
         : contextCommand(Command::CN_CSTATUS, contextCommand::UNDEFINED_CONTEXT, false)
      {}

      contextStatusCommand::contextStatusCommand(t_int const _context_id, bool const _allContexts)
         : contextCommand(Command::CN_CSTATUS, _context_id, _allContexts)
      {}

      contextStatusCommand::~contextStatusCommand()
      {}

   } // namespace core
} // namespace btg

/*
 * btg Copyright (C) 2008 Michael Wojciechowski.
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

#include "opid.h"
#include <daemon/modulelog.h>

namespace btg
{
   namespace daemon
   {
      const t_uint min_id = 1;
      const std::string moduleName("opid");

      opId::opId(btg::core::LogWrapperType _logwrapper)
         : btg::core::Logable(_logwrapper),
           current_id(min_id),
           max_id(0xFFFFFF)
      {
         BTG_MNOTICE(logWrapper(), "constructed");
      }

      t_uint opId::id()
      {
         t_uint id = current_id;

         BTG_MNOTICE(logWrapper(), "Operation id = " << id);

         current_id++;

         if (id > max_id)
            {
               current_id = min_id;
            }

         return id;
      }

      opId::~opId()
      {
         BTG_MNOTICE(logWrapper(), "destructed");
      }

   } // namespace daemon
} // namespace btg


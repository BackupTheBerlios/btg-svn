#include "btg_assert.h"

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

#include "logmacro.h"

namespace btg
{
   namespace core
   {

      void btg_assert(bool _a,
                      LogWrapperType _logwrapper,
                      std::string const& _message)
      {
         if (!_a)
            {
#ifndef NDEBUG
               // Debug.
               BTG_FATAL_ERROR(_logwrapper, "assert", _message);
               exit(-1);
#endif
            }
      }

   } // namespace core
} // namespace btg


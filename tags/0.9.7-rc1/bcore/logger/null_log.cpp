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

#include "null_log.h"

extern "C"
{
#include <string.h>
}

#include <string>
#include <iostream>

using namespace btg::core;
using namespace std;

namespace btg
{
   namespace core
   {
      namespace logger
      {

         nullLogger::nullLogger()
            : logBuffer(0, 64)
         {
         }

         nullLogger::~nullLogger()
         {
         }

         void nullLogger::put_buffer()
         {
            if (pbase() != pptr())
               {
                  // Just delete the data.
                  setp(pbase(), epptr());
               }
         }

         void nullLogger::put_char(int _char)
         {
            _char = _char;
            // Do nothing.
         }

      } // namespace logger
   } // namespace core
} // namespace btg

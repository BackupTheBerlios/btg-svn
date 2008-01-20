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

#include "console_log.h"

extern "C"
{
#include <string.h>
}

#include <string>
#include <iostream>

using namespace btg::core;

namespace btg
{
   namespace core
   {
      namespace logger
      {

         consoleLogger::consoleLogger()
            : logBuffer(0, 1024)
         {
         }

         consoleLogger::~consoleLogger()
         {
         }

         void consoleLogger::put_buffer()
         {
            if (pbase() != pptr())
               {
                  int len      = (pptr() - pbase());
                  char* buffer = new char[len + 1];

                  strncpy(buffer, pbase(), len);
                  buffer[len] = '\0';

                  std::string s(buffer);
                  std::cout << s;

                  setp(pbase(), epptr());
                  delete [] buffer;
                  buffer = 0;
               }
         }

         void consoleLogger::put_char(int _char)
         {
            char* c = new char;
            *c = _char;
            std::string s(c);
            std::cout << s;
            delete c;
            c = 0;
         }

      } // namespace logger
   } // namespace core
} // namespace btg

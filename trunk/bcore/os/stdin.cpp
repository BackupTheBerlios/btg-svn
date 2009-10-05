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

#include "stdin.h"

extern "C"
{
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
}

#include <iostream>

// getpass replacement, taken from glibc documentation .. does NOT work.
// The problem is mixing C function calls and C++ iostream.
// 
// http://www.gnu.org/software/libc/manual/html_node/getpass.html
// 

namespace btg
{
   namespace core
   {
      namespace os
      {
         bool stdIn::getPassword(std::string & _password)
         {
            bool status = false;

            struct termios old, tnew;
            FILE* stream = stdin;

            /* Turn echoing off and fail if we can't. */
            if (tcgetattr (fileno (stream), &old) != 0)
               {
                  std::cout << "error, tcgetattr (1)." << std::endl;
                  return status;
               }
            tnew = old;
            tnew.c_lflag &= ~ECHO;
            if (tcsetattr (fileno (stream), TCSAFLUSH, &tnew) != 0)
               {
                  std::cout << "error, tcgetattr (2)." << std::endl;
                  return status;
               }
            
            std::cin >> _password;
            if (!std::cin.good())
               {
                  std::cout << "Unable to get password." << std::endl;
                  return status;
               }

            /* Got input. */
            status = true;

            /* Restore terminal. */
            (void) tcsetattr (fileno (stream), TCSAFLUSH, &old);
     
            return status;
         }

      } // namespace os
   } // namespace core
} // namespace btg


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
}

ssize_t getpass_replacement(char **lineptr, 
                            size_t *n, 
                            FILE *stream);

namespace btg
{
   namespace core
   {
      namespace os
      {
         bool stdIn::getPassword(std::string & _password)
         {
            bool status = false;
#if 0
            char *buffer = getpass("");

            if (buffer != NULL)
               {
                  if (strlen(buffer) > 0)
                     {
                        std::string s(buffer);
                        _password = s;
                        status    = true;
                     }
               }
#endif
            char* line = NULL;
            FILE* fp = stdin;
            size_t s = -1;
            ssize_t size = getpass_replacement(&line, 
                                               &s /*ignored */,
                                               fp);

            if ((size > 0) && (line != NULL))
               {
                  std::string s(line);
                  _password = s;
                  status    = true;
               }

            return status;
         }

      } // namespace os
   } // namespace core
} // namespace btg

// getpass replacement, taken from glibc documentation.
// 
// http://www.gnu.org/software/libc/manual/html_node/getpass.html
// 
ssize_t getpass_replacement(char **lineptr, 
                            size_t *n, 
                            FILE *stream)
{
   struct termios old, tnew;
   int nread;
   
   /* Turn echoing off and fail if we can't. */
   if (tcgetattr (fileno (stream), &old) != 0)
      {
         return -1;
      }
   tnew = old;
   tnew.c_lflag &= ~ECHO;
   if (tcsetattr (fileno (stream), TCSAFLUSH, &tnew) != 0)
      {
         return -1;
      }

   /* Read the password. */
   nread = getline (lineptr, n, stream);
     
   /* Restore terminal. */
   (void) tcsetattr (fileno (stream), TCSAFLUSH, &old);
     
   return nread;
}

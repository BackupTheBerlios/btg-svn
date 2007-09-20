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
 * $Id: nscreen_log.cpp,v 1.5.4.1 2006/04/05 18:44:09 wojci Exp $
 */

#include "nscreen_log.h"

extern "C"
{
#include <string.h>
}

#include <string>

using namespace btg::core;
using namespace btg::core::logger;
using namespace std;

namespace btg
{
   namespace UI
   {
      namespace cli
      {

         nscreenLogger::nscreenLogger(logInterface *_target)
            : logBuffer(_target, 1024)
         {
         }

         nscreenLogger::~nscreenLogger()
         {
         }

         void nscreenLogger::put_buffer(void)
         {
            if (pbase() != pptr())
               {
                  int len      = (pptr() - pbase());
                  char *buffer = new char[len + 1];

                  strncpy(buffer, pbase(), len);
                  buffer[len] = '\0';

                  string s(buffer);
                  this->getTarget()->writeLog(s);

                  setp(pbase(), epptr());
                  delete [] buffer;
               }
         }

         void nscreenLogger::put_char(int _char)
         {
            char *c = new char;
            * c = _char;
            string s(c);
            this->getTarget()->writeLog(s);
            delete c;
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

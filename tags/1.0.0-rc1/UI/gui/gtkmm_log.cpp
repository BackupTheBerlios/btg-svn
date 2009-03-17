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

#include "gtkmm_log.h"

#include <bcore/logger/logif.h>

extern "C"
{
#include <string.h>
}

#include "logtextview.h"

#include <string>

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         using namespace btg::core;
         using namespace btg::core::logger;

         gtkmmLogger::gtkmmLogger(logInterface *_target)
            : logBuffer(_target, 1024)
         {
         }

         gtkmmLogger::~gtkmmLogger()
         {
         }

         void gtkmmLogger::put_buffer(void)
         {
            if (pbase() != pptr())
               {
                  int len      = (pptr() - pbase());
                  char* buffer = new char[len + 1];

                  strncpy(buffer, pbase(), len);
                  buffer[len] = '\0';

                  std::string s(buffer);
                  this->getTarget()->writeLog(s);

                  setp(pbase(), epptr());
                  delete [] buffer;
               }
         }

         void gtkmmLogger::put_char(int _char)
         {
            char* c = new char;
            *c = _char;
            std::string s(c);

            this->getTarget()->writeLog(s);
            delete c;
         }

      } // namespace gui
   } // namespace UI
} // namespace btg

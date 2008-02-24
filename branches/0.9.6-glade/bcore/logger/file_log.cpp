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

#include <config.h>
#include "file_log.h"

extern "C"
{
#include <string.h>
}

#include <string>

#if HAVE_IOSTREAM
#include <iostream>
#elif HAVE_IOSTREAM_H
#include <iostream.h>
#else
#error "No <iostream> or <iostream.h>"
#endif

using namespace btg::core;
using namespace std;
namespace btg
{
   namespace core
   {
      namespace logger
      {

         fileLogger::fileLogger(string _filename)
            : logBuffer(0, 1024), 
              filename(_filename),
              file()
         {
         }

         bool fileLogger::open()
         {
#if HAVE_IOS_BASE
            file.open(this->filename.c_str(), ios_base::out);
#else
            file.open(this->filename.c_str(), ios::out);
#endif
            return file.is_open();
         }

         bool fileLogger::close()
         {
            file.close();
            if (file.is_open() == true)
               {
                  return false;
               }
            return true;
         }

         fileLogger::~fileLogger()
         {
            close();
         }

         void fileLogger::put_buffer()
         {
            if (pbase() != pptr())
               {
                  int len      = (pptr() - pbase());
                  char *buffer = new char[len + 1];

                  strncpy(buffer, pbase(), len);
                  buffer[len] = '\0';

                  string s(buffer);

                  file << s;
                  file.flush();

                  setp(pbase(), epptr());
                  delete [] buffer;
               }
         }

         void fileLogger::put_char(int _char)
         {
            char *c = new char;
            *c = _char;
            string s(c);

            file << s;
            file.flush();

            delete c;
         }
      } // namespace logger
   } // namespace core
} // namespace btg

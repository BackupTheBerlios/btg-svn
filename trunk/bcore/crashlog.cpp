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

#include "crashlog.h"

#ifdef HAVE_VERBOSE_TERMINATE_HANDLER
#include <exception>
#endif // HAVE_VERBOSE_TERMINATE_HANDLER

#include "logmacro.h"

#if HAVE_EXECINFO_H
extern "C"
{
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
}
#endif // HAVE_EXECINFO_H

#include <iostream>

namespace btg
{
   namespace core
   {

      void crashLog::init()
      {
         BTG_NOTICE("Installing crash handler");
#ifdef HAVE_VERBOSE_TERMINATE_HANDLER
         std::set_terminate(handler);
#endif // HAVE_VERBOSE_TERMINATE_HANDLER
      }

      void crashLog::handler()
      {
         crashLog::printStackTrace();
#ifdef HAVE_VERBOSE_TERMINATE_HANDLER
         std::set_terminate (__gnu_cxx::__verbose_terminate_handler);
         throw;
#endif // HAVE_VERBOSE_TERMINATE_HANDLER
      }

      void crashLog::printStackTrace()
      {
#if HAVE_EXECINFO_H
         // Print a stacktrace using libc.
         // The printed addresses can be translated into file lines
         // with:
         // # addr2line -C -e executable addresse.

         int const array_size = 20;
         void *array[array_size];
         char **strings;

         size_t act_size = backtrace (array, array_size);
         strings         = backtrace_symbols (array, act_size);

         std::cout << "Stacktrace:" << std::endl;
         std::cout << "Got " << (act_size-1) << " stack frames." << std::endl;

         for (size_t i = 0; i < act_size; i++)
            {
               std::cout << "Frame " << i << ": " << strings[i] << std::endl;
            }
         free (strings);
#endif // HAVE_EXECINFO_H
      }


   } // namespace core
} // namespace btg


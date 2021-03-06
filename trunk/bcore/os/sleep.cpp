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

#include "sleep.h"

extern "C"
{
#include <time.h>
#include <unistd.h>
}

namespace btg
{
   namespace core
   {
      namespace os
      {
         void Sleep::sleepSecond(t_uint _s)
         {
            sleep(_s);
         }

         void Sleep::sleepMiliSeconds(t_uint _ms)
         {
            // usleep(8)
            // usleep - suspend execution for microsecond intervals
            usleep(_ms * 1000);
         }

         void Sleep::sleepNanoSeconds(t_int _ns)
         {
            struct timespec req;
            struct timespec rem;

            req.tv_sec  = 0;
            req.tv_nsec = _ns;
            req.tv_sec  = 0;
            req.tv_nsec = 0;

            nanosleep(&req, &rem);
         }


      } // namespace os
   } // namespace core
} // namespace btg

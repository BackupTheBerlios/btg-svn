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
 * $Id: sleep.h,v 1.1.2.4 2007/08/20 16:30:45 wojci Exp $
 */

#ifndef SLEEP_H
#define SLEEP_H

#include <string>
#include <bcore/type.h>

namespace btg
{
   namespace core
      {
         namespace os
            {
               /**
                * \addtogroup OS
                */
               /** @{ */

               /// Implements system dependent sleep functions.
               class Sleep
                  {
                  public:
                     /// Sleep for a number of seconds.
                     static void sleepSecond(t_uint _s);
                     /// Sleep for a number of nano seconds.
                     static void sleepNanoSeconds(t_int _ns);
                     /// Sleep for a number of mili seconds.
                     static void sleepMiliSeconds(t_uint _ms);
                  };

               /** @} */

            } // namespace os
      } // namespace core
} // namespace btg

#endif



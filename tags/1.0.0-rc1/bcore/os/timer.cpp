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

#include "timer.h"

#include <bcore/logmacro.h>
#include <bcore/verbose.h>

// Note: Noticed that boost::timer does not work when boost threads
// are involved. Gerd Riesselmann also noticed this and wrote a simple
// timer using boost:xtime. I looked at his implementation and wrote
// this.

namespace btg
{
   namespace core
   {
      namespace os
      {

         Timer::Timer(t_uint const _max_value)
            : max_value_(_max_value)
         {
            xtime_get(&startTime_, boost::TIME_UTC);
         }

         t_uint Timer::maxValue() const
         {
            return max_value_;
         }

         double Timer::elapsed() const
         {
            boost::xtime now;
            xtime_get(&now, boost::TIME_UTC);

            return boost::lexical_cast<double>(now.sec - startTime_.sec);
         }


         bool Timer::Timeout()
         {
            bool status = false;

            t_uint value = static_cast<t_uint>(elapsed());

            if (value >= max_value_)
               {
                  status = true;
               }

            return status;
         }

         void Timer::Reset()
         {
            xtime_get(&startTime_, boost::TIME_UTC);
         }

         Timer::~Timer()
         {
         }


      } // namespace os
   } // namespace core
} // namespace btg

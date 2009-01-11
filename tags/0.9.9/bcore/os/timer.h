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

#ifndef TIMER_H
#define TIMER_H

#include <bcore/type.h>
#include <boost/thread/xtime.hpp>

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

               /// Implements system dependent timer.
               class Timer
                  {
                  public:
                     /// Constructor.
                     /// @param [in] _max_value Time in seconds.
                     Timer(t_uint const _max_value);

                     /// Return true, when the timer reaches its
                     /// maximum value set in the constructor.
                     bool Timeout();

                     /// Reset the timer.
                     void Reset();

                     /// Get the max value this timer can have. In
                     /// seconds.
                     t_uint maxValue() const;

                     /// Destructor.
                     virtual ~Timer();
                  private:
                     /// Max value this timer reaches before returning
                     /// true from Timeout().
                     t_uint const max_value_;

                     /// When this timer was started.
                     boost::xtime startTime_;

                     /// Return the number of seconds that elapsed.
                     double elapsed() const;
                  };

               /** @} */

            } // namespace os
      } // namespace core
} // namespace btg

#endif // TIMER_H



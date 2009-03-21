/*
 * btg Copyright (C) 2009 Michael Wojciechowski.
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

#ifndef allocate_resources_impl_h
#define allocate_resources_impl_h

#include <bcore/logable.h>
#include <bcore/logmacro.h>

#include <boost/shared_ptr.hpp>
#include <daemon/limitval.h>
#include <boost/cstdint.hpp>
typedef boost::int64_t size_type;

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#define descrLabel "allocate_resources_impl:"

namespace btg
{
   namespace daemon
   {


      /// Allocate resources (used for global limits).
		template<class It, class T>
		void allocate_resources_impl(btg::core::LogWrapperType _logwrapper,
                                   int _resources,
                                   It _start,
                                   It _end,
                                   limitValue T::* _res)
		{
			assert(_resources >= 0);

         unsigned int size = 0;

         for (It i = _start; i != _end; ++i)
            {
               limitValue& r = (*i).*_res;
               r.leftovers = (std::max)(r.given-r.used, 0);

               BTG_NOTICE(_logwrapper, descrLabel << ", given = " << r.given << ", used " << r.used << ", left = " << r.leftovers);

               if (_resources == limitValue::inf)
               {
                  // No competition for resources.
                  // Just give everyone what they want.
                  r.given = r.max;
               }

               size++;
            }
         
         if (_resources == limitValue::inf)
			{
				return;
			}

         if (size == 0)
         {
            return;
         }

         assert(size > 0);

         size++;

         int minres = _resources / size;

         BTG_NOTICE(_logwrapper, descrLabel << ", resources = " << _resources << ", minres " << minres << ", size = " << size);

         // Resources left after distributing the minimum.
         int left = 0;
         // Producers which can use extra resources.
         int needing = 0;
         int count = 0;
         for (It i = _start; i != _end; ++i)
         {
            limitValue& r = (*i).*_res;

            if (r.used < minres)
               {
                  // Using less than the minimum.
                  r.given = (std::min)(r.used, r.max);
                  r.leftovers = 0;
                  left += (std::max)(minres-r.used, 0);
                  if (r.given == 0)
                     {
                        r.given++;
                        left--;
                     }
               }
            else
               {
                  // Give minimal resources to everyone.
                  r.given = minres;
                  r.leftovers = 1;
                  needing++;
               }

            count++;
         }

         if ((left > 0) && (needing > 0))
            {
               minres = left / needing;
               count = 0;
               for (It i = _start; i != _end; ++i)
                  {
                     limitValue& r = (*i).*_res;
                     if (r.leftovers == 1)
                        {
                           r.given += minres;
                           left -= minres;
                        }
                     count++;
                  }
            }

		}

   } // namespace daemon
} // namespace btg


#endif // allocate_resources_impl_h


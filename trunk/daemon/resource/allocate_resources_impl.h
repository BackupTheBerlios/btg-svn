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
 
         size_type size = 0;

         int count = 0;
         for (It i = _start; i != _end; ++i)
            {
               limitValue& r = (*i).*_res;
               r.leftovers = (std::max)(r.given-r.used, 0);

               //BTG_NOTICE(_logwrapper, "count=" << count << ", used = " << r.used);
               count++;

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

         size_type minres = _resources / size;

         /// Amount of resources to give, if none are required.
         const size_type giveIfNotNeeding = 1 * _resources / 100;

         //BTG_NOTICE(_logwrapper, descrLabel << ", minres = " << minres << ", size = " << size);

         //BTG_NOTICE(_logwrapper, descrLabel << ", resources = " << _resources << ", minres " << minres << ", size = " << size);

         // Resources left after distributing the minimum.
         size_type left = 0;
         // Producers which can use extra resources.
         size_type needing = 0;
         count = 0;
         for (It i = _start; i != _end; ++i)
         {
            limitValue& r = (*i).*_res;

            if (r.used < minres)
               {
                  // Using less than the minimum.
                  r.given     = (std::min)(r.used, r.max);
                  r.leftovers = 0;
                  if ((minres-r.used) > 0)
                     {
                        left += (minres-r.used);
                     }

                  if (r.given == 0)
                     {
                        r.given += giveIfNotNeeding;
                        left    -= giveIfNotNeeding;
                     }
               }
            else
               {
                  // Give minimal resources to everyone.
                  r.given     = minres;
                  r.leftovers = 1;
                  needing++;
               }

            //BTG_NOTICE(_logwrapper, descrLabel << " count = " << count << ", r.given = " << r.given << ", left = " << left);

            count++;
         }

         //BTG_NOTICE(_logwrapper, descrLabel << " left = " << left);
         //BTG_NOTICE(_logwrapper, descrLabel << " needing = " << needing);

         if ((left > 0) && (needing > 0))
            {
               if (needing == 1)
                  {
                     for (It i = _start; i != _end; ++i)
                        {
                           limitValue& r = (*i).*_res;
                           if (r.leftovers == 1)
                              {
                                 r.given += left;
                                 left = 0;
                                 break;
                              }
                        }
                  }
               else
                  {
                     minres = left / needing;
                     count = 0;
                     for (It i = _start; i != _end; ++i)
                        {
                           limitValue& r = (*i).*_res;
                           if (r.leftovers == 1)
                              {
                                 r.given += minres;
                                 left    -= minres;
                                 
                                 //BTG_NOTICE(_logwrapper, descrLabel << " giving minres = " << minres << ", left = " << left );
                              }
                           count++;
                        }
                  }
            }

         // Still resources left, distribute.
         if (left > 0)
            {
               minres = left / size;
               count = 0;
               for (It i = _start; i != _end; ++i)
                  {
                     limitValue& r = (*i).*_res;
                     r.given      += minres;
                  }
               left = 0;
            }

         // Clear the used resources.
         count = 0;
         for (It i = _start; i != _end; ++i)
            {
               limitValue& r = (*i).*_res;
               
               if (r.given < giveIfNotNeeding)
                  {
                     r.given = giveIfNotNeeding;
                  }

               //BTG_NOTICE(_logwrapper, descrLabel << "count = " << count << ", given = " << r.given);

               r.used = 0;
               count++;
            }
		}

   } // namespace daemon
} // namespace btg


#endif // allocate_resources_impl_h


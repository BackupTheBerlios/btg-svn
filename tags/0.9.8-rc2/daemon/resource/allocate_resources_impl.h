/*

Copyright (c) 2003, Magnus Jonsson
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the distribution.
    * Neither the name of the author nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*/

// wojci: this was taken from libtorrent 0.12.
//        Changed slightly so it can be used on
//        different data.

#ifndef TORRENT_ALLOCATE_RESOURCES_IMPL_HPP_INCLUDED
#define TORRENT_ALLOCATE_RESOURCES_IMPL_HPP_INCLUDED

//#include <map>
//#include <utility>

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

namespace btg
{
   namespace daemon
   {
      
      int saturated_add(int a, int b);
      
		/// Give num_resources to r.
		/// @return How how many were actually accepted.
		inline int give(limitValue& r, int num_resources)
		{
			assert(num_resources >= 0);
			assert(r.given <= r.max);
			
			int accepted = (std::min)(num_resources, r.max - r.given);
			assert(accepted >= 0);

			r.given += accepted;
			assert(r.given <= r.max);

			return accepted;
		}

      /// Divide, rounding up.
		inline int div_round_up(int numerator, int denominator)
		{
			return (numerator + denominator - 1) / denominator;
		}

      /// Allocate resources (used for global limits).
		template<class It, class T>
		void allocate_resources_impl(
			int resources
			, It start
			, It end
			, limitValue T::* res)
		{
			assert(resources >= 0);

			for (It i = start; i != end; ++i)
			{
				limitValue& r = (*i).*res;
				r.leftovers = (std::max)(r.used - r.given, 0);
			}

			if (resources == limitValue::inf)
			{
				// No competition for resources.
				// Just give everyone what they want.
				for (It i = start; i != end; ++i)
				{
					((*i).*res).given = ((*i).*res).max;
				}
				return;
			}

			// Resources are scarce

			int sum_max = 0;
			int sum_min = 0;
			// the number of consumer that saturated their
			// quota last time slice
			int num_saturated = 0;
			// the total resources that those saturated their
			// quota used. This is used to calculate the mean
			// of the saturating consumers, in order to
			// balance their quotas for the next time slice.
			size_type saturated_sum = 0;
			for (It i = start; i != end; ++i)
			{
				limitValue& r = (*i).*res;
				sum_max = saturated_add(sum_max, r.max);
				assert(r.min < limitValue::inf);
				assert(r.min >= 0);
				assert(r.min <= r.max);
				sum_min += r.min;
				
				// a consumer that uses 95% or more of its assigned
				// quota is considered saturating
				size_type used = r.used;
				if (r.given == 0) continue;
				if (used * 20 / r.given >= 19)
				{
					++num_saturated;
					saturated_sum += r.given;
				}
			}

			if (sum_max <= resources)
			{
				// it turns out that there's no competition for resources
				// after all.
				for (It i = start; i != end; ++i)
				{
					((*i).*res).given = ((*i).*res).max;
				}
				return;
			}

			if (sum_min >= resources)
			{
				// the amount of resources is smaller than
				// the minimum resources to distribute, so
				// give everyone the minimum
				for (It i = start; i != end; ++i)
				{
					((*i).*res).given = ((*i).*res).min;
				}
				return;
			}

			// now, the "used" field will be used as a target value.
			// the algorithm following this loop will then scale the
			// used values to fit the available resources and store
			// the scaled values as given. So, the ratios of the
			// used values will be maintained.
			for (It i = start; i != end; ++i)
			{
				limitValue& r = (*i).*res;
				
				int target;
				size_type used = r.used;
				if (r.given > 0 && used * 20 / r.given >= 19)
				{
					assert(num_saturated > 0);
					target = div_round_up(saturated_sum, num_saturated);
					target += div_round_up(target, 10);
				}
				else
				{
					target = r.used;
				}
				if (target > r.max) target = r.max;
				else if (target < r.min) target = r.min;

				// move 12.5% towards the the target value
				r.used = r.given + div_round_up(target - r.given, 8);
				r.given = r.min;
			}


			resources = (std::max)(resources, sum_min);
			int resources_to_distribute = (std::min)(resources, sum_max) - sum_min;
			assert(resources_to_distribute >= 0);

			while (resources_to_distribute > 0)
			{
				// in order to scale, we need to calculate the sum of
				// all the used values.
				size_type total_used = 0;
				size_type max_used = 0;
				for (It i = start; i != end; ++i)
				{
					limitValue& r = (*i).*res;
					if (r.given == r.max) continue;

					assert(r.given < r.max);

					max_used = (std::max)(max_used, (size_type)r.used + 1);
					total_used += (size_type)r.used + 1;
				}


				size_type kNumer = resources_to_distribute;
				size_type kDenom = total_used;
				assert(kNumer >= 0);
				assert(kDenom >= 0);
				assert(kNumer <= (std::numeric_limits<int>::max)());

				if (kNumer * max_used <= kDenom)
				{
					kNumer = 1;
					kDenom = max_used;
					assert(kDenom >= 0);
				}

				for (It i = start; i != end && resources_to_distribute > 0; ++i)
				{
					limitValue& r = (*i).*res;
					if (r.given == r.max) continue;

					assert(r.given < r.max);

					size_type used = (size_type)r.used + 1;
					if (used < 1) used = 1;
					size_type to_give = used * kNumer / kDenom;
					if (to_give > resources_to_distribute)
						to_give = resources_to_distribute;
					assert(to_give >= 0);
					assert(to_give <= resources_to_distribute);

					resources_to_distribute -= give(r, (int)to_give);
					assert(resources_to_distribute >= 0);
				}

				assert(resources_to_distribute >= 0);
			}
			assert(resources_to_distribute == 0);
		}

   } // namespace daemon
} // namespace btg


#endif

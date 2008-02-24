/*

  Copyright (c) 2006, Magnus Jonsson, Arvid Norberg
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

#include "allocate_resources.h"
#include "allocate_resources_impl.h"

#include <cassert>
#include <algorithm>
#include <boost/limits.hpp>

namespace btg
{
   namespace daemon
   {

      int saturated_add(int a, int b)
      {
         assert(a >= 0);
         assert(b >= 0);
         assert(a <= limitValue::inf);
         assert(b <= limitValue::inf);
         assert(limitValue::inf + limitValue::inf < 0);

         unsigned int sum = unsigned(a) + unsigned(b);
         if (sum > unsigned(limitValue::inf))
            sum = limitValue::inf;

         assert(sum >= unsigned(a) && sum >= unsigned(b));
         return int(sum);
      }

      void allocate_resources(
                              int resources
                              , std::vector<sessionData>& _sessions
                              , limitValue sessionData::* res)
      {
         //typedef std::vector<session*>::iterator orig_iter;
         //typedef session* in_param;
         //typedef boost::transform_iterator<session& (*)(in_param), orig_iter> new_iter;
         
         allocate_resources_impl(
                                 resources
                                 , _sessions.begin()
                                 , _sessions.end()
                                 , res);
      }
      
   } // namespace daemon
} // namespace btg


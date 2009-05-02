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

/*
 * $Id$
 */

#include "allocate_resources.h"
#include "allocate_resources_impl.h"
//#include "allocate_resources_impl_lt.h"

#include <cassert>
#include <algorithm>
#include <boost/limits.hpp>

namespace btg
{
   namespace daemon
   {

      void allocate_resources(btg::core::LogWrapperType _logwrapper,
                              int _resources,
                              std::vector<sessionData>& _sessions,
                              limitValue sessionData::* _res)
      {
         allocate_resources_impl(_logwrapper,
                                 _resources,
                                 _sessions.begin(),
                                 _sessions.end(),
                                 _res);
      }
      
   } // namespace daemon
} // namespace btg


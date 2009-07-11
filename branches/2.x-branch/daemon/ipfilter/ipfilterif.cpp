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

#include "ipfilterif.h"

namespace btg
{
   namespace daemon
   {
      IpFilterIf::IpFilterIf(btg::core::LogWrapperType _logwrapper,
                             bool const _verboseFlag, 
                             TYPE const _type, 
                             std::string const& _filename)
         : btg::core::Logable(_logwrapper),
           verboseFlag_(_verboseFlag),
           type_(_type),
           filename_(_filename),
           initialized_(false),
           numberOfentries_(0),
           numberOfInvalidentries_(0),
           filter_()
      {
      }

      bool IpFilterIf::initialized() const
      {
         return initialized_;
      }

      t_uint IpFilterIf::numberOfEntries() const
      {
         return numberOfentries_;
      }

      IpFilterIf::~IpFilterIf()
      {

      }

   } // namespace daemon
} // namespace btg

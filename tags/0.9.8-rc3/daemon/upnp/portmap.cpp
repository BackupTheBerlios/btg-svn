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

#include "portmap.h"

namespace btg
{
   namespace daemon
   {
      namespace upnp
      {
         portMappingList::portMappingList()
            : protocol(portMappingList::UNDEF)
         {

         }

         portMappingList::portMappingList(std::pair<t_uint, t_uint> const& _range,
                                          std::string const& _externalAddress,
                                          std::string const& _internalAddress,
                                          PROTOCOL const _protocol)
            : externalAddress(_externalAddress),
              internalAddress(_internalAddress),
              protocol(_protocol)
         {
            for (t_uint counter = _range.first;
                 counter <= _range.second;
                 counter++)
               {
                  mapping.push_back(counter);
               }
         }

         portMappingList::~portMappingList()
         {
            mapping.clear();
         }

      } // namespace upnp
   } // namespace daemon
} // namespace btg

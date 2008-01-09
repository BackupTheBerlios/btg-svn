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

#ifndef UPNPIF_PORTMAP_H
#define UPNPIF_PORTMAP_H

#include <bcore/type.h>
#include <utility>
#include <string>
#include <vector>

namespace btg
{
   namespace daemon
      {
         namespace upnp
            {

               /**
                * \addtogroup upnp
                */
               /** @{ */

               /// Helper class used to describe ports to be mapped
               /// using UPnP.
               class portMappingList
                  {
                  public:
                     /// Supported protocols.
                     enum PROTOCOL
                        {
                           UNDEF = 0, //!< Undefined protocol.
                           TCP,       //!< TCP/IP.
                           UDP        //!< UDP/IP.
                        };
                  public:
                     /// Default constructor.
                     portMappingList();

                     /// Constructor.
                     /// @param [in] _range           Port range.
                     /// @param [in] _externalAddress External IP address.
                     /// @param [in] _internalAddress Internal IP address.
                     /// @param [in] _protocol        Protocol used.
                     portMappingList(std::pair<t_uint, t_uint> const& _range,
                                     std::string const& _externalAddress,
                                     std::string const& _internalAddress,
                                     PROTOCOL const _protocol);

                     /// Destructor.
                     ~portMappingList();

                  public:
                     /// Ports.
                     std::vector<t_uint> mapping;

                     /// The external address.
                     std::string externalAddress;

                     /// The internal address.
                     std::string internalAddress;

                     /// The protocol.
                     PROTOCOL    protocol;
                  };
               /** @} */

            } // namespace upnp
      } // namespace daemon
} // namespace btg

#endif // UPNPIF_PORTMAP_H

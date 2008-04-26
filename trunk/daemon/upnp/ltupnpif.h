/*
 * btg Copyright (C) 2008 Michael Wojciechowski.
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

#ifndef LTUPNPIF_H
#define LTUPNPIF_H

#include "upnpif.h"
#include "portmap.h"
#include <bcore/addrport.h>
#include <bcore/logable.h>

#include <asio.hpp>
#include <asio/io_service.hpp>
#include <libtorrent/upnp.hpp>
#include <boost/thread/mutex.hpp>

#include <vector>

#include <daemon/lt_version.h>

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

               class libtorrentUpnpIf: public upnpIf
                  {
                  public:
                     /// Constructor.
                     libtorrentUpnpIf(btg::core::LogWrapperType _logwrapper,
                                      bool const _verboseFlag,
                                      libtorrent::address const& _addr);
                     
                     bool open(std::pair<t_int, t_int> const& _range);

                     bool close();
                     
                     /// Destructor.
                     virtual ~libtorrentUpnpIf();

                  private:
                     void portMap(int _index, 
                                  int _externalPort, 
                                  std::string const& _errorMessage);
#if BTG_LT_0_14
                     std::string                  agent;
                     libtorrent::io_service       io;
                     libtorrent::connection_queue queue; 
                     libtorrent::upnp             upnp;

                     struct portIndex
                     {
                        enum
                           {
                              INVALID_INDEX = -1
                           };

                        portIndex()
                        : port(-1),
                           index_tcp(INVALID_INDEX),
                           index_udp(INVALID_INDEX),
                           enabled_tcp(false),
                           enabled_udp(false),
                           success_tcp(false),
                           success_udp(false)
                        {}

                        t_int port;
                        t_int index_tcp;
                        t_int index_udp;
                        bool  enabled_tcp;
                        bool  enabled_udp;
                        bool  success_tcp;
                        bool  success_udp;
                     };

                     bool findIndex(t_int const _index, portIndex & _portIndex);

                     boost::mutex                 indicesMutex;

                     portIndex                    currentPortIndex;
                     std::vector<portIndex>       indices;

                     t_uint                       portsMapped;
                     t_uint                       portsFailed;
#endif // BTG_LT_0_14
                  };

               /** @} */

            } // namespace upnp
      } // namespace daemon
} // namespace btg

#endif // LTUPNPIF_H

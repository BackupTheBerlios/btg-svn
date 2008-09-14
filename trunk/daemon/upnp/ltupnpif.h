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

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <libtorrent/upnp.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/intrusive_ptr.hpp>
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

               /// UPnP implementation, can use libtorrent 0.14 or better.
               class libtorrentUpnpIf: public upnpIf
               {
               public:
                  /// Constructor.
                  libtorrentUpnpIf(btg::core::LogWrapperType _logwrapper,
                                   bool const _verboseFlag,
                                   libtorrent::address const& _addr);
                  
                  bool open(std::pair<t_int, t_int> const& _range);
                  
                  void suspend();
                  
                  void resume();
                  
                  bool close();
                     
                  /// Destructor.
                  virtual ~libtorrentUpnpIf();
               protected:
                  /// Pointer to the log wrapper used by this class.
                  btg::core::LogWrapperType   logwrapper;
                  /// Indicates if this interface should do verbose
                  /// logging.
                  bool const                  verboseFlag;
                  /// The internal address used.
                  libtorrent::address const   addr;
                  /// The implementation.
                  class libtorrentUpnpIfImpl* impl;
                  /// State information from libtorrent.
                  void*                       state;
               };

               /// UPnP implementation, can use libtorrent 0.14 or better.
               class libtorrentUpnpIfImpl: public upnpIf
                  {
                  public:
                     /// Constructor.
                     libtorrentUpnpIfImpl(btg::core::LogWrapperType _logwrapper,
                                          bool const _verboseFlag,
                                          libtorrent::address const& _addr,
                                          void* _state = 0);
                     
                     bool open(std::pair<t_int, t_int> const& _range);

                     /// Suspend, saving state information to _state.
                     void suspend(void* _state);

                     bool close();
                     
                     /// Destructor.
                     virtual ~libtorrentUpnpIfImpl();

                  private:
                     /// Callback, indicates if a port was mapped.
                     void portMap(int _index, 
                                  int _externalPort, 
                                  std::string const& _errorMessage);

                     /// Agent name, received from network.
                     std::string                  agent;
                     /// Used by libtorrent UPnP interface.
                     libtorrent::io_service       io;
                     /// Used by libtorrent UPnP interface.
                     libtorrent::connection_queue queue; 
                     /// Pointer to the libtorrent UPnP interface.
                     boost::intrusive_ptr<libtorrent::upnp> upnp;

                     /// Used to keep track of which ports were
                     /// opened by this interface.
                     struct portIndex
                     {
                        enum
                           {
                              INVALID_INDEX = -1
                           };

                        /// Constructor.
                        portIndex()
                        : port(-1),
                           index_tcp(INVALID_INDEX),
                           index_udp(INVALID_INDEX),
                           success_tcp(false),
                           success_udp(false)
                        {}

                        /// Port.
                        t_int port;
                        /// Libtorrent id.
                        t_int index_tcp;
                        /// Libtorrent id.
                        t_int index_udp;
                        /// Indicates if the opening was sucessful.
                        bool  success_tcp;
                        /// Indicates if the opening was sucessful.
                        bool  success_udp;
                     };

                     std::vector<portIndex>::iterator findIndex(t_int _index);

                     /// Mutex used.
                     boost::mutex                 indicesMutex;
                     /// List of port indices.
                     std::vector<portIndex>       indices;
                     /// State information saved in suspend().
                     void*                        lt_drained_state;
                  };

               /** @} */

            } // namespace upnp
      } // namespace daemon
} // namespace btg

#endif // LTUPNPIF_H

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

#include "ltupnpif.h"
#include <boost/bind.hpp>
#include <bcore/verbose.h>
#include <daemon/modulelog.h>
#include <bcore/os/sleep.h>

#include <libtorrent/socket.hpp>
#include <libtorrent/connection_queue.hpp>
#include <libtorrent/time.hpp>
#include <boost/ref.hpp>
#include <boost/intrusive_ptr.hpp>
#include <asio/basic_deadline_timer.hpp>

namespace btg
{
   namespace daemon
      {
         namespace upnp
            {
               const std::string moduleName("upnp-lt");

               libtorrentUpnpIf::libtorrentUpnpIf(btg::core::LogWrapperType _logwrapper,
                                                  bool const _verboseFlag,
                                                  libtorrent::address const& _addr)
                  : upnpIf(_logwrapper, _verboseFlag),
                    agent("BTG"),
                    io(),
                    queue(io), 
                    upnp(io, 
                         queue, 
                         _addr, 
                         agent, 
                         boost::bind(&libtorrentUpnpIf::portMap, this, _1, _2, _3),
                         false),
                    indicesMutex(),
                    indices(),
                    pi()
               {
                  upnp.discover_device();

                  libtorrent::deadline_timer timer(io);
                  timer.expires_from_now(libtorrent::seconds(2));
                  timer.async_wait(boost::bind(&libtorrent::io_service::stop, boost::ref(io)));

                  MVERBOSE_LOG(logWrapper(), verboseFlag_, "Broadcasting for UPnP device.");

                  io.reset();
                  io.run();

                  MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                               "Router info: '" << upnp.router_model() << "'.");

                  initialized_ = true;
               }

               void libtorrentUpnpIf::portMap(int _index, 
                                              int _externalPort, 
                                              std::string const& _errorMessage)
               {
                  boost::mutex::scoped_lock scoped_lock(indicesMutex);

                  if (_errorMessage.size() == 0)
                     {
                        // Success.
                        if (pi.index_tcp == _index)
                           {
                              pi.success_tcp = true;
                              MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                                           "Mapped TCP port " << pi.port << ".");
                           }
                        else if (pi.index_udp == _index)
                           {
                              pi.success_udp = true;
                              MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                                           "Mapped UDP port " << pi.port << ".");
                           }
                     }
                  else
                     {
                        // Failure.
                        if (pi.index_tcp == _index)
                           {
                              BTG_MERROR(logWrapper(), 
                                         "Unable to map TCP port " << pi.port << ".");
                           }
                        else if (pi.index_udp == _index)
                           {
                              BTG_MERROR(logWrapper(), 
                                         "Unable to map TCP port " << pi.port << ".");
                           }
                     }
               }

               bool libtorrentUpnpIf::open(std::pair<t_int, t_int> const& _range)
               {
                  if (!initialized_)
                     {
                        return false;
                     }
                  
                  libtorrent::deadline_timer timer(io);

                  for (t_int port = _range.first;
                       port < _range.second;
                       port++)
                     {
                        MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                                    "Mapping port " << port << ".");

                        {
                           boost::mutex::scoped_lock scoped_lock(indicesMutex);
                           pi.port        = port;
                           pi.success_tcp = false;
                           pi.success_udp = false;
                           pi.index_tcp   = upnp.add_mapping(libtorrent::upnp::tcp, port, port);
                           pi.index_udp   = upnp.add_mapping(libtorrent::upnp::udp, port, port);
                        }
                        
                        timer.expires_from_now(libtorrent::seconds(10));
                        timer.async_wait(boost::bind(&libtorrent::io_service::stop, 
                                                     boost::ref(io)));
                        io.reset();
                        io.run();

                        if (!pi.success_tcp || !pi.success_udp)
                           {
                              MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                                           "Port " << port << " not mapped. Aborting.");
                              return false;
                           }

                        indices.push_back(pi);
                     }

                  MVERBOSE_LOG(logWrapper(), verboseFlag_, "Mapped all ports.");
                  return true;
               }

               bool libtorrentUpnpIf::close()
               {
                  if (!initialized_)
                     {
                        return false;
                     }

                  libtorrent::deadline_timer timer(io);

                  for (std::vector<portIndex>::const_iterator iter = indices.begin();
                       iter != indices.end();
                       iter++)
                     {
                        if (iter->index_tcp != portIndex::INVALID_INDEX)
                           {
                              MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                                           "Unmapping TCP port " << iter->port << ".");
                              upnp.delete_mapping(iter->index_tcp);
                           }
                        if (iter->index_udp != portIndex::INVALID_INDEX)
                           {
                              MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                                           "Unmapping UDP port " << iter->port << ".");
                              upnp.delete_mapping(iter->index_udp);
                           }

                        timer.expires_from_now(libtorrent::seconds(10));
                        timer.async_wait(boost::bind(&libtorrent::io_service::stop, 
                                                     boost::ref(io)));

                        io.reset();
                        io.run();
                     }

                  indices.clear();

                  upnp.close();

                  initialized_ = false;

                  return true;
               }
                     
               libtorrentUpnpIf::~libtorrentUpnpIf()
               {
                  if (initialized_)
                     {
                        upnp.close();
                     }
               }

            } // namespace upnp
      } // namespace daemon
} // namespace btg

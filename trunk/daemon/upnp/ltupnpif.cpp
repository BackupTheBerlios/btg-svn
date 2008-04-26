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
#if BTG_LT_0_14
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
                    portsMapped(0),
                    portsFailed(0)
               {
                  upnp.discover_device();

                  MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                               "Router info: '" << upnp.router_model() << "'.");

                  initialized_ = true;
               }
#else
{
}
#endif // BTG_LT_0_14

               void libtorrentUpnpIf::portMap(int _index, 
                                              int _externalPort, 
                                              std::string const& _errorMessage)
               {
                  boost::mutex::scoped_lock scoped_lock(indicesMutex);

                  portIndex pi;
                  if (!findIndex(_index, pi))
                     {
                        BTG_MERROR(logWrapper(), 
                                   "Unable to find index " << _index << ".");
                        return;
                     }

                  if (_errorMessage.size() == 0)
                     {
                        // Success.
                        if (pi.index_tcp == _index)
                           {
                              portsMapped++;
                              pi.success_tcp = true;
                              MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                                           "Mapped TCP port " << pi.port << ".");
                           }
                        else if (pi.index_udp == _index)
                           {
                              portsMapped++;
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
                              portsFailed++;
                              BTG_MERROR(logWrapper(), 
                                         "Unable to map TCP port " << pi.port << ".");
                           }
                        else if (pi.index_udp == _index)
                           {
                              portsFailed++;
                              BTG_MERROR(logWrapper(), 
                                         "Unable to map TCP port " << pi.port << ".");
                           }
                     }
               }

               bool libtorrentUpnpIf::findIndex(t_int const _index, portIndex & _portIndex)
               {
                  std::vector<portIndex>::iterator iter;
                  for (iter = indices.begin();
                       iter != indices.end();
                       iter++)
                     {
                        if (iter->index_tcp == _index || iter->index_udp == _index)
                           {
                              _portIndex = *iter;
                              return true;
                           }
                     }

                  return false;
               }

               bool libtorrentUpnpIf::open(std::pair<t_int, t_int> const& _range)
               {
#if BTG_LT_0_14
                  if (!initialized_)
                     {
                        return false;
                     }
                  
                  t_int numberOfPorts = 0;

                  for (t_int port = _range.first;
                       port < _range.second;
                       port++)
                     {
                        boost::mutex::scoped_lock scoped_lock(indicesMutex);

                        MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                                    "Mapping port " << port << ".");

                        currentPortIndex.port = port;

                        currentPortIndex.index_tcp = 
                           upnp.add_mapping(libtorrent::upnp::tcp, port, port);
                        if (currentPortIndex.index_tcp == portIndex::INVALID_INDEX)
                           {
                              // Write a verbose message.
                              BTG_MERROR(logWrapper(), 
                                         "Unable to map TCP port " << port << ".");
                           }
                        else
                           {
                              currentPortIndex.enabled_tcp = true;
                           }

                        currentPortIndex.index_udp = 
                           upnp.add_mapping(libtorrent::upnp::udp, port, port);

                        if (currentPortIndex.index_udp == portIndex::INVALID_INDEX)
                           {
                              // Write a verbose message.
                              BTG_MERROR(logWrapper(), 
                                         "Unable to map UDP port " << port << ".");
                           }
                        else 
                           {
                              currentPortIndex.enabled_udp = true;
                           }

                        indices.push_back(currentPortIndex);

                        numberOfPorts += 2;

                        if (currentPortIndex.index_tcp == portIndex::INVALID_INDEX || 
                            currentPortIndex.index_udp == portIndex::INVALID_INDEX)
                           {
                              return false;
                           }
                     }

                  // Wait for libtorrent to map all ports.
                  while (true)
                     {
                        if (portsMapped == numberOfPorts)
                           {
                              // Done.
                              MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                                           "Mapped all ports.");
                              return true;
                           }

                        if (portsFailed > 0)
                           {
                              return false;
                           }
                        if (terminate())
                           {
                              BTG_MERROR(logWrapper(), 
                                         "Terminating.");
                              return false;
                           }

                        btg::core::os::Sleep::sleepMiliSeconds(64);
                     }
                  return true;
#else
                  return false;
#endif
               }

               bool libtorrentUpnpIf::close()
               {
#if BTG_LT_0_14
                  if (!initialized_)
                     {
                        return false;
                     }

                  for (std::vector<portIndex>::const_iterator iter = indices.begin();
                       iter != indices.end();
                       iter++)
                     {
                        if (iter->index_tcp != portIndex::INVALID_INDEX)
                           {
                              upnp.delete_mapping(iter->index_tcp);
                           }
                        if (iter->index_udp != portIndex::INVALID_INDEX)
                           {
                              upnp.delete_mapping(iter->index_udp);
                           }
                     }

                  indices.clear();

                  return true;
#else
                  return false;
#endif // BTG_LT_0_14
               }
                     
               libtorrentUpnpIf::~libtorrentUpnpIf()
               {
#if BTG_LT_0_14
                  if (initialized_)
                     {
                        upnp.close();
                     }
#endif
               }

            } // namespace upnp
      } // namespace daemon
} // namespace btg

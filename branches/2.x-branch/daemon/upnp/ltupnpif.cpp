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
#include <bcore/btg_assert.h>

#include <libtorrent/socket.hpp>
#include <libtorrent/connection_queue.hpp>
#include <libtorrent/time.hpp>
#include <boost/ref.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/asio/basic_deadline_timer.hpp>

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
                    logwrapper(_logwrapper),
                    verboseFlag(_verboseFlag),
                    addr(_addr),
                    impl(new libtorrentUpnpIfImpl(_logwrapper, _verboseFlag, _addr, 0)),
                    state(0)
               {
               }

               bool libtorrentUpnpIf::open(std::pair<t_int, t_int> const& _range)
               {
                  btg::core::btg_assert(impl != 0, logwrapper, "UPnP impl pointer is null"); 
                  return impl->open(_range);
               }

               void libtorrentUpnpIf::suspend()
               {
                  btg::core::btg_assert(state == 0, logwrapper, "UPnP state pointer is not null"); 

                  impl->suspend(state);

                  delete impl;
                  impl = 0;

                  MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                               "Libtorrent UPnP interface suspended.");
               }
                  
               void libtorrentUpnpIf::resume()
               {
                  btg::core::btg_assert(state != 0, logwrapper, "UPnP state pointer is null"); 

                  impl = new libtorrentUpnpIfImpl(logwrapper, verboseFlag, addr, state);

                  MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                               "Libtorrent UPnP interface resumed.");
               }
                  
               bool libtorrentUpnpIf::close()
               {
                  btg::core::btg_assert(impl != 0, logwrapper, "UPnP impl pointer is null"); 
                  return impl->close();
               }
                     
               libtorrentUpnpIf::~libtorrentUpnpIf()
               {
                  delete impl;
                  impl = 0;
               }

               /* */

               libtorrentUpnpIfImpl::libtorrentUpnpIfImpl(btg::core::LogWrapperType _logwrapper,
                                                          bool const _verboseFlag,
                                                          libtorrent::address const& _addr,
                                                          void* _state)
                  : upnpIf(_logwrapper, _verboseFlag),
                    agent("BTG"),
                    io(),
                    queue(io), 
                    upnp(new libtorrent::upnp(io, 
                                              queue, 
                                              _addr, 
                                              agent, 
                                              boost::bind(&libtorrentUpnpIfImpl::portMap, this, _1, _2, _3),
                                              false,
                                              _state)),
                    indicesMutex(),
                    indices(),
                    lt_drained_state(0)
               {
                  upnp->discover_device();

                  libtorrent::deadline_timer timer(io);
                  timer.expires_from_now(libtorrent::seconds(2));
                  timer.async_wait(boost::bind(&libtorrent::io_service::stop, boost::ref(io)));

                  MVERBOSE_LOG(logWrapper(), verboseFlag_, "Broadcasting for UPnP device.");

                  io.reset();
                  io.run();

                  MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                               "Router info: '" << upnp->router_model() << "'.");

                  initialized_ = true;
               }

               std::vector<libtorrentUpnpIfImpl::portIndex>::iterator libtorrentUpnpIfImpl::findIndex(t_int _index)
               {
                  std::vector<portIndex>::iterator ret_iter = indices.end();

                  for (std::vector<portIndex>::iterator iter = indices.begin();
                       iter != indices.end();
                       iter++)
                     {
                        if ((iter->index_udp == _index) || (iter->index_tcp == _index))
                           {
                              ret_iter = iter;
                              break;
                           }
                     }

                  return ret_iter;
               }

               void libtorrentUpnpIfImpl::portMap(int _index, 
                                                  int _externalPort, 
                                                  std::string const& _errorMessage)
               {
                  boost::mutex::scoped_lock scoped_lock(indicesMutex);

                  std::vector<portIndex>::iterator iter = findIndex(_index);

                  if (iter == indices.end())
                     {
                        MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                                     "Uknown UPNP index " << _index << ".");
                        return;
                     }

                  if (_errorMessage.size() == 0)
                     {
                        // Success.
                        if (iter->index_tcp == _index)
                           {
                              iter->success_tcp = true;
                              MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                                           "Mapped TCP port " << iter->port << " to " << _externalPort << ".");
                           }
                        else if (iter->index_udp == _index)
                           {
                              iter->success_udp = true;
                              MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                                           "Mapped UDP port " << iter->port << " to " << _externalPort << ".");
                           }
                     }
                  else
                     {
                        // Failure.
                        if (iter->index_tcp == _index)
                           {
                              BTG_MERROR(logWrapper(), 
                                         "Unable to map TCP port " << iter->port << ".");
                           }
                        else if (iter->index_udp == _index)
                           {
                              BTG_MERROR(logWrapper(), 
                                         "Unable to map TCP port " << iter->port << ".");
                           }
                     }
               }

               bool libtorrentUpnpIfImpl::open(std::pair<t_int, t_int> const& _range)
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

                           portIndex pi;
                           pi.port        = port;
                           pi.success_tcp = false;
                           pi.success_udp = false;
                           pi.index_tcp   = upnp->add_mapping(libtorrent::upnp::tcp, port, port);
                           pi.index_udp   = upnp->add_mapping(libtorrent::upnp::udp, port, port);
                           
                           indices.push_back(pi);
                        }
                     }

                  timer.expires_from_now(libtorrent::seconds(10));
                  timer.async_wait(boost::bind(&libtorrent::io_service::stop, 
                                               boost::ref(io)));
                  io.reset();
                  io.run();

                  {
                     boost::mutex::scoped_lock scoped_lock(indicesMutex);

                     std::vector<portIndex>::const_iterator iter;
                     for (iter = indices.begin();
                          iter != indices.end();
                          iter++)
                        {
                           if (!iter->success_tcp || !iter->success_udp)
                              {
                                 MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                                              "Port " << iter->port << " not mapped. Aborting.");
                                 return false;
                              }
                        }
                  }

                  MVERBOSE_LOG(logWrapper(), verboseFlag_, "Mapped all ports.");
                  return true;
               }

               void libtorrentUpnpIfImpl::suspend(void* _state)
               {
                  io.reset();
                  io.stop();

                  _state = upnp->drain_state(); 
               }

               bool libtorrentUpnpIfImpl::close()
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
                              upnp->delete_mapping(iter->index_tcp);
                           }
                        if (iter->index_udp != portIndex::INVALID_INDEX)
                           {
                              MVERBOSE_LOG(logWrapper(), verboseFlag_, 
                                           "Unmapping UDP port " << iter->port << ".");
                              upnp->delete_mapping(iter->index_udp);
                           }

                        
                     }

                  timer.expires_from_now(libtorrent::seconds(5));
                  timer.async_wait(boost::bind(&libtorrent::io_service::stop, 
                                               boost::ref(io)));
                  io.reset();
                  io.run();
                  
                  indices.clear();

                  upnp->close();

                  timer.expires_from_now(libtorrent::seconds(5));
                  timer.async_wait(boost::bind(&libtorrent::io_service::stop, 
                                               boost::ref(io)));
                  
                  io.reset();
                  io.run();

                  initialized_ = false;

                  return true;
               }
                     
               libtorrentUpnpIfImpl::~libtorrentUpnpIfImpl()
               {
                  if (initialized_)
                     {
                        upnp->close();

                        io.reset();
                        io.run();
                     }
               }

            } // namespace upnp
      } // namespace daemon
} // namespace btg

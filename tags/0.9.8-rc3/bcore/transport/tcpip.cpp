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

#include "tcpip.h"

#include <bcore/logmacro.h>

#include <bcore/os/socket_exception.h>

namespace btg
{
   namespace core
   {
      tcpTransport::tcpTransport(LogWrapperType _logwrapper,
                                 btg::core::externalization::Externalization* _e,
                                 t_int const _bufferSize,
                                 DIRECTION const _direction,
                                 addressPort const & _addressPort,
                                 t_uint const _timeout)
         : tcpTransportBase(_logwrapper, _e, _bufferSize, _direction, _timeout)
      {
         setup(_direction, _addressPort, true);
      }

      tcpTransport::tcpTransport(LogWrapperType _logwrapper,
                                 btg::core::externalization::Externalization* _e,
                                 t_int const _bufferSize,
                                 DIRECTION const _direction,
                                 addressPort const & _addressPort,
                                 bool const _createSocket,
                                 t_uint const _timeout)
         : tcpTransportBase(_logwrapper, _e, _bufferSize, _direction, _timeout)
      {
         setup(_direction, _addressPort, _createSocket);
      }

      void tcpTransport::setup(DIRECTION const _direction,
                               addressPort const & _addressPort,
                               bool const _createSocket)
      {
         if(_createSocket)
            {
               switch (_direction)
                  {
                  case TO_SERVER:
                     {
                        try
                           {
                              btg::core::os::ClientSocket* tempClient =
                                 new btg::core::os::ClientSocket(logWrapper(), 
                                                                 _addressPort.getIp(), 
                                                                 _addressPort.getPort());

                              setSocket(tempClient);
                           }
                        catch (btg::core::os::socketException& _se)
                           {
                              initialized = false;
#if BTG_TRANSPORT_DEBUG
                              BTG_NOTICE(logWrapper(), "Failed to init client: " << _se.description());
#endif // BTG_TRANSPORT_DEBUG
                           }
                        break;
                     }
                  case FROM_SERVER:
                     {
                        try
                           {
                              btg::core::os::ServerSocket* tempServer = new btg::core::os::ServerSocket(logWrapper(), 
                                                                                                        _addressPort.getIp(),
                                                                                                        _addressPort.getPort());
                              setSocket(tempServer);
                           }
                        catch (btg::core::os::socketException& _se)
                           {
                              initialized = false;
#if BTG_TRANSPORT_DEBUG
                              BTG_NOTICE(logWrapper(), "Failed to init server: " << _se.description() );
#endif // BTG_TRANSPORT_DEBUG
                           }
                        // client = 0;
                        break;
                     }
                  }
            }
      }

      messageTransport::TRANSPORT tcpTransport::getType() const
      {
         return messageTransport::TCP;
      }

      void tcpTransport::acceptNewConnections()
      {
#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE(logWrapper(), "tcpipTransport::acceptNewConnections, accept on #" << server->getSockId() );
#endif // BTG_TRANSPORT_DEBUG
         // Handle accepting new clients here.
         btg::core::os::ServerSocket* acceptsock = new btg::core::os::ServerSocket(logWrapper());

         if (server->accept(*acceptsock))
            {

#if BTG_TRANSPORT_DEBUG
               BTG_NOTICE(logWrapper(), "Accepted new connection.");
#endif // BTG_TRANSPORT_DEBUG

               // Got a new client.
               tcpClient *tc = new tcpClient(logWrapper());
               tc->socket = acceptsock;
               tc->connectionID = -1;

               // Find a new connection ID
               for(int i = 0; i < MAX_CONNECTIONS_ALLOWED; i++)
                  {
                     if(clients.find(i) == clients.end())
                        {
                           tc->connectionID = i;
                           break;
                        }
                  }

               if(tc->connectionID == NO_CONNECTION_ID)
                  {
#if BTG_TRANSPORT_DEBUG
                     BTG_NOTICE(logWrapper(), "All connections are in use. Dropping connection.");
#endif // BTG_TRANSPORT_DEBUG
                     acceptsock->shutdown();
                     delete tc;
                     tc = 0;
                     delete acceptsock;
                     acceptsock = 0;
                     return;
                  }

               socketGroup.addSocket(acceptsock);
#if BTG_TRANSPORT_DEBUG
               BTG_NOTICE(logWrapper(), "Added new connection " << tc->connectionID);
#endif // BTG_TRANSPORT_DEBUG
               clients[tc->connectionID] = tc;
            }
         else
            {
#if BTG_TRANSPORT_DEBUG
               BTG_NOTICE(logWrapper(), "Rejected new connection.");
#endif // BTG_TRANSPORT_DEBUG
               delete acceptsock;
               acceptsock = 0;
            }
      }

      tcpTransport::~tcpTransport()
      {
      }

   } // namespace core
} // namespace btg

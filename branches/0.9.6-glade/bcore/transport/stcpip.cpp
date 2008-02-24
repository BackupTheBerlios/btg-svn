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

#include "stcpip.h"

#include <bcore/logmacro.h>

#include <bcore/os/socket_exception.h>

namespace btg
{
   namespace core
   {
      // Server constructor.
      secureTcpTransport::secureTcpTransport(btg::core::externalization::Externalization* _e,
                                             btg::core::os::gtlsGlobalServerData* _ggsd,
                                             t_int const _bufferSize,
                                             DIRECTION const _direction,
                                             addressPort const & _addressPort,
                                             t_uint const _timeout)
         : tcpTransportBase(_e, _bufferSize, _direction, _timeout)
      {
         try
            {
               btg::core::os::SecureServerSocket* tempServer =
                  new btg::core::os::SecureServerSocket(_ggsd, 
                                                        _addressPort.getIp(), 
                                                        _addressPort.getPort());

               setSocket(tempServer);
            }
         catch (btg::core::os::socketException& _se)
            {
               initialized = false;
#if BTG_TRANSPORT_DEBUG
               BTG_NOTICE("Failed to init server: " << _se.description() );
#endif // BTG_TRANSPORT_DEBUG
            }
      }

      // Client constructor.
      secureTcpTransport::secureTcpTransport(btg::core::externalization::Externalization* _e,
                                             btg::core::os::gtlsClientData* _gcd,
                                             t_int const _bufferSize,
                                             DIRECTION const _direction,
                                             addressPort const & _addressPort,
                                             t_uint const _timeout)
         : tcpTransportBase(_e, _bufferSize, _direction, _timeout)
      {
         try
            {
               btg::core::os::SecureClientSocket* tempClient =
                  new btg::core::os::SecureClientSocket(_gcd, _addressPort.getIp(), _addressPort.getPort());

               setSocket(tempClient);

            }
         catch (btg::core::os::socketException& _se)
            {
               initialized = false;
#if BTG_TRANSPORT_DEBUG
               BTG_NOTICE("Failed to init client: " << _se.description());
#endif // BTG_TRANSPORT_DEBUG
            }
      }

      messageTransport::TRANSPORT secureTcpTransport::getType() const
      {
         return messageTransport::STCP;
      }

      void secureTcpTransport::acceptNewConnections()
      {
#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE("secureTcpTransport::acceptNewConnections, accept on #" << server->getSockId() );
#endif // BTG_TRANSPORT_DEBUG
         // Handle accepting new clients here.
         btg::core::os::SecureServerSocket* acceptsock = new btg::core::os::SecureServerSocket();

         if (server->accept(*acceptsock))
            {

#if BTG_TRANSPORT_DEBUG
               BTG_NOTICE("Accepted new connection.");
#endif // BTG_TRANSPORT_DEBUG

               // Got a new client.
               tcpClient *tc = new tcpClient;
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
                     BTG_NOTICE("All connections are in use. Dropping connection.");
                     acceptsock->shutdown();
                     delete tc;
                     tc = 0;
                     delete acceptsock;
                     acceptsock = 0;
                     return;
                  }

               socketGroup.addSocket(acceptsock);

               BTG_NOTICE("Added new connection " << tc->connectionID);
               clients[tc->connectionID] = tc;
            }
         else
            {
               BTG_NOTICE("Rejected new connection.");
               delete acceptsock;
               acceptsock = 0;
            }
      }

      secureTcpTransport::~secureTcpTransport()
      {
      }

   } // namespace core
} // namespace btg

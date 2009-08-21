/*
 * btg Copyright (C) 2005 Michael Wojciechowski.
 * HTTP/XMLRPC part written by Johan Ström.
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

#include "shttp.h"
#include "http.h"

#include <bcore/logmacro.h>
#include <bcore/util.h>
#include <bcore/os/socket_exception.h>
#include <bcore/os/sleep.h>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>

namespace btg
{
   namespace core
   {
      // Server constructor.
      secureHttpTransport::secureHttpTransport(LogWrapperType _logwrapper,
                                               btg::core::externalization::Externalization* _e,
                                               btg::core::os::gtlsGlobalServerData* _ggsd,
                                               t_int const _bufferSize,
                                               DIRECTION const _direction,
                                               addressPort const & _addressPort,
                                               t_uint const _timeout)
         : httpTransport(_logwrapper, _e, _bufferSize, _direction, _addressPort, false, _timeout)
      {
         try
            {
               btg::core::os::SecureServerSocket* tempServer =
                  new btg::core::os::SecureServerSocket(logWrapper(),
                                                        _ggsd,
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
      }

      // Client constructor.
      secureHttpTransport::secureHttpTransport(LogWrapperType _logwrapper,
                                               btg::core::externalization::Externalization* _e,
                                               btg::core::os::gtlsClientData* _gcd,
                                               t_int const _bufferSize,
                                               DIRECTION const _direction,
                                               addressPort const & _addressPort,
                                               t_uint const _timeout)
         : httpTransport(_logwrapper, _e, _bufferSize, _direction, _addressPort, false, _timeout)
      {
         try
            {
               btg::core::os::SecureClientSocket* tempClient =
                  new btg::core::os::SecureClientSocket(logWrapper(),
                                                        _gcd, 
                                                        _addressPort.getIp(), 
                                                        _addressPort.getPort()
                                                        );

               // When calling the httpTransport initializer we
               // created a ClientSocket. This will be deleted in
               // this setSocket call.
               setSocket(tempClient);

            }
         catch (btg::core::os::socketException& _se)
            {
               initialized = false;
#if BTG_TRANSPORT_DEBUG
               BTG_NOTICE(logWrapper(), "Failed to init client: " << _se.description());
#endif // BTG_TRANSPORT_DEBUG
            }
      }

      messageTransport::TRANSPORT secureHttpTransport::getType() const
      {
         return messageTransport::SXMLRPC;
      }

      void secureHttpTransport::acceptNewConnections()
      {
         // Handle accepting new clients here.
         btg::core::os::SecureServerSocket* acceptsock = new btg::core::os::SecureServerSocket(logWrapper());

         if (server->accept(*acceptsock))
            {

#if BTG_TRANSPORT_DEBUG
               BTG_NOTICE(logWrapper(), "Accepted new connection. (" << reinterpret_cast<void*>(acceptsock) << ") id=" << acceptsock->getSockId());
#endif // BTG_TRANSPORT_DEBUG

               // Got a new client.
               httpClient* hc   = new httpClient(logWrapper(), getGzipIf());
               hc->socket       = acceptsock;
               hc->connectionID = -1;

               // Find a new connection ID
               for (int i = 0; i < MAX_CONNECTIONS_ALLOWED; i++)
                  {
                     if (clients.find(i) == clients.end())
                        {
                           hc->connectionID = i;
                           break;
                        }
                  }

               if(hc->connectionID == NO_CONNECTION_ID)
                  {
                     BTG_NOTICE(logWrapper(), "All connections are in use. Dropping connection.");
                     acceptsock->shutdown();
                     delete hc;
                     hc = 0;
                     delete acceptsock;
                     acceptsock = 0;
                     return;
                  }

               socketGroup.addSocket(acceptsock);

               BTG_NOTICE(logWrapper(), "Added new connection " << hc->connectionID);
               clients[hc->connectionID] = hc;
            }
         else
            {
               delete acceptsock;
               acceptsock = 0;
            }
      }

      secureHttpTransport::~secureHttpTransport()
      {
      }
   } // namespace core
} // namespace btg

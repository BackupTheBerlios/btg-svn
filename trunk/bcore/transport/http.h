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

#ifndef HTTP_H
#define HTTP_H

#include "transport.h"
#include <bcore/dbuf.h>
#include <bcore/type.h>
#include "tcpip.h"
#include "httprequest.h"
#include <bcore/os/gzipif.h>

#include <map>
#include <string>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup transport
          */
         /** @{ */

         /// Class used for representing HTTP clients.
         class httpClient: public tcpClient
            {
            public:
               /// Constructor.
               httpClient(btg::core::os::gzipIf* _gzipif);
               /// HTTP request.
               httpRequestServerSide httpRequest;
            };

         /// A http transport.
         class httpTransport: public tcpTransport
            {
            public:
               /// Constructor.
               /// @param [in] _e              The externalization used.
               /// @param [in] _bufferSize     Buffer size.
               /// @param [in] _direction      Direction.
               /// @param [in] _addressPort    Address and port.
               /// @param [in] _timeout        Max time in milliseconds to block in a read(), 0 for no block.
               httpTransport(btg::core::externalization::Externalization* _e,
                             t_int const _bufferSize,
                             DIRECTION const _direction,
                             addressPort const & _addressPort,
                             t_uint const _timeout);

               /// Constructor.
               /// @param [in] _e              The externalization used.
               /// @param [in] _bufferSize     Buffer size.
               /// @param [in] _direction      Direction.
               /// @param [in] _addressPort    Address and port.
               /// @param [in] _createSocket   Flag deciding if a socket should be created (used when subclassing)
               /// @param [in] _timeout        Max time in milliseconds to block in a read(), 0 for no block.
               httpTransport(btg::core::externalization::Externalization* _e,
                             t_int const _bufferSize,
                             DIRECTION const _direction,
                             addressPort const & _addressPort,
                             bool const _createSocket,
                             t_uint const _timeout);

               /// Get the type of this transport.
               messageTransport::TRANSPORT getType() const;

               /// Read data and store it in a buffer.
               t_int read(dBuffer & _buffer);

               /// Read data and store it in a buffer.
               t_int read(dBuffer & _buffer, t_int & _connectionID);

               /// Get the gzip interface used by this instance.
               btg::core::os::gzipIf* getGzipIf() const;
 
               /// Destructor.
               virtual ~httpTransport();

            private:
               /// Setup function shared between constructors.
               /// @param [in] _direction      Direction.
               void setup(DIRECTION const _direction);

               /// Write a message.
               t_int write(t_byteCP _msg, t_int const _size, t_int const _connectionID);

               /// Accept new connections.
               void acceptNewConnections();

               /// Create a timestamp.
               std::string createTimestamp();

               /// Client socket.
               httpRequestClientSide* clientRequest;

               /// The gzip interface used by this instance.
               btg::core::os::gzipIf* gzipif;
            private:
               /// Copy constructor.
               httpTransport(httpTransport const& _ht);
               /// Assignment operator.
               httpTransport& operator=(httpTransport const& _ht);
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

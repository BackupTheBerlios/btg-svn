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
 * $Id: tcpipbase.h,v 1.1.4.6 2006/08/12 17:37:23 wojci Exp $
 */

#ifndef TCPIPBASE_H
#define TCPIPBASE_H

#include "transport.h"
#include <bcore/dbuf.h>

#include <bcore/os/socket.h>
#include <bcore/os/socketgroup.h>

#include <map>

namespace btg
{
   namespace core
      {

         /**
          * \addtogroup transport
          */
         /** @{ */

         /// Reperesents a clients which uses TCP/IP to connect the
         /// the daemon.
         class tcpClient
            {
            public:
               /// Constructor.
               tcpClient();

               /// Destructor.
               virtual ~tcpClient();

               /// Equality operator.
               virtual bool operator== (tcpClient  const & _tcpClient) const;

               /// Pointer to the socket used.
               btg::core::os::Socket* socket;

               /// The connection id.
               t_int connectionID;
            private:
               /// Copy constructor.
               tcpClient(tcpClient const& _tc);
               /// Assignment operator.
               tcpClient& operator=(tcpClient const& _tc);
            };

         /// A TCP/IP transport.
         class tcpTransportBase: public messageTransport
            {
            public:
               /// Constructor.
               /// @param [in] _e              Externalization.
               /// @param [in] _bufferSize     Buffer size.
               /// @param [in] _direction      Direction.
               /// @param [in] _timeout        Max time in milliseconds to block in a read(), 0 for no block.
               tcpTransportBase(btg::core::externalization::Externalization* _e,
                                t_int const _bufferSize,
                                DIRECTION const _direction,
                                t_uint const _timeout);

               /// Set the socket to be used.
               /// @param [in] _socket The socket to set.
               void setSocket(btg::core::os::Socket* _socket);

               /// Write a buffer.
               virtual t_int write(dBuffer & _msg);

               /// Write a buffer.
               virtual t_int write(dBuffer & _msg, t_int const _connectionID);

               /// Read data and store it in a buffer.
               virtual t_int read(dBuffer & _buffer);

               /// Read data and store it in a buffer.
               virtual t_int read(dBuffer & _buffer, t_int & _connectionID);

               /// Returns true if this transport is initialized.
               virtual bool isInitialized() const;

               /// Get the type of this transport.
               virtual messageTransport::TRANSPORT getType() const;

               /// Called to end a connection.
               void endConnection(t_int _connectionID);

               /// Destructor.
               virtual ~tcpTransportBase();
            protected:
               /// Write a message.
               virtual t_int write(t_byteCP _msg, t_int const _size, t_int const _connectionID);

               /// Resolve a socket to a tcpClient
               virtual tcpClient * resolve(btg::core::os::Socket* _socket);

               /// Resolve a connection id to a tcpClient
               virtual tcpClient * resolve(t_int _connectionID);

               /// Accept new connections.
               virtual void acceptNewConnections() = 0;

               /// Return true, if there is more to be read from the socket.
               virtual bool continueReading(const btg::core::os::Socket* _socket);

               /// The buffer used to send/receive packets.
               t_byteP                sndrec_buffer;

               /// Server socket.
               btg::core::os::Socket* server;

               /// Client socket.
               btg::core::os::Socket* client;

               /// Keeps track of connected clients.
               std::map<t_int, tcpClient*> clients;

               /// Used for selecting on sockets.
               btg::core::os::SocketGroup socketGroup;
            private:
               /// Copy constructor.
               tcpTransportBase(tcpTransportBase const& _ttb);
               /// Assignment operator.
               tcpTransportBase& operator=(tcpTransportBase const& _ttb);
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

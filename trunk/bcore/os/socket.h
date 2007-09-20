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
 * The original class was from an article by Rob Tougher,
 * Linux Socket Programming In C++,
 * http://www.linuxgazette.com/issue74/tougher.html#1.
 */

/*
 * $Id: socket.h,v 1.1.2.14 2006/11/29 18:06:29 wojci Exp $
 */

#ifndef SOCKET_H
#define SOCKET_H

#include <bcore/type.h>
#include <bcore/addrport.h>

#include <string>
#include <map>
#include <vector>

extern "C"
{
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
}

namespace btg
{
   namespace core
      {
         namespace os
            {
               /**
                * \addtogroup OS
                */
               /** @{ */

               /// Implements a low level tcp/ip socket.
               class Socket
                  {
                  public:

                     enum
                        {
                           /// Max number of clients an instance of
                           /// this class will accept.
                           MAXCONNECTIONS = 1024,
                           /// Indicates that a socket is not
                           /// initalized or was deleted.
                           UNINITIALIZED  = -1
                        };

                     /// Constructor.
                     Socket();

                     /// Copy constructor.
                     Socket(Socket const& _socket);

                     /// Create a socket.
                     virtual bool create();

                     /// Bind to a port.
                     virtual bool bind(std::string const& _host, const t_uint _port);

                     /// Listen.
                     virtual bool listen() const;

                     /// Accept a client.
                     virtual bool accept(Socket & _socket) const;

                     /// Client initialization.
                     /// @param [in] _host Host to connect to.
                     /// @param [in] _port Port to be used.
                     virtual bool connect(std::string const& _host, const t_uint _port);

                     // Data Transimission

                     /// Send a message.
                     virtual bool send(t_byteCP _msg, t_int const _size);

                     /// Receive a message. If the recv call is
                     /// sucessful, update _readCount and return
                     /// true. Else return false.
                     virtual bool recv(t_byteP _msg, t_int const _msg_size, t_int & _readCount);

                     /// Write.
                     virtual bool write(t_byteCP _msg, t_int const _size) = 0;

                     /// Read.
                     virtual t_int read(t_byteP _msg, t_int const _size) = 0;

                     /// Shutdown socket
                     virtual void shutdown();

                     /// Returns true if this socket is valid.
                     virtual bool is_valid() const;

                     /// The assignment operator.
                     Socket& operator= (Socket const& _socket);

                     /// The equality operator.
                     bool operator== (Socket const& _socket) const;

                     /// Return the socket id.
                     virtual t_int getSockId() const;

                     /// Returns true if this socket was deleted.
                     virtual bool deleted() const;

                     /// Mark this socket for deletion.
                     void markAsDeleted();

                     /// Destructor.
                     virtual ~Socket();

                  protected:
                     /// Indicates that this socket should be deleted.
                     bool deleted_;

                  private:
                     /// Socket id.
                     t_int       sock_;

                     /// Socket address.
                     sockaddr_in addr_;
                  };

               /** @} */

            } // namespace os
      } // namespace core
} // namespace btg

#endif

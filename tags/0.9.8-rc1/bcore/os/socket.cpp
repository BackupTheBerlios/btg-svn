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
 * $Id$
 */

#include "socket.h"

extern "C"
{
#include <errno.h>
#include <fcntl.h>
}

namespace btg
{
   namespace core
   {
      namespace os
      {

         Socket::Socket(LogWrapperType _logwrapper)
            : Logable(_logwrapper),
              deleted_(false),
              sock_(Socket::UNINITIALIZED)

         {
            memset (&addr_, 0, sizeof(addr_));
         }

         Socket::Socket(Socket const& _socket)
            : Logable(_socket),
              deleted_(_socket.deleted_),
              sock_(_socket.sock_),
              addr_(_socket.addr_)
         {
         }

         bool Socket::create()
         {
            sock_ = socket ( AF_INET, SOCK_STREAM, 0);

            if (!is_valid())
               {
#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE(logWrapper(), "Socket::create, false");
#endif // BTG_TRANSPORT_DEBUG
                  return false;
               }

            // TIME_WAIT - argh
            t_int on = 1;
            if (setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&on), sizeof(on)) == -1)
               {
                  return false;
               }
            //#if defined(__APPLE__) && defined(__MACH__)
#if !defined(MSG_NOSIGNAL)
            // On OSX, MSG_NOSIGNAL is not supported.
            // Instead use SO_NOSIGPIPE.
            on = 1;
            if (setsockopt(sock_, SOL_SOCKET, SO_NOSIGPIPE, reinterpret_cast<const char*>(&on), sizeof(on)) == -1)
               {
#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE(logWrapper(), "Socket::create, false");
#endif // BTG_TRANSPORT_DEBUG
                  return false;
               }
#endif // OSX
#if BTG_TRANSPORT_DEBUG
            BTG_NOTICE(logWrapper(), "Socket::create, true");
#endif // BTG_TRANSPORT_DEBUG
            return true;
         }

         bool Socket::bind(std::string const& _host, const t_uint _port)
         {
            if (!is_valid())
               {
#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE(logWrapper(), "Socket::bind, false");
#endif // BTG_TRANSPORT_DEBUG
                  return false;
               }

            struct in_addr addr;

            if (_host == "0.0.0.0")
               {
                  addr.s_addr = INADDR_ANY;
                  // addr = INADDR_ANY;
               }
            else
               {
                  if (inet_aton(_host.c_str(), &addr) == 0)
                     {
                        // Converting the address as a string to a
                        // binary representation failed.
#if BTG_TRANSPORT_DEBUG
                        BTG_NOTICE(logWrapper(), "Socket::bind, false");
#endif // BTG_TRANSPORT_DEBUG
                        return false;
                     }
               }

            addr_.sin_family      = AF_INET;
            addr_.sin_addr.s_addr = addr.s_addr;
            addr_.sin_port        = htons ( _port );

            t_int bind_return = ::bind (sock_, reinterpret_cast<struct sockaddr*>(&addr_), sizeof(addr_));

            if (bind_return == -1)
               {
                  perror("bind");
#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE(logWrapper(), "Socket::bind, false");
#endif // BTG_TRANSPORT_DEBUG
                  return false;
               }
#if BTG_TRANSPORT_DEBUG
            BTG_NOTICE(logWrapper(), "Socket::bind, true");
#endif // BTG_TRANSPORT_DEBUG
            return true;
         }


         bool Socket::listen() const
         {
            if (!is_valid())
               {
#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE(logWrapper(), "Socket::listen, false");
#endif // BTG_TRANSPORT_DEBUG
                  return false;
               }

            t_int listen_return = ::listen(sock_, Socket::MAXCONNECTIONS);

            if (listen_return == -1)
               {
#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE(logWrapper(), "Socket::listen, false");
#endif // BTG_TRANSPORT_DEBUG
                  return false;
               }
#if BTG_TRANSPORT_DEBUG
            BTG_NOTICE(logWrapper(), "Socket::listen, true");
#endif // BTG_TRANSPORT_DEBUG
            return true;
         }

         bool Socket::accept(Socket & _new_socket) const
         {
            sockaddr  addr;
            socklen_t addr_length = sizeof(addr);
            //t_int addr_length = sizeof(addr_);
            _new_socket.sock_ = ::accept(sock_, &addr, &addr_length);

            if (_new_socket.sock_ == -1)
               {
#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE(logWrapper(), "Socket::accept, false");
#endif // BTG_TRANSPORT_DEBUG
                  return false;
               }
#if BTG_TRANSPORT_DEBUG
            const int ntopBufferSize = 512;
            char ntopBuffer[ntopBufferSize];

            sockaddr_in* addr_i = reinterpret_cast<sockaddr_in*>(&addr);

            inet_ntop (AF_INET, &addr_i->sin_addr /* &addr_.sin_addr */, ntopBuffer, ntopBufferSize);
            BTG_NOTICE(logWrapper(), "Connection from " << ntopBuffer << ", port " << ntohs(addr_i->sin_port) /* ntohs(addr_.sin_port)*/);
#endif // BTG_TRANSPORT_DEBUG

#if BTG_TRANSPORT_DEBUG
            BTG_NOTICE(logWrapper(), "Socket::accept, id = " << static_cast<int>(_new_socket.sock_) << ", true");
#endif // BTG_TRANSPORT_DEBUG
            return true;
         }

         bool Socket::send(t_byteCP _msg, t_int const _size)
         {
            if (!is_valid())
               {
                  return false;
               }

            bool status = true;

            t_int stat = ::send (sock_, _msg, _size,
                                 //#if defined(__APPLE__) && defined(__MACH__)
#if !defined(MSG_NOSIGNAL)
                                 0
#else
                                 MSG_NOSIGNAL
#endif
                                 );
            if (stat == Socket::UNINITIALIZED)
               {
                  status = false;
               }

            return status;
         }

         bool Socket::recv(t_byteP _msg, t_int const _msg_size, t_int & _readCount)
         {
            if (!is_valid())
               {
                  return false;
               }

            bool status = true;

            memset(_msg, 0, _msg_size);

            t_int stat = ::recv(sock_, _msg, _msg_size, 0);

            if (stat == 0)
               {
		  markAsDeleted();
                  _readCount  = 0;
                  status      = false;
               }
            else if (stat == -1)
               {
                  status      = false;
                  _readCount  = 0;
               }
            else
               {
                  _readCount = stat;
                  status     = true;
               }

            return status;
         }

         bool Socket::connect(std::string const& _host, const t_uint _port)
         {
            if (!is_valid())
               {
                  return false;
               }

            addr_.sin_family = AF_INET;
            addr_.sin_port = htons ( _port );

            t_int status = inet_pton ( AF_INET, _host.c_str(), &addr_.sin_addr );

            if (errno == EAFNOSUPPORT)
               {
                  return false;
               }

            status = ::connect(sock_, reinterpret_cast<sockaddr*>(&addr_), sizeof(addr_));

            if ( status == 0 )
               {
                  return true;
               }

            return false;
         }

         bool Socket::is_valid() const
         {
            bool status = false;

            if (sock_ != Socket::UNINITIALIZED)
               {
                  status = true;
               }

            return status;
         }

         Socket& Socket::operator= (Socket const& _socket)
         {
            bool status = (_socket == *this);

            if (!status)
               {
                  Logable::operator=(_socket);

                  sock_ = _socket.sock_;
                  addr_ = _socket.addr_;
               }
            return *this;
         }

         bool Socket::operator== (Socket const& _socket) const
         {
            bool status = Logable::operator==(_socket);

            if (sock_ != _socket.sock_)
               {
                  status = false;
               }

            return status;
         }

         void Socket::shutdown()
         {
            if (is_valid())
               {
                  ::shutdown(sock_, SHUT_RDWR);
               }
         }

         t_int Socket::getSockId() const
         {
            return sock_;
         }

         bool Socket::deleted() const
         {
#if BTG_TRANSPORT_DEBUG
            BTG_NOTICE(logWrapper(), "Checking if socket with id = " << sock_ <<
                       " is deleted, result = " << deleted_);
#endif // BTG_TRANSPORT_DEBUG
            return deleted_;
         }

         void Socket::markAsDeleted()
         {
#if BTG_TRANSPORT_DEBUG
            BTG_NOTICE(logWrapper(), "Marking socket with id = " << sock_ << " as deleted");
#endif // BTG_TRANSPORT_DEBUG

            deleted_ = true;
         }

         Socket::~Socket()
         {
            if (is_valid())
               {
                  ::close(sock_);
               }
         }

      } // namespace os
   } // namespace core
} // namespace btg

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

#include "tcps.h"
#include "socket_exception.h"

#include <bcore/logmacro.h>

namespace btg
{
   namespace core
   {
      namespace os
      {
         ServerSocket::ServerSocket(LogWrapperType _logwrapper,
                                    std::string const& _host,
                                    t_uint const _port)
            : Socket::Socket(_logwrapper)
         {
            if (!Socket::create())
               {
                  throw socketException("Could not create server socket.");
               }

#if BTG_TRANSPORT_DEBUG
            BTG_NOTICE(logWrapper(), "Attempt to bind to port " << _port);
#endif // BTG_TRANSPORT_DEBUG

            if (!Socket::bind(_host, _port))
               {
                  throw socketException("Could not bind to port.");
               }

            if (!Socket::listen())
               {
                  throw socketException("Could not listen to socket.");
               }
         }

         ServerSocket::ServerSocket(ServerSocket const& _serversocket)
            : Socket::Socket(_serversocket)
         {
         }

         ServerSocket::ServerSocket(LogWrapperType _logwrapper)
            : Socket::Socket(_logwrapper)
         {

         }

         bool ServerSocket::write(t_byteCP _msg, t_int const _size)
         {
            if (!Socket::is_valid())
               {
#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE(logWrapper(), "(write) Marking socket with id=" << getSockId() << " as deleted");
#endif // BTG_TRANSPORT_DEBUG
                  this->markAsDeleted();
               }

            bool status = true;

            if (!Socket::send(_msg, _size))
               {
                  status = false;
               }
            return status;
         }

         t_int ServerSocket::read(t_byteP _msg, t_int const _size)
         {
            if (!Socket::is_valid())
               {
#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE(logWrapper(), "(read) Marking socket with id=" << getSockId() << " as deleted");
#endif // BTG_TRANSPORT_DEBUG
                  this->markAsDeleted();
               }

            t_int readCount = 0;

            if (!Socket::recv(_msg, _size, readCount))
               {
                  readCount = 0;
               }

            return readCount;
         }

         bool ServerSocket::accept(Socket & _socket) const
         {
            bool status = true;

            if (!Socket::accept(_socket))
               {
                  status = false;
                  // throw socketException ( "Could not accept socket." );
               }

            return status;
         }

         t_int ServerSocket::getSockId() const
         {
            return Socket::getSockId();
         }

         ServerSocket::~ServerSocket()
         {
         }

      } // namespace os
   } // namespace core
} // namespace btg


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

#include "tcpc.h"

#include "socket_exception.h"

#include <bcore/logmacro.h>
#include <bcore/helpermacro.h>

namespace btg
{
   namespace core
   {
      namespace os
      {

         ClientSocket::ClientSocket (std::string const& _host, t_uint const _port)
         {
            if (!Socket::create())
               {
                  throw socketException("Could not create client socket.");
               }

            if (!Socket::connect(_host, _port))
               {
                  std::stringstream s;
                  s << "Could not connect to " << _host << ":" << _port << ".";
                  throw socketException(s.str());
               }
         }

         bool ClientSocket::write(t_byteCP _msg, t_int const _size)
         {
            bool status = true;

            if (!Socket::send(_msg, _size))
               {
                  status = false;
               }
            return status;
         }

         t_int ClientSocket::read(t_byteP _msg, t_int const _size)
         {
            t_int readCount = 0;

            if (!Socket::recv(_msg, _size, readCount))
               {
                  readCount = 0;
               }

            return readCount;
         }

      } // namespace os
   } // namespace core
} // namespace btg





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

#include "stcpc.h"

#include "socket_exception.h"

#include <bcore/logmacro.h>
#include <bcore/helpermacro.h>

#include "gnutlsmacro.h"

namespace btg
{
   namespace core
   {
      namespace os
      {

         SecureClientSocket::SecureClientSocket(LogWrapperType _logwrapper,
                                                gtlsClientData* _gcd,
                                                std::string const& _host,
                                                t_uint const _port)
            : Socket(_logwrapper),
              gcd_(_gcd)
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

            int sockid                   = getSockId();
            gnutls_session_t tempSession = gcd_->session();

            gnutls_handshake_set_max_packet_length(tempSession, 0);

            gnutls_transport_set_ptr(tempSession, reinterpret_cast<gnutls_transport_ptr>(sockid));

            int ret = gnutls_handshake(tempSession);

            if (ret < 0)
               {
                  gnutls_perror(ret);
                  throw socketException("GNU TLS: handshake failed.");
               }
         }

         bool SecureClientSocket::write(t_byteCP _msg, t_int const _size)
         {
            bool status = true;

            t_byteCP dataptr         = _msg;
            ssize_t total_bytes_sent = 0;
            ssize_t bytes_sent       = 0;

            // Big messages has to be sent in chunks...
            while (total_bytes_sent < _size)
               {
                  bytes_sent = gnutls_record_send(gcd_->session(), dataptr, _size - total_bytes_sent);

                  // Check for errors
                  if (bytes_sent < 0)
                     {
                        if ((bytes_sent == GNUTLS_E_AGAIN) || (bytes_sent == GNUTLS_E_INTERRUPTED))
                           {
                              // Docs says try again if these errors are encountered
                              continue;
                           }
                        else
                           {
                              gnutls_perror(bytes_sent);
                              throw socketException("SecureClientSocket::write() failed");
                           }
                     }

                  // No errors, increase total bytes sent counter and data pointer
                  total_bytes_sent += bytes_sent;
                  dataptr          += bytes_sent;
               }

            return status;
         }

         t_int SecureClientSocket::read(t_byteP _msg, t_int const _size)
         {
            t_int readCount = gnutls_record_recv(gcd_->session(), _msg, _size);

            if (readCount <= 0)
               {
                  readCount = 0;
               }

            return readCount;
         }

         SecureClientSocket::~SecureClientSocket()
         {
            gnutls_bye(gcd_->session(), GNUTLS_SHUT_WR);
         }

      } // namespace os
   } // namespace core
} // namespace btg





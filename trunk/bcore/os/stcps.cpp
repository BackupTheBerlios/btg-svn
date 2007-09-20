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
 * $Id: stcps.cpp,v 1.1.4.5 2006/11/29 18:06:29 wojci Exp $
 */

#include "stcps.h"
#include "socket_exception.h"

#include <bcore/logmacro.h>

#include "gnutlsmacro.h"

#include <bcore/os/sleep.h>

namespace btg
{
   namespace core
   {
      namespace os
      {
         SecureServerSocket::SecureServerSocket(gtlsGlobalServerData* _ggsd, 
                                                std::string const& _host, 
                                                t_uint const _port)
            : ggsd_(_ggsd),
              gsd_(0)
         {
            if (!Socket::create())
               {
                  throw socketException("Could not create server socket.");
               }

#if BTG_TRANSPORT_DEBUG
            BTG_NOTICE("Attempt to bind to port " << _port);
#endif // BTG_TRANSPORT_DEBUG

            if(!Socket::bind(_host, _port))
               {
                  throw socketException("Could not bind to port.");
               }

            if (!Socket::listen())
               {
                  throw socketException("Could not listen to socket.");
               }
         }

         SecureServerSocket::SecureServerSocket()
            : Socket::Socket(),
              ggsd_(0),
              gsd_(0)
         {

         }

         SecureServerSocket::SecureServerSocket(SecureServerSocket const& _serversocket)
            : Socket::Socket(_serversocket),
              ggsd_(0),
              gsd_(_serversocket.gsd_)
         {
         }

         bool SecureServerSocket::write(t_byteCP _msg, t_int const _size)
         {
            if (!Socket::is_valid())
               {
#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE("(write) Marking socket with id=" << getSockId() << " as deleted");
#endif // BTG_TRANSPORT_DEBUG
                  this->markAsDeleted();
                  return false;
               }

            bool status = true;

            t_byteCP dataptr         = _msg;
            ssize_t total_bytes_sent = 0;
            ssize_t bytes_sent       = 0;

            // Big messages has to be sent in chunks...
            while(total_bytes_sent < _size)
               {
                  bytes_sent = gnutls_record_send(gsd_->getSession(), dataptr, _size - total_bytes_sent);

                  // Check for errors
                  if(bytes_sent < 0)
                     {
                        if(bytes_sent == GNUTLS_E_AGAIN || bytes_sent == GNUTLS_E_INTERRUPTED)
                           // Docs says try again if these errors are encountered
                           continue;
                        else
                           {
                              gnutls_perror(bytes_sent);
                              throw socketException("SecureClientSocket::write() failed");
                           }
                     }

                  // No errors, increase total bytes sent counter and data pointer
                  total_bytes_sent+=bytes_sent;
                  dataptr+=bytes_sent;
               }

            return status;
         }

         t_int SecureServerSocket::read(t_byteP _msg, t_int const _size)
         {
            if (!Socket::is_valid())
               {
#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE("(read) Marking socket with id=" << getSockId() << " as deleted");
#endif // BTG_TRANSPORT_DEBUG
                  this->markAsDeleted();
               }

            int ret = gnutls_record_recv (gsd_->getSession(), _msg, _size);

            if (ret <= 0)
               {
                  this->markAsDeleted();
                  return 0;
               }

            return ret;
         }

         bool SecureServerSocket::accept(Socket & _socket) const
         {
            SecureServerSocket & temp_socket = dynamic_cast<SecureServerSocket&>(_socket);

            bool status = true;

            if (!ggsd_->initialized())
               {
#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE("Global data is not initialized!");
#endif // BTG_TRANSPORT_DEBUG
                  status = false;
                  return status;
               }

            gnutls_session_t current_session;

            temp_socket.setServerData(new gtlsServerData(ggsd_));
            temp_socket.gsd()->initSession(current_session);

            if (!temp_socket.gsd()->initialized())
               {
                  // The session is not initialized.
                  status = false;
                  return status;
               }

            if (!Socket::accept(temp_socket))
               {
                  status = false;
               }

            if (status)
               {
                  int sockid = temp_socket.getSockId();
                  int ret    = 0;

                  gnutls_transport_set_ptr(current_session,
                                           reinterpret_cast<gnutls_transport_ptr>(sockid));

                  bool handshake_ok = true;
                  bool cert_ok      = true;

                  ret = gnutls_handshake(current_session);

                  if (ret < 0)
                     {
#if BTG_TRANSPORT_DEBUG
                        BTG_NOTICE("GNUTLS handshake failed.");
#endif // BTG_TRANSPORT_DEBUG
                        handshake_ok = false;
                        cert_ok      = false;
                     }

                  if (handshake_ok)
                     {
#if BTG_TRANSPORT_DEBUG
                        BTG_NOTICE("GNUTLS handshake ok.");
#endif // BTG_TRANSPORT_DEBUG
                        if (!gtlsGeneric::verifyCertificate(current_session))
                           {
#if BTG_TRANSPORT_DEBUG
                              BTG_NOTICE("Certificate verification failed.");
#endif // BTG_TRANSPORT_DEBUG
                              cert_ok = false;
                           }
                     }

                  if (handshake_ok && cert_ok)
                     {
#if BTG_TRANSPORT_DEBUG
                        BTG_NOTICE("Accepted new TLS connection.");
                        gtlsGeneric::logSessionInfo(current_session);
#endif // BTG_TRANSPORT_DEBUG
                        temp_socket.gsd()->setSession(current_session);
                     }
                  else
                     {
#if BTG_TRANSPORT_DEBUG
                        BTG_NOTICE("Rejected new TLS connection.");
#endif // BTG_TRANSPORT_DEBUG
                        status = false;

                        /// Clean up.
                        gnutls_deinit(current_session);
                     }
               }

            return status;
         }

         t_int SecureServerSocket::getSockId() const
         {
            return Socket::getSockId();
         }

         gtlsServerData* SecureServerSocket::gsd() const
         {
            return gsd_;
         }

         void SecureServerSocket::setServerData(gtlsServerData* _gsd)
         {
            gsd_ = _gsd;
         }

         SecureServerSocket::~SecureServerSocket()
         {
            delete gsd_;
            gsd_ = 0;
         }

      } // namespace os
   } // namespace core
} // namespace btg


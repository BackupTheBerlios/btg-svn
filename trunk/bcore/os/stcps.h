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
 * $Id: stcps.h,v 1.1.4.4 2006/11/29 18:06:29 wojci Exp $
 */

#ifndef STCPS_H
#define STCPS_H

#include "socket.h"
#include <bcore/externalization/externalization.h>

#include "gnutlsif.h"

#include <string>

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

               /// A secure server socket implemented using GnuTLS.
               class SecureServerSocket : public Socket
                  {
                  public:
                     /// Constructor.
                     SecureServerSocket(gtlsGlobalServerData* _ggsd, 
                                        std::string const& _host,
                                        t_uint const _port);

                     /// Default constructor.
                     SecureServerSocket();

                     /// Copy constructor.
                     SecureServerSocket(SecureServerSocket const& _serversocket);

                     /// Write.
                     virtual bool write(t_byteCP _msg, t_int const _size);

                     /// Read.
                     virtual t_int read(t_byteP _msg, t_int const _size);

                     /// Get the socket id.
                     virtual t_int getSockId() const;

                     /// Accept a new client.
                     virtual bool accept(Socket & _socket) const;

                     /// The equality operator.
                     // bool operator== (ServerSocket const& _serversocket) const;

                     /// Get a pointer to the server data (GnuTLS data) used.
                     gtlsServerData* gsd() const;

                     /// Set the server data (GnuTLS data) to be used.
                     void setServerData(gtlsServerData* _gsd);

                     /// Destructor.
                     virtual ~SecureServerSocket();
                  private:

                     /// The assignment operator.
                     SecureServerSocket& operator= (SecureServerSocket const& _socket);

                     /// Pointer to the global GnuTLS server data used.
                     gtlsGlobalServerData* ggsd_;

                     /// Data used by GNUTLS.
                     gtlsServerData*       gsd_;
                  };

               /** @} */
            } // namespace os
      } // namespace core
} // namespace btg

#endif

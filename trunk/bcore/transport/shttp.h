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
 * $Id: shttp.h,v 1.1.4.1 2006/03/13 18:40:48 jstrom Exp $
 */

#ifndef SHTTP_H
#define SHTTP_H

#include "transport.h"
#include <bcore/dbuf.h>
#include <bcore/type.h>
#include <bcore/os/gnutlsif.h>
#include <bcore/os/stcps.h>
#include <bcore/os/stcpc.h>
#include "http.h"

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

         /// A secure http transport.
         class secureHttpTransport: public httpTransport
            {
            public:
               /// Constructor, used to construct a server transport.
               /// @param [in] _e              Externalization.
               /// @param [in] _ggsd           Pointer to already initialized global GnuTLS data used by the underlaying secure socket.
               /// @param [in] _bufferSize     Buffer size.
               /// @param [in] _direction      Direction.
               /// @param [in] _addressPort    Address and port.
               /// @param [in] _timeout        Max time in milliseconds to block in a read(), 0 for no block.
               secureHttpTransport(btg::core::externalization::Externalization* _e,
                                   btg::core::os::gtlsGlobalServerData* _ggsd,
                                   t_int const _bufferSize,
                                   DIRECTION const _direction,
                                   addressPort const & _addressPort,
                                   t_uint const _timeout);

               /// Constructor, used to construct a client transport.
               /// @param [in] _e              Externalization.
               /// @param [in] _gcd            Pointer to already initialized GnuTLS data used by the underlaying secure socket.
               /// @param [in] _bufferSize     Buffer size.
               /// @param [in] _direction      Direction.
               /// @param [in] _addressPort    Address and port.
               /// @param [in] _timeout        Max time in milliseconds to block in a read(), 0 for no block.
               secureHttpTransport(btg::core::externalization::Externalization* _e,
                                   btg::core::os::gtlsClientData* _gcd,
                                   t_int const _bufferSize,
                                   DIRECTION const _direction,
                                   addressPort const & _addressPort,
                                   t_uint const _timeout);

               /// Get the type of this transport.
               messageTransport::TRANSPORT getType() const;

               /// Destructor.
               virtual ~secureHttpTransport();

            private:
               /// Accept new connections.
               void acceptNewConnections();
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

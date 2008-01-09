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
 * $Id$
 */

#ifndef TCPIP_H
#define TCPIP_H

#include "tcpipbase.h"
#include <bcore/dbuf.h>

#include <bcore/os/socket.h>
#include <bcore/os/tcps.h>
#include <bcore/os/tcpc.h>
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

         /// A TCP/IP transport.
         class tcpTransport: public tcpTransportBase
            {
            public:
               /// Constructor.
               /// @param [in] _e              Externalization.
               /// @param [in] _bufferSize     Buffer size.
               /// @param [in] _direction      Direction.
               /// @param [in] _addressPort    Address and port.
               /// @param [in] _timeout        Max time in milliseconds to block in a read(), 0 for no blocking.
               tcpTransport(LogWrapperType _logwrapper,
                            btg::core::externalization::Externalization* _e,
                            t_int const _bufferSize,
                            DIRECTION const _direction,
                            addressPort const & _addressPort,
                            t_uint const _timeout);

               /// Constructor.
               /// @param [in] _e              Externalization.
               /// @param [in] _bufferSize     Buffer size.
               /// @param [in] _direction      Direction.
               /// @param [in] _addressPort    Address and port.
               /// @param [in] _createSocket   Flag deciding if a socket should be created (used when subclassing)
               /// @param [in] _timeout        Max time in milliseconds to block in a read(), 0 for no blocking.
               tcpTransport(LogWrapperType _logwrapper,
                            btg::core::externalization::Externalization* _e,
                            t_int const _bufferSize,
                            DIRECTION const _direction,
                            addressPort const & _addressPort,
                            bool const _createSocket,
                            t_uint const _timeout);

               /// Get the type of this transport.
               virtual messageTransport::TRANSPORT getType() const;

               /// Destructor.
               virtual ~tcpTransport();
            protected:
               /// Accept new connections.
               virtual void acceptNewConnections();
            private:

               /// Internal function, which set ups an instance of
               /// this class. Used by the constructors.
               /// @param [in] _direction      Direction.
               /// @param [in] _addressPort    Address and port.
               /// @param [in] _createSocket   If true, the socket will be created.
               void setup(DIRECTION const _direction,
                          addressPort const & _addressPort,
                          bool const _createSocket);

            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

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

#ifndef MESSAGETRANSPORT_H
#define MESSAGETRANSPORT_H

#include <string>
#include <map>

#include <bcore/dbuf.h>
#include <bcore/externalization/externalization.h>
#include <bcore/transport/direction.h>

namespace btg
{
   namespace core
      {

         /**
          * \addtogroup transport
          */
         /** @{ */

         /// This class is a transport interface. This interface is
         /// implemented by a number of different transport.
         class messageTransport
            {
            public:

               /// Type of transport.
               enum TRANSPORT
                  {
                     UNDEFINED, //!< Undefined transport.
                     TCP,       //!< TCP.
                     STCP,      //!< Secure TCP (GNUTLS).
                     XMLRPC,    //!< XMLRPC (Realy just HTTP...).
                     SXMLRPC    //!< Secure XMLRPC (GNUTLS).
                  };

               /// Constructor.
               /// @param [in] _e           The externalization used.
               /// @param [in] _buffer_size The size of the que.
               /// @param [in] _direction   Set to true for the server and false for the client.
               /// @param [in] _timeout     Max time in milliseconds to block in a read(), 0 for no block.
               messageTransport(btg::core::externalization::Externalization* _e,
                                t_int const _buffer_size,
                                DIRECTION const _direction = FROM_SERVER,
                                t_uint const _timeout = 0);

               /// Write a message to the other side.
               /// \note This function blocks and erases the contents of _msg after writing it.
               /// @param [in] _msg  Reference to a message buffer.
               /// @return The number of bytes written or OPERATION_FAILED on error.
               virtual t_int write(dBuffer & _msg) = 0;

               /// Write a message to the other side.
               /// \note This function blocks and erases the contents of _msg after writing it.
               /// @param [in] _msg  Reference to a message buffer.
               /// @param [in] _connectionID  Connection ID
               /// @return The number of bytes written or OPERATION_FAILED on error.
               virtual t_int write(dBuffer & _msg, t_int const _connectionID) = 0;

               /// Receive a message.
               /// This works on client side only!
               /// \note This function blocks and erases the contents of _buffer before writing to it.
               /// @param [in] _buffer The output buffer.
               /// @returns            The number of bytes received or OPERATION_FAILED on error.
               virtual t_int read(dBuffer & _buffer) = 0;

               /// Receive a message.
               /// \note This function blocks and erases the contents of _buffer before writing to it.
               /// @param [in] _buffer The output buffer.
               /// @param [in] _connectionID The connection ID that the data was retreived from.
               /// @returns            The number of bytes received or OPERATION_FAILED on error.
               virtual t_int read(dBuffer & _buffer, t_int & _connectionID) = 0;

               /// Returns true if the que is initialized.
               virtual bool isInitialized() const = 0;

               /// Get the type of this transport.
               virtual messageTransport::TRANSPORT getType() const = 0;

               /// Indicate to the transport that the specific connection should be terminated
               virtual void endConnection(t_int _connectionID) = 0;

               /// Returns a list of connection IDs whos connections have died since last call to this function.
               /// Only makes sense to use on server side. When an ID is marked as dead by this function, the connection
               /// is compleatly gone (in other words,  endConnection should not be called!)
               t_intList getDeadConnections();

               /// Destructor.
               virtual ~messageTransport();

            public:
               enum
                  {
                     OPERATION_SUCCESS = 0,  //!< Used by member functions of this class to indicate success.
                     OPERATION_FAILED  = -2, //!< Used by member functions of this class to indicate failture.
                     SYS_CALL_FAILED   = -1  //!< Used to detect if the underlaying system function failed.
                  };

               enum
                  {
                     NO_CONNECTION_ID = -1,            //!< Used to indicate that we use the only avialable connection (the client)
                     MAX_CONNECTIONS_ALLOWED = 1024    //!< Max number of concurrent connections allowed.
                  };

            protected:
               /// The externalization used.
               btg::core::externalization::Externalization* externalization;

               /// The direction of this message transport.
               DIRECTION direction;

               /// Indicates if this message transport is initialized.
               bool initialized;

               /// The size of this que.
               const t_int buffer_size;

               /// Max time in milliseconds to block in a read(), 0 for no block
               t_uint timeout;

               /// List of dead connections
               t_intList dead_connections;
            private:
               /// Copy constructor.
               messageTransport(messageTransport const& _mt);
               /// Assignment operator.
               messageTransport& operator=(messageTransport const& _mt);
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

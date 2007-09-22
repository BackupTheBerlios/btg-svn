/*
 * btg Copyright (C) 2005 Michael Wojciechowski.
 * Connection handler by Johan Ström.
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

#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H

#include <string>
#include <map>
#include <vector>

#include <bcore/type.h>
#include <bcore/type_btg.h>

#include "connectionextrastate.h"

namespace btg
{
   namespace daemon
      {
         class ConnectionExtraState;


         /**
          * \addtogroup daemon
          */
         /** @{ */
         class Connection
            {
            public:
               /// Constructor.
               /// @param [in] _connectionID The connection id.
               Connection(t_int _connectionID);

               /// Destructor.
               ~Connection();

               /// Return a string representation of the connection.
               std::string toString();

               /// Set session ID for this connection
               void setSession(t_long const _session);
               /// Get session ID for this connection
               t_long getSession() const;

               /// If the connection has authed with a username, set
               /// it with this method.
               void setUsername(std::string const _username);

               /// Returns the username if one is set, or an empty
               /// string if the user is not authed.
               std::string getUsername() const;

               /// Returns true if the user is authed (setUsername has
               /// been called...), false otherwise.
               bool isAuthed() const;

               /// Return some extra protocol related per-connection state
               ConnectionExtraState& ExtraState();
            private:
               /// Connection ID for this connection.
               t_int connectionID_;

               /// The session ID this connection is attached to.
               t_long sessionID_;

               /// If the connection has been authorized.
               bool isAuthed_;

               /// If authed, the username we are authed as.
               std::string username_;

               /// Extra protocol related per-connection state
               ConnectionExtraState extraState_;
            };

         /// A class which keeps track of running connections.
         class connectionHandler
            {
            public:
               /// Constructor.
               connectionHandler();

               /// Destructor.
               ~connectionHandler();

               /// Add a connection ID to keep track of.
	       ///
               /// Returns true if connection was added successfully,
               /// or false if it failed for some reason.
               bool addConnection(t_int const _connectionID);

               /// Remove a connection ID that we should not keep
               /// track of any more.
               /// Returns true if connection was added successfully,
               /// or false if it failed for some reason.
               bool removeConnection(t_int const _connectionID);

               /// Return a Connection object associated with the
               /// conneciton ID if it exists, or NULL if unknown
               /// connection.
               Connection *getConnection(t_int const _connectionID);
            private:
               /// Maps connection IDs to pointers to connection
               /// instances.
               std::map<t_int, Connection*> connections_;
            };

         /** @} */

      } // namespace daemon
} // namespace btg

#endif



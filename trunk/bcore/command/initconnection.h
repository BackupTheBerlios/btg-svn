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

#ifndef COMMAND_INITCONNECTION_H
#define COMMAND_INITCONNECTION_H

#include "command.h"
#include <bcore/dbuf.h>
#include <bcore/auth/hash.h>
#include <string>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup commands
          */
         /** @{ */

         /// initConnection command - used to initiate a session.
         class initConnectionCommand: public Command
            {
            public:
               /// Default constructor.
               initConnectionCommand();
               /// Constructor.
               /// @param [in] _username Username.
               /// @param [in] _password Password in cleartext, will be hashed.
               initConnectionCommand(std::string const& _username, std::string const& _password);
               /// Constructor.
               /// @param [in] _username Username.
               /// @param [in] _hash     Hashed password
               initConnectionCommand(std::string const& _username, btg::core::Hash const& _hash);

               /// Print this command.
               std::string toString() const;

               /// Get username
               std::string getUsername() const;
               /// Set username
               void setUsername(std::string const _username);

               /// Get password hash in string format
               std::string getPassword() const;

               /// Set password, plain text
               void setPassword(std::string const _password);
               /// Set password, existing hash
               void setPassword(btg::core::Hash _hash);

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Destructor.
               virtual ~initConnectionCommand();
            private:
               /// Username.
               std::string username_;
               /// Hash of password.
               btg::core::Hash hash_;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

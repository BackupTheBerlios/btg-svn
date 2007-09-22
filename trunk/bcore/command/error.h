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

#ifndef ERROR_COMMAND_H
#define ERROR_COMMAND_H

#include "command.h"
#include <bcore/dbuf.h>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup commands
          */
         /** @{ */

         /// Generic error.
         class errorCommand: public Command
            {
            public:
               /// Default constructor.
               errorCommand();
               /// Constructor.
               /// @param [in] _which_command  Which command this error command was produced by.
               /// @param [in] _message        The error message.
               errorCommand(t_int const _which_command, std::string const& _message);
               std::string toString() const;
               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);
               /// The the command which produced this error command.
               t_int getErrorCommand() const;
               /// Set the message.
               void setMessage(std::string const& _message);
               /// Get the message.
               std::string getMessage() const;
               /// Returns true if a message is present.
               bool messagePresent() const;
               /// The equality operator.
               bool operator== (const errorCommand* _ec);
               /// Destructor.
               virtual ~errorCommand();
            private:
               /// ID of the command which produced this error command.
               t_int which_command;
               /// The message this command carries.
               std::string message;
               /// True if there is a message.
               bool isMessagePresent;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

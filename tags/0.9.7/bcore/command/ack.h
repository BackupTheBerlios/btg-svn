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

#ifndef COMMAND_ACK_H
#define COMMAND_ACK_H

#include "command.h"
#include <bcore/dbuf.h>
#include <string>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup commands
          */
         /** @{ */

         /// Ack command - used to report the success of a command.
         class ackCommand: public Command
            {
            public:
               /// Default constructor.
               ackCommand();
               /// Constructor.
               /// @param [in] _command_type   The type of command this command acknowledges.
               ackCommand(t_int const _command_type);
               /// Print this command.
               std::string toString() const;
               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);
               /// Set the command type, which this ACK command is for.
               void setCommandType(t_int const _command_type);
               /// Get the command type, which this ACK command is for.
               t_int getCommandType() const;
               /// Destructor.
               virtual ~ackCommand();
            private:
               /// The command type, which this ACK command is for.
               t_int command_type;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

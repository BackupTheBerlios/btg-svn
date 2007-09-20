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
 * $Id: session_error.h,v 1.1.4.4 2006/03/13 18:40:48 jstrom Exp $
 */

#ifndef SESSION_ERROR_COMMAND_H
#define SESSION_ERROR_COMMAND_H

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

         /// Used by the daemon to tell clients that their session is invalid.
         class sessionErrorCommand: public Command
            {
            public:
               /// Default constructor.
               sessionErrorCommand();
               /// Constructor.
               /// @param [in] _which_command  Which command this error command was produced by.
               sessionErrorCommand(t_int const _which_command);
               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);
               /// Destructor.
               virtual ~sessionErrorCommand();
            private:
               /// ID of the command which produced this error command.
               t_int which_command;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

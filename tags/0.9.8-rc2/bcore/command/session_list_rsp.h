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
 * Id    : $Id$
 */

#ifndef COMMAND_LIST_SESSION_RESPONSE_H
#define COMMAND_LIST_SESSION_RESPONSE_H

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

         /// Response to a request to list sessions.
         class listSessionResponseCommand: public Command
            {
            public:
               /// Default constructor.
               listSessionResponseCommand();
               /// Constructor.
               /// @param [in] _sessions List of sessions.
               /// @param [in] _names    List of session names.
               listSessionResponseCommand(t_longList _sessions,
                                          t_strList _names);

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Get the list of sessions.
               t_longList getSessions() const;
               
               /// Get list of session names.
               t_strList getNames() const;

               /// Destructor.
               virtual ~listSessionResponseCommand();
            private:
               /// List of sessions.
               t_longList sessions;
               /// List of session names.
               t_strList  names;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

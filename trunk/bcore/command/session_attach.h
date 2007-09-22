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

#ifndef COMMAND_ATTACH_SESSION_H
#define COMMAND_ATTACH_SESSION_H

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

         /// A client sends this command to the daemon in order to attach to a detached session.
         class attachSessionCommand: public Command
            {
            public:
               /// Default constructor. Used for serialization/deserialization.
               attachSessionCommand();
               /// Constructor.
               /// @param [in] _buildID        The build ID.
               /// @param [in] _session        The session which this command instructs the daemon to attach to.
               attachSessionCommand(std::string const& _buildID,
                                    t_long const _session);

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Get the build ID.
               std::string getBuildID() const;
               /// Returns the session which this command instructs the daemon to attach to.
               t_long getSession() const;
               /// Destructor.
               virtual ~attachSessionCommand();
            private:
               /// Build ID, used to decide if a client should be able
               /// to attach.
               std::string buildID;
               /// The session which this command instructs the daemon to attach to.
               t_long session;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

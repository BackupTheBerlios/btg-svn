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
 * Id    : $Id: context_last.h,v 1.1.2.2 2007/04/25 06:02:46 wojci Exp $
 */

#ifndef COMMAND_LAST_CID_H
#define COMMAND_LAST_CID_H

#include "command.h"
#include "context.h"

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup commands
          */
         /** @{ */

         /// Get the last added context ID (through the same connection).
         class lastCIDCommand: public Command
            {
            public:
               /// Constructor.
               lastCIDCommand();

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Destructor.
               virtual ~lastCIDCommand();
            };

         /// Last added context ID (through the same connection).
         class lastCIDResponseCommand: public Command
            {
            public:
               /// Default constructor.
               lastCIDResponseCommand();

               /// Constructor.
               /// @param [in] _cid The context id.
               lastCIDResponseCommand(t_int const _cid);

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Destructor.
               virtual ~lastCIDResponseCommand();

               /// Get the context id of the last added torrent.
               t_int getContextId() const;

            private:
               /// The context id
               t_int context_id;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif // COMMAND_LAST_CID_H


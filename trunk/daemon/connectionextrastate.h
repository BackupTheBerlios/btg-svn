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
 * $Id: connectionextrastate.h,v 1.1.2.2 2007/04/28 14:31:15 wojci Exp $
 */

#ifndef CONNECTIONEXTRASTATE_H
#define CONNECTIONEXTRASTATE_H

#include <string>
#include <map>
#include <vector>

#include <bcore/type.h>
#include <bcore/type_btg.h>

namespace btg
{
   namespace daemon
      {
         /**
          * \addtogroup daemon
          */
         /** @{ */
         /** Holds some extra per-connection state, not related to the
          *  connection itself, but to the state of the communication
          *  protocol.
          */
         class ConnectionExtraState
            {
            public:
               /// Constructor.
               ConnectionExtraState();

               /// Set the last created context ID.
               void setLastCreatedContextId(t_int lastCreatedContextId)
               {
                  this->lastCreatedContextId = lastCreatedContextId;
               }

               /// Returns the last created context ID, or contextCommand::UNDEFINED_CONTEXT if there is none.
               t_int getLastCreatedContextId() const
               {
                  return this->lastCreatedContextId;
               }

            private:
               /// The last created context ID.
               t_int lastCreatedContextId;
            };
         /** @} */

      } // namespace daemon
} // namespace btg

#endif


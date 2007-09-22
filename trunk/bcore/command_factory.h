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

#ifndef COMMAND_FACTORY_H
#define COMMAND_FACTORY_H

#include "type.h"

#include <bcore/externalization/externalization.h>

namespace btg
{
   namespace core
      {

         class Command;

         /**
          * \addtogroup commands
          */
         /** @{ */

         /// Convert between commands and externalized representation
         /// and the other way around.
         class commandFactory
            {
            public:
               /// The createFromBytes function uses these enums to
               /// report status.
               enum decodeStatus
                  {
                     DS_OK = 0,  //!< OK.
                     DS_UNKNOWN, //!< Unknown command.
                     DS_FAILED   //!< Failed to deserialize command.
                  };

               /// Create an instance of Command by reading from a byte array.
               /// @param [in]  _e      Pointer to the externalization which is used.
               /// @param [out] _status Status code.
               /// @return              A pointer to a command object or 0 on failture.
               static Command* createFromBytes(btg::core::externalization::Externalization* _e,
                                               decodeStatus & _status);

               /// Convert a command into bytes.
               /// @param [out] _e           Pointer to the externalization which is used.
               /// @param [in]  _command     Pointer to a command object.
               /// @return                   The number of bytes written to _destination.
               static bool convertToBytes(btg::core::externalization::Externalization* _e,
                                          const Command* _command);
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

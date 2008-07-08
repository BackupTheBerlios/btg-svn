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
#include <bcore/logable.h>

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
         class commandFactory : public btg::core::Logable
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

               /// Constructor.
               /// @param [in] _logwrapper Used for logging by this class.
               /// @param [in] _e          Pointer to the externalization which is used.
               commandFactory(LogWrapperType _logwrapper,
                              btg::core::externalization::Externalization& _e);

               /// Create an instance of Command by reading from a byte array.
               /// @param [out] _status Status code.
               /// @return              A pointer to a command object or 0 on failture.
               Command* createFromBytes(decodeStatus & _status);

               /// Convert a command into bytes.
               /// @param [in]  _command     Pointer to a command object.
               /// @return                   The number of bytes written to _destination.
               bool convertToBytes(const Command* _command);

               /// Get the bytes produced by a sucessful call to convertToBytes.
               void getBytes(btg::core::DIRECTION const _dir, dBuffer & _dbuffer);

               /// Destructor.
               virtual ~commandFactory();

            protected:
               /// The externalization interface used.
               btg::core::externalization::Externalization& e;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

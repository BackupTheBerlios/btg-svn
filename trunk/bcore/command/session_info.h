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

#ifndef COMMAND_SESSION_INFO_H
#define COMMAND_SESSION_INFO_H

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

      /// Get info about a session.
      class sessionInfoCommand: public Command
      {
      public:
         /// Default constructor.
         sessionInfoCommand();

         bool serialize(btg::core::externalization::Externalization* _e) const;
         bool deserialize(btg::core::externalization::Externalization* _e);

         /// Destructor.
         virtual ~sessionInfoCommand();
      };

      /// Information about a session.
      class sessionInfoResponseCommand: public Command
      {
      public:
         /// Default constructor.
         sessionInfoResponseCommand();
         /// Constructor.
         sessionInfoResponseCommand(bool _encryption, bool _dht);

         bool serialize(btg::core::externalization::Externalization* _e) const;
         bool deserialize(btg::core::externalization::Externalization* _e);

         /// Indicates if encryption is enabled.
         bool encryption() const;

         /// Indicates if dht is enabled.
         bool dht() const;

         /// Destructor.
         virtual ~sessionInfoResponseCommand();
      private:
         /// Indicates if encryption is enabled.
         bool encryption_;
         /// Indicates if dht is enabled.
         bool dht_;
      };

      /** @} */

   } // namespace core
} // namespace btg

#endif // COMMAND_SESSION_INFO_H


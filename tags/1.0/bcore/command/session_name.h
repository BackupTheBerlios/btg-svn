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

#ifndef COMMAND_SESSION_NAME_H
#define COMMAND_SESSION_NAME_H

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

      /// Request a session's name.
      class sessionNameCommand: public Command
      {
      public:
         /// Default constructor.
         sessionNameCommand();

         bool serialize(btg::core::externalization::Externalization* _e) const;
         bool deserialize(btg::core::externalization::Externalization* _e);

         /// Destructor.
         virtual ~sessionNameCommand();
      };

      /// Get a session's name.
      class sessionNameResponseCommand: public Command
      {
      public:
         /// Default constructor.
         sessionNameResponseCommand();
         /// Constructor.
         sessionNameResponseCommand(std::string _name);

         bool serialize(btg::core::externalization::Externalization* _e) const;
         bool deserialize(btg::core::externalization::Externalization* _e);

         /// Get list of session names.
         std::string getName() const;

         /// Destructor.
         virtual ~sessionNameResponseCommand();
      private:
         /// Name.
         std::string name;
      };

      /// Set a session's name.
      class setSessionNameCommand: public Command
      {
      public:
         /// Default constructor.
         setSessionNameCommand();

         /// Constructor.
         setSessionNameCommand(std::string _name);

         bool serialize(btg::core::externalization::Externalization* _e) const;
         bool deserialize(btg::core::externalization::Externalization* _e);

         /// Get list of session names.
         std::string getName() const;

         /// Destructor.
         virtual ~setSessionNameCommand();
      protected:
         /// Name.
         std::string name;
      };

      /** @} */

   } // namespace core
} // namespace btg

#endif // COMMAND_SESSION_NAME_H


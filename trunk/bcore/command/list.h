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

#ifndef COMMAND_LIST_H
#define COMMAND_LIST_H

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

         /// List contexts which the daemon knows about.
         class listCommand: public Command
            {
            public:
               /// Default constructor.
               listCommand();
               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);
               /// Destructor.
               virtual ~listCommand();
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

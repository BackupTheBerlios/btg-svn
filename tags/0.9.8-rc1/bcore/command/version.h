/*
 * btg Copyright (C) 2008 Michael Wojciechowski.
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

#ifndef COMMAND_VERSION_H
#define COMMAND_VERSION_H

#include "command.h"
#include <bcore/dbuf.h>
#include <bcore/addrport.h>
#include <bcore/copts.h>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup commands
          */
         /** @{ */

         /// Request version and options.
         class versionCommand: public Command
            {
            public:
               /// Constructor.
               versionCommand();

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Destructor.
               virtual ~versionCommand();
            };

         /// Version and options.
         class versionResponseCommand: public OptionBase, public Command
            {
            public:
               /// Default constructor.
               versionResponseCommand();

               versionResponseCommand(t_byte const _mayor, 
                                      t_byte const _minor, 
                                      t_byte const _revision);
               
               void setOption(OptionBase::Option const _option);

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Destructor.
               virtual ~versionResponseCommand();
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif // COMMAND_VERSION_H

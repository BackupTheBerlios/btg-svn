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

#ifndef COMMAND_CONTEXT_H
#define COMMAND_CONTEXT_H

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

         /// Less general command which all commands that use a single context
         /// id are based on.
         class contextCommand: public Command
            {
            public:
               /// Represents an undefined context.
               static const t_int UNDEFINED_CONTEXT;
               /// The default constructor.
               contextCommand(commandType _type);

               /// Constructor.
               /// If _allContexts is true, this command works on all contexts in the attached session
               contextCommand(commandType _type, 
                              t_int const _context_id, 
                              bool const _allContexts=false);
               virtual std::string toString() const;
               /// Get the ID of the context which this command are supposed to work on.
               virtual t_int getContextId() const;
               virtual bool serialize(btg::core::externalization::Externalization* _e) const;
               virtual bool deserialize(btg::core::externalization::Externalization* _e);
               /// Make this command work on all contexts.
               virtual void setAllContextsFlag();
               /// Returns true if the flag which indicates that this command
               /// works on all contexts is set.
               virtual bool isAllContextsFlagSet() const;
               /// Destructor.
               virtual ~contextCommand();
            protected:
               /// The context id.
               t_int context_id;
               /// Flag, indicates that this command works on all contexts.
               bool allContexts;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

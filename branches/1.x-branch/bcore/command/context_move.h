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

#ifndef CONTEXT_MOVE_H
#define CONTEXT_MOVE_H

#include "context.h"

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup commands
          */
         /** @{ */

         /// Move a context or all contexts from the current session
         /// to the one specified as the argument to the constructor.
         class contextMoveToSessionCommand: public contextCommand
            {
            public:
               /// Default constructor.
               contextMoveToSessionCommand();

               /// Constructor.
               /// @param [in] _context_id  The context ID to move.
               /// @param [in] _session_id  The session to move the context to.
               /// @param [in] _allContexts If true, this command works on all contexts.
               contextMoveToSessionCommand(t_int const _context_id,
                                           t_long const _session_id, 
                                           bool const _allContexts=false);
               
               virtual bool serialize(btg::core::externalization::Externalization* _e) const;
               virtual bool deserialize(btg::core::externalization::Externalization* _e);

               /// Get the contained session.
               t_long session() const;

               /// Destructor.
               virtual ~contextMoveToSessionCommand();
            protected:
               /// The session to move the context to.
               t_long session_;
            };
         
         /** @} */
         
      } // namespace core
} // namespace btg

#endif // CONTEXT_MOVE_H


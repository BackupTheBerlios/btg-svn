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
 * $Id: context_status.h,v 1.7.4.3 2006/03/13 18:40:47 jstrom Exp $
 */

#ifndef COMMAND_CONTEXT_STATUS_H
#define COMMAND_CONTEXT_STATUS_H

#include "context.h"
#include <bcore/dbuf.h>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup commands
          */
         /** @{ */

         /// Get status information about a context.
         class contextStatusCommand: public contextCommand
            {
            public:
               /// Default constructor.
               contextStatusCommand();
               /// Constructor.
               /// @param [in] _context_id     The context ID of the context of which a status is required.
               /// @param [in] _allContexts    If set to True, return status of all contexts.
               contextStatusCommand(t_int const _context_id,
                                    bool const _allContexts = false);
               /// Destructor.
               virtual ~contextStatusCommand();
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

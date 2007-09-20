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
 * $Id: context_clean.h,v 1.7.4.5 2006/07/21 08:49:39 wojci Exp $
 */

#ifndef COMMAND_CONTEXT_CLEAN_H
#define COMMAND_CONTEXT_CLEAN_H

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

         /// Clean up contexts that finished downloading.
         class contextCleanCommand: public contextCommand
            {
            public:
               /// Default constructor.
               contextCleanCommand();

               /// Constructor.
               /// @param [in] _context_id     The context ID of the context which is to be started.
               /// @param [in] _allContexts    If set to true, return status of all contexts.
               contextCleanCommand(t_int const _context_id,
                                   bool const _allContexts = false);
               /// Destructor.
               virtual ~contextCleanCommand();
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

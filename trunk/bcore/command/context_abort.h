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
 * $Id: context_abort.h,v 1.7.4.3 2006/05/29 16:50:57 wojci Exp $
 */

#ifndef COMMAND_ABORT_CONTEXT_H
#define COMMAND_ABORT_CONTEXT_H

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

         /// Abort a context.
         class contextAbortCommand: public contextCommand
            {
            public:
               /// The default constructor. Creates an invalid command per design.
               contextAbortCommand();
               /// Constructs a valid command which aborts a context.
               /// @param [in] _context_id  Context ID.
               /// @param [in] _eraseData   Indicates if data shall be erased.
               /// @param [in] _allContexts Indicates that this command works on all contexts.
               contextAbortCommand(t_int const _context_id, 
                                   bool const _eraseData,
                                   bool const _allContexts = false);

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               std::string toString() const;

               /// Indicates if data shall be erased.
               bool eraseData() const;

               /// The destructor.
               virtual ~contextAbortCommand();
            private:
               /// Indicates if data shall be erased.
               bool eraseData_;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

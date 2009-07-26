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

#ifndef COMMAND_CONTEXT_FILE_INFO_RESPONSE_H
#define COMMAND_CONTEXT_FILE_INFO_RESPONSE_H

#include "context.h"
#include <bcore/dbuf.h>
#include <bcore/type_btg.h>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup commands
          */
         /** @{ */

         /// Command sent by the daemon to inform the client about the status
         /// for a context.
         class contextFileInfoResponseCommand: public contextCommand
            {
            public:
               /// Default constructor.
               contextFileInfoResponseCommand();

               /// Constructor.
               /// @param [in] _context_id     The context ID.
               /// @param [in] _fileinfolist   List of file info objects.
               contextFileInfoResponseCommand(t_int const _context_id,
                                              t_fileInfoList const& _fileinfolist);

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Return a list of file info objects.
               t_fileInfoList getFileInfoList() const;
               /// Destructor.
               virtual ~contextFileInfoResponseCommand();
            private:
               /// List of file info objects.
               t_fileInfoList fileinfolist;
            };


         /** @} */
      } // namespace core
} // namespace btg

#endif

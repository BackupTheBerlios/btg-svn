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

#ifndef COMMAND_LIST_RESP_H
#define COMMAND_LIST_RESP_H

#include "command.h"
#include <bcore/dbuf.h>

#include <string>
#include <vector>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup commands
          */
         /** @{ */

         /// The response to a List command. Contains a list of context IDs and filenames.
         class listCommandResponse: public Command
            {
            public:
               /// Default constructor. Used for creating an instance used for de-serialization.
               listCommandResponse();
               /// Constructor.
               /// @param [in] _context_ids    List of contexts IDs.
               /// @param [in] _filenames      List of filenames.
               listCommandResponse(t_intList const& _context_ids, t_strList const& _filenames);
               /// Convert an instance of this class into a std::string.
               std::string toString() const;
               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);
               /// Get the contained context ids.
               t_intList getIDs() const;
               /// Get the contained file names.
               t_strList getFilanames() const;
               /// Destructor.
               virtual ~listCommandResponse();
            private:
               /// The contained context ids.
               t_intList context_ids;
               /// The contained file names.
               t_strList filenames;
            };

         /** @} */

      } // namespace btg
} // namespace core

#endif

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
 * $Id: context_clean_rsp.h,v 1.10.4.4 2006/03/13 18:40:47 jstrom Exp $
 */

#ifndef COMMAND_CONTEXT_CLEAN_RESPONSE_H
#define COMMAND_CONTEXT_CLEAN_RESPONSE_H

#include <vector>
#include <string>

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

         /// A List of contexts that got cleaned up.
         class contextCleanResponseCommand: public Command
            {
            public:
               /// Default constructor.
               contextCleanResponseCommand();
               /// Constructor.
               /// @param [in] _filenames      List of filenames.
               /// @param [in] _contextIDs     List of contexts.
               contextCleanResponseCommand(t_strList const& _filenames, t_intList const& _contextIDs);
               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Get the contained list of filenames.
               t_strList getFilenames() const;

               /// Get the contained list of context IDs.
               t_intList getContextIDs() const;

               /// Destructor.
               virtual ~contextCleanResponseCommand();
            private:
               /// List of filenames.
               t_strList filenames;
               /// List of context IDs.
               t_intList contextIDs;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

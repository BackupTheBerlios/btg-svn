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

#ifndef CONTEXT_FILE_H
#define CONTEXT_FILE_H

#include "context.h"
#include <bcore/dbuf.h>
#include <bcore/file_info.h>

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

         /// Set files.
         class contextSetFilesCommand: public contextCommand
            {
            public:
               /// Default constructor.
               contextSetFilesCommand();

               /// Constructor.
               contextSetFilesCommand(t_int const _context_id, 
                                      selectedFileEntryList const& _files);
               
               virtual bool serialize(btg::core::externalization::Externalization* _e) const;
               virtual bool deserialize(btg::core::externalization::Externalization* _e);

               /// Get the contained files.
               selectedFileEntryList getFiles() const;

               /// Destructor.
               virtual ~contextSetFilesCommand();
            protected:
               /// The list of contained files.
               selectedFileEntryList files_;
            };

         /// Get selected files.
         class contextGetFilesCommand: public contextCommand
            {
            public:
               /// Default constructor.
               contextGetFilesCommand();

               /// Constructor.
               contextGetFilesCommand(t_int const _context_id);

               /// Destructor.
               virtual ~contextGetFilesCommand();
            };
         
         /// Response containing list of selected files.
         class contextGetFilesResponseCommand: public contextCommand
            {
            public:
               /// Default constructor.
               contextGetFilesResponseCommand();

               /// Constructor.
               contextGetFilesResponseCommand(t_int const _context_id, 
                                              selectedFileEntryList const& _files);
               
               virtual bool serialize(btg::core::externalization::Externalization* _e) const;
               virtual bool deserialize(btg::core::externalization::Externalization* _e);

               /// Get the selected files.
               selectedFileEntryList getFiles() const;

               /// Destructor.
               virtual ~contextGetFilesResponseCommand();
            protected:
               /// List of selected files.
               selectedFileEntryList files_;
            };
         
         /** @} */
         
      } // namespace core
} // namespace btg

#endif // CONTEXT_FILE_H

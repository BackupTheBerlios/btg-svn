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

#ifndef FILE_OP_H
#define FILE_OP_H

#include <string>
#include <bcore/type.h>

namespace btg
{
   namespace core
      {
         namespace os
            {

               /**
                * \addtogroup OS
                */
               /** @{ */

               /// Implements system dependent file operations.
               class fileOperation
                  {
                  public:
                     /// Check that a file or directory exists.
                     /// Make sure that paths beginning with a '~' gets
                     /// translated into the homedir and that files and
                     /// directories exist.
                     /// @param _path         The path to check.
                     /// @param _errorMessage If/when the function fails, this argument will contain the reason.
                     /// @param _directory    True - check if the _path argument, which is a directory exists, false - check if the _path argument, which is a file exists.
                     static bool check(std::string & _path,
                                       std::string & _errorMessage,
                                       bool const _directory);

                     /// As the above function, just without reporting
                     /// why a file or directory does not exists.
                     static bool check(std::string & _path,
                                       bool const _directory = false);

                     /// Resolve a path using realpath, that is replace "~" or relative paths.
                     /// @param [in] _path The path to check.
                     /// @return True if the path was resolved, false otherwise.
                     static bool resolvePath(std::string & _path);

                     /// Create a directory.
                     /// @return True - the directory was created. False - creation failed.
                     static bool createDirectory(std::string const& _path);

                     /// Delete a file.
                     /// @param [in] _filename The filename to delete.
                     /// @return True, if the file was deleted, false otherwise.
                     static bool remove(std::string const& _filename);
                  };

               /** @} */

            } // namespace os
      } // namespace core
} // namespace btg

#endif


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
 * $Id: exec.h,v 1.1.2.2 2006/04/01 20:30:50 wojci Exp $
 */

#ifndef EXEC_H
#define EXEC_H

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

               /// Implements system dependent file exec operations.
               class Exec
                  {
                  public:
                     /// Execute a file, wait for it to finish execution.
                     /// @param [in] _filename  The filename of the file to execute.
                     /// @param [in] _arguments List containing arguments.
                     /// \return True - file was executed, false - file was not executed.
                     static bool execFile(std::string const& _filename,
                                          std::vector<std::string> const& _arguments);

                     /// Check if a file is executable (by the current user).
                     /// @param [in] _filename  The filename of the file to check.
                     /// \return True - file is executable, false - file is not executable.
                     static bool isExecutable(std::string const& _filename);
                  };

               /** @} */

            } // namespace os
      } // namespace core
} // namespace btg

#endif


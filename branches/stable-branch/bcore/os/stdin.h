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

#ifndef BTG_STDIN_H
#define BTG_STDIN_H

#include <string>

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

               /// Implements system dependent input operations.
               class stdIn
                  {
                  public:

                     /// Read a password from stdin, not echoing the
                     /// characters read to stdout.
                     /// @param [in] _password Reference to a string in which to store the password.
                     /// @return True - success, got a password, false - failture, got 0 characters.
                     static bool getPassword(std::string & _password);
                  };

               /** @} */

            } // namespace os
      } // namespace core
} // namespace btg

#endif


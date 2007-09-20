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
 * The original class was from an article by Rob Tougher,
 * Linux Socket Programming In C++,
 * http://www.linuxgazette.com/issue74/tougher.html#1.
 */

/*
 * $Id: socket_exception.h,v 1.1.2.2 2005/11/12 13:00:31 wojci Exp $
 */

#ifndef SOCKET_EXCEPTION_H
#define SOCKET_EXCEPTION_H

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

               /// Exception class. Used by the socket impl..
               class socketException
                  {
                  public:
                     /// Constructor.
                     /// @param [in] _s Description.
                     socketException (std::string const& _s);

                     /// Destructor.
                     ~socketException();

                     /// Get the description.
                     std::string description();
                  private:
                     /// Description.
                     std::string text_;
                  };

               /** @} */

            } // namespace os
      } // namespace core
} // namespace btg

#endif

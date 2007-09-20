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
 * $Id: id.h,v 1.1.2.4 2006/05/14 10:56:22 wojci Exp $
 */

#ifndef ID_H
#define ID_H

#include <bcore/type.h>
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

               /// Implements system dependent way of changing the
               /// owner of the currently executing process.
               class ID
                  {
                  public:

                     /// Check if its possible to change the uid/gid of the currently executing process.
                     /// @return True, possible. False, not possible.
                     static bool changePossible();

                     /// Change the owner/group of the currently executing process.
                     /// @return True if successful, false otherwise.
                     static bool changeUserAndGroup(std::string const& _user, std::string const& _group);
                  };

               /** @} */

            } // namespace os
      } // namespace core
} // namespace btg

#endif


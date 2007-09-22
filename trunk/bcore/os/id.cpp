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

#include "id.h"

extern "C"
{
#  include <sys/types.h>
#  include <pwd.h>
#  include <grp.h>
}

#include <bcore/logmacro.h>

namespace btg
{
   namespace core
   {
      namespace os
      {
         bool ID::changePossible()
         {
            bool result = false;

            // If this process executes as root ..
            if ((getuid() == 0) && (geteuid() == 0))
               {
                  result = true;
               }

            return result;
         }

         bool ID::changeUserAndGroup(std::string const& _user, std::string const& _group)
         {
            bool result = true;

            // Resolve the user and group into uid/gid.

            /* User. */
            struct passwd* s_passwd = getpwnam(_user.c_str());

            if (s_passwd == 0)
               {
                  result = false;
                  return result;
               }

            uid_t uid = s_passwd->pw_uid;

            /* Group. */

            struct group* s_group = getgrnam(_group.c_str());

            if (s_group == 0)
               {
                  result = false;
                  return result;
               }

            gid_t gid = s_group->gr_gid;

            /* Do the change. */

            if (setgid(gid) != 0)
               {
                  BTG_NOTICE("setgid failed.");
                  result = false;
               }

            if (setuid(uid) != 0)
               {
                  BTG_NOTICE("setuid failed.");
                  result = false;
               }

            return result;
         }

      } // namespace os
   } // namespace core
} // namespace btg


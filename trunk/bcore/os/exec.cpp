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

#include "exec.h"

#include <stdlib.h>

#include <bcore/logmacro.h>

namespace btg
{
   namespace core
   {
      namespace os
      {

         bool Exec::execFile(std::string const& _filename,
                             std::vector<std::string> const& _arguments)
         {
            bool status = false;

            std::string command = _filename;

            std::vector<std::string>::const_iterator iter;

            for (iter = _arguments.begin();
                 iter != _arguments.end();
                 iter++)
               {
                  // Fix of bug 12725.
                  command += " '";
                  command += *iter;
                  command += "'";
               }

            // Lets not wait for the callback to complete.
            command += " &";

            BTG_NOTICE("Executing command '" << command << "'.");

            int result = ::system(command.c_str());
            if (result >= 0)
               {
                  status = true;
               }

            BTG_NOTICE("Status: " << status << ".");

            return status;
         }

         bool Exec::isExecutable(std::string const& _filename)
         {
            bool status = false;

            int result = access(_filename.c_str(), R_OK|X_OK);

            if (result == 0)
               {
                  status = true;
               }

            BTG_NOTICE("Checking if '" << _filename << "' is executable: " << status << ".");

            return status;
         }

      } // namespace os
   } // namespace core
} // namespace btg


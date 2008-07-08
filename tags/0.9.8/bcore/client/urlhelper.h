/*
 * btg Copyright (C) 2008 Michael Wojciechowski.
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

#ifndef URLHELPER_H
#define URLHELPER_H

#include <string>
#include <bcore/command/opstat.h>

namespace btg
{
   namespace core
   {
      namespace client
      {
         /**
          * \addtogroup gencli
          */
         /** @{ */

         /// Return true if the URL is valid.
         bool isUrlValid(std::string const& _url);

         /// Return true and update _filename, if the url ends with a
         /// file name.
         bool getFilenameFromUrl(std::string const& _url, std::string & _filename);

         /** @} */

      } // client
   } // core
} // btg

#endif // URLHELPER_H


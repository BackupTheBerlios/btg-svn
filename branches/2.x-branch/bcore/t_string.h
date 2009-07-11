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

#ifndef TFSTRING_H
#define TFSTRING_H

#include <iostream>
#include <sstream>
#include <string>

namespace btg
{
   namespace core
      {

	/**
	 * \addtogroup core
	 */
	/** @{ */

         /// Template which converts string to type T.
         template<typename T> T convertStringTo(const std::string& _string)
            {
               std::istringstream iss(_string);
               T x;
               iss >> x;
               return x;
            }

         /// Template which converts from type T to string.
         template<typename T> std::string convertToString(const T& _x)
            {
               std::ostringstream oss;
               oss << _x;
               return oss.str();
            }

	 /** @} */

      } // namespace core
} // namespace btg

#endif

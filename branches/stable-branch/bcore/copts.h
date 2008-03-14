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

#ifndef COPTS_H
#define COPTS_H

#include <string>

namespace btg
{
   namespace core
      {

         /**
          * \addtogroup core
          */
         /** @{ */
         
         /// Modify _output to contain a description of the compile
         /// time options used.
         void getCompileTimeOptions(std::string & _output);
         
         /** @} */
         
      } // namespace core
} // namespace btg

#endif // COPTS_H

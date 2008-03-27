/*
 * btg Copyright (C) 2005 Michael Wojciechowski.
 *     Copyright (C) 2008 Roman Rybalko.
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

#include "fstream.h"

#include <unistd.h>

namespace btg
{
   namespace core
   {
      namespace os
      {

         bool filebuf::truncate()
         {
            return truncate( seekoff(0, std::ios_base::cur, std::ios_base::out) );
         }

         bool filebuf::truncate(filebuf::pos_type _pos)
         {
            return ftruncate(_M_file.fd(), _pos) != -1;
         }

         fstream::fstream()
         {
            
         }

         fstream::fstream(const char *_fname, std::ios_base::openmode _mode)
            : std::fstream(_fname,_mode)
         {
            
         }
         
         fstream::__filebuf_type* fstream::rdbuf() const
         {
            return static_cast<__filebuf_type*>(std::fstream::rdbuf());
         }
      
      } // namespace os
   } // namespace core
} // namespace btg

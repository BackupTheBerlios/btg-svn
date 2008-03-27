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

#ifndef FSTREAM_H_
#define FSTREAM_H_

#include <fstream>

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
            
               /// filebuf implementation with truncate facility
               class filebuf: public std::filebuf
                  {
                  public:
                     /// Truncate file in current tellp() position
                     /// @note will flush stream (seekp called)
                     bool truncate();
                     
                     /// Truncate file
                     /// @param _pos position from beginning of file
                     /// @see truncate(2)
                     bool truncate(pos_type _pos);
                  };

               /// fstream with our filebuf implementation
               class fstream: public std::fstream
                  {
                  public:
                     /// replacing filebuf
                     typedef filebuf __filebuf_type;
                     
                     /// @see std::fstream
                     fstream();
                     
                     /// @see std::fstream
                     explicit fstream(const char *_fname, std::ios_base::openmode _mode = std::ios_base::in | std::ios_base::out);

                     /// returns aggregated filebuf object
                     __filebuf_type* rdbuf() const;
                  };

               /** @} */

            } // namespace os
      } // namespace core
} // namespace btg

#endif /*FSTREAM_H_*/

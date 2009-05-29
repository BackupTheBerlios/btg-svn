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

#ifndef GZIPIF_H
#define GZIPIF_H

#include <string>
#include <bcore/type.h>

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

         /// Gzip Compression/decompression.
         class gzipIf
         {
         public:
            /// Constructor.
            gzipIf();

            /// Use gzip to compress _src, place compressed data in
            /// _dst.
            virtual void gzip_compress(std::string const& _src, std::string& _dst) = 0;
		    
            /// Use gzip to decompress _src, place decompressed data
            /// in _dst.
            virtual void gzip_decompress(std::string const& _src, std::string& _dst) = 0;

            /// Return true if this interface was initialized and is
            /// ready for use.
            bool initialized() const;

            /// Destructor.
            virtual ~gzipIf();
         protected:
            bool initialized_;
         };

         /** @} */

      } // namespace os
   } // namespace core
} // namespace btg

#endif // GZIPIF_H


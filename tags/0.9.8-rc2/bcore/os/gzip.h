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

#ifndef GZIP_H
#define GZIP_H

#include "gzipif.h"

#include <sstream>
#include <boost/iostreams/filtering_streambuf.hpp>

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

         /// Gzip compression/decompression using boost, filtered
         /// streams and zlib.
         /// 
         /// Note: compression and decompression cannot be done at the
         /// same time.
         class gzip: public gzipIf
         {
         public:
            /// Constructor.
            gzip();

            virtual void gzip_compress(std::string const &_src, std::string &_dst);
            
            virtual void gzip_decompress(std::string const &_src, std::string &_dst);
            
            /// Destructor.
            virtual ~gzip();
         private:
            /// Used as input to a filtered input streambuffer.
            std::stringstream srcstream;
            /// Used as input from a filtered input streambuffer.
            std::stringstream dststream;

            /// Filtered input streambuffer used for compression.
            boost::iostreams::filtering_istreambuf compress_filter;

            /// Filtered input streambuffer used for decompression.
            boost::iostreams::filtering_istreambuf decompress_filter;
         };

         /** @} */

      } // namespace os
   } // namespace core
} // namespace btg

#endif


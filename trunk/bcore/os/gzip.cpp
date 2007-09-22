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

#include "gzip.h"

#include <bcore/project.h>

#include <iostream>

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <sstream>

namespace btg
{
   namespace core
   {
      namespace os
      {
         gzip::gzip()
            : gzipIf(),
              srcstream(),
              dststream(),
              compress_filter(),
              decompress_filter()
         {
            
         }

         void gzip::gzip_compress(std::string const& _src, std::string& _dst)
         {
            srcstream << _src;

            compress_filter.push(boost::iostreams::gzip_compressor());
            compress_filter.push(srcstream);
            boost::iostreams::copy(compress_filter, dststream);
	  
            _dst.clear();
            _dst = dststream.str();

            compress_filter.reset();
            srcstream.str("");
            dststream.str("");
         }
	
         void gzip::gzip_decompress(std::string const& _src, std::string& _dst)
         {
            srcstream << _src;

            decompress_filter.push(boost::iostreams::gzip_decompressor());
            decompress_filter.push(srcstream);
            boost::iostreams::copy(decompress_filter, dststream);
	  
            _dst.clear();
            _dst = dststream.str();

            decompress_filter.reset();
            srcstream.str("");
            dststream.str("");
         }

         gzip::~gzip()
         {
            
         }

      } // namespace os
   } // namespace core
} // namespace btg

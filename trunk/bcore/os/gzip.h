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
#include <zlib.h>
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

         /// Gzip compression/decompression using boost, filtered
         /// streams and zlib.
         /// 
         /// Note: compression and decompression cannot be done at the
         /// same time.
         class gzip: public gzipIf
         {
         public:
            enum
            {
               GZIP_MAX_BUFFER_SIZE = 16*1024
            };

            /// Constructor.
            gzip();

            virtual void gzip_compress(std::string const &_src, std::string & _dst);
            
            virtual void gzip_decompress(std::string const &_src, std::string & _dst);
            
            /// Destructor.
            virtual ~gzip();
         private:
            t_byte*  inputBuffer;
            t_byte*  outputBuffer;
            z_stream strmDef;
            z_stream strmInf;

            void gzip_compress_impl(int bufferSize, std::string & _dst, bool _flush = false);
            void gzip_decompress_impl(int _bufferSize, std::string & _dst, bool _flush = false );

            void getOutput(int _bytes, std::string & _dst);
            void clearOutputBuffers();
         };

         /** @} */

      } // namespace os
   } // namespace core
} // namespace btg

#endif // GZIP_H


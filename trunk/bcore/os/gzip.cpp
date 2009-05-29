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
#include <string.h>

#include <iostream>

#ifndef GZIP_BTG_DEBUG
#define GZIP_BTG_DEBUG 0
#endif

namespace btg
{
   namespace core
   {
      namespace os
      {
         gzip::gzip()
            : gzipIf()
         {
            inputBuffer  = new t_byte[GZIP_MAX_BUFFER_SIZE];
            memset(&inputBuffer[0], 0, GZIP_MAX_BUFFER_SIZE);
            outputBuffer = new t_byte[GZIP_MAX_BUFFER_SIZE];
            memset(&outputBuffer[0], 0, GZIP_MAX_BUFFER_SIZE);

            strmDef.zalloc    = Z_NULL;
            strmDef.zfree     = Z_NULL;
            strmDef.opaque    = Z_NULL;
            strmDef.total_out = 0;
            strmDef.next_in   = &inputBuffer[0];
            strmDef.avail_in  = 0;

            strmInf.zalloc    = Z_NULL;
            strmInf.zfree     = Z_NULL;
            strmInf.opaque    = Z_NULL;
            strmInf.total_out = 0;
            strmInf.next_in   = &inputBuffer[0];
            strmInf.avail_in  = 0;

            int retDef = deflateInit2(&strmDef, Z_DEFAULT_COMPRESSION, Z_DEFLATED, (15+16), 8, Z_DEFAULT_STRATEGY);
            int retInf = inflateInit2(&strmInf, (15+32));

            if ((retDef == Z_OK) && (retInf == Z_OK))
               {
                  initialized_ = true;
               }
         }

         void gzip::gzip_compress(std::string const& _src, std::string& _dst)
         {
            deflateReset(&strmDef);

            int inputCounter = _src.size();
            int counter      = 0;
            while (inputCounter > 0)
               {
                  if (inputCounter > GZIP_MAX_BUFFER_SIZE)
                     {
                        memcpy(&inputBuffer[0], _src.c_str()+counter, GZIP_MAX_BUFFER_SIZE);
                        counter       += GZIP_MAX_BUFFER_SIZE;
                        gzip_compress_impl(GZIP_MAX_BUFFER_SIZE, _dst);
                        inputCounter -=  GZIP_MAX_BUFFER_SIZE;
                     }
                  else
                     {
                        memcpy(&inputBuffer[0], _src.c_str()+counter, inputCounter);
                        counter += inputCounter;
                        gzip_compress_impl(inputCounter, _dst);
                        inputCounter =  0;
                     }
               }

            gzip_compress_impl(0, _dst, true /* flush */);
         }

         void gzip::gzip_compress_impl(int _bufferSize, std::string & _dst, bool _flush)
         {
            if (_flush)
               {
                  int status = Z_OK;
                  do 
                     {
                        strmDef.avail_in  = 0;
                        strmDef.next_out  = &outputBuffer[0];
                        strmDef.avail_out = GZIP_MAX_BUFFER_SIZE;
                        
                        status = deflate(&strmDef, Z_FINISH);
                        switch (status)
                           {
                           case Z_OK:
                              {
                                 int count = GZIP_MAX_BUFFER_SIZE - strmDef.avail_out;
                                 getOutput(count, _dst);
                                 clearOutputBuffers();
                                 break;
                              }
                           case Z_STREAM_END:
                              {
                                 //int count = GZIP_MAX_BUFFER_SIZE - strmDef.avail_out;
                                 //getOutput(count, _dst);
                                 //clearOutputBuffers();
                                 break;
                              }
                           default:
                              {
#if GZIP_BTG_DEBUG
                                 std::cout << "zlib compress error(" << status << ")." << std::endl;
#endif
                                 return;
                                 break;
                              }
                           }
                     } while (status == Z_OK);
               }
            else
               {
                  strmDef.next_out  = &outputBuffer[0];
                  strmDef.avail_out = GZIP_MAX_BUFFER_SIZE;
                  
                  strmDef.next_in  = &inputBuffer[0];
                  strmDef.avail_in = _bufferSize;

                  int status = deflate(&strmDef, Z_NO_FLUSH);
            
                  if (status != Z_OK)
                     {
#if GZIP_BTG_DEBUG
                        std::cout << "zlib compress error." << std::endl;
#endif
                        return;
                     }
               }

            int count = GZIP_MAX_BUFFER_SIZE - strmDef.avail_out;
#if GZIP_BTG_DEBUG
            std::cout << "Got (compress) " << count << " bytes .." << std::endl;
#endif
            if (count)
               {
                  getOutput(count, _dst);
               }

            clearOutputBuffers();
         }

         void gzip::clearOutputBuffers()
         {
            //memset(&inputBuffer[0], 0xFF, GZIP_MAX_BUFFER_SIZE);
            memset(&outputBuffer[0], 0xFF, GZIP_MAX_BUFFER_SIZE);
         }

         void gzip::gzip_decompress(std::string const& _src, std::string& _dst)
         {
            inflateReset(&strmInf);

            int inputCounter = _src.size();
            int counter      = 0;
            const char* p =  _src.c_str();

            while (inputCounter > 0)
               {
                  if (inputCounter > GZIP_MAX_BUFFER_SIZE)
                     {
                        memcpy(&inputBuffer[0], &p[counter], GZIP_MAX_BUFFER_SIZE);
                        gzip_decompress_impl(GZIP_MAX_BUFFER_SIZE, _dst);
                        counter       += GZIP_MAX_BUFFER_SIZE;
                        inputCounter -=  GZIP_MAX_BUFFER_SIZE;
                     }
                  else
                     {
                        memcpy(&inputBuffer[0], &p[counter], inputCounter);
                        gzip_decompress_impl(inputCounter, _dst);
                        counter += inputCounter;
                        inputCounter =  0;
                     }
               }

            // gzip_decompress_impl(0, _dst, true /* flush */);
         }

         void gzip::gzip_decompress_impl(int _bufferSize, std::string & _dst, bool _flush)
         {
#if GZIP_BTG_DEBUG
            std::cout << "Attempt to decompress: " << _bufferSize << " bytes." << std::endl;

            for (int i = 0; i < _bufferSize; i++)
               {
                  std::cout << " " << std::hex << (int)inputBuffer[i];
               }
            std::cout << std::dec << std::endl;
#endif
            int ret = Z_OK;
            
            if (_flush)
               {
                  strmInf.next_in   = &inputBuffer[0];
                  strmInf.avail_in  = 0;
                  strmInf.avail_out = GZIP_MAX_BUFFER_SIZE;
                  strmInf.next_out  = &outputBuffer[0];

                  ret = inflate(&strmInf, Z_FINISH);

                  int count = GZIP_MAX_BUFFER_SIZE - strmInf.avail_out;
#if GZIP_BTG_DEBUG
                  std::cout << "Got (decompress) " << count << " bytes .." << std::endl;
#endif
                  if (count)
                     {
                        getOutput(count, _dst);
                     }
                  clearOutputBuffers();
                  return;
               }

            strmInf.avail_in = _bufferSize;
            strmInf.next_in = &inputBuffer[0];
            
            do 
               {
                  strmInf.avail_out = GZIP_MAX_BUFFER_SIZE;
                  strmInf.next_out  = &outputBuffer[0];
#if GZIP_BTG_DEBUG
                  std::cout << "strmInf: " << strmInf.avail_out << ", " << (int)strmInf.next_out << ", " << strmInf.avail_in << ", " << (int)strmInf.next_in << "." << std::endl;
#endif
                  ret = inflate(&strmInf, Z_NO_FLUSH);
#if GZIP_BTG_DEBUG
                  std::cout << "strmInf: " << strmInf.avail_out << ", " << (int)strmInf.next_out << ", " << strmInf.avail_in << ", " << (int)strmInf.next_in << "." << std::endl;
#endif
                  switch (ret) 
                     {
                     case Z_NEED_DICT:
                        {
                           ret = Z_DATA_ERROR;
                        }
                     case Z_DATA_ERROR:
                        {
#if GZIP_BTG_DEBUG
                           std::cout << "zlib decompress error('" << ret << "', msg='" << strmInf.msg << "')." << std::endl;
#endif
                           return;
                           break;
                        }
                     case Z_MEM_ERROR:
                        {
#if GZIP_BTG_DEBUG
                           std::cout << "zlib memory error('" << ret << "', msg='" << strmInf.msg << "')." << std::endl;
#endif
                           return;
                           break;
                        }
                     case Z_STREAM_END:
                        {
#if GZIP_BTG_DEBUG
                           std::cout << "Stream end." << std::endl;
#endif
                           break;
                        }
                     case Z_OK:
                        {
                           // Get output:

                           int count2 = GZIP_MAX_BUFFER_SIZE - strmInf.avail_out;
#if GZIP_BTG_DEBUG
                           std::cout << "Got (decompress, OK) " << count2 << " bytes .." << std::endl;
#endif                  
                           if (count2)
                              {
                                 getOutput(count2, _dst);
                                 clearOutputBuffers();
                              }

                           // More output waiting.
                           bool cont = true;

                           do
                              {
                                 strmInf.avail_out = GZIP_MAX_BUFFER_SIZE;
                                 strmInf.next_out  = &outputBuffer[0];

                                 int ret2 = inflate(&strmInf, Z_NO_FLUSH);

                                 switch (ret2)
                                    {
                                    case Z_STREAM_END:
#if GZIP_BTG_DEBUG
                                       std::cout << "END 3." << std::endl;
#endif
                                       cont = false;
                                       break;
                                    case Z_NEED_DICT:
                                    case Z_DATA_ERROR:
                                    case Z_MEM_ERROR:
                                       {
                                          cont = false;
#if GZIP_BTG_DEBUG
                                          std::cout << "zlib error " << ret << "." << std::endl;
#endif
                                          break;
                                       }
                                    case Z_OK:
#if GZIP_BTG_DEBUG
                                       std::cout << "OK 3." << std::endl;
#endif
                                       break;
                                    default:
                                       break;
                                    }

                                 int count3 = GZIP_MAX_BUFFER_SIZE - strmInf.avail_out;
#if GZIP_BTG_DEBUG
                                 std::cout << "Got (decompress, OK 2) " << count3 << " bytes .." << std::endl;
#endif                  
                                 if (count3 > 0)
                                    {
                                       getOutput(count3, _dst);
                                       clearOutputBuffers();
                                    }
                              } while ((strmInf.avail_out == 0) && (cont)); 
                           // while (cont);
#if GZIP_BTG_DEBUG
                           std::cout << "OK." << std::endl;
#endif
                           continue;
                           break;
                        }
                     default:
                        {
#if GZIP_BTG_DEBUG
                           std::cout << "zlib decompress ('" << ret << "')." << std::endl;  
#endif
                           break;
                        }
                     }
                  
                  int count = GZIP_MAX_BUFFER_SIZE - strmInf.avail_out;
#if GZIP_BTG_DEBUG
                  std::cout << "Got (decompress) " << count << " bytes .." << std::endl;
#endif                  
                  if (count)
                     {
                        getOutput(count, _dst);
                        clearOutputBuffers();
                     }
               } while (strmInf.avail_out == 0);
      }

      void gzip::getOutput(int _bytes, std::string & _dst)
      {
#if GZIP_BTG_DEBUG
         std::cout << "getOutput(" << _bytes << "):" << std::endl;
#endif
         for (int i = 0; i < _bytes; i++)
            {
#if GZIP_BTG_DEBUG
               unsigned char c = outputBuffer[i];
               std::cout << std::hex << (int)c << " ";
#endif
               _dst.push_back(outputBuffer[i]);
            }
#if GZIP_BTG_DEBUG
         std::cout << std::dec << std::endl;
#endif
      }

      gzip::~gzip()
      {
         inflateEnd(&strmInf);
         deflateEnd(&strmDef);
      }

      } // namespace os
   } // namespace core
} // namespace btg

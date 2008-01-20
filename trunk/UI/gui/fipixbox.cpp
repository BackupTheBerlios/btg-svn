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

#include "fipixbox.h"
#include <bcore/type.h>

#include <iostream>

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         using namespace btg::core;

         fileInfoPixbox::fileInfoPixbox(int const _bits_per_sample,
                                        int const _sizeX,
                                        int const _sizeY,
                                        fileInformation const& _fileinfo)
            : sizeX(_sizeX),
              sizeY(_sizeY),
              pixbuf(),
              rowstride(0),
              buffer(0),
              max_pixel_offset(0),
              bits(_fileinfo.getBits()),
              fileinfoSize(_fileinfo.size()),
              bitsize(2),
              full(false)
         {
            if (_fileinfo.isFull())
               {
                  full = true;
               }

            pixbuf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, 
                                         FALSE, 
                                         _bits_per_sample, 
                                         _sizeX, 
                                         _sizeY);

            pixbuf->fill(0);

            this->rowstride        = pixbuf->get_rowstride();
            this->buffer           = pixbuf->get_pixels();

            this->max_pixel_offset = (_sizeY * this->rowstride) + (_sizeX * 3);

            draw();
         }

         fileInfoPixbox::fileInfoPixbox(fileInformation const& _fileinfo, 
                                        Glib::RefPtr<Gdk::Pixbuf> _pixbuf)
            : sizeX(_pixbuf->get_width()),
              sizeY(_pixbuf->get_height()),
              pixbuf(_pixbuf),
              rowstride(_pixbuf->get_rowstride()),
              buffer(pixbuf->get_pixels()),
              max_pixel_offset((this->sizeY * this->rowstride) + (this->sizeX * 3)),
              bits(_fileinfo.getBits()),
              fileinfoSize(_fileinfo.size()),
              bitsize(2),
              full(false)
         {
            if (_fileinfo.isFull())
               {
                  full = true;
               }
            draw();
         }

         void fileInfoPixbox::findSizes()
         {
            int max_size = 2;

            for (int temp_size = max_size; temp_size < 32; temp_size++)
               {
                  int temp_x = ((this->sizeX-1) / temp_size);
                  int temp_y = ((this->sizeY-1) / temp_size);

                  if ((temp_x * temp_y) > this->fileinfoSize)
                     {
                        max_size = temp_size;
                     }
               }
            if ((max_size > bitsize) && (max_size > 0))
               {
                  bitsize = max_size;
               }
         }

         void fileInfoPixbox::draw()
         {
            this->findSizes();

            t_bitListCI bitIter = this->bits.begin();

            int x = 0;
            int y = 0;

            int temp_x = 0;
            int temp_y = 0;

            // Special case, every bit downloaded.
            if (full)
               {
                  for (temp_y = 0; temp_y < this->sizeY; temp_y++)
                     {
                        for (temp_x = 0; temp_x < this->sizeX; temp_x++)
                           {
                              this->setPixel(temp_x, temp_y, 0, 255, 0); // green
                           }
                     }
                  return;
               }

            // Special case, one bit only.
            if (bits.size() == 1)
               {
                  if (*bitIter)
                     {
                        for (temp_y = 0; temp_y < this->sizeY; temp_y++)
                           {
                              for (temp_x = 0; temp_x < this->sizeX; temp_x++)
                                 {
                                    this->setPixel(temp_x, temp_y, 0, 255, 0); // green
                                 }
                           }
                     }
                  else
                     {
                        for (temp_y = 0; temp_y < this->sizeY; temp_y++)
                           {
                              for (temp_x = 0; temp_x < this->sizeX; temp_x++)
                                 {
                                    this->setPixel(temp_x, temp_y, 255, 0, 0); // red
                                 }
                           }
                     }
                  return;
               }

            // TODO: Find out what to add to y, so the drawing appears
            // on the middle of the pixbuf.

            x = 0;
            y = 0;
            temp_x = 0;
            temp_y = 0;

            while (bitIter != bits.end())
               {
                  for (temp_y = y; temp_y<(y+bitsize); temp_y++)
                     {
                        for (temp_x = x; temp_x<(x+bitsize); temp_x++)
                           {
                              if (*bitIter)
                                 {
                                    this->setPixel(temp_x, temp_y, 0, 255, 0); // green
                                 }
                              else
                                 {
                                    this->setPixel(temp_x, temp_y, 255, 0, 0); // red
                                 }
                           }
                     }

                  x += bitsize;

                  if (x >= this->sizeX)
                     {
                        x = 0;
                        y += bitsize;
                     }

                  if (y >= this->sizeY)
                     {
                        break;
                     }

                  bitIter++;
               }

         }

         void fileInfoPixbox::setPixel(int const _x, 
                                       int const _y, 
                                       int const _r, 
                                       int const _g, int 
                                       const _b)
         {
            unsigned int pixel_offset = (_y * this->rowstride) + (_x * 3);

            if (pixel_offset < this->max_pixel_offset)
               {
                  buffer[pixel_offset]     = _r;
                  buffer[pixel_offset + 1] = _g;
                  buffer[pixel_offset + 2] = _b;
               }
         }

         Glib::RefPtr<Gdk::Pixbuf> fileInfoPixbox::getPixBuf() const
         {
            return pixbuf;
         }

         fileInfoPixbox::~fileInfoPixbox()
         {
         }

      } // namespace gui
   } // namespace UI
} // namespace btg

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

#ifndef FILEINFOPIXBOX_H
#define FILEINFOPIXBOX_H

#include <gtkmm.h>
#include <bcore/bitvector.h>

namespace btg
{
   namespace core
   {
      class fileInformation;
   }

   namespace UI
      {
         namespace gui
            {
               /**
                * \addtogroup gui
                */
               /** @{ */

               /// Implements a pixbox upon which a files' pieces are drawn.
               class fileInfoPixbox
                  {
                  public:
                     /// Constructor.
                     fileInfoPixbox(int const _bits_per_sample,
                                    int const _sizeX,
                                    int const _sizeY,
                                    btg::core::fileInformation const& _fileinfo);

                     /// Another constructor (is used in extended list of peers).
                     fileInfoPixbox(int const _bits_per_sample,
                                                    int const _sizeX,
                                                    int const _sizeY,
                                                    t_bitVector const& _bits);

                     /// Constructor. Takes an existing Gdk::Pixbuf
                     /// which is used for drawing on.
                     fileInfoPixbox(btg::core::fileInformation const& _fileinfo,
                                    Glib::RefPtr<Gdk::Pixbuf> _pixbuf);

                     /// Get a smart pointer to the Pixbuf contained in this class.
                     Glib::RefPtr<Gdk::Pixbuf> getPixBuf() const;

                     /// Destructor.
                     ~fileInfoPixbox();

                  private:
                     /// Find an optimal size for representing one bit.
                     void findSizes();

                     /// Draw the file information on the pixbuf.
                     void draw();

                     /// Set one pixel on the pixbuf.
                     /// @param [in] _x X-coordinate.
                     /// @param [in] _y Y-coordinate.
                     /// @param [in] _r RGB, r.
                     /// @param [in] _g RGB, g.
                     /// @param [in] _b RGB, b.
                     void setPixel(int const _x, 
                                   int const _y, 
                                   int const _r, 
                                   const int _g, 
                                   int const _b);

                     /// The width of the buffer.
                     int sizeX;

                     /// The height of the buffer.
                     int sizeY;

                     /// Smart pointer to the contained pixbuf object.
                     Glib::RefPtr<Gdk::Pixbuf>  pixbuf;

                     /// Rowstride.
                     int                        rowstride;

                     /// Pointer to the buffer used by the pixbuf.
                     guint8*                    buffer;

                     /// Used to check in the setPixel that writes to
                     /// buffer are not overflowing it.
                     unsigned int               max_pixel_offset;

                     /// Copy of the bits from the fileInformation
                     /// object given in constructor.
                     t_bitVector                  bits;

                     /// Number of bits.
                     int                        fileinfoSize;

                     /// Size in pixels of the (rectangular) area
                     /// representing one bit.
                     int                        bitsize;

                     /// Indicates that all the bits are set.
                     bool                       full;
                  private:
                     /// Copy constructor.
                     fileInfoPixbox(fileInfoPixbox const& _fopx);
                     /// Assignment operator.
                     fileInfoPixbox& operator=(fileInfoPixbox const& _fopx);
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif

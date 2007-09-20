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
 * $Id: sbuf.h,v 1.1.2.7 2007/01/16 20:26:36 wojci Exp $
 */

#ifndef SBUF_H
#define SBUF_H

#include "type.h"
#include "serializable.h"
#include "printable.h"
#include "dbuf.h"

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup core
          */
         /** @{ */

         /// This class is a simple serializable buffer.
         class sBuffer: public Serializable, public Printable
            {
            public:
               /// Default constructor.
               sBuffer();

               /// Constructor. Copies data from its argument.
               sBuffer(dBuffer & _dbuffer);

               /// Copy constructor.
               sBuffer(sBuffer const& _sbuffer);

               /// Read file from _filename.
               /// @param [in] _filename The file name from which the reading is to be done.
               bool read(std::string const& _filename);

               /// Write to a file.
               /// @param [in] _filename The file name from which the writing is to be done.
               bool write(std::string const& _filename) const;

               /// Return the size of the contained data.
               t_uint size() const;

               /// Reset buffer
               void reset();

               bool serialize(btg::core::externalization::Externalization* _e) const;

               bool deserialize(btg::core::externalization::Externalization* _e);

               std::string toString() const;

               /// Assignment operator.
               sBuffer & operator= (sBuffer const & _sbuffer);

               /// Quality operator.
               bool operator== (sBuffer const & _sbuffer) const;

               /// Destructor.
               ~sBuffer();
            private:
               /// The size of this buffer.
               t_uint  size_;
               /// The pointer to the contained buffer.
               t_byteP buffer_;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

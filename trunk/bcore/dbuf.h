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

#ifndef DBUF_H
#define DBUF_H

#include <bcore/type.h>

#include <vector>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup core
          */
         /** @{ */

         /// This class is a simple dynamic buffer, used for serialization/deserialization.
         class dBuffer
            {
            public:
               /// Constructor.
               /// @param [in] _initialSize The initial size of this buffer.
               dBuffer(t_int const _initialSize = 0);

               /// Constructor.
               /// @param [in] _buffer     Pointer to a byte array.
               /// @param [in] _bufferSize The size of the array.
               dBuffer(t_byteCP _buffer, t_int const _bufferSize);

               /// The assignment operator.
               dBuffer & operator= (dBuffer const & _dbuffer);

               /// The equality operator.
               bool operator== (dBuffer const & _dbuffer) const;

               /// The append operator.
               dBuffer& operator+=(dBuffer const& _dbuf);

               /// getByte and getBytes returns this if the destination does not
               /// contain enough memory to store the requested number of bytes.
               static t_int const INVALID_RWOP = -1;

               /// Add one byte.
               /// Resizes the used memory if needed.
               t_int addByte(t_byte const _src);

               /// Like memcpy.
               /// Resizes the used memory if needed.
               t_int addBytes(t_byteCP _src, t_int const _size);

               /// Get one byte.
               /// @returns 1 or INVALID_RWOP.
               t_int getByte(t_byteP _dst);

               /// Like memcpy.
               /// @returns _size or INVALID_RWOP.
               t_int getBytes(t_byteP _dst, t_int const _size);

               /// Get N bytes.
               /// @returns _n bytes or INVALID_RWOP.
               t_int getNBytes(t_int const _n, t_byteP _dst, t_int const _dst_size);

               /// Get the number of bytes inserted by the last add* operation.
               t_int lastInserted() const;

               /// Get the number of bytes removed by the last get* operation.
               t_int lastRemoved() const;

               /// Peek inside the buffer. If there is any data, return the
               /// first byte. If no data is present, this function returns 0,
               /// which is all right, since zero is never used by btg for
               /// command types.
               /// @return The first byte or 0 if no data is present.
               // t_byte peek() const;

               /// Peek inside the buffer. If the first bytes can
               /// represent an integer, return it.  Else return
               /// INVALID_RWOP.
               t_int peek() const;

               /// Get a counter that indicates how many bytes were removed.
               /// This resets the counter. This is used to find out the size of
               /// deserialized data.
               /// @param [in] _resetCounter Indicates if the counter should be
               /// reset to 0 after reading its current value.
               t_int getReadCounter(bool const _resetCounter = true);

               /// Erase the contents.
               void erase();

               /// Reset all read/write counters.
               void resetRWCounters();

               /// Returns the size of the contents.
               t_uint size() const;

               /// Convert the contents to a string. Only useful for printing the contents.
               /// @param [in] _maxOutput Output this many characters.
               std::string toString(t_uint const _maxOutput = 254) const;

               /// Destructor.
               ~dBuffer();
            private:
               /// The contents of this buffer are represented by this object.
               t_byteList   contents;
               /// The current size of this buffer.
               t_int        current_size;
               /// Counter updated by the get* functions.
               t_int        read_counter;

               /// The number of bytes which were written by the last call to a
               /// add* function.
               t_int        lastInsertedBytes;

               /// The number of bytes which were read by the last call to a
               /// get* function.
               t_int        lastRemovedBytes;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

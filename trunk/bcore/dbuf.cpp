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
 * $Id: dbuf.cpp,v 1.8.4.7 2006/08/17 19:56:55 wojci Exp $
 */

#include "dbuf.h"
#include "logmacro.h"

#include <iostream>

extern "C"
{
#include <string.h>
#include <assert.h>
}

#include "t_string.h"

namespace btg
{
   namespace core
   {

      using namespace std;

      dBuffer::dBuffer(t_int const _initialSize)
         : contents(_initialSize),
           current_size(_initialSize),
           read_counter(0),
           lastInsertedBytes(0),
           lastRemovedBytes(0)
      {
      }

      dBuffer::dBuffer(t_byteCP _buffer, t_int const _bufferSize)
         : contents(0),
           current_size(0),
           read_counter(0),
           lastInsertedBytes(0),
           lastRemovedBytes(0)
      {
         addBytes(_buffer, _bufferSize);
      }

      dBuffer & dBuffer::operator= (dBuffer const & _dbuffer)
      {
         if (_dbuffer == *this)
            {
               return *this;
            }

         this->contents          = _dbuffer.contents;
         this->current_size      = _dbuffer.current_size;
         this->read_counter      = _dbuffer.read_counter;
         this->lastInsertedBytes = _dbuffer.lastInsertedBytes;
         this->lastRemovedBytes  = _dbuffer.lastRemovedBytes;

         return *this;
      }

      dBuffer& dBuffer::operator+=(dBuffer const& _dbuf)
      {
         dBuffer cdbuf = _dbuf;

         t_uint src_size = cdbuf.size();

         t_byte space[src_size];

         cdbuf.getBytes(space, src_size);

         addBytes(space, src_size);

         return *this;
      }

      bool dBuffer::operator== (dBuffer const& _dbuffer) const
      {
         if (contents != _dbuffer.contents)
            {
               return false;
            }

         if (current_size != _dbuffer.current_size)
            {
               return false;
            }

         if (read_counter != _dbuffer.read_counter)
            {
               return false;
            }

         if (lastInsertedBytes != _dbuffer.lastInsertedBytes)
            {
               return false;
            }

         if (lastRemovedBytes != _dbuffer.lastRemovedBytes)
            {
               return false;
            }

         return true;
      }

      t_int dBuffer::addByte(t_byte const _src)
      {
         contents.push_back(_src);
         current_size++;
         lastInsertedBytes = 1;
         return 1;
      }

      t_int dBuffer::addBytes(t_byteCP _src, t_int const _size)
      {
         t_int argCounter;

         for (argCounter = 0; argCounter < _size; argCounter++)
            {
               contents.push_back(_src[argCounter]);
               current_size++;
            }
         lastInsertedBytes = _size;
         return _size;
      }

      t_int dBuffer::getByte(t_byteP _dst)
      {
         t_byteList_I bli = contents.begin();
         if (bli != contents.end())
            {
               _dst[0] = *bli;
               contents.erase(bli);
               current_size--;
               read_counter++;
               lastRemovedBytes = 1;
               return 1;
            }
         lastRemovedBytes = dBuffer::INVALID_RWOP;
         return dBuffer::INVALID_RWOP;
      }

      t_int dBuffer::getBytes(t_byteP _dst, t_int const _size)
      {
         if (current_size > _size)
            {
               lastRemovedBytes = dBuffer::INVALID_RWOP;
               return dBuffer::INVALID_RWOP;
            }
         else
            {
               // Clear the byte array first.
               memset(_dst, 0, _size);
               t_int counter = 0;
               for (t_byteList_CI blci = contents.begin(); blci != contents.end(); blci++)
                  {
                     _dst[counter++] = *blci;
                     current_size--;
                  }
               contents.clear();
               read_counter += counter;
               lastRemovedBytes = counter;
               return counter;
            }
      }

      t_int dBuffer::getNBytes(t_int const _n, t_byteP _dst, t_int const _dst_size)
      {
         if (_n > current_size)
            {
               BTG_NOTICE("dBuffer::getNBytes failed: _n(" << _n << ") > current_size(" << current_size << ").");

               lastRemovedBytes = dBuffer::INVALID_RWOP;
               return dBuffer::INVALID_RWOP;
            }

         if (_n > _dst_size)
            {
               BTG_NOTICE("dBuffer::getNBytes failed: _n > _dst_size.");

               lastRemovedBytes = dBuffer::INVALID_RWOP;
               return dBuffer::INVALID_RWOP;
            }

         t_int counter = 0;
         for (t_byteList_CI blci = contents.begin(); blci != (contents.begin() + _n); blci++)
            {
               _dst[counter++] = *blci;
               current_size--;
            }
         contents.erase(contents.begin(), contents.begin() + _n);
         read_counter += counter;
         lastRemovedBytes = counter;
         return counter;
      }

      t_int dBuffer::peek() const
      {
         t_int dest        = dBuffer::INVALID_RWOP;
         t_byteP dest_p    = reinterpret_cast<t_byteP>(&dest);
         t_uint const size = sizeof(t_int);

         if (contents.size() >= size)
            {
               t_byteList_CI blci = contents.begin();

               // Get the sizeof int bytes.
               for (t_uint counter = 0;
                    counter < size;
                    counter++)
                  {
                     dest_p[counter] = *blci;
                     blci++;
                  }
            }

         if (dest <=0)
            {
               dest = dBuffer::INVALID_RWOP;
            }

         return dest;
      }

      void dBuffer::erase()
      {
         contents.clear();

         current_size      = 0;
         read_counter      = 0;
         lastRemovedBytes  = dBuffer::INVALID_RWOP;
         lastInsertedBytes = dBuffer::INVALID_RWOP;
      }

      t_uint dBuffer::size() const
      {
         return current_size;
      }

      t_int dBuffer::getReadCounter(bool const _resetCounter)
      {
         t_int temp_read_counter = read_counter;

         if (_resetCounter == true)
            {
               read_counter = 0;
            }

         return temp_read_counter;
      }

      std::string dBuffer::toString(t_uint const _maxOutput) const
      {
         t_uint counter = 0;
         std::string output;
         for (t_byteList_CI blci = contents.begin(); blci != contents.end(); blci++)
            {
               t_int c = static_cast<t_int>(*blci);
               output += " " + btg::core::convertToString<t_int>(c);
               if (counter >= _maxOutput)
                  {
                     break;
                  }
               counter++;
            }
         return output;
      }

      dBuffer::~dBuffer()
      {
         contents.clear();
      }

      t_int dBuffer::lastInserted() const
      {
         return lastInsertedBytes;
      }

      t_int dBuffer::lastRemoved() const
      {
         return lastRemovedBytes;
      }

      void dBuffer::resetRWCounters()
      {
         lastInsertedBytes = 0;
         lastRemovedBytes  = 0;
      }

   } // namespace core
} // namespace btg

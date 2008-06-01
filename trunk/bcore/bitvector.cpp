/*
 * btg Copyright (C) 2008 Roman Rybalko.
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

#include <bcore/bitvector.h>

#include <bcore/helpermacro.h>

namespace btg
{
   namespace core
   {

      bitvector::bitvector(size_t _n, const bool& _value) :
         std::vector<bool>(_n, _value)
      {
         
      }

      bitvector::bitvector(const std::vector<bool>& _src) :
         std::vector<bool>(_src)
      {

      }

      bool bitvector::serialize(btg::core::externalization::Externalization* _e) const
      {
         // Length, in bits.
         t_int length = size();
         _e->intToBytes(&length);
         BTG_RCHECK(_e->status());
         
         std::string bytes = toString();
         _e->addBytes(reinterpret_cast<const t_byte*>(bytes.c_str()), bytes.size());
         BTG_RCHECK(_e->status());
         
         return true;
      }
      
      bool bitvector::deserialize(btg::core::externalization::Externalization* _e)
      {
         // Length, in bits.
         t_int length = 0;
         _e->bytesToInt(&length);
         BTG_RCHECK(_e->status());
         
         t_byteP bytes;
         t_uint size;
         if (_e->getBytes(&bytes, size))
         {
            std::string sData(reinterpret_cast<const char*>(bytes), size);
            fromString(sData, length);
            delete bytes;
         }
         BTG_RCHECK(_e->status());
         
         return true;         
      }

      // 11001010 01010011 01111110 100001 -> 0x53 0xCA 0x7E 0x21
      std::string bitvector::toString() const
      {
         std::string data(size() / 8 + ( size() % 8 ? 1 : 0 ), (char)0);
         for(size_t ibyte = 0, ibit = 0; ibit < size(); ++ibit)
         {
            data[ibyte] |= at(ibit) << (ibit % 8);
            if (ibit % 8 == 7)
               ++ibyte;
         }
         return data;
      }
      
      void bitvector::fromString(const std::string & _sData, size_t _size)
      {
         resize(_size);
         for(size_t ibyte = 0, ibit = 0; ibit < size(); ++ibit)
         {
            at(ibit) = (_sData[ibyte] >> (ibit % 8)) & 1;
            if (ibit % 8 == 7)
               ++ibyte;
         }
      }
      
      bool bitvector::full() const
      {
         for (const_iterator i = begin(); i != end(); ++i)
            if (!*i)
               return false;
         return true;
      }
      
   } // namespace core
} // namespace btg

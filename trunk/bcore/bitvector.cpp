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

      bitvector::bitvector(size_t _n, const bool& _value)
         : std::vector<bool>(_n, _value)
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

      std::string bitvector::toString() const
      {
         // see http://www.everfall.com/paste/id.php?ghhy9v0dlo75 for full example
         return std::string( (const char *)_M_impl._M_start._M_p, size() / 8 + ( size() % 8 ? 1 : 0 ) );
      }
      
      void bitvector::fromString(const std::string & sData, size_t size)
      {
         _M_deallocate();
         _M_initialize(size);
         std::copy(sData.begin(),
            sData.begin() +
               std::min( (const char *)_M_impl._M_end_of_storage - (const char *)_M_impl._M_start._M_p,
                  (int)sData.size() ),
            (char *)_M_impl._M_start._M_p);
      }
      
   } // namespace core
} // namespace btg

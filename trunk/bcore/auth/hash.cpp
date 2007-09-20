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
 * $Id: hash.cpp,v 1.1.4.2 2007/06/06 22:04:31 wojci Exp $
 */

#include "hash.h"

#include <sstream>

#include <bcore/helpermacro.h>
#include <bcore/os/gnutlsif.h>

namespace btg
{
   namespace core
   {
      t_uint const hash_size = 20;

      Hash::Hash()
      {
         memset(hash_, 0, hash_size);
      }

      Hash::Hash(Hash const& _hash)
         : Serializable()           
      {
         memset(hash_, 0, hash_size);

         if (_hash != *this)
            {
               memset(hash_, 0, hash_size);
               memcpy(hash_, _hash.hash_, hash_size);
            }
      }

      void Hash::generate(std::string const& _text)
      {
         if (_text.size() == 0)
            {
               return;
            }

         t_ulong const size = strlen(_text.c_str());

         unsigned char input[size+1];
         memset(input, 0, size+1);

         strncpy(reinterpret_cast<char*>(&input[0]), _text.c_str(), size);

         btg::core::os::gtlsGeneric::init();
         const unsigned char * hash_pointer = btg::core::os::gtlsGeneric::generateSha1(input, size);

         if(!hash_pointer)
            return;

         memcpy(hash_, hash_pointer, hash_size);
      }

      void Hash::set(std::string const& _hash)
      {
         // Expect hash in hex
         if (_hash.size() != hash_size*2)
            {
               BTG_NOTICE("Hash:set, broken hash, " << _hash.size() << " bytes");
               return;
            }

         t_uint output_counter = 0;
         for(t_uint i = 0; i < hash_size*2; i+=2)
         {
            std::istringstream iss(_hash.substr(i, 2));
            t_uint tmp;
            iss >> std::hex >> tmp;
            if (output_counter > hash_size)
            {
               break;
            }
            hash_[output_counter] = static_cast<t_byte>(tmp);
            output_counter++;
         }
      }

      void Hash::get(std::string & _hash) const
      {
         std::string output;
         std::ostringstream oss;
         oss << std::setfill('0');
         for ( t_uint i = 0; i < hash_size; i++ )
         {
            oss << std::setw(2) << std::hex << static_cast<int>(hash_[i]);
         }
         _hash.assign(oss.str());
      }

      bool Hash::operator== (Hash const& _hash) const
      {
         bool status = true;

         for(t_uint i = 0; i < hash_size; i++)
            {
               if (hash_[i] != _hash.hash_[i])
                  {
                     status = false;
                     break;
                  }
            }

         return status;
      }

      bool Hash::operator!= (Hash const& _hash) const
      {
         bool status = (*this == _hash);

         if (status)
            {
               status = false;
            }
         else
            {
               status = true;
            }

         return status;
      }

      Hash& Hash::operator= (Hash const& _hash)
      {
         if (_hash != *this)
            {
               memcpy(hash_, _hash.hash_, hash_size);
            }

         return *this;
      }

      bool Hash::serialize(btg::core::externalization::Externalization* _e) const
      {
         std::string temp_hash;
         get(temp_hash);

         BTG_RCHECK(_e->stringToBytes(&temp_hash));

         return true;
      }

      bool Hash::deserialize(btg::core::externalization::Externalization* _e)
      {
         std::string temp_hash;
         BTG_RCHECK(_e->bytesToString(&temp_hash));

         set(temp_hash);

         return true;
      }

      Hash::~Hash()
      {
      }

   } // namespace core
} // namespace btg



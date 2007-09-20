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
 * $Id: hash.h,v 1.1.4.1 2006/03/13 18:40:47 jstrom Exp $
 */

#ifndef HASH_H
#define HASH_H

#include <bcore/type.h>
#include <bcore/serializable.h>

#include <string>

namespace btg
{
   namespace core
      {
         /// Reperesents a hash of a string, which is used in btg.
         class Hash: public Serializable
            {
            public:
               /// Default constructor.
               Hash();

               /// Copy constructor.
               Hash(Hash const& _hash);

               /// Generate hash from _text.
               void generate(std::string const& _text);

               /// Set the hash by reading a string of hex numbers.
               void set(std::string const& _hash);

               /// Get the hash as a string containing hex numbers.
               void get(std::string & _hash) const;

               /// The equality operator.
               bool operator== (Hash const& _hash) const;

               /// The not equal operator.
               bool operator!= (Hash const& _hash) const;

               /// The assignment operator.
               Hash& operator= (Hash const& _hash);

               bool serialize(btg::core::externalization::Externalization* _e) const;

               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Destructor.
               virtual ~Hash();
            private:
               /// Array containing the hash.
               t_byte       hash_[20];
            };

      } // namespace core
} // namespace btg

#endif

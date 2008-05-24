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

#ifndef BITVECTOR_H_
#define BITVECTOR_H_

#include <vector>

#include <bcore/serializable.h>
#include <bcore/printable.h>

namespace btg
{
   namespace core
      {
   /**
          * \addtogroup core
          */
         /** @{ */

         /// Intended to hold information about pieces.
         class bitvector: public std::vector<bool>, public Serializable, public Printable
            {
            public:
               /// Constructor (without allocator reference)
               bitvector(size_t = 0, const bool& = false);
               
               /// Used to convert this object to a byte array. This does not
               /// create a valid representation, since the session information
               /// and command type is not included, only the data from this
               /// object.
               bool serialize(btg::core::externalization::Externalization* _e) const;

               /// Setup this object with data contained in the byte array.
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Convert an instance of this class to a std::string.
               /// The string cntains the vector in packed format, when 8 bits occupies 1 byte
               std::string toString() const;
               
               /// Initialize object from string
               /// @param [in] _sData The string of bits in packed format (i.e. 8 bits per byte)
               /// @param [in] _size the count of bits to initialize.
               ///             Trailing bits (if string contains less bits then size)
               ///             will not be initialized.
               virtual void fromString(const std::string & _sData, size_t _size);
            };

         /** @} */

      } // namespace core
} // namespace btg

/**
 * \addtogroup core
 */
/** @{ */

/// List of bits.
typedef btg::core::bitvector                      t_bitVector;

/// List of bits - iterator.
typedef btg::core::bitvector::iterator            t_bitVectorI;

/// List of bits - const iterator.
typedef btg::core::bitvector::const_iterator      t_bitVectorCI;

/** @} */

#endif /*BITVECTOR_H_*/

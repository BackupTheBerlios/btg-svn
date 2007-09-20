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
 * $Id: hru.h,v 1.9.4.3 2007/01/16 20:26:36 wojci Exp $
 */

#ifndef HRU_H
#define HRU_H

#include <string>

#include "type.h"

namespace btg
{
   namespace core
      {
	/**
          * \addtogroup core
          */
         /** @{ */

         /// Convert number of bytes into a human readable format.
         class humanReadableUnit
            {
            public:
               /// Units.
               enum EIC_UNIT
                  {
                     UNKN = 0, //!< unknown unit.
                     B    = 1, //!< bytes.
                     KiB  = 2, //!< kibibyte.
                     MiB  = 3, //!< mebibyte.
                     GiB  = 4, //!< gibibyte.
                     TiB  = 5, //!< tebibyte.
                     PiB  = 6  //!< pebibyte.
                  };

            public:
               /// Constructor.
               /// @param [in] _value          The value.
               /// @param [in] _value_unit     The unit of the value.
               /// @param [in] _remainder      The remainder.
               /// @param [in] _remainder_unit The unit of the remainder.
               humanReadableUnit(t_ullong const _value,
                                 EIC_UNIT const _value_unit,
                                 t_ullong const _remainder,
                                 EIC_UNIT const _remainder_unit);
               /// Convert number of bytes into a human readable format.
               /// @param [in] _numberOfbytes The number of bytes to convert.
               /// @param [in] _cont          Should be set to true.
               static humanReadableUnit convert(t_ullong const _numberOfbytes,
                                                bool const     _cont = true);
               /// Convert this instance to a string.
               std::string toString() const;

               /// Get the contained value.
               inline t_ullong getValue() const;

               /// Get the unit the value is in.
               inline EIC_UNIT getValueUnit() const;

               /// Get the contained reminder
               inline t_ullong getRemainder() const;

               /// Get the unit the reminder is in.
               inline EIC_UNIT getRemainderUnit() const;

               /// Destructor.
               ~humanReadableUnit();

            private:
               /// The value.
               t_ullong  value;
               /// The unit describing this value.
               EIC_UNIT value_unit;
               /// The remainder.
               t_ullong  remainder;
               /// The unit describing this remainder.
               EIC_UNIT remainder_unit;
            };

         /// Shorter type name for this class.
         typedef class humanReadableUnit hRU;

         // Inlined functions.

         t_ullong humanReadableUnit::getValue() const
            {
               return value;
            }

         humanReadableUnit::EIC_UNIT humanReadableUnit::getValueUnit() const
            {
               return value_unit;
            }

         t_ullong humanReadableUnit::getRemainder() const
            {
               return remainder;
            }

         humanReadableUnit::EIC_UNIT humanReadableUnit::getRemainderUnit() const
            {
               return remainder_unit;
            }

	 /** @} */

      } // namespace core
} // namespace btg

#endif // HRU

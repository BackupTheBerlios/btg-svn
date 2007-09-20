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
 * $Id: hrr.h,v 1.3.4.4 2007/01/16 20:26:36 wojci Exp $
 */

#ifndef HRR_H
#define HRR_H

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

         /// Convert number of bytes per second into a human readable format.
         class humanReadableRate
            {
            public:
               /// Units.
               enum EIC_UNIT
                  {
                     UNKN    = 0, //!< unknown unit.
                     B_PS    = 1, //!< bytes per second.
                     KiB_PS  = 2, //!< kibibyte per second.
                     MiB_PS  = 3, //!< mebibyte per second.
                     GiB_PS  = 4, //!< gibibyte per second.
                     TiB_PS  = 5, //!< tebibyte per second.
                     PiB_PS  = 6  //!< pebibyte per second.
                  };

               /// Constructor.
               /// @param [in] _value          The value.
               /// @param [in] _value_unit     The unit of the value.
               humanReadableRate(t_ulong const  _value,
                                 EIC_UNIT const _value_unit);

               /// Convert number of bytes per second into a human
               /// readable format.
               /// @param [in] _numberOfbytesPerSecond The number of bytes per second to convert.
               static humanReadableRate convert(t_ulong const _numberOfbytesPerSecond);

               /// Convert this instance to a string.
               /// @param [in] _shrt Output short units.
               std::string toString(bool const _shrt = false) const;

               /// Get the contained value.
               inline t_ulong getValue() const;

               /// Get the unit the value is in.
               inline EIC_UNIT getValueUnit() const;

               ~humanReadableRate();
            private:
               /// The value.
               t_ulong  value;
               /// The unit describing this value.
               EIC_UNIT value_unit;
            };

         /// Short type.
         typedef class humanReadableRate hRR;

         // Inlined functions.

         t_ulong humanReadableRate::getValue() const
            {
               return value;
            }

         humanReadableRate::EIC_UNIT humanReadableRate::getValueUnit() const
            {
               return value_unit;
            }

	 /** @} */

      } // namespace core
} // namespace btg

#endif // HRR

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

#ifndef HRT_H
#define HRT_H

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

         /// Convert number of seconds into a human readable format.
         class humanReadableTime
            {
            public:
               /// Units.
               enum HRT_UNIT
                  {
                     UNKN    = 0, //!< unknown unit.
                     SECOND  = 1, //!< second.
                     MINUTE  = 2, //!< minute.
                     HOUR    = 3, //!< hour.
                     DAY     = 4, //!< day.
                     WEEK    = 5, //!< week.
                     MONTH   = 6, //!< month.
                     YEAR    = 7  //!< month.
                  };

            public:
               /// Constructor.
               /// @param [in] _value          The value.
               /// @param [in] _value_unit     The unit of the value.
               /// @param [in] _remainder      The remainder.
               /// @param [in] _remainder_unit The unit of the remainder.
               humanReadableTime(t_ullong const _value,
                                 HRT_UNIT const _value_unit,
                                 t_ullong const _remainder,
                                 HRT_UNIT const _remainder_unit);

               /// Convert number of minutes into a human readable format.
               /// @param [in] _numberOfseconds The number of bytes to convert.
               /// @param [in] _cont          Should be set to true.
               static humanReadableTime convert(t_ullong const _numberOfseconds,
                                                bool const     _cont = true);

               /// Convert this instance to a string.
               std::string toString() const;

               /// Get the contained value.
               inline t_ullong getValue() const;

               /// Get the unit the value is in.
               inline HRT_UNIT getValueUnit() const;

               /// Get the contained reminder
               inline t_ullong getRemainder() const;

               /// Get the unit the reminder is in.
               inline HRT_UNIT getRemainderUnit() const;

               /// Destructor.
               ~humanReadableTime();

            private:
               /// The value.
               t_ullong  value;
               /// The unit describing this value.
               HRT_UNIT value_unit;
               /// The remainder.
               t_ullong  remainder;
               /// The unit describing this remainder.
               HRT_UNIT remainder_unit;

	       /// Convert an unit to a string.
	       /// @param [in]  _unit   The unit.
	       /// @param [in]  _value  The value.
	       /// @param [out] _output Place the string in this buffer.
	       void unitToString(HRT_UNIT const _unit, 
				 t_ullong const _value,
				 std::string & _output) const;
            };

         /// Shorter type name for this class.
         typedef class humanReadableTime hRT;

         // Inlined functions.

         t_ullong humanReadableTime::getValue() const
            {
               return value;
            }

         humanReadableTime::HRT_UNIT humanReadableTime::getValueUnit() const
            {
               return value_unit;
            }

         t_ullong humanReadableTime::getRemainder() const
            {
               return remainder;
            }

         humanReadableTime::HRT_UNIT humanReadableTime::getRemainderUnit() const
            {
               return remainder_unit;
            }

	 /** @} */

      } // namespace core
} // namespace btg

#endif // HRT

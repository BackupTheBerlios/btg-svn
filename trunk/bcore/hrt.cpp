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
 * $Id: hrt.cpp,v 1.1.2.2 2007/02/12 21:31:36 wojci Exp $
 */

#include "hrt.h"
#include "t_string.h"
#include <iostream>

using namespace std;

namespace btg
{
   namespace core
   {
      humanReadableTime::humanReadableTime(t_ullong const  _value,
                                           HRT_UNIT const _value_unit,
                                           t_ullong const  _remainder,
                                           HRT_UNIT const _remainder_unit)
         : value(_value),
           value_unit(_value_unit),
           remainder(_remainder),
           remainder_unit(_remainder_unit)
      {
      }

      humanReadableTime::~humanReadableTime()
      {
      }

      humanReadableTime humanReadableTime::convert(t_ullong const _numberOfseconds, 
						   bool const _cont)
      {
         humanReadableTime::HRT_UNIT pri_unit  = humanReadableTime::UNKN;
         t_ullong                    pri_value = 0;

         humanReadableTime::HRT_UNIT sec_unit  = humanReadableTime::UNKN;
         t_ullong                    sec_value = 0;

         t_ullong u_sec, u_min, u_hour, u_day, u_week, u_month, u_year;

         u_sec   = 1ull;
         u_min   = 60ull * u_sec;
         u_hour  = 60ull * u_min;
         u_day   = 24ull * u_hour;
         u_week  = 7ull  * u_day;
         u_month = 4ull  * u_week;
         u_year  = 12ull * u_month;

         // Check for zero.
         if (_numberOfseconds == 0)
            {
               humanReadableTime zerovalue(0, humanReadableTime::SECOND, 
					   0, humanReadableTime::UNKN);
               return zerovalue;
            }

         if (_numberOfseconds >= u_year)
            {
               pri_value = _numberOfseconds / u_year;
               pri_unit  = humanReadableTime::YEAR;

               sec_value = _numberOfseconds - (pri_value * u_year);
               sec_unit  = humanReadableTime::SECOND;
            }
         else if (_numberOfseconds >= u_month)
            {
               pri_value = _numberOfseconds / u_month;
               pri_unit  = humanReadableTime::MONTH;

               sec_value = _numberOfseconds - (pri_value * u_month);
               sec_unit  = humanReadableTime::SECOND;
            }
         else if (_numberOfseconds >= u_week)
            {
               pri_value = _numberOfseconds / u_week;
               pri_unit  = humanReadableTime::WEEK;

               sec_value = _numberOfseconds - (pri_value * u_week);
               sec_unit  = humanReadableTime::SECOND;
            }
         else if (_numberOfseconds >= u_day)
            {
               pri_value = _numberOfseconds / u_day;
               pri_unit  = humanReadableTime::DAY;

               sec_value = _numberOfseconds - (pri_value * u_day);
               sec_unit  = humanReadableTime::SECOND;
            }
         else if (_numberOfseconds >= u_hour)
            {
               pri_value = _numberOfseconds / u_hour;
               pri_unit  = humanReadableTime::HOUR;

               sec_value = _numberOfseconds - (pri_value * u_hour);
               sec_unit  = humanReadableTime::SECOND;
            }

         else if (_numberOfseconds >= u_min)
            {
               pri_value = _numberOfseconds / u_min;
               pri_unit  = humanReadableTime::MINUTE;

               sec_value = _numberOfseconds - (pri_value * u_min);
               sec_unit  = humanReadableTime::SECOND;
            }
         else
            {
               pri_value = _numberOfseconds;
               pri_unit  = humanReadableTime::SECOND;

               sec_value = 0;
               sec_unit  = humanReadableTime::SECOND;
            }

         humanReadableTime finalValue(pri_value, pri_unit, sec_value, sec_unit);

         if (_cont && pri_unit != humanReadableTime::SECOND)
            {
               humanReadableTime hru = humanReadableTime::convert(sec_value, false);
               if (hru.value_unit != humanReadableTime::SECOND)
                  {
                     finalValue.remainder_unit = hru.value_unit;
                     finalValue.remainder      = hru.value;
                  }
            }

         return finalValue;
      }

      string humanReadableTime::toString() const
      {
         string output;

         if (value == 0)
            {
               output = "0 seconds";
            }
         else 
            {
               output += convertToString<t_ullong>(value);
	       unitToString(value_unit, value, output);
            } // value > 0

         if (remainder > 0)
            {
               output += ", ";
               output += convertToString<unsigned long>(remainder);

	       unitToString(remainder_unit, remainder, output);

            } // remainder > 0

         return output;
      }

     void humanReadableTime::unitToString(HRT_UNIT const _unit, 
					  t_ullong const _value,
					  std::string & _output) const
     {
       switch(_unit)
	 {
	 case SECOND:
	   _output += " second";
	   break;
	 case MINUTE:
	   _output += " minute";
	   break;
	 case HOUR:
	   _output += " hour";
	   break;
	 case DAY:
	   _output += " day";
	   break;
	 case WEEK:
	   _output += " week";
	   break;
	 case MONTH:
	   _output += " month";
	   break;
	 case YEAR:
	   _output += " year";
	   break;
	 default:
	   // Output nothing if the unit is not known.
	   break;
	 }

       if ((_value > 1) && _unit != UNKN)
	 {
	   _output += "s";
	 }

     }

   } // namespace core
} // namespace btg

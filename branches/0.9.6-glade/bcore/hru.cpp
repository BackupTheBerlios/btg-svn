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

#include "hru.h"
#include "t_string.h"
#include <iostream>

using namespace std;

namespace btg
{
   namespace core
   {
      humanReadableUnit::humanReadableUnit(t_ullong const  _value,
                                           EIC_UNIT const _value_unit,
                                           t_ullong const  _remainder,
                                           EIC_UNIT const _remainder_unit)
         : value(_value),
           value_unit(_value_unit),
           remainder(_remainder),
           remainder_unit(_remainder_unit)
      {
      }

      humanReadableUnit::~humanReadableUnit()
      {
      }

      humanReadableUnit humanReadableUnit::convert(t_ullong const _numberOfbytes, bool const _cont)
      {
         humanReadableUnit::EIC_UNIT pri_unit  = humanReadableUnit::UNKN;
         t_ullong                    pri_value = 0;

         humanReadableUnit::EIC_UNIT sec_unit  = humanReadableUnit::UNKN;
         t_ullong                    sec_value = 0;

         t_ullong u_kib, u_mib, u_gib, u_tib, u_pib;

         u_kib = (1024ull);
         u_mib = (1024ull * u_kib);
         u_gib = (1024ull * u_mib);
         u_tib = (1024ull * u_gib);
         u_pib = (1024ull * u_tib);
         //u_eib = (1024 * u_pib);

         // Check for zero.
         if (_numberOfbytes == 0)
            {
               humanReadableUnit zerovalue(0, humanReadableUnit::B, 0, humanReadableUnit::UNKN);
               return zerovalue;
            }

         if (_numberOfbytes >= u_pib)
            {
               pri_value = _numberOfbytes / u_pib;
               pri_unit  = humanReadableUnit::PiB;

               sec_value = _numberOfbytes - (pri_value * u_pib);
               sec_unit  = humanReadableUnit::B;
            }
         else if (_numberOfbytes >= u_tib)
            {
               pri_value = _numberOfbytes / u_tib;
               pri_unit  = humanReadableUnit::TiB;

               sec_value = _numberOfbytes - (pri_value * u_tib);
               sec_unit  = humanReadableUnit::B;
            }
         else if (_numberOfbytes >= u_gib)
            {
               pri_value = _numberOfbytes / u_gib;
               pri_unit  = humanReadableUnit::GiB;

               sec_value = _numberOfbytes - (pri_value * u_gib);
               sec_unit  = humanReadableUnit::B;
            }
         else if (_numberOfbytes >= u_mib)
            {
               pri_value = _numberOfbytes / u_mib;
               pri_unit  = humanReadableUnit::MiB;

               sec_value = _numberOfbytes - (pri_value * u_mib);
               sec_unit  = humanReadableUnit::B;
            }
         else if (_numberOfbytes >= u_kib)
            {
               pri_value = _numberOfbytes / u_kib;
               pri_unit  = humanReadableUnit::KiB;

               sec_value = _numberOfbytes - (pri_value * u_kib);
               sec_unit  = humanReadableUnit::B;
            }
         else
            {
               pri_value = _numberOfbytes;
               pri_unit  = humanReadableUnit::B;

               sec_value = 0;
               sec_unit  = humanReadableUnit::B;
            }

         humanReadableUnit finalValue(pri_value, pri_unit, sec_value, sec_unit);

         if (_cont == true && pri_unit != humanReadableUnit::B)
            {
               humanReadableUnit hru = humanReadableUnit::convert(sec_value, false);
               if (hru.value_unit != humanReadableUnit::B)
                  {
                     finalValue.remainder_unit = hru.value_unit;
                     finalValue.remainder      = hru.value;
                  }
            }

         return finalValue;
      }

      string humanReadableUnit::toString() const
      {
         string output;

         if (value == 0)
            {
               output = "0 B";
            }
         else 
            {
               output += convertToString<t_ullong>(value);

               switch(value_unit)
                  {
                  case B:
                     output += " B";
                     break;
                  case KiB:
                     output += " KiB";
                     break;
                  case MiB:
                     output += " MiB";
                     break;
                  case GiB:
                     output += " GiB";
                     break;
                  case TiB:
                     output += " TiB";
                     break;
                  case PiB:
                     output += " PiB";
                     break;
                  default:
                     output += " unknown unit";
                     break;
                  }
            } // value > 0

         if (remainder > 0)
            {
               output += ", ";
               output += convertToString<unsigned long>(remainder);

               switch(remainder_unit)
                  {
                  case B:
                     output += " B";
                     break;
                  case KiB:
                     output += " KiB";
                     break;
                  case MiB:
                     output += " MiB";
                     break;
                  case GiB:
                     output += " GiB";
                     break;
                  case TiB:
                     output += " TiB";
                     break;
                  case PiB:
                     output += " PiB";
                     break;
                  default:
                     output += " unknown unit";
                     break;
                  }
            } // remainder > 0

         return output;
      }
   } // namespace core
} // namespace btg

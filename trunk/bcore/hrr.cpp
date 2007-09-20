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
 * $Id: hrr.cpp,v 1.2.4.1 2006/06/04 18:17:14 wojci Exp $
 */

#include "hrr.h"
#include "hru.h"
#include "t_string.h"
#include <iostream>

using namespace std;

namespace btg
{
   namespace core
   {
      humanReadableRate::humanReadableRate(t_ulong const  _value,
                                           EIC_UNIT const _value_unit)
         : value(_value),
           value_unit(_value_unit)
      {
      }

      humanReadableRate::~humanReadableRate()
      {
      }

      humanReadableRate humanReadableRate::convert(t_ulong const _numberOfbytesPerSecond)
      {
         humanReadableUnit hru = humanReadableUnit::convert(_numberOfbytesPerSecond);

         EIC_UNIT u;

         switch(hru.getValueUnit())
            {
            case humanReadableUnit::B:
               u = humanReadableRate::B_PS;
               break;
            case humanReadableUnit::KiB:
               u = humanReadableRate::KiB_PS;
               break;
            case humanReadableUnit::MiB:
               u = humanReadableRate::MiB_PS;
               break;
            case humanReadableUnit::GiB:
               u = humanReadableRate::GiB_PS;
               break;
            case humanReadableUnit::TiB:
               u = humanReadableRate::TiB_PS;
               break;
            case humanReadableUnit::PiB:
               u = humanReadableRate::PiB_PS;
               break;
            default:
               u = humanReadableRate::UNKN;
               break;
            }

         return humanReadableRate(hru.getValue(), u);
      }

      string humanReadableRate::toString(bool const _shrt) const
      {
         string output;

         output += convertToString<t_ulong>(value);

         if (_shrt)
            {
               switch(value_unit)
                  {
                  case humanReadableRate::B_PS:
                     output += " B/s";
                     break;
                  case humanReadableRate::KiB_PS:
                     output += " K/s";
                     break;
                  case humanReadableRate::MiB_PS:
                     output += " M/s";
                     break;
                  case humanReadableRate::GiB_PS:
                     output += " G/s";
                     break;
                  case humanReadableRate::TiB_PS:
                     output += " T/s";
                     break;
                  case humanReadableRate::PiB_PS:
                     output += " P/s";
                     break;
                  default:
                     output += " U/s";
                     break;
                  }
            }
         else
            {
               switch(value_unit)
                  {
                  case humanReadableRate::B_PS:
                     output += " B/sec";
                     break;
                  case humanReadableRate::KiB_PS:
                     output += " KiB/sec";
                     break;
                  case humanReadableRate::MiB_PS:
                     output += " MiB/sec";
                     break;
                  case humanReadableRate::GiB_PS:
                     output += " GiB/sec";
                     break;
                  case humanReadableRate::TiB_PS:
                     output += " TiB/sec";
                     break;
                  case humanReadableRate::PiB_PS:
                     output += " PiB/sec";
                     break;
                  default:
                     output += " UNKN/sec";
                     break;
                  }
            }
         return output;
      }

   } // namespace core
} // namespace btg

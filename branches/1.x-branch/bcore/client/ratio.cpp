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

#include "ratio.h" 
#include <bcore/t_string.h>

extern "C"
{
#include <math.h>
}

namespace btg
{
   namespace core
   {
      namespace client
      {

         void CalculateUlDlRatio(btg::core::Status const& _s, 
                                 std::string & _output)
         {
            t_ulong dl_total = _s.downloadTotal();
            t_ulong ul_total = _s.uploadTotal();

            CalculateUlDlRatio(dl_total, ul_total, _output);
         }

         void CalculateUlDlRatio(t_ulong _dl_total,
                                 t_ulong _ul_total,
                                 std::string & _output)
         {
            t_float ratio = 0;
            if (_dl_total > 0)
               {
                  ratio = (1.0f*_ul_total) / (1.0f*_dl_total);
               }

            // Only show two decimal places.
            t_float rounded = roundf(ratio * 100.0f) / 100.0f;

            _output = btg::core::convertToString<t_float>(rounded);            
         }

      } // client
   } // core
} // btg




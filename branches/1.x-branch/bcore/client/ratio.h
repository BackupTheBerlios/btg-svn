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

#ifndef CLIENT_RATIO_H
#define CLIENT_RATIO_H

#include <bcore/type_btg.h>

namespace btg
{
   namespace core
   {
      namespace client
      {
         /**
          * \addtogroup gencli
          */
         /** @{ */

         /// Given a status object calculate the ul/dl ratio and
         /// update _output.
         void CalculateUlDlRatio(btg::core::Status const& _s, 
                                 std::string & _output);

         /// Given ul/dl totals, calculate the ul/dl ratio and
         /// update _output.
         void CalculateUlDlRatio(t_ulong _dl_total,
                                 t_ulong _ul_total,
                                 std::string & _output);
         /** @} */

      } // client
   } // core
} // btg

#endif // CLIENT_RATIO_H


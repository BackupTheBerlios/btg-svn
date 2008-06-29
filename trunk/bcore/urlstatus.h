/*
 * btg Copyright (C) 2008 Michael Wojciechowski.
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

#ifndef URLSTATUS_H
#define URLSTATUS_H

#include <bcore/command/opstat.h>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup core
          */
         /** @{ */

         bool getDlProgress(btg::core::opStatusResponseCommand const & _cosrc,
                            t_uint & _dltotal, 
                            t_uint & _dlnow, 
                            t_uint & _dlspeed);
         
         /// Set the download progress info.
         void setDlProgress(btg::core::opStatusResponseCommand & _cosrc,
                            t_uint const _dltotal, 
                            t_uint const _dlnow, 
                            t_uint const _dlspeed);

         /** @} */

      } // namespace core
} // namespace btg

#endif // URLSTATUS_H


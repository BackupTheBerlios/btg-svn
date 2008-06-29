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

#include "urlstatus.h"
#include "t_string.h"
#include "util.h"

namespace btg
{
   namespace core
      {

         bool getDlProgress(btg::core::opStatusResponseCommand const & _cosrc,
                            t_uint & _dltotal, 
                            t_uint & _dlnow, 
                            t_uint & _dlspeed)
         {
            std::string input = _cosrc.data();

            if (input.size() == 0)
               {
                  return false;
               }

            t_strList sl = btg::core::Util::splitLine(input, ",");
            
            if (sl.size() < 2)
               {
                  return false;
               }

            _dltotal = btg::core::convertStringTo<t_uint>(sl[0]);
            _dlnow   = btg::core::convertStringTo<t_uint>(sl[1]);
            _dlspeed = btg::core::convertStringTo<t_uint>(sl[2]);

            return true;
         }
         
         void setDlProgress(btg::core::opStatusResponseCommand & _cosrc,
                            t_uint const _dltotal, 
                            t_uint const _dlnow, 
                            t_uint const _dlspeed)
         {
            std::string output = 
               btg::core::convertToString<t_uint>(_dltotal) + "," + 
               btg::core::convertToString<t_uint>(_dlnow) + "," + 
               btg::core::convertToString<t_uint>(_dlspeed);

            _cosrc.setData(output);
         }

      } // namespace core
} // namespace btg




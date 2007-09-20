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
 * $Id: logif.h,v 1.1.2.1 2006/04/05 18:44:10 wojci Exp $
 */

#ifndef LOGIF_H
#define LOGIF_H

#include <bcore/type.h>
#include <string>

namespace btg
{
   namespace core
      {
         namespace logger
            {

               /**
                * \addtogroup logger
                */
               /** @{ */


               /// Classes wishing to get log input implement this interface.
               class logInterface
                  {
                  public:
                     /// Constructor.
                     logInterface() {};
                     /// Log a string.
                     virtual void writeLog(std::string const& _string) = 0;
                     /// Destructor.
                     virtual ~logInterface() {};
                  };

               /** @} */

            } // namespace logger
      } // namespace core
} // namespace btg

#endif // LOGIF_H


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
 * $Id: null_log.h,v 1.6.4.2 2006/04/05 18:44:10 wojci Exp $
 */

#ifndef CONSOLE_LOGGER_H
#define CONSOLE_LOGGER_H

#include "logger.h"

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

               /// Logger which works like /dev/null.
               class nullLogger : public logBuffer
                  {
                  public:
                     /// Constructor.
                     nullLogger();
                     /// Destructor.
                     virtual ~nullLogger();
                  private:
                     /// Throw the buffer out.
                     void put_buffer();
                     /// Throw the char out.
                     void put_char(int _char);
                  };

               /** @} */

            } // namespace logger
      } // namespace daemon
} // namespace btg

#endif

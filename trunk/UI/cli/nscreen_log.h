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

#ifndef NSCREEN_LOGGER_H
#define NSCREEN_LOGGER_H

#include <bcore/logger/logger.h>
#include <bcore/logger/logif.h>
#include "nscreen.h"

namespace btg
{
   namespace UI
      {
         namespace cli
            {
               /// Logger which writes output using my ncurses wrapper class.
               class nscreenLogger : public btg::core::logger::logBuffer
                  {
                  public:
                     /// Constructor.
                     /// @param [in] _target Pointer to where to write the debug data.
                     nscreenLogger(btg::core::logger::logInterface *_target);
                     /// Destructor.
                     virtual ~nscreenLogger();
                  private:
                     /// Write the buffer using the ncurses wrapper.
                     void put_buffer(void);
                     /// Write a char using the ncurses wrapper.
                     void put_char(t_int _char);
                  };

            } // namespace cli
      } // namespace UI
} // namespace btg
#endif

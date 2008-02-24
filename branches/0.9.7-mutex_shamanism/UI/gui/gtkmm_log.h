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

#ifndef GTKMM_LOGGER_H
#define GTKMM_LOGGER_H

#include <bcore/logger/logger.h>

namespace btg
{
   namespace UI
      {
         namespace gui
            {
               /**
                * \addtogroup gui
                */
               /** @{ */

               class logTextview;

               /// Class used for logging to a text view.
               class gtkmmLogger : public btg::core::logger::logBuffer
                  {
                  public:
                     /// Constructor.
                     gtkmmLogger(btg::core::logger::logInterface *_target);

                     /// Destructor.
                     virtual ~gtkmmLogger();
                  private:
                     void put_buffer(void);
                     void put_char(int _char);
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif

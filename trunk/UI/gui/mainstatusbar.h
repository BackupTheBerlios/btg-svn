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

#ifndef MAINSTATUSBAR_H
#define MAINSTATUSBAR_H

#include <gtkmm/statusbar.h>
#include <string>

#include <bcore/type.h>

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

               /// Implements a status bar with one line of text.
               class mainStatusbar: public Gtk::Statusbar
                  {
                  public:
                     /// Constructor.
                     /// @param [in] _timeout Number of calls to
                     /// mainStatusbar::trigger after which the
                     /// statusbar gets cleared.

                     mainStatusbar(t_uint _timeout = 10);

                     /// Set the string this status bar shows to _message.
                     void set(std::string const& _message);

                     /// Call this function to update timeout, so the
                     /// status bar will get cleared once in a while.
                     void trigger();

                     /// Destructor.
                     virtual ~mainStatusbar();
                  private:
                     /// Timeout, after which this statusbar erases its current shown message.
                     t_uint const timeout;
                     /// Counter, used to count invocations of the trigger function.
                     t_uint trigger_counter;
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif

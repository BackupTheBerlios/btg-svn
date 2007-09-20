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
 * $Id: mainstatusbar.cpp,v 1.5.4.1 2007/02/25 21:32:50 wojci Exp $
 */

#include "mainstatusbar.h"

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         using namespace std;

         mainStatusbar::mainStatusbar(t_uint _timeout)
            : timeout(_timeout),
              trigger_counter(0)
         {
            Gtk::Statusbar *ms = this;
            ms->show();

            set("Ready.");
         }

         void mainStatusbar::set(std::string const& _message)
         {
            trigger_counter = 0;

            this->pop();
            this->push(_message);
         }

         void mainStatusbar::trigger()
         {
            trigger_counter++;
            if (trigger_counter >= timeout)
               {
                  trigger_counter = 0;
                  set("Ready.");
               }
         }

         mainStatusbar::~mainStatusbar()
         {
         }

      } // namespace gui
   } // namespace UI
} // namespace btg

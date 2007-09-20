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
 * $Id: topwindow.cpp,v 1.1.2.6 2006/12/31 00:34:00 wojci Exp $
 */

#include "topwindow.h"

namespace btg
{
   namespace UI
   {
      namespace cli
      {
         topWindow::topWindow(keyMapping const& _keymap)
            : baseWindow(_keymap),
              title_(),
              status_()
         {
         }

         void topWindow::resize(windowSize const& _ws)
         {
	   destroy();

	   init(_ws);

	   refresh();
         }

         bool topWindow::destroy()
         {
            if (window_ != 0)
               {
                  delwin(window_);
                  window_ = 0;
               }

            return true;
         }

         void topWindow::refresh()
         {
            if (window_ == 0)
               {
                  return;
               }

            clear();
            draw();

            wrefresh(window_);
         }

         void topWindow::setTitle(std::string const _text)
         {
            title_ = _text;
         }

         void topWindow::setStatus(std::string const _text)
         {
            status_ = _text;
         }

         void topWindow::draw()
         {
	   setColor(Colors::C_NORMAL);

            std::string spaces;

            for (t_uint counter = 0;
                 counter < width_;
                 counter++)
               {
                  spaces += " ";
               }

            ::wattron(window_, A_REVERSE);

            // First fill with spaces.
            mvwprintw(window_, 0, 0, "%s", spaces.c_str());

            // Then write the title.
            mvwprintw(window_, 0, 0, "* %s", title_.c_str());

            // Write the status.
            t_int xpos = width_ - status_.size();
            if (xpos > 0)
               {
                  mvwprintw(window_, 0, xpos, "%s", status_.c_str());
               }

            ::wattroff(window_, A_REVERSE);
	    unSetColor(Colors::C_NORMAL);
         }

         windowSize topWindow::calculateDimenstions(windowSize const& _ws) const
         {
            windowSize ws;

            ws.topX   = 0;
            ws.topY   = 0;
            ws.width  = _ws.width;
            ws.height = 2;

            return ws;
         }

         topWindow::~topWindow()
         {
            destroy();
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

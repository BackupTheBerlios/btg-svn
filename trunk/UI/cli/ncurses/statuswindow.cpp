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

#include "statuswindow.h"

namespace btg
{
   namespace UI
   {
      namespace cli
      {
         statusWindow::statusWindow(keyMapping const& _kmap)
            : baseWindow(_kmap),
              status_(),
              currentColor_(Colors::C_NORMAL)
         {
         }

         void statusWindow::resize(windowSize const& _ws)
         {
            destroy();

            init(_ws);

            refresh();
         }

         bool statusWindow::destroy()
         {
            if (window_ != 0)
               {
                  delwin(window_);
                  window_ = 0;
               }

            return true;
         }

         void statusWindow::refresh()
         {
            if (window_ == 0)
               {
                  return;
               }

            draw();

            wrefresh(window_);
         }

         void statusWindow::setStatus(std::string const _text)
         {
            currentColor_ = Colors::C_NORMAL;
            clear();
            status_ = _text;
            refresh();
         }

         void statusWindow::setError(std::string const _text)
         {
            currentColor_ = Colors::C_ERROR;
            clear();
            status_ = _text;
            refresh();
         }

         void statusWindow::draw()
         {
            setColor(currentColor_);
            std::string spaces;

            // t_int spacecount = width_ - status_.size();

            for (t_uint counter = 0; counter < width_; counter++)
               {
                  spaces += " ";
               }

            ::wattron(window_, A_REVERSE);

            // First fill with spaces.
            mvwprintw(window_, 0, 0, "%s", spaces.c_str());

            if (status_.size() > width_)
               {
                  status_ = status_.substr(0, width_);
               }

            mvwprintw(window_, 0, 0, "%s", status_.c_str());
            ::wattroff(window_, A_REVERSE);
            unSetColor(currentColor_);
         }

         windowSize statusWindow::calculateDimenstions(windowSize const& _ws) const
         {
            windowSize ws;
            ws.topX   = 0;
            ws.topY   = _ws.height - 1;
            ws.width  = _ws.width;
            ws.height = 1;

            return ws;
         }

         statusWindow::~statusWindow()
         {
            destroy();
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

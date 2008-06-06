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


#include <bcore/project.h>

#include "progress.h"

namespace btg
{
   namespace UI
   {
      namespace cli
      {

         progressWindow::progressWindow(windowSize const& _ws,
                                        keyMapping const& _kmap)
            : baseWindow(_kmap),
              size_(_ws),
              middle_x(0),
              middle_y(0)
         {
            init(size_);
            middle_x = _ws.width / 2;
            middle_y = _ws.height / 2;

            refresh();
         }

         void progressWindow::setText(std::string const& _text)
         {
            t_int loc = middle_x - (_text.size() / 2);

            mvwprintw(window_, middle_y, loc, "%s", _text.c_str());
         }

         void progressWindow::setPercent(t_uint const _percent)
         {
            std::string perc_str;

            t_int width  = (width_ - 2);
            t_int size_x = _percent * width / 100;

            // Add percent.
            for (t_int counter = 0;
                 counter < size_x;
                 counter++)
               {
                  perc_str += "#";
               }

            // Add spaces.
            t_int spaces = width - size_x;
            for (t_int counter = 0;
                 counter < spaces;
                 counter++)
               {
                  perc_str += ".";
               }

            mvwprintw(window_, middle_y + 1, 0, "[%s]", perc_str.c_str());

         }

         void progressWindow::updateProgress(t_uint _percent, 
                                             std::string const _text)
         {
            clear();
            setPercent(_percent);
            setText(_text);
            refresh();
         }

         void progressWindow::resize(windowSize const& _ws)
         {

         }
	
         windowSize progressWindow::calculateDimenstions(windowSize const& _ws) const
         {
            // Use the screen size given.
            return _ws;
         }
	
         void progressWindow::refresh()
         {
            if (window_ == 0)
               {
                  return;
               }

            wrefresh(window_);
         }

         progressWindow::~progressWindow()
         {
         }

         /** @} */

      } // namespace cli
   } // namespace UI
} // namespace btg

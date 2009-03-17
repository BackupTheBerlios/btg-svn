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


#include <bcore/project.h>

#include "initwindow.h"

namespace btg
{
   namespace UI
   {
      namespace cli
      {

         initWindow::initWindow(keyMapping const& _kmap)
            : baseWindow(_kmap),
              hidden(true),
              middle_x(0),
              middle_y(0)
         {
         }

         void initWindow::show()
         {
            baseWindow::ncursesInit();

            windowSize ws;
            ws.topX = 0;
            ws.topY = 0;

            baseWindow::getScreenSize(ws.width, ws.height);

            middle_x = ws.width / 2;
            middle_y = ws.height / 2;

            init(ws);
            
            hidden = false;
         }

         void initWindow::hide()
         {
            baseWindow::ncursesDestroy();
            hidden = true;
         }

         void initWindow::setText(std::string const& _text)
         {
            t_int loc = middle_x - (_text.size() / 2);

            mvwprintw(window_, middle_y, loc, "%s", _text.c_str());
         }

         void initWindow::setPercent(t_uint const _percent)
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

         void initWindow::updateProgress(INITEVENT _event)
         {
            clear();

            switch(_event)
               {
               case IEV_START:
                  {
                     setPercent(0);
                     setText("Started initialization");
                     refresh();
                     break;
                  }
               case IEV_RCONF:
                  {
                     setPercent(15);
                     setText("Reading config");
                     refresh();
                     break;
                  }
               case IEV_RCONF_KEYS:
                  {
                     setPercent(20);
                     setText("Reading config");
                     refresh();
                     break;
                  }
               case IEV_RCONF_DONE:
                  {
                     setPercent(30);
                     setText("Config read");
                     refresh();
                     break;
                  }
               case IEV_CLA:
                  {
                     setPercent(45);
                     setText("Parsing arguments");
                     refresh();
                     break;
                  }
               case IEV_CLA_DONE:
                  {
                     setPercent(50);
                     setText("Arguments parsed");
                     refresh();
                     break;
                  }
               case IEV_TRANSP:
                  {
                     setPercent(65);
                     setText("Creating transport");
                     refresh();
                     break;
                  }
               case IEV_TRANSP_DONE:
                  {
                     setPercent(75);
                     setText("Transport created");
                     refresh();
                     break;
                  }
               case IEV_SETUP:
                  {
                     setPercent(85);
                     setText("Registering");
                     refresh();
                     break;
                  }
               case IEV_SETUP_DONE:
                  {
                     setPercent(95);
                     setText("Registered");
                     refresh();
                     break;
                  }
               case IEV_END:
                  {
                     setPercent(100);
                     setText("Initialization done");
                     refresh();
                     break;
                  }
               }
         }

         void initWindow::showError(std::string const& _error)
         {
            clear();

            std::string error = "Error: " + _error;

            t_int loc = middle_x - (error.size() / 2);
	  
            mvwprintw(window_, middle_y, loc, "%s", error.c_str());

            refresh();

            readAnyKey();
         }

         void initWindow::resize(windowSize const& _ws)
         {

         }
	
         windowSize initWindow::calculateDimenstions(windowSize const& _ws) const
         {
            // Use the screen size given.
            return _ws;
         }
	
         void initWindow::refresh()
         {
            if (window_ == 0)
               {
                  return;
               }

            wrefresh(window_);
         }

         initWindow::~initWindow()
         {
            if (!hidden)
               {
                  hide();
               }

            if (window_ == 0)
               {
                  return;
               }

            destroy();
            baseWindow::ncursesDestroy();
         }

         /** @} */

      } // namespace cli
   } // namespace UI
} // namespace btg

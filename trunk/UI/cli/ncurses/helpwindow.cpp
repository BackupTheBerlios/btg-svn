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
 * $Id: helpwindow.cpp,v 1.1.2.8 2006/12/31 00:34:00 wojci Exp $
 */

#include "helpwindow.h"

namespace btg
{
   namespace UI
   {
      namespace cli
      {
         helpWindow::helpWindow(keyMapping const& _kmap,
                                std::vector<std::string> const& _text)
            : baseWindow(_kmap),
              text_(_text)
         {
         }

         void helpWindow::resize(windowSize const& _ws)
         {

         }

         bool helpWindow::destroy()
         {
            if (window_ != 0)
               {
                  delwin(window_);
                  window_ = 0;
               }

            return true;
         }

         void helpWindow::refresh()
         {
            if (window_ == 0)
               {
                  return;
               }

            draw();

            wrefresh(window_);
         }

	dialog::RESULT helpWindow::run()
	{
	  windowSize display_ws;
	  display_ws.topX = 0;
	  display_ws.topY = 0;
	  
            baseWindow::getScreenSize(display_ws.width, display_ws.height);
	    
            windowSize ws = calculateDimenstions(display_ws);
	    
            if (!baseWindow::init(ws))
	      {
		return dialog::R_NCREAT;
	      }

            refresh();

            // Handle keyboard.
            bool cont = true;

            while(cont)
               {
                  switch (handleKeyboard())
                     {
                     case keyMapping::K_UNDEF:
                        refresh();
                        break;
                     case keyMapping::K_RESIZE:
                        return dialog::R_RESIZE;
                        break;
                     default:
                        cont = false;
                        break;
                     }
               }

            return dialog::R_QUIT;
         }

         void helpWindow::findMaxLineLength(t_uint & _width, t_uint & _height) const
         {
            _width  = 0;
            _height = 0;

            // Width.
            std::vector<std::string>::const_iterator iter;

            for (iter = text_.begin();
                 iter != text_.end();
                 iter++)
               {
                  if (iter->size() > _width)
                     {
                        _width = iter->size();
                     }
               }

            // Height.
            _height = text_.size()+1; // Add some extra space.
         }

         void helpWindow::draw()
         {
	   setColor(Colors::C_NORMAL);

            ::wattron(window_, A_REVERSE);
            for (t_uint y = 0; y < height_; y++)
               {
                  for (t_uint x = 0; x < width_; x++)
                     {
                        mvwaddch(window_, y, x, ' ');
                     }
               }

            t_uint counter = 0;
            std::vector<std::string>::const_iterator iter;

            for (iter = text_.begin();
                 iter != text_.end();
                 iter++)
               {
                  mvwprintw(window_, counter, 0, "%s", iter->c_str());
                  counter++;
               }

            ::wattroff(window_, A_REVERSE);
	    unSetColor(Colors::C_NORMAL);
         }

         windowSize helpWindow::calculateDimenstions(windowSize const& _ws) const
         {
            windowSize ws;

            t_uint text_width = 0;
            t_uint text_height = 0;

            findMaxLineLength(text_width, text_height);

            ws.topX = (_ws.width/2) - (text_width/2);
            ws.topY = (_ws.height/2) - (text_height/2);

            ws.width = text_width;
            ws.height = text_height;

            return ws;
         }

         bool helpWindow::generateHelpForKey(keyMapping const& _kmap,
                                             keyMapping::KEYLABEL const _label,
                                             std::string const& _explanation,
                                             std::string & _output,
                                             bool _short)
         {
            std::string value;
            if (!_kmap.getValue(_label, value))
               {
                  // Unable to find a representation of this key.
                  return false;
               }

            if (_short)
               {
                  _output = "Press /" + value + "/ " + _explanation + ".";
               }
            else 
               {
                  _output = "/" + value + "/ " + _explanation;
               }

            return true;
         }

         helpWindow::~helpWindow()
         {
            destroy();
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

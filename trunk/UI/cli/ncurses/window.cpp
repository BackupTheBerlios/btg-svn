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

#include "window.h"

#include <bcore/os/sleep.h>

namespace btg
{
   namespace UI
   {
      namespace cli
      {

         baseWindow::baseWindow(keyMapping const& _kmap)
            : window_(0),
              topX_(0),
              topY_(0),
              width_(0),
              height_(0),
              kmap_(_kmap)
         {

         }

         bool baseWindow::init(windowSize const& _ws)
         {
            window_ = newwin(_ws.height, _ws.width, _ws.topY, _ws.topX);

            if (window_ == 0)
               {
                  return false;
               }

            keypad(window_, TRUE);
            scrollok(window_, TRUE);
            // Non-blocking input.
            nodelay(window_, TRUE);

            wtimeout(window_, 5); 

            // Hide cursor.
            curs_set(0);

            wrefresh(window_);

            height_ = _ws.height;
            width_  = _ws.width;
            topY_   = _ws.topY;
            topX_   = _ws.topX;

            return true;
         }

         bool baseWindow::destroy()
         {
            if (window_ == 0)
               {
                  return false;
               }

            delwin(window_);
            window_ = 0;

            return true;
         }

         void baseWindow::getScreenSize(t_uint & _width,
                                        t_uint & _height)
         {
            _height = LINES;
            _width  = COLS;
         }

         void baseWindow::ncursesInit()
         {
            // Init ncurses:
            ::initscr();
	  
            ::cbreak();
            ::noecho();
         }
	
         void baseWindow::ncursesDestroy()
         {
            ::endwin();
         }

         void baseWindow::getSize(windowSize & _ws)
         {
            _ws.topX   = topX_;
            _ws.topY   = topY_;
            _ws.width  = width_;
            _ws.height = height_;
         }

         void baseWindow::clear()
         {
            werase(window_);
         }

         t_int baseWindow::readAnyKey()
         {
            bool cont  = true;
            t_int key = 0;

            while(cont)
               {
                  key = wgetch(window_);

                  if (key == ERR)
                     {
                        btg::core::os::Sleep::sleepMiliSeconds(10);
                     }
                  else
                     {
                        cont = false;
                     }
                  refresh();
               }
            return key;
         }

         keyMapping::KEYLABEL baseWindow::handleKeyboard()
         {
            int ch = wgetch(window_);

            keyMapping::KEYLABEL keylabel = keyMapping::K_UNDEF;
            kmap_.get(ch, keylabel);

            if (keylabel == keyMapping::K_UNDEF)
               {
                  // No keypress, so sleep a bit.
                  btg::core::os::Sleep::sleepMiliSeconds(10);
               }

            return keylabel;
         }

         bool baseWindow::handleKeyboard(t_int const _key, 
                                         keyMapping::KEYLABEL & _label) const
         {
            keyMapping::KEYLABEL keylabel = keyMapping::K_UNDEF;
            kmap_.get(_key, keylabel);

            if (keylabel != keyMapping::K_UNDEF)
               {
                  _label = keylabel;
                  return true;
               }

            return false;
         }

         void baseWindow::setColor(Colors::COLORLABEL _label)
         {
            ::wattron(window_, COLOR_PAIR(_label));
         }

         void baseWindow::unSetColor(Colors::COLORLABEL _label)
         {
            ::wattroff(window_, COLOR_PAIR(_label));
         }

         void baseWindow::genSpaces(std::string const& _input, std::string & _spaces) const
         {
            t_int spaces = width_ - _input.size() - 1 /* extra space is inserted at
                                                       * the beginning of the line.
                                                       */;
            if (spaces > 0)
               {
                  for (t_int spacecounter = 0;
                       spacecounter < spaces;
                       spacecounter++)
                     {
                        _spaces += " ";
                     }
               }
         }
	
         baseWindow::~baseWindow()
         {

         }

         /* */
         /* */
         /* */

         dialog::dialog()
         {

         }
	
         dialog::~dialog()
         {
	  
         }

      } // namespace cli
   } // namespace UI
} // namespace btg


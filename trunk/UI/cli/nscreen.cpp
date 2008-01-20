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

#include "nscreen.h"

#include <bcore/util.h>
#include <bcore/os/sleep.h>
#include <bcore/logmacro.h>

#include <iostream>

#include "runstate.h"

extern t_int global_btg_run;

namespace btg
{
   namespace UI
   {
      namespace cli
      {

         using namespace btg::core;

         ncursesScreen::ncursesScreen(bool const _pretend)
            : pretend(_pretend),
              insert_mode(ncursesScreen::OVERWRITE_MODE),
              height(0),
              main_window(0),
              input_init_str("# "),
              input_min(2),
              input_max(0),
              input(),
              last_input(),
              input_beg_y(0),
              input_beg_x(0)
         {
            if (!pretend)
               {
                  initscr();
                  cbreak();
                  noecho();

                  // Create a window.
                  main_window = newwin(LINES, COLS, 0, 0);
                  // Set properties.
                  height      = LINES-1;
                  input_max   = COLS-1;

                  keypad(main_window, TRUE);
                  scrollok(main_window, TRUE);
                  // Non-blocking input.
                  nodelay (main_window, 0);
                  wtimeout(main_window, 5);

                  refresh();
               }
         }

         void ncursesScreen::moveLeft()
         {
            if (pretend)
               {
                  return;
               }

            t_int mx, my;

            getyx(main_window, my, mx);
            mx -= 1;

            if (mx < input_min)
               {
                  mx  = input_max;
                  my -= 1;
               }

            // Make sure that one cannot move
            // beyond the beg. of the command
            // string.
            if ((my >= input_beg_y) &&
                (mx >= input_beg_x))
               {
                  wmove(main_window, my, mx);
               }
         }

         void ncursesScreen::moveRight()
         {
            if (pretend)
               {
                  return;
               }

            t_int mx, my;

            getyx(main_window, my, mx);
            mx += 1;

            if (mx > input_max)
               {
                  mx  = input_min;
                  my += 1;
               }
            wmove(main_window, my, mx);
         }

         ncursesScreen::EVENT ncursesScreen::getKeys()
         {
            if (pretend)
               {
                  return ncursesScreen::EVENT_NO_KEY;
               }

            t_int ch;
            ncursesScreen::EVENT done = ncursesScreen::EVENT_NO_KEY;
            char *s                   = new char[2];
            t_int mx, my;

            this->input.clear();

            getyx(main_window, input_beg_y, input_beg_x);

            while (done == ncursesScreen::EVENT_NO_KEY)
               {
                  ch = wgetch(main_window);
                  if (ch != ERR)
                     {
                        switch(ch)
                           {
                           case KEY_RESIZE:
                              {
                                 //BTG_NOTICE("Resize event received (1).");
                                 return ncursesScreen::EVENT_RESIZE;
                              }
                           case KEY_LEFT:
                              {
                                 this->moveLeft();
                                 this->refresh();
                                 break;
                              }
                           case KEY_RIGHT:
                              {
                                 this->moveRight();
                                 this->refresh();
                                 break;
                              }
                           case KEY_NPAGE:
                           case KEY_UP:
                              {
                                 break;
                              }
                           case KEY_PPAGE:
                           case KEY_DOWN:
                              {
                                 break;
                              }
                           case KEY_IC:
                              {
                                 if (getInsertMode() == ncursesScreen::INSERT_MODE)
                                    {
                                       this->setInsertMode(ncursesScreen::OVERWRITE_MODE);
                                       break;
                                    }

                                 if (getInsertMode() == ncursesScreen::OVERWRITE_MODE)
                                    {
                                       this->setInsertMode(ncursesScreen::INSERT_MODE);
                                       break;
                                    }

                                 break;
                              }
                           case KEY_DC:
                              {
                                 this->moveLeft();
                                 wdelch(main_window);
                                 break;
                              }
                           case KEY_HOME:
                              {
                                 getyx(main_window, my, mx);
                                 mx  = input_min;
                                 wmove(main_window, my, mx);
                                 break;
                              }
                           case KEY_END:
                              {
                                 getyx(main_window, my, mx);
                                 mx  = input_max;
                                 wmove(main_window, my, mx);
                                 break;
                              }
                           case KEY_BACKSPACE:
                              {
                                 this->moveLeft();
                                 wdelch(main_window);

                                 break;
                              }
                           case 10:
                           case KEY_ENTER:
                              {
                                 bool status = getInputWindowText(this->last_input, true /* strip whitespace */);

                                 // Move one line down.
                                 // Set to zero so the response to the
                                 // command typed starts at the
                                 // beg. of the line.

                                 wprintw(main_window, "%s", "\n");
                                 this->refresh();

                                 if (status == true)
                                    {
                                       done = ncursesScreen::EVENT_KEY;
                                    }
                                 else
                                    {
                                       done = ncursesScreen::EVENT_NO_KEY;
                                       return done;
                                    }

                                 break;
                              }
                           default:
                              s[0] = static_cast<char>(ch);
                              s[1] = '\0';

                              // Count lines.
                              getyx(main_window, my, mx);

                              if ((mx+1) >= input_max)
                                 {
                                    if (my >= this->height)
                                       {
                                          wprintw(main_window, "%s", "\n");
                                       }
                                    else
                                       {
                                          // Move one line down.
                                          wmove(main_window, ++my, input_min);
                                       }
                                 }

                              // Check: the x position is 0 after scrolling.

                              t_int sy, sx;
                              getyx(main_window, sy, sx);
                              if (sx == 0)
                                 {
                                    wmove(main_window, my, input_min);
                                    input_beg_y -= 1;
                                 }

                              if (getInsertMode() == ncursesScreen::INSERT_MODE)
                                 {
                                    winsnstr(main_window, s, 1);
                                    // Move the cursor.
                                    getyx(main_window, my, mx);
                                    if ((mx+1) <= input_max)
                                       {
                                          wmove(main_window, my, ++mx);
                                       }
                                 }
                              else
                                 {
                                    wprintw(main_window, "%s", s);
                                 }

                              this->refresh();
                              break;
                           }
                     } // if
                  else
                     {
                        // Got ERR, so sleep a bit.
                        btg::core::os::Sleep::sleepNanoSeconds(100);
                     }
               } // while

            delete [] s;
            return ncursesScreen::EVENT_KEY;
         }

         bool ncursesScreen::inputValid(t_int const _c)
         {
            bool status = false;

            if (std::isalpha(_c) || std::isalnum(_c) || std::isgraph(_c))
               {
                  status = true;
               }

            return status;
         }

         bool ncursesScreen::getInputWindowText(std::string & _text, bool const _strip)
         {
            bool status = false;

            if (pretend)
               {
                  return status;
               }

            t_int sav_mx  = 0;
            t_int sav_my  = 0;

            // Save current position.
            getyx(main_window, sav_my, sav_mx);

            // Move to saved begin position.
            wmove(main_window, input_beg_y, input_beg_x);

            // Find out if the command is spread on multiple lines.
            bool multipleLines = false;
            if ((sav_my - input_beg_y) > 0)
               {
                  multipleLines = true;
               }

            std::string tempInput;

            t_int culm_size = 0;
            t_int x         = 0;
            char win_str[2];

            // Copy the contents of the command window.
            if (multipleLines == false)
               {
                  for (x = this->input_min; x < this->input_max-1; x++)
                     {
                        if (mvwinnstr(main_window, input_beg_y, x, win_str, 1) != ERR)
                           {
                              tempInput.push_back(win_str[0]);
                           }
                        // wprintw(main_window, "%c", 'x');
                        culm_size++;
                     }
               }
            else
               {
                  // Multiple lines, read each line.
                  t_int y = 0;

                  for (y = input_beg_y; y <= sav_my; y++)
                     {
                        for (x = this->input_min; x < this->input_max-1; x++)
                           {
                              if (mvwinnstr(main_window, y, x, win_str, 1) != ERR)
                                 {
                                    tempInput.push_back(win_str[0]);
                                 }
                              // wprintw(main_window, "%c", 'x');
                              culm_size++;
                           }
                     }
               }

            // Get the input.
            // Attempt to remove whitespace at the end of the string.

            if (culm_size > 0)
               {
                  _text.clear();

                  // Find out where the command ends.
                  if (_strip == true)
                     {
                        // Strip spaces from the end of string.

                        std::string::iterator si;
                        std::string::iterator si_last;

                        // Used to determine if any characters were found.
                        bool found_alpha = false;

                        for (si = tempInput.end();
                             si >= tempInput.begin();
                             si--)
                           {
                              if (this->inputValid(*si))
                                 {
                                    si_last = si;
                                    si_last++;
                                    found_alpha = true;
                                    break;
                                 }
                           }

                        if (found_alpha == false)
                           {
                              // BTG_NOTICE("Input is an empty string..");

                              // Move cursor to its current position.
                              wmove(main_window, sav_my, sav_mx);
                              return status;
                           }

                        // Remove trailing whitespace.
                        if ( si_last > tempInput.begin() && si_last < tempInput.end() )
                           {
                              tempInput.erase(si_last, tempInput.end());
                           }

                        // Copy the string.
                        for (si = tempInput.begin();
                             si != tempInput.end();
                             si++)
                           {
                              // Remove newlines etc.
                              if (this->inputValid(*si))
                                 {
                                    _text.push_back(*si);
                                 }
                           }
                     }
                  {
                     // No stripping.
                     _text = tempInput;
                  }

                  status = true;

                  // BTG_NOTICE("Got " << culm_size << " chars.");
               }

            // Move cursor to its current position.
            wmove(main_window, sav_my, sav_mx);

            return status;
         }

         void ncursesScreen::refresh()
         {
            if (pretend)
               {
                  return;
               }

            wrefresh(main_window);
         }

         void ncursesScreen::toUser(std::string const& _s)
         {
            if (pretend)
               {
                  return;
               }

            wprintw(main_window, "%s%c", _s.c_str(), GPD->cNEWLINE());
         }

         void ncursesScreen::setInput(std::string const& _s)
         {
            if (pretend)
               {
                  return;
               }

            t_int mx = 0;
            t_int my = 0;

            // Move to the beg. of the window.
            getyx(main_window, my, mx);
            mx = 0;
            wmove(main_window, my, mx);

            std::string::const_iterator iter = _s.begin();
            char c;
            while (iter != _s.end())
               {
                  c = *iter;
                  wprintw(main_window, "%c", c);
                  iter++;

                  getyx(main_window, my, mx);
                  if (mx >= input_max)
                     {
                        my++;
                        mx = input_min;
                        wmove(main_window, my, mx);
                     }
               }
         }

         std::string ncursesScreen::getInput()
         {
            return this->last_input;
         }

         void ncursesScreen::setOutput(std::string const& _s)
         {
            toUser(_s);
            refresh();
         }

         void ncursesScreen::writeLog(std::string const& _string)
         {
            setOutput(_string);
         }

         void ncursesScreen::setInsertMode(IMODE const _mode)
         {
            if (pretend)
               {
                  return;
               }

            this->insert_mode = _mode;
         }

         ncursesScreen::IMODE ncursesScreen::getInsertMode() const
         {
            return this->insert_mode;
         }

         void ncursesScreen::resize()
         {
            if (pretend)
               {
                  return;
               }

            t_int lines   = -1;
            t_int columns = -1;

            if (is_term_resized(lines, columns) == true)
               {
                  resizeterm(lines, columns);
               }

            this->height    = LINES-1;
            this->input_max = COLS-1;
         }

         void ncursesScreen::clear()
         {
            wclear(main_window);
         }

         bool ncursesScreen::isUserSure()
         {
            bool status = true;

            if (pretend)
               {
                  // When pretending to display, there is no need to
                  // ask the user if he is sure.
                  return status;
               }

            setOutput("Are you sure? (y/n)");

            bool gotExpectedResponse = false;

            while (!gotExpectedResponse)
               {
                  char key = static_cast<char>(wgetch(main_window));
                  if (key == 'n' || key == 'N')
                     {
                        status              = false;
                        gotExpectedResponse = true;
                     }
                  else if (key == 'y' || key == 'Y')
                     {
                        status              = true;
                        gotExpectedResponse = true;
                     }
               }

            return status;
         }

         ncursesScreen::~ncursesScreen()
         {
            if (!pretend)
               {
                  delwin(main_window);
                  endwin();
               }
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

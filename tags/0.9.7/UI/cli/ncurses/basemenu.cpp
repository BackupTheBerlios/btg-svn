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

#include "basemenu.h"

namespace btg
{
   namespace UI
   {
      namespace cli
      {

         menuEntry::menuEntry(t_uint _ID,
                              std::string _label,
                              std::string _descr)
            : ID(_ID),
              label(_label),
              descr(_descr)
         {
         }

         /* */
         /* */
         /* */

         baseMenu::baseMenu(keyMapping const& _kmap,
                            windowSize const& _ws,
                            std::string const& _title,
                            std::vector<menuEntry> const& _contents,
                            statusWindow & _statusWindow)
            : baseWindow(_kmap),
              size_(_ws),
              title_(_title),
              contents_(_contents),
              statusWindow_(_statusWindow),
              selection_(0),
              selected_(false)
         {
            // Keys used by this window.
            std::vector<keyMapping::KEYLABEL> labels;
            labels.push_back(keyMapping::K_QUIT);
            labels.push_back(keyMapping::K_MENU);
            labels.push_back(keyMapping::K_DOWN);
            labels.push_back(keyMapping::K_UP);
            labels.push_back(keyMapping::K_LIST_START);
            labels.push_back(keyMapping::K_LIST_END);
            labels.push_back(keyMapping::K_SELECT);
            labels.push_back(keyMapping::K_RESIZE);
            labels.push_back(keyMapping::K_HELP);

            kmap_.setUsed(labels);
         }

         dialog::RESULT baseMenu::run()
         {
            windowSize ws = calculateDimenstions(size_);
            if (!baseWindow::init(ws))
               {
                  return dialog::R_NCREAT;
               }

            refresh();

            // Handle keyboard.
            bool quit = false;
            while(!quit)
               {
                  keyMapping::KEYLABEL kl = handleKeyboard();
                  switch (kl)
                     {
                     case keyMapping::K_MENU:
                     case keyMapping::K_QUIT:
                        {
                           quit = true;
                           break;
                        }
                     case keyMapping::K_UP:
                        {
                           selection_--;
                           if (selection_ < 0)
                              {
                                 selection_ = 0;
                              }

                           refresh();
                           break;
                        }
                     case keyMapping::K_DOWN:
                        {
                           selection_++;

                           const t_int contents_size = contents_.size() - 1;
                           if (selection_ > contents_size)
                              {
                                 selection_ = contents_size;
                              }

                           refresh();

                           break;
                        }
                     case keyMapping::K_LIST_START:
                        {
                           selection_ = 0;
                           refresh();
                           break;
                        }
                     case keyMapping::K_LIST_END:
                        {
                           const t_int contents_size = contents_.size() - 1;
                           selection_ = contents_size;
                           refresh();
                           break;
                        }
                     case keyMapping::K_SELECT:
                        {
                           selected_ = true;
                           quit      = true;
                           break;
                        }
                     case keyMapping::K_RESIZE:
                        {
                           return dialog::R_RESIZE;
                           break;
                        }
                     default:
                        {
                           refresh();
                           selected_ = false;
                           break;
                        }
                     }
               }

            return dialog::R_QUIT;
         }

         void baseMenu::draw()
         {
            setColor(Colors::C_NORMAL);
            // Write a title and a separator.

            t_uint ypos    = 1;

            mvwprintw(window_, ypos, 1, "%s", title_.c_str());
            ypos++;

            mvwprintw(window_, ypos, 1, "%s", "--");
            ypos++;

            std::vector<menuEntry>::const_iterator iter;

            t_int counter = 0;

            for (iter = contents_.begin();
                 iter != contents_.end();
                 iter++)
               {

                  if (selection_ == counter)
                     {
                        unSetColor(Colors::C_NORMAL);
                        setColor(Colors::C_MARK);
                        ::wattron(window_, A_REVERSE);

                        std::string spacestr;
                        t_int spaces = width_ - iter->label.size() - 2; // box takes two characters.

                        if (spaces > 0)
                           {
                              for (t_int spacecounter = 0;
                                   spacecounter < spaces;
                                   spacecounter++)
                                 {
                                    spacestr += " ";
                                 }
                           }

                        mvwprintw(window_, ypos, 1, "%s%s", iter->label.c_str(), spacestr.c_str());
                        unSetColor(Colors::C_MARK);
                        setColor(Colors::C_NORMAL);
                        ::wattroff(window_, A_REVERSE);

                        statusWindow_.setStatus(iter->descr);
                     }
                  else
                     {
                        mvwprintw(window_, ypos, 1, "%s", iter->label.c_str());
                     }

                  counter++;
                  ypos++;
               }
            unSetColor(Colors::C_NORMAL);
         }

         void baseMenu::resize(windowSize const& _ws)
         {
            // This window will not be resized.
         }

         t_int baseMenu::getResult() const
         {
            if (!selected_)
               {
                  return baseMenu::BM_cancel;
               }

            bool found     = false;
            t_int counter = 0;
            // Find out what was selected.
            std::vector<menuEntry>::const_iterator iter;
            for (iter = contents_.begin();
                 iter != contents_.end();
                 iter++)
               {
                  if (counter == selection_)
                     {
                        found = true;
                        break;
                     }
                  counter++;
               }

            if (found)
               {
                  // Found a corresponding ID.
                  return iter->ID;
               }

            // Unable to find the id.
            return baseMenu::BM_cancel;
         }

         void baseMenu::refresh()
         {
            if (window_ == 0)
               {
                  return;
               }

            clear();
            setColor(Colors::C_BORDER);
            box(window_, 0 , 0);
            unSetColor(Colors::C_BORDER);
            draw();
            wrefresh(window_);
         }

         windowSize baseMenu::calculateDimenstions(windowSize const& _ws) const
         {
            windowSize ws;

            ws.topX = 1;
            ws.topY = 5;

            t_uint max_width = 0;
            std::vector<menuEntry>::const_iterator iter;
            for (iter = contents_.begin();
                 iter != contents_.end();
                 iter++)
               {
                  if (iter->label.size() > max_width)
                     {
                        max_width = iter->label.size();
                     }
               }

            if (max_width > title_.size())
               {
                  ws.width = max_width;
               }
            else
               {
                  ws.width  = title_.size();
               }

            ws.width += 2; // Add extra space for the box sorrounding the window.
            ws.height = contents_.size() + 4; // Add extra space for the title and the surronding box.

            return ws;
         }

         baseMenu::~baseMenu()
         {
            destroy();
         }

      } // namespace cli
   } // namespace UI
} // namespace btg


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

#include "sessionselect.h"

#include <algorithm>

#include "helpwindow.h"

#include <bcore/util.h>
#include <bcore/t_string.h>

#include <bcore/command/command.h>

namespace btg
{
   namespace UI
   {
      namespace cli
      {
         const t_uint linesPerSession = 2;

         sessionSelect::sessionSelect(keyMapping const& _kmap, 
                                   windowSize const& _ws,
                                   t_longList const& _sessions,
                                   t_strList  const& _names,
                                   t_long const _current_session)
            : baseWindow(_kmap),
              size_(_ws),
              numberOfLines_(0),
              positionWindowStart_(0),
              positionWindowEnd_(0),
              currentPosition_(0),
              currentId_(0),
              pressed_select_key_(false),
              selected_session_(btg::core::Command::INVALID_SESSION)
         {
            // Keys used by this window.
            std::vector<keyMapping::KEYLABEL> labels;
            labels.push_back(keyMapping::K_HELP);
            labels.push_back(keyMapping::K_QUIT);
            labels.push_back(keyMapping::K_DOWN);
            labels.push_back(keyMapping::K_UP);
            labels.push_back(keyMapping::K_LIST_START);
            labels.push_back(keyMapping::K_LIST_END);
            labels.push_back(keyMapping::K_SELECT);
            labels.push_back(keyMapping::K_RESIZE);
            kmap_.setUsed(labels);

            t_longListCI sessionIter = _sessions.begin();
            t_strListCI nameIter     = _names.begin();

            for (sessionIter = _sessions.begin();
                 sessionIter != _sessions.end();
                 sessionIter++)
               {
                  sessionIdAndName sian;
                  sian.id   = *sessionIter;
                  sian.name = *nameIter;
                  if (sian.id != _current_session)
                     {
                        sessions_.push_back(sian);
                     }
                  nameIter++;
               }
         }

         dialog::RESULT sessionSelect::run()
         {
            if (!init(size_))
               {
                  return dialog::R_NCREAT;
               }

            refresh();

            // Handle keyboard.

            keyMapping::KEYLABEL label;

            bool cont    = true;

            while (cont)
               {
                  label = handleKeyboard();

                  switch (label)
                     {
                     case keyMapping::K_HELP:
                        {
                           if (showHelp() == dialog::R_RESIZE)
                              {
                                 // Resized window.
                                 return dialog::R_RESIZE;
                              }
                           break;
                        }
                     case keyMapping::K_QUIT:
                        {
                           cont = false;
                           break;
                        }
                     case keyMapping::K_DOWN:
                        {
                           moveDown();
                           refresh();
                           break;
                        }
                     case keyMapping::K_UP:
                        {
                           moveUp();
                           refresh();
                           break;
                        }
                     case keyMapping::K_LIST_START:
                        {
                           toStart();
                           refresh();
                           break;
                        }
                     case keyMapping::K_LIST_END:
                        {
                           toEnd();
                           refresh();
                           break;
                        }
                     case keyMapping::K_SELECT:
                        {
                           handleSelect();
                           cont = false;
                           break;
                        }
                     case keyMapping::K_RESIZE:
                        {
                           // Close this window, and make the parent
                           // resize its window.
                           return dialog::R_RESIZE;
                           break;
                        }
                     default:
                        {
                           refresh();
                           break;
                        }
                     }
               }

            return dialog::R_QUIT;
         }

         bool sessionSelect::init(windowSize const& _ws)
         {
            bool status = baseWindow::init(_ws);

            if (!status)
               {
                  return status;
               }

            numberOfLines_  = (_ws.height-1) / linesPerSession;

            if (numberOfLines_ < 1)
               {
                  // Not enough space to show even one file.
                  // Abort.
                  return false;
               }

            return status;
         }

         void sessionSelect::resize(windowSize const& _ws)
         {

         }

         bool sessionSelect::destroy()
         {
            if (window_ != 0)
               {
                  delwin(window_);
                  window_ = 0;
               }

            return true;
         }

         void sessionSelect::refresh()
         {
            if (window_ == 0)
               {
                  return;
               }

            clear();
            drawList();
            wrefresh(window_);
         }

         void sessionSelect::drawList()
         {
            std::vector<sessionIdAndName>::const_iterator iterBeg = sessions_.begin();
            std::vector<sessionIdAndName>::const_iterator iterEnd = sessions_.begin();

            if (positionWindowEnd_ == 0)
               {
                  positionWindowEnd_ = numberOfLines_;
               }

            if (positionWindowEnd_ > static_cast<t_int>(sessions_.size()))
               {
                  positionWindowEnd_ = sessions_.size();
               }

            // Move the iterator the the start of the window to display.
            for (t_int counter = 0;
                 counter < positionWindowStart_;
                 counter++)
               {
                  iterBeg++;
               }

            // Move the iterator the the end of the window to display.
            for (t_int counter = 0;
                 counter < positionWindowEnd_;
                 counter++)
               {
                  iterEnd++;
               }

            // Display the window.
            t_int counter     = 0;
            t_int scr_counter = 0;
            std::vector<sessionIdAndName>::const_iterator iter;
            for (iter = iterBeg;
                 iter != iterEnd;
                 iter++)
               {
                  std::string name = "name:" + iter->name;
                  std::string id   = "id: " + btg::core::convertToString<t_long>(iter->id);
                  // Find out how much padding is required.
                  std::string name_spaces;
                  genSpaces(name, name_spaces);
                  std::string id_spaces;
                  genSpaces(id, id_spaces);

                  if (counter == currentPosition_)
                     {
                        ::wattron(window_, A_REVERSE);
                        
                        setColor(Colors::C_FILE);

                        mvwprintw(window_, scr_counter,   0, " %s%s", id.c_str(),   id_spaces.c_str());
                        mvwprintw(window_, scr_counter+1, 0, " %s%s", name.c_str(), name_spaces.c_str());

                        unSetColor(Colors::C_FILE);

                        ::wattroff(window_, A_REVERSE);
                     }
                  else
                     {
                        setColor(Colors::C_FILE);
                        mvwprintw(window_, scr_counter,   0, " %s%s", id.c_str(),   id_spaces.c_str());
                        mvwprintw(window_, scr_counter+1, 0, " %s%s", name.c_str(), name_spaces.c_str());
                        unSetColor(Colors::C_FILE);
                     }

                  counter++;
                  scr_counter += linesPerSession;
               }
         }

         void sessionSelect::moveDown()
         {
            const t_int list_max = sessions_.size();

            if (currentPosition_ < (numberOfLines_-1))
               {
                  currentPosition_++;

                  if (sessions_.size() > 0)
                     {
                        if (currentPosition_ > (list_max-1))
                           {
                              currentPosition_--;
                           }
                     }
               }
            else
               {
                  if (positionWindowEnd_ < list_max)
                     {
                        positionWindowStart_++;
                        positionWindowEnd_++;
                     }
               }

         }

         void sessionSelect::moveUp()
         {
            if (currentPosition_ > 0)
               {
                  currentPosition_--;
               }
            else
               {
                  if ((positionWindowStart_-1) >= 0)
                     {
                        positionWindowStart_--;
                        positionWindowEnd_--;
                     }
               }


         }

         void sessionSelect::toStart()
         {
            currentPosition_     = 0;
         }

         void sessionSelect::toEnd()
         {
            currentPosition_ = (numberOfLines_-1);

            t_int max_position = sessions_.size();
            if (max_position > 0)
               {
                  max_position--;
               }

            if (currentPosition_ > max_position)
               {
                  currentPosition_ = max_position;
               }
         }

         windowSize sessionSelect::calculateDimenstions(windowSize const& _ws) const
         {
            // Use the parents dimensions.
            windowSize ws = _ws;

            return ws;
         }

         dialog::RESULT sessionSelect::showHelp()
         {
            // Show a help window on the middle of the screen.
            std::vector<std::string> helpText;

            helpText.push_back("Help");
            helpText.push_back("    ");

            std::string keyDescr;

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_DOWN,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to move down");
               }


            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_UP,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to move up");
               }

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_SELECT,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to select a session");
               }

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_QUIT,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to quit this display");
               }

            helpText.push_back(" ");

            helpWindow hw(kmap_, helpText);

            return hw.run();
         }

         void sessionSelect::handleSelect()
         {
            pressed_select_key_ = true;

            t_uint current = positionWindowStart_ + currentPosition_;

            std::vector<sessionIdAndName>::const_iterator iter = sessions_.begin();
            iter += current;
            
            if (iter != sessions_.end())
               {
                  selected_session_ = iter->id;
               }
         }
         bool sessionSelect::getSession(t_long & _session)
         {
            _session = selected_session_; 
            return pressed_select_key_;
         }

         sessionSelect::~sessionSelect()
         {
            destroy();
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

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

#include "textinput.h"

#include "helpwindow.h"
#include "keys.h"

#include <bcore/t_string.h>
#include <bcore/hru.h>
#include <bcore/hrr.h>


namespace btg
{
   namespace UI
   {
      namespace cli
      {
         textInput::textInput(keyMapping const& _kmap,
                              windowSize const& _ws,
                              std::string const& _topic,
                              std::string const& _text)
            : baseWindow(_kmap),
              size_(_ws),
              topic_(_topic),
              text_(_text),
              counter_(0),
              changed_(false),
              text_entered(false)
         {
            // Keys used by this window.
            std::vector<keyMapping::KEYLABEL> labels;
            labels.push_back(keyMapping::K_BACK);
            labels.push_back(keyMapping::K_HELP);
            labels.push_back(keyMapping::K_DELETE);
            labels.push_back(keyMapping::K_SELECT);
            labels.push_back(keyMapping::K_RESIZE);
            kmap_.setUsed(labels);
         }

         dialog::RESULT textInput::run()
         {
            if (!init(size_))
               {
                  return dialog::R_NCREAT;
               }

            refresh();

            bool                 cont  = true;
            keyMapping::KEYLABEL label = keyMapping::K_UNDEF;
            t_uint               key   = 0;

            while (cont)
               {
                  key = readAnyKey();
                  if (!handleKeyboard(key, label))
                     {
                        // Got something that is not recognized as one of
                        // the keys that are used.
                        enterKey(key);
                        continue;
                     }

                  switch (label)
                     {
                     case keyMapping::K_BACK:
                        {
                           cont = false;
                           break;
                        }
                     case keyMapping::K_HELP:
                        {
                           if (showHelp())
                              {
                                 // Resized.
                                 return dialog::R_RESIZE;
                              }
                           refresh();
                           break;
                        }
                     case keyMapping::K_DELETE:
                        {
                           deleteCurrent();
                           refresh();
                           break;
                        }
                     case keyMapping::K_SELECT:
                        {
                           // Done setting session name.
                           cont         = false;
                           text_entered = true;
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
                           break;
                        }
                     }
               }

            return dialog::R_QUIT;
         }

         void textInput::resize(windowSize const& _ws)
         {

         }

         void textInput::refresh()
         {
            if (window_ == 0)
               {
                  return;
               }

            clear();
            draw();
            wrefresh(window_);
         }

         void textInput::draw()
         {
            setColor(Colors::C_NORMAL);

            counter_ = 0;

            addTopic(topic_);

            addValue();
		
            unSetColor(Colors::C_NORMAL);
         }

         void textInput::addTopic(std::string const& _text)
         {
            ::wattron(window_, A_BOLD);
            mvwprintw(window_, counter_, 0, "%s", _text.c_str());
            ::wattroff(window_, A_BOLD);
            counter_++;
         }

         void textInput::addValue()
         {
            if (text_.size() == 0)
               {
                  return;
               }

            if (changed_)
               {
                  // Hightlight on.
                  ::wattron(window_, A_REVERSE);
               }

            mvwprintw(window_, counter_, 0, "%s", text_.c_str());
            counter_++;

            if (changed_)
               {
                  // Hightlight off.
                  ::wattroff(window_, A_REVERSE);
               }
         }

         windowSize textInput::calculateDimenstions(windowSize const& _ws) const
         {
            // Use the same dimensions as the parent.
            windowSize ws = _ws;

            return ws;
         }

         bool textInput::showHelp()
         {
            // Show a help window on the middle of the screen.
            std::vector<std::string> helpText;

            helpText.push_back("Help");
            helpText.push_back("    ");

            helpText.push_back("Enter a string using the keyboard.");
            helpText.push_back("    ");

            std::string keyDescr;

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_DELETE,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  delete last character");
               }

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_SELECT,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to finish entering text");
               }

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_BACK,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to abort entering text");
               }

            helpText.push_back(" ");

            helpWindow hw(kmap_, helpText);

            dialog::RESULT res = hw.run();

            bool resized = false;
            if (res == dialog::R_RESIZE)
               {
                  resized = true;
               }

            return resized;
         }

         void textInput::enterKey(t_uint const _key)
         {
            std::string::iterator iter = text_.end();
            iter--;
            if (iter != text_.end())
               {
                  // Append.
                  text_.push_back(_key);
                  changed_ = true;
               }
         }

         void textInput::deleteCurrent()
         {
            if (text_.size() == 0)
               {
                  return;
               }

            std::string::iterator iter = text_.end();
            iter--;

            if (iter != text_.end())
               {
                  text_.erase(iter);
                  changed_ = true;
               }
         }

         bool textInput::getText(std::string & _text) const
         {
            if (text_entered)
               {
                  _text = text_;
               }

            return text_entered;
         }

         textInput::~textInput()
         {
            destroy();
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

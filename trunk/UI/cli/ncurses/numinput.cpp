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

#include "numinput.h"

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
         numberInputWindow::numberInputWindow(keyMapping const& _kmap,
                                              windowSize const& _ws,
                                              std::string const& _topic,
                                              t_int const& _number,
                                              t_int const _min,
                                              t_int const _max)
            : baseWindow(_kmap),
              size_(_ws),
              topic_(_topic),
              counter_(0),
              changed_(false),
              input_(),
              min_(_min),
              max_(_max)
         {
            // Keys used by this window.
            std::vector<keyMapping::KEYLABEL> labels;
            labels.push_back(keyMapping::K_BACK);
            labels.push_back(keyMapping::K_HELP);
            labels.push_back(keyMapping::K_DELETE);
            labels.push_back(keyMapping::K_SELECT);
            labels.push_back(keyMapping::K_RESIZE);
            kmap_.setUsed(labels);

            input_ = btg::core::convertToString<t_int>(_number);

            topic_ += " (between ";
            topic_ += btg::core::convertToString<t_int>(_min);
            topic_ += " and ";
            topic_ += btg::core::convertToString<t_int>(_max);
            topic_ += ")";
         }

         dialog::RESULT numberInputWindow::run()
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
                           cont = false;
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

         void numberInputWindow::resize(windowSize const& _ws)
         {

         }

         void numberInputWindow::refresh()
         {
            if (window_ == 0)
               {
                  return;
               }

            clear();
            draw();
            wrefresh(window_);
         }

         void numberInputWindow::draw()
         {
            setColor(Colors::C_NORMAL);

            counter_ = 0;

            addTopic(topic_);

            addValue();
		
            unSetColor(Colors::C_NORMAL);
         }

         void numberInputWindow::addTopic(std::string const& _text)
         {
            ::wattron(window_, A_BOLD);
            mvwprintw(window_, counter_, 0, "%s", _text.c_str());
            ::wattroff(window_, A_BOLD);
            counter_++;
         }

         void numberInputWindow::addValue()
         {
            if (input_.size() == 0)
               {
                  return;
               }

            if (changed_)
               {
                  // Hightlight on.
                  ::wattron(window_, A_REVERSE);
               }

            mvwprintw(window_, counter_, 0, "%s", input_.c_str());
            counter_++;

            if (changed_)
               {
                  // Hightlight off.
                  ::wattroff(window_, A_REVERSE);
               }
         }

         windowSize numberInputWindow::calculateDimenstions(windowSize const& _ws) const
         {
            // Use the same dimensions as the parent.
            windowSize ws = _ws;

            return ws;
         }

         bool numberInputWindow::showHelp()
         {
            // Show a help window on the middle of the screen.
            std::vector<std::string> helpText;

            helpText.push_back("Help");
            helpText.push_back("    ");

            helpText.push_back("Enter a number using the 0-9 and - keys.");
            helpText.push_back("    ");

            std::string keyDescr;

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_DELETE,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  delete last number");
               }

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_SELECT,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to enter the choosen number");
               }

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_BACK,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to abort entering a number");
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

         void numberInputWindow::stripLeadingZeros()
         {
            t_int n = 0;
            std::string::const_iterator leading_iter = input_.begin();
            if (isNegative())
               {
                  leading_iter++;
               }

            if (leading_iter == input_.end())
               {
                  return;
               }

            for (; leading_iter != input_.end(); leading_iter++)
               {
                  if (*leading_iter == '0')
                     {
                        n++;
                     }
                  else
                     {
                        break;
                     }
               }

            if (n > 1)
               {
                  std::string::iterator iter = input_.begin();
                  if (isNegative())
                     {
                        iter++;
                     }

                  if (iter != input_.end())
                     {
                        for (t_int i = 0; i < (n-1); i++)
                           {
                              input_.erase(iter++);
                              changed_ = true;
                           }
                     }
               }
         }

         bool numberInputWindow::isNegative() const
         {
            bool n = false;
            std::string::const_iterator negative_iter = input_.begin();
            if (negative_iter != input_.end())
               {
                  if (*negative_iter == '-')
                     {
                        n = true;
                     }
               }

            return n;
         }

         void numberInputWindow::enterKey(t_uint const _key)
         {
            bool change_sign = false;

            std::string k;
            switch (_key)
               {
               case '0':
                  {
                     k = "0";
                     if ((input_.size() == 1) && isNegative())
                        {
                           return;
                        }
                     break;
                  }
               case '1':
                  k = "1";
                  break;
               case '2':
                  k = "2";
                  break;
               case '3':
                  k = "3";
                  break;
               case '4':
                  k = "4";
                  break;
               case '5':
                  k = "5";
                  break;
               case '6':
                  k = "6";
                  break;
               case '7':
                  k = "7";
                  break;
               case '8':
                  k = "8";
                  break;
               case '9':
                  k = "9";
                  break;
               case '-':
                  change_sign = true;
                  break;
               default:
                  return;
               }

            if (change_sign)
               {
                  if (btg::core::convertStringTo<t_int>(input_) == 0)
                     {
                        return;
                     }

                  std::string temp = input_;

                  std::string::iterator iter = temp.begin();
                  if (iter != temp.end())
                     {
                        if (*iter == '-')
                           {
                              temp.erase(iter);
                           }
                        else
                           {
                              temp.insert(iter, _key);                              
                           }
                     }
               
                  if (!validate(temp))
                     {
                        return;
                     }

                  iter = input_.begin();
                  if (iter != input_.end())
                     {
                        if (*iter == '-')
                           {
                              input_.erase(iter);
                           }
                        else
                           {
                              input_.insert(iter, _key);                              
                           }
                        changed_ = true;
                     }
               }
            else
               {
                  std::string temp = input_ + k;

                  if (!validate(temp))
                     {
                        return;
                     }

                  std::string::iterator iter = input_.end();
                  iter--;
                  if (iter != input_.end())
                     {
                        // Append.
                        input_.push_back(_key);
                        changed_ = true;
                     }
               }

            stripLeadingZeros();
         }

         bool numberInputWindow::validate(std::string const& _input)
         {
            t_int value      = btg::core::convertStringTo<t_int>(_input);
            if ((value < min_) || (value > max_))
               {
                  return false;
               }

            return true;
         }

         void numberInputWindow::deleteCurrent()
         {
            if (input_.size() == 0)
               {
                  return;
               }

            std::string::iterator iter = input_.end();
            iter--;

            if (iter != input_.end())
               {
                  input_.erase(iter);
                  changed_ = true;
               }
         }

         bool numberInputWindow::changed() const
         {
            return changed_;
         }

         t_int numberInputWindow::getNumber() const
         {
            t_int number = btg::core::convertStringTo<t_int>(input_);

            return number;
         }

         numberInputWindow::~numberInputWindow()
         {
            destroy();
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

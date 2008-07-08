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

#include "limitwindow.h"

#include <bcore/t_string.h>
#include <bcore/hru.h>
#include <bcore/hrr.h>

#include "numinput.h"
#include "helpwindow.h"

namespace btg
{
   namespace UI
   {
      namespace cli
      {
         limitWindow::limitWindow(keyMapping const& _kmap,
                                  windowSize const& _ws,
                                  t_int _counter1_value,
                                  t_int _counter1_min,
                                  t_int _counter1_max,
                                  std::string const& _counter1label,
                                  t_int _counter2_value,
                                  t_int _counter2_min,
                                  t_int _counter2_max,
                                  std::string const& _counter2label,
                                  t_int _counter3_value,
                                  t_int _counter3_min,
                                  t_int _counter3_max,
                                  std::string const& _counter3label,
                                  t_int _counter4_value,
                                  t_int _counter4_min,
                                  t_int _counter4_max,
                                  std::string const& _counter4label)
            : baseWindow(_kmap),
              org_kmap_(_kmap),
              size_(_ws),
              currentLimit_(limitWindow::COUNTER1),
              counter_(1),
              choice_(limitWindow::COUNTER1),
              limitSet_(false)
         {
            counters_[limitWindow::COUNTER1].value = _counter1_value;
            counters_[limitWindow::COUNTER2].value = _counter2_value;
            counters_[limitWindow::COUNTER3].value = _counter3_value;
            counters_[limitWindow::COUNTER4].value = _counter4_value;
            counters_[limitWindow::COUNTER1].initial = _counter1_value;
            counters_[limitWindow::COUNTER2].initial = _counter2_value;
            counters_[limitWindow::COUNTER3].initial = _counter3_value;
            counters_[limitWindow::COUNTER4].initial = _counter4_value;
            counters_[limitWindow::COUNTER1].minimum = _counter1_min;
            counters_[limitWindow::COUNTER2].minimum = _counter2_min;
            counters_[limitWindow::COUNTER3].minimum = _counter3_min;
            counters_[limitWindow::COUNTER4].minimum = _counter4_min;
            counters_[limitWindow::COUNTER1].maximum = _counter1_max;
            counters_[limitWindow::COUNTER2].maximum = _counter2_max;
            counters_[limitWindow::COUNTER3].maximum = _counter3_max;
            counters_[limitWindow::COUNTER4].maximum = _counter4_max;
            counters_[limitWindow::COUNTER1].label = _counter1label;
            counters_[limitWindow::COUNTER2].label = _counter2label;
            counters_[limitWindow::COUNTER3].label = _counter3label;
            counters_[limitWindow::COUNTER4].label = _counter4label;

            // Keys used by this window.
            std::vector<keyMapping::KEYLABEL> labels;

            labels.push_back(keyMapping::K_BACK);
            labels.push_back(keyMapping::K_NEXT);
            labels.push_back(keyMapping::K_PREV);
            labels.push_back(keyMapping::K_DOWN);
            labels.push_back(keyMapping::K_UP);
            labels.push_back(keyMapping::K_LIST_START);
            labels.push_back(keyMapping::K_LIST_END);
            labels.push_back(keyMapping::K_HELP);
            labels.push_back(keyMapping::K_SELECT);
            labels.push_back(keyMapping::K_RESIZE);
            labels.push_back(keyMapping::K_INPUT);

            kmap_.setUsed(labels);
         }

         dialog::RESULT limitWindow::run()
         {
            if (!init(size_))
               {
                  return dialog::R_NCREAT;
               }

            refresh();

            keyMapping::KEYLABEL label;

            bool cont = true;

            while (cont)
               {
                  label = handleKeyboard();

                  switch (label)
                     {
                     case keyMapping::K_BACK:
                        {
                           cont = false;
                           break;
                        }
                     case keyMapping::K_INPUT:
                        {
                           if (handleInput())
                              {
                                 // Resized.
                                 return dialog::R_RESIZE;
                              }
                           refresh();
                           break;
                        }
                     case keyMapping::K_NEXT:
                        {
                           nextChoice();
                           refresh();
                           break;
                        }
                     case keyMapping::K_PREV:
                        {
                           prevChoice();
                           refresh();
                           break;
                        }
                     case keyMapping::K_DOWN:
                        {
                           decValue();
                           refresh();
                           break;
                        }
                     case keyMapping::K_UP:
                        {
                           incValue();
                           refresh();
                           break;
                        }
                     case keyMapping::K_LIST_START:
                        {
                           incValue(50);
                           refresh();
                           break;
                        }
                     case keyMapping::K_LIST_END:
                        {
                           decValue(50);
                           refresh();
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
                     case keyMapping::K_SELECT:
                        {
                           // Done setting limits.
                           limitSet_ = true;
                           cont      = false;
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

         void limitWindow::resize(windowSize const& _ws)
         {

         }

         void limitWindow::refresh()
         {
            if (window_ == 0)
               {
                  return;
               }

            clear();
            draw();
            wrefresh(window_);
         }

         void limitWindow::draw()
         {
            setColor(Colors::C_NORMAL);

            counter_ = 1;

            // Draw the counters.
	    
            for (t_uint cnt = COUNTER1;
                 cnt <= COUNTER4;
                 cnt++)
               {
                  LIMIT l = static_cast<LIMIT>(cnt);
                  addTopic(counters_[l].label);
		
                  bool changed = false;
                  if (counters_[l].initial != 
                      counters_[l].value)
                     {
                        changed = true;
                     }
                  addValue(l, 
                           counters_[l].value, 
                           changed);
		
               }
            unSetColor(Colors::C_NORMAL);
         }

         bool limitWindow::getLimits(t_int & _counter1,
                                     t_int & _counter2,
                                     t_int & _counter3,
                                     t_int & _counter4) const
         {
            if (limitSet_)
               {
                  _counter1 = counters_[COUNTER1].value;
                  _counter2 = counters_[COUNTER2].value;
                  _counter3 = counters_[COUNTER3].value;
                  _counter4 = counters_[COUNTER4].value;
               }

            return limitSet_;
         }

         void limitWindow::addTopic(std::string const& _text)
         {
            ::wattron(window_, A_BOLD);
            mvwprintw(window_, counter_, 0, "%s", _text.c_str());
            ::wattroff(window_, A_BOLD);
            counter_++;
         }

         void limitWindow::addValue(LIMIT const _type, 
                                    t_int const _value, 
                                    bool const _changed)
         {
            if (_changed)
               {
                  unSetColor(Colors::C_NORMAL);
                  setColor(Colors::C_MARK);
               }

            if (_type == choice_)
               {
                  // Hightlight on.
                  ::wattron(window_, A_REVERSE);
               }

            if (_value == -1)
               {
                  mvwprintw(window_, counter_, 0, "%s", "unlimited");
               }
            else
               {
                  mvwprintw(window_, counter_, 0, "%d", _value);
               }

            if (_type == choice_)
               {
                  ::wattroff(window_, A_REVERSE);
                  // Hightlight off.
               }

            if (_changed)
               {
                  setColor(Colors::C_NORMAL);
                  unSetColor(Colors::C_MARK);
               }

            counter_++;
         }

         bool limitWindow::handleInput()
         {
            windowSize numberdimensions;
            getSize(numberdimensions);

            numberInputWindow niw(org_kmap_, 
                                  numberdimensions,
                                  counters_[choice_].label,
                                  counters_[choice_].value, 
                                  counters_[choice_].minimum,
                                  counters_[choice_].maximum);

            dialog::RESULT res = niw.run();
            
            if (res == dialog::R_RESIZE)
               {
                  // Resized.
                  return true;
               }

            if (niw.changed())
               {
                  t_int limit = niw.getNumber();
                  if (limit == 0)
                     {
                        limit = counters_[choice_].minimum;
                     }

                  counters_[choice_].value = limit;
               }

            return false;
         }

         void limitWindow::nextChoice()
         {
            switch (choice_)
               {
               case limitWindow::COUNTER1:
                  choice_ = limitWindow::COUNTER2;
                  break;
               case limitWindow::COUNTER2:
                  choice_ = limitWindow::COUNTER3;
                  break;
               case limitWindow::COUNTER3:
                  choice_ = limitWindow::COUNTER4;
                  break;
               case limitWindow::COUNTER4:
                  choice_ = limitWindow::COUNTER1;
                  break;
               }
         }

         void limitWindow::prevChoice()
         {
            switch (choice_)
               {
               case limitWindow::COUNTER1:
                  choice_ = limitWindow::COUNTER4;
                  break;
               case limitWindow::COUNTER2:
                  choice_ = limitWindow::COUNTER1;
                  break;
               case limitWindow::COUNTER3:
                  choice_ = limitWindow::COUNTER2;
                  break;
               case limitWindow::COUNTER4:
                  choice_ = limitWindow::COUNTER3;
                  break;
               }
         }

         void limitWindow::incValue(t_int const _increment)
         {
            switch (choice_)
               {
               case limitWindow::COUNTER1:
                  genInc(_increment, 
                         counters_[COUNTER1].value, 
                         counters_[COUNTER1].maximum);
                  break;
               case limitWindow::COUNTER2:
                  genInc(_increment, 
                         counters_[COUNTER2].value, 
                         counters_[COUNTER2].maximum);
                  break;
               case limitWindow::COUNTER3:
                  genInc(_increment, 
                         counters_[COUNTER3].value, 
                         counters_[COUNTER3].maximum);
                  break;
               case limitWindow::COUNTER4:
                  genInc(_increment, 
                         counters_[COUNTER4].value, 
                         counters_[COUNTER4].maximum);
                  break;
               }
         }

         void limitWindow::decValue(t_int const _increment)
         {
            switch (choice_)
               {
               case limitWindow::COUNTER1:
                  genDec(_increment, 
                         counters_[COUNTER1].value, 
                         counters_[COUNTER1].minimum);
                  break;
               case limitWindow::COUNTER2:
                  genDec(_increment, 
                         counters_[COUNTER2].value, 
                         counters_[COUNTER2].minimum);
                  break;
               case limitWindow::COUNTER3:
                  genDec(_increment, 
                         counters_[COUNTER3].value, 
                         counters_[COUNTER3].minimum);
                  break;
               case limitWindow::COUNTER4:
                  genDec(_increment, 
                         counters_[COUNTER4].value, 
                         counters_[COUNTER4].minimum);
                  break;
               }
         }

         void limitWindow::genInc(t_int const _increment, t_int & _value, t_int const _max) const
         {
            if (_value == -1)
               {
                  _value = 1;
               }
            else
               {
                  _value += _increment;
               }

            if (_value > _max)
               {
                  _value = _max;
               }
         }

         void limitWindow::genDec(t_int const _increment, t_int & _value, t_int const _min) const
         {
            if (_value == 1)
               {
                  _value = -1;
               }
            else
               {
                  _value -= _increment;
               }

            if (_value < _min)
               {
                  _value = _min;
               }
         }

         windowSize limitWindow::calculateDimenstions(windowSize const& _ws) const
         {
            // Use the same dimensions as the parent.
            windowSize ws = _ws;

            return ws;
         }

         bool limitWindow::showHelp()
         {
            // Show a help window on the middle of the screen.
            std::vector<std::string> helpText;

            helpText.push_back("Help");
            helpText.push_back("    ");

            std::string keyDescr;

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_NEXT,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to move to the next limit");
               }

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_PREV,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to move to the previous limit");
               }

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_DOWN,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to dec. the currently selected limit by one");
               }


            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_UP,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to inc. the currently selected limit by one");
               }

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_LIST_START,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to inc. the currently selected limit by 100");
               }

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_LIST_END,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to dec. the currently selected limit by 100");
               }

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_INPUT,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to input a number using the keyboard");
               }

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_SELECT,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to set the choosen limits");
               }

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_BACK,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to abort setting limits");
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

         limitWindow::~limitWindow()
         {
            destroy();
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

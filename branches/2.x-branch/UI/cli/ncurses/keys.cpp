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

#include "keys.h"

#include <bcore/t_string.h>

#include <locale>

extern "C"
{
#include <ncurses.h>
}

namespace btg
{
   namespace UI
   {
      namespace cli
      {
         keyMapping::keyMapping()
            : keys()
         {
         }

         void keyMapping::setDefaults()
         {
            // Map the enums used by this client to ncurses key macros or
            // values.
            if (!exists(keyMapping::K_HELP))
               {
                  add(keyMapping::K_HELP, KEY_F(1));
               }

            if (!exists(keyMapping::K_DETACH))
               {
                  add(keyMapping::K_DETACH, 'd');
               }

            if (!exists(keyMapping::K_QUIT))
               {
                  add(keyMapping::K_QUIT, 'q');
               }

            // Used for dialogs where one cannot press for example
            // 'q', since the dialog accepts this key.
            if (!exists(keyMapping::K_BACK))
               {
                  add(keyMapping::K_BACK, KEY_F(10));
               }

            if (!exists(keyMapping::K_LOAD))
               {
                  add(keyMapping::K_LOAD, 'l');
               }

            if (!exists(keyMapping::K_LOAD_URL))
               {
                  add(keyMapping::K_LOAD_URL, 'u');
               }

            if (!exists(keyMapping::K_GLIMIT))
               {
                  add(keyMapping::K_GLIMIT, 'g');
               }

            if (!exists(keyMapping::K_DOWN))
               {
                  add(keyMapping::K_DOWN, KEY_DOWN);
               }

            if (!exists(keyMapping::K_UP))
               {
                  add(keyMapping::K_UP, KEY_UP);
               }

            if (!exists(keyMapping::K_LIST_START))
               {
                  add(keyMapping::K_LIST_START, KEY_HOME);
               }

            if (!exists(keyMapping::K_LIST_END))
               {
                  add(keyMapping::K_LIST_END, KEY_END);
               }

            if (!exists(keyMapping::K_NEXT))
               {
                  add(keyMapping::K_NEXT, KEY_RIGHT);
               }

            if (!exists(keyMapping::K_PREV))
               {
                  add(keyMapping::K_PREV, KEY_LEFT);
               }

            if (!exists(keyMapping::K_MARK))
               {
                  add(keyMapping::K_MARK, 'w');
               }

            if (!exists(keyMapping::K_MARK_ALL))
               {
                  add(keyMapping::K_MARK_ALL, 'a');
               }

            if (!exists(keyMapping::K_SELECT))
               {
                  add(keyMapping::K_SELECT, '\n');
               }

            if (!exists(keyMapping::K_MENU))
               {
                  add(keyMapping::K_MENU, 'm');
               }

            if (!exists(keyMapping::K_SESNAME))
               {
                  add(keyMapping::K_SESNAME, 'n');
               }

            if (!exists(keyMapping::K_SORT))
               {
                  add(keyMapping::K_SORT, 's');
               }

            if (!exists(keyMapping::K_STATS))
               {
                  add(keyMapping::K_STATS, 'i');
               }

            if (!exists(keyMapping::K_RESIZE))
               {
                  add(keyMapping::K_RESIZE, KEY_RESIZE);
               }

            if (!exists(keyMapping::K_DELETE))
               {
                  add(keyMapping::K_DELETE, KEY_DC);
               }

            if (!exists(keyMapping::K_INPUT))
               {
                  add(keyMapping::K_INPUT, 'e');
               }
         }

         void keyMapping::add(KEYLABEL const _keyl, int const _value)
         {
            std::pair<KEYLABEL,int> p(_keyl, _value);

            keys.insert(p);
         }

         bool keyMapping::getValue(KEYLABEL const _keyl, std::string & _value) const
         {
            bool status = false;

            std::map<KEYLABEL,int>::const_iterator iter = keys.find(_keyl);

            const int enter = '\n';

            if (iter != keys.end())
               {
                  int val = iter->second;
                  if (std::isprint(iter->second) && (val != enter))
                     {
                        char c = iter->second;

                        _value = btg::core::convertToString<char>(c);
		     
                        status = true;
                     }
                  else
                     {
                        // Non printable character.
                        // Handle some common control characters used by ncurses.

                        _value = "";

                        switch (iter->second)
                           {
                           case KEY_BREAK:
                              {
                                 _value = "break";
                                 break;
                              }
                           case KEY_DOWN:
                              {
                                 _value = "down";
                                 break;
                              }
                           case KEY_UP:
                              {
                                 _value = "up";
                                 break;
                              }
                           case KEY_LEFT:
                              {
                                 _value = "left";
                                 break;
                              }
                           case KEY_RIGHT:
                              {
                                 _value = "right";
                                 break;
                              }
                           case KEY_HOME:
                              {
                                 _value = "home";
                                 break;
                              }
                           case KEY_END:
                              {
                                 _value = "end";
                                 break;
                              }
                           case KEY_BACKSPACE:
                              {
                                 _value = "backspace";
                                 break;
                              }
                           case KEY_F(1):
                              {
                                 _value = "F1";
                                 break;
                              }
                           case KEY_F(2):
                              {
                                 _value = "F2";
                                 break;
                              }
                           case KEY_F(3):
                              {
                                 _value = "F3";
                                 break;
                              }
                           case KEY_F(4):
                              {
                                 _value = "F4";
                                 break;
                              }
                           case KEY_F(5):
                              {
                                 _value = "F5";
                                 break;
                              }
                           case KEY_F(6):
                              {
                                 _value = "F6";
                                 break;
                              }
                           case KEY_F(7):
                              {
                                 _value = "F7";
                                 break;
                              }
                           case KEY_F(8):
                              {
                                 _value = "F8";
                                 break;
                              }
                           case KEY_F(9):
                              {
                                 _value = "F9";
                                 break;
                              }
                           case KEY_F(10):
                              {
                                 _value = "F10";
                                 break;
                              }
                           case KEY_F(11):
                              {
                                 _value = "F11";
                                 break;
                              }
                           case KEY_F(12):
                              {
                                 _value = "F12";
                                 break;
                              }
                           case KEY_DL:
                              {
                                 _value = "delete line";
                                 break;
                              }
                           case KEY_DC:
                              {
                                 _value = "delete";
                                 break;
                              }
                           case KEY_IC:
                              {
                                 _value = "insert";
                                 break;
                              }
                           case KEY_NPAGE:
                              {
                                 _value = "next page";
                                 break;
                              }
                           case KEY_PPAGE:
                              {
                                 _value = "prev page";
                                 break;
                              }
                           case enter:
                           case KEY_ENTER:
                              {
                                 _value = "enter";
                                 break;
                              }
                           }

                        if (_value.size() > 0)
                           {
                              status = true;
                           }
                     }
               }

            return status;
         }

         void keyMapping::setUsed(std::vector<KEYLABEL> const& _labels)
         {
            std::vector<KEYLABEL> toErase;

            std::map<KEYLABEL, int>::iterator keyIter;
            for (keyIter = keys.begin();
                 keyIter != keys.end();
                 keyIter++)
               {
                  std::vector<KEYLABEL>::const_iterator fIter = std::find(_labels.begin(), 
                                                                          _labels.end(), 
                                                                          keyIter->first);
                  if (fIter == _labels.end())
                     {
                        toErase.push_back(keyIter->first);
                     }
               }

            std::vector<KEYLABEL>::const_iterator iter;
            for (iter = toErase.begin();
                 iter != toErase.end();
                 iter++)
               {
                  keys.erase(*iter);
               }
         }

         bool keyMapping::get(t_int const _value, KEYLABEL & _keyl) const
         {
            bool status = false;

            std::map<KEYLABEL,int>::const_iterator iter;
            for (iter = keys.begin();
                 iter != keys.end();
                 iter++)
               {
                  if (_value == iter->second)
                     {
                        status = true;
                        _keyl  = iter->first;
                        break;
                     }
               }

            return status;
         }

         bool keyMapping::exists(KEYLABEL const _keyl) const
         {
            bool status = false;

            std::map<KEYLABEL,int>::const_iterator iter = keys.find(_keyl);

            if (iter != keys.end())
               {
                  status = true;
               }

            return status;
         }

         bool keyMapping::check(std::string & _error) const
         {
            bool status = true;

            std::map<KEYLABEL,int>::const_iterator iter;
            for (iter = keys.begin();
                 iter != keys.end();
                 iter++)
               {
                  t_int value         = iter->second;
                  t_int value_counter = 0;
                  std::map<KEYLABEL,int>::const_iterator viter;
                  for (viter = keys.begin();
                       viter != keys.end();
                       viter++)
                     {
                        if (viter->second == value)
                           {
                              value_counter++;
                           }
                     }
                  if (value_counter != 1)
                     {
                        _error = "Keyboard: value " + 
                           btg::core::convertToString<t_int>(value) 
                           + " used more  than once.";
                        return false;
                     }
               }
            return status;
         }

         keyMapping::~keyMapping()
         {
         }

      } // namespace cli
   } // namespace UI
} // namespace btg



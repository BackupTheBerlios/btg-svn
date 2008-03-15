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

#include "nconfig.h"
#include <bcore/util.h>

#include "colors.h"

namespace btg
{
   namespace UI
   {
      namespace cli
      {
	      using namespace btg::core::client;

	      std::string const SECTION_BTGNCLI("btgncli");
	      
	      std::string const KEY_HELP("KEY_HELP");
	      std::string const KEY_DETACH("KEY_DETACH");
	      std::string const KEY_QUIT("KEY_QUIT");
	      std::string const KEY_QUITSCREEN("KEY_QUITSCREEN");
	      std::string const KEY_LOAD("KEY_LOAD");
	      std::string const KEY_MENU("KEY_MENU");
	      std::string const KEY_GLIMIT("KEY_GLIMIT");
	      std::string const KEY_DOWN("KEY_DOWN");
	      std::string const KEY_UP("KEY_UP");
	      std::string const KEY_LIST_START("KEY_LIST_START");
	      std::string const KEY_LIST_END("KEY_LIST_END");
	      std::string const KEY_SELECT("KEY_SELECT");
	      std::string const KEY_MARK("KEY_MARK");
	      std::string const KEY_MARK_ALL("KEY_MARK_ALL");
	      std::string const KEY_NEXT("KEY_NEXT");
	      std::string const KEY_PREV("KEY_PREV");
         std::string const KEY_DELETE("KEY_DELETE");
         std::string const KEY_INPUT("KEY_INPUT");

	      std::string const COLOR_NORMAL("COLOR_NORMAL");
	      std::string const COLOR_BORDER("COLOR_BORDER");
	      std::string const COLOR_FILE("COLOR_FILE");
	      std::string const COLOR_DIRECTORY("COLOR_DIRECTORY");
	      std::string const COLOR_MARK("COLOR_MARK");
	      std::string const COLOR_WARNING("COLOR_WARNING");
	      std::string const COLOR_ERROR("COLOR_ERROR");

	      ncliConfiguration::ncliConfiguration(btg::core::LogWrapperType _logwrapper,
                                              std::string const& _filename)
            : clientConfiguration(_logwrapper, _filename),
              helpKey(keyMapping::K_UNDEF),
              detachKey(keyMapping::K_UNDEF),
              quitKey(keyMapping::K_UNDEF),
              quitscreenKey(keyMapping::K_UNDEF),
              loadKey(keyMapping::K_UNDEF),
              menuKey(keyMapping::K_UNDEF),
              limitKey(keyMapping::K_UNDEF),
              downKey(keyMapping::K_UNDEF),
              upKey(keyMapping::K_UNDEF),
              listStartKey(keyMapping::K_UNDEF),
              listEndKey(keyMapping::K_UNDEF),
              selectKey(keyMapping::K_UNDEF),
              markKey(keyMapping::K_UNDEF),
              markAllKey(keyMapping::K_UNDEF),
              nextKey(keyMapping::K_UNDEF),
              prevKey(keyMapping::K_UNDEF),
              deleteKey(keyMapping::K_UNDEF),
              inputKey(keyMapping::K_UNDEF)
	      {
		
	      }

	      bool ncliConfiguration::read(bool const _force)
	      {
            bool status = clientConfiguration::read(_force);

            if (status)
               {
                  // Read keys.

                  std::string key;
                  key = inifile->GetValue(KEY_HELP, SECTION_BTGNCLI);
                  convertKey(key, helpKey);

                  key = inifile->GetValue(KEY_DETACH, SECTION_BTGNCLI);
                  convertKey(key, detachKey);

                  key = inifile->GetValue(KEY_QUIT, SECTION_BTGNCLI);
                  convertKey(key, quitKey);

                  key = inifile->GetValue(KEY_QUITSCREEN, SECTION_BTGNCLI);
                  convertKey(key, quitscreenKey);

                  key = inifile->GetValue(KEY_LOAD, SECTION_BTGNCLI);
                  convertKey(key, loadKey);

                  key = inifile->GetValue(KEY_MENU, SECTION_BTGNCLI);
                  convertKey(key, menuKey);

                  key = inifile->GetValue(KEY_GLIMIT, SECTION_BTGNCLI);
                  convertKey(key, limitKey);

                  key = inifile->GetValue(KEY_DOWN, SECTION_BTGNCLI);
                  convertKey(key, downKey);

                  key = inifile->GetValue(KEY_UP, SECTION_BTGNCLI);
                  convertKey(key, upKey);

                  key = inifile->GetValue(KEY_LIST_START, SECTION_BTGNCLI);
                  convertKey(key, listStartKey);

                  key = inifile->GetValue(KEY_LIST_END, SECTION_BTGNCLI);
                  convertKey(key, listEndKey);

                  key = inifile->GetValue(KEY_SELECT, SECTION_BTGNCLI);
                  convertKey(key, selectKey);

                  key = inifile->GetValue(KEY_MARK, SECTION_BTGNCLI);
                  convertKey(key, markKey);

                  key = inifile->GetValue(KEY_MARK_ALL, SECTION_BTGNCLI);
                  convertKey(key, markAllKey);

                  key = inifile->GetValue(KEY_NEXT, SECTION_BTGNCLI);
                  convertKey(key, nextKey);

                  key = inifile->GetValue(KEY_PREV, SECTION_BTGNCLI);
                  convertKey(key, prevKey);

                  key = inifile->GetValue(KEY_DELETE, SECTION_BTGNCLI);
                  convertKey(key, deleteKey);

                  key = inifile->GetValue(KEY_INPUT, SECTION_BTGNCLI);
                  convertKey(key, inputKey);

                  

                  // Read colors.

                  std::string color = inifile->GetValue(COLOR_NORMAL, SECTION_BTGNCLI);
                  convertColor(color, Colors::C_NORMAL, c_normal);

                  color = inifile->GetValue(COLOR_BORDER, SECTION_BTGNCLI);
                  convertColor(color, Colors::C_BORDER, c_border);

                  color = inifile->GetValue(COLOR_FILE, SECTION_BTGNCLI);
                  convertColor(color, Colors::C_FILE, c_file);

                  color = inifile->GetValue(COLOR_DIRECTORY, SECTION_BTGNCLI);
                  convertColor(color, Colors::C_DIRECTORY, c_directory);

                  color = inifile->GetValue(COLOR_MARK, SECTION_BTGNCLI);
                  convertColor(color, Colors::C_MARK, c_mark);

                  color = inifile->GetValue(COLOR_WARNING, SECTION_BTGNCLI);
                  convertColor(color, Colors::C_WARNING, c_warning);

                  color = inifile->GetValue(COLOR_ERROR, SECTION_BTGNCLI);
                  convertColor(color, Colors::C_ERROR, c_error);
               }

            return status;
	      }

	      bool ncliConfiguration::write(bool const _force)
	      {
            bool status = clientConfiguration::write(_force);

            return status;
	      }

	      std::string ncliConfiguration::getSyntax() const
	      {
            std::string output = clientConfiguration::getSyntax();

            formatSection(SECTION_BTGNCLI, output);

            std::vector<std::string> temp;

            temp.push_back(std::string("ascii value (integer)"));

            std::string keyDescription("key mapping");

            formatKey(KEY_HELP,
                      keyDescription,
                      temp,
                      output);

            formatKey(KEY_DETACH,
                      keyDescription,
                      temp,
                      output);

            formatKey(KEY_QUIT,
                      keyDescription,
                      temp,
                      output);

            formatKey(KEY_QUITSCREEN,
                      keyDescription,
                      temp,
                      output);

            formatKey(KEY_LOAD,
                      keyDescription,
                      temp,
                      output);

            formatKey(KEY_MENU,
                      keyDescription,
                      temp,
                      output);

            formatKey(KEY_GLIMIT,
                      keyDescription,
                      temp,
                      output);

            formatKey(KEY_DOWN,
                      keyDescription,
                      temp,
                      output);

            formatKey(KEY_UP,
                      keyDescription,
                      temp,
                      output);

            formatKey(KEY_LIST_START,
                      keyDescription,
                      temp,
                      output);

            formatKey(KEY_LIST_END,
                      keyDescription,
                      temp,
                      output);

            formatKey(KEY_SELECT,
                      keyDescription,
                      temp,
                      output);

            formatKey(KEY_MARK,
                      keyDescription,
                      temp,
                      output);

            formatKey(KEY_MARK_ALL,
                      keyDescription,
                      temp,
                      output);

            formatKey(KEY_NEXT,
                      keyDescription,
                      temp,
                      output);

            formatKey(KEY_PREV,
                      keyDescription,
                      temp,
                      output);

            formatKey(KEY_DELETE,
                      keyDescription,
                      temp,
                      output);

            formatKey(KEY_INPUT,
                      keyDescription,
                      temp,
                      output);

            temp.clear();
            temp.push_back(std::string("R,G,B:R,G,B"));

            std::string colorDescription("color definition (foreground RGB, background RGB)");

            formatKey(COLOR_NORMAL,
                      colorDescription,
                      temp,
                      output);

            formatKey(COLOR_BORDER,
                      colorDescription,
                      temp,
                      output);

            formatKey(COLOR_FILE,
                      colorDescription,
                      temp,
                      output);

            formatKey(COLOR_DIRECTORY,
                      colorDescription,
                      temp,
                      output);

            formatKey(COLOR_MARK,
                      colorDescription,
                      temp,
                      output);

            formatKey(COLOR_WARNING,
                      colorDescription,
                      temp,
                      output);

            formatKey(COLOR_ERROR,
                      colorDescription,
                      temp,
                      output);

            return output;
	      }

	      bool ncliConfiguration::getKey(keyMapping::KEYLABEL const _label, t_int & _value)
	      {
            t_int value = keyMapping::K_UNDEF;

            switch (_label)
               {
               case keyMapping::K_HELP:
                  {
                     value = helpKey;
                     break;
                  }
               case keyMapping::K_DETACH:
                  {
                     value = detachKey;
                     break;
                  }
               case keyMapping::K_QUITSCREEN:
                  {
                     value = quitscreenKey;
                     break;
                  }
               case keyMapping::K_QUIT:
                  {
                     value = quitKey;
                     break;
                  }
               case keyMapping::K_LOAD:
                  {
                     value = loadKey;
                     break;
                  }
               case keyMapping::K_MENU:
                  {
                     value = menuKey;
                     break;
                  }
               case keyMapping::K_GLIMIT:
                  {
                     value = limitKey;
                     break;
                  }
               case keyMapping::K_DOWN:
                  {
                     value = downKey;
                     break;
                  }
               case keyMapping::K_UP:
                  {
                     value = upKey;
                     break;
                  }
               case keyMapping::K_LIST_START:
                  {
                     value = listStartKey;
                     break;
                  }
               case keyMapping::K_LIST_END:
                  {
                     value = listEndKey;
                     break;
                  }
               case keyMapping::K_SELECT:
                  {
                     value = selectKey;
                     break;
                  }
               case keyMapping::K_MARK:
                  {
                     value = markKey;
                     break;
                  }
               case keyMapping::K_MARK_ALL:
                  {
                     value = markAllKey;
                     break;
                  }
               case keyMapping::K_NEXT:
                  {
                     value = nextKey;
                     break;
                  }
               case keyMapping::K_PREV:
                  {
                     value = prevKey;
                     break;
                  }
               case keyMapping::K_DELETE:
                  {
                     value = deleteKey;
                     break;
                  }
               case keyMapping::K_INPUT:
                  {
                     value = inputKey;
                     break;
                  }

               default:
                  {
                     value =  keyMapping::K_UNDEF;
                     break;
                  }
               }

            if (value != keyMapping::K_UNDEF)
               {
                  _value = value;
                  return true;
               }

            return false;
	      }

	      void ncliConfiguration::convertKey(std::string const& _key, 
                                            t_int & _value)
	      {
            if (_key.size() == 0)
               {
                  return;
               }
            t_int value = 0;
            if (btg::core::stringToNumber<t_int>(_key, value))
               {
                  if ((value >= 0) && (value <= 127))
                     {
                        _value = value;
                     }
               }
	      }

	      void ncliConfiguration::convertColor(std::string const& _color,
                                              t_uint const _label,
                                              colorDef & _colordef)
	      {
            // Read two rgb values: r,g,b:r,g,b
            t_strList lst = btg::core::Util::splitLine(_color, ":");
            if (lst.size() == 2)
               {
                  if (convertColor(lst[0], _colordef.foreground) &&
                      convertColor(lst[1], _colordef.background))
                     {
                        _colordef.label    = _label;
                        _colordef.flag_set = true;
                     }
               }
	      }

	      bool ncliConfiguration::convertColor(std::string const& _color,
                                              rgbDef & _rgbdef)
	      {
            // Read rgb value: r,g,b
            t_strList lst = btg::core::Util::splitLine(_color, ",");
            if (lst.size() == 3)
               {
                  t_int r = -1;
                  t_int g = -1;
                  t_int b = -1;

                  if (btg::core::stringToNumber<t_int>(lst[0], r) &&
                      btg::core::stringToNumber<t_int>(lst[1], g) &&
                      btg::core::stringToNumber<t_int>(lst[2], b))
                     {
                        if ((r >= 0 && r <= 254) &&
                            (g >= 0 && g <= 254) &&
                            (b >= 0 && b <= 254))
                           {
                              _rgbdef.r = r;
                              _rgbdef.g = g;
                              _rgbdef.b = b;

                              return true;
                           }
                     }
               }
            return false;
	      }

	      void ncliConfiguration::getColors(std::vector<colorDef> & _colors)
	      {
            if (c_normal.flag_set)
               {
                  _colors.push_back(c_normal);
               }
            if (c_border.flag_set)
               {
                  _colors.push_back(c_border);
               }
            if (c_file.flag_set)
               {
                  _colors.push_back(c_file);
               }
            if (c_directory.flag_set)
               {
                  _colors.push_back(c_directory);
               }
            if (c_mark.flag_set)
               {
                  _colors.push_back(c_mark);
               }
            if (c_warning.flag_set)
               {
                  _colors.push_back(c_warning);
               }
            if (c_error.flag_set)
               {
                  _colors.push_back(c_error);
               }
	      }

	      ncliConfiguration::~ncliConfiguration()
	      {
		
	      }

      } // namespace cli
   } // namespace UI
} // namespace btg


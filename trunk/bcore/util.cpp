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

#include "util.h"

#include "project.h"

namespace btg
{
   namespace core
   {

      t_strList Util::splitLine(std::string const& _s, std::string const& _delimiter)
      {
         t_strList tokens;
         // Skip delimiters at beginning.
         std::string::size_type lastPos = _s.find_first_not_of(_delimiter, 0);
         // Find first "non-delimiter".
         std::string::size_type pos     = _s.find_first_of(_delimiter, lastPos);

         while (std::string::npos != pos || std::string::npos != lastPos)
            {
               // Found a token, add it to the list of strings.
               tokens.push_back(_s.substr(lastPos, pos - lastPos));
               // Skip delimiters.  Note the "not_of"
               lastPos = _s.find_first_not_of(_delimiter, pos);
               // Find next "non-delimiter"
               pos = _s.find_first_of(_delimiter, lastPos);
            }

         return tokens;
      }

      bool Util::compareStringNoCase(std::string const& _s1, std::string const& _s2)
      {
         std::string::const_iterator p1 = _s1.begin();
         std::string::const_iterator p2 = _s2.begin();

         if(_s2.size() != _s1.size())
            return false;

         while (p1 != _s2.end() && p2 != _s2.end() )
            {
               if (toupper(*p1) != toupper(*p2))
                  return false;
               ++p1;
               ++p2;
            }

         return true;
      }

      bool Util::stringToInt(std::string const& _input, t_int & _buffer)
      {
         return stringToNumber<t_int>(_input, _buffer);
      }

      bool Util::stringToUInt(std::string const& _input, t_uint & _buffer)
      {
         return stringToNumber<t_uint>(_input, _buffer);
      }

      bool Util::getFileFromPath(std::string const& _input, std::string & _output)
      {
         std::string ps = projectDefaults::sPATH_SEPARATOR();
         size_t pos = _input.rfind(ps);
         if (pos != std::string::npos)
         {
            _output = _input.substr( pos + ps.length() );
            return true;
         }
         return false;
      }

      t_float Util::valueUnit(t_float _value, std::string &_unit)
      {
         if (_value > 1024)
         {
            _value /= 1024;
            _unit = 'k';
         }
         if (_value > 1024)
         {
            _value /= 1024;
            _unit = 'M';
         }
         if (_value > 1024)
         {
            _value /= 1024;
            _unit = 'G';
         }
         if (_value > 1024)
         {
            _value /= 1024;
            _unit = 'T';
         }
         if (_value > 1024)
         {
            _value /= 1024;
            _unit = 'P';
         }
         // http://en.wikipedia.org/wiki/SI_unit
         if (_value > 1024)
         {
            _value /= 1024;
            _unit = 'E'; // exa-
         }
         if (_value > 1024)
         {
            _value /= 1024;
            _unit = 'Z'; // zetta-
         }
         if (_value > 1024)
         {
            _value /= 1024;
            _unit = 'Y'; // yotta-
         }
         return _value;
      }

   } // namespace core
} // namespace btg

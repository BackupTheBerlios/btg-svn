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

#ifndef UTIL_H
#define UTIL_H

#include "type.h"
#include "project.h"

#include <string>
#include <sstream>

#include "dbuf.h"
#include "logmacro.h"

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup core
          */
         /** @{ */

         /// Utility functions.
         class Util
            {
            public:
               /// Split a string using a delimiter.
               /// @param [in] _s                Input string.
               /// @param [in] _delimiter        The delimiter.
               /// @return                       A list of sub-strings.
               static t_strList splitLine(std::string const& _s, std::string const& _delimiter = " ");

               /// Compare two strings ignoring case
               /// @param [in] _s1               First string
               /// @param [in] _s2               Second string
               /// @return                       True if both strings are equal, false otherwise.
               static bool compareStringNoCase(std::string const& _s1, std::string const& _s2);

               /// Convert a string which represents a number into an integer.
               /// @param [in]  _input  The input.
               /// @param [out] _buffer Reference to the destination.
               /// @return              True if successfull, false if not.
               static bool stringToInt(std::string const& _input, t_int & _buffer);

               /// Convert a string representing an unsigned integer to an unsigned integer.
               /// @param [in]  _input  A string representing an unsigned integer.
               /// @param [out] _buffer Output of the conversion.
               /// @return True if successful, false if not.
               static bool stringToUInt(std::string const& _input, t_uint & _buffer);

               /// Get the file part from an unix path.
               /// @param [in]  _input  The input, a unix path.
               /// @param [out] _output The output, a file name.
               /// @return True if the file part could be extracted, false otherwise.
               static bool getFileFromPath(std::string const& _input, std::string & _output);
            };

         /// Convert a string to a number.
         template<typename destinationType>
            bool stringToNumber(std::string const& _input, destinationType & _destination)
            {
               bool result = true;
               std::stringstream ssBuffer(_input);

               ssBuffer >> std::dec >> _destination;

               if (ssBuffer.fail())
                  {
                     result = false;
                  }

               return result;
            }
         /** @} */

      } // namespace core
} // namespace btg

#endif

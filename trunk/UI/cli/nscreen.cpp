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
#include <bcore/t_string.h>

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

         Screen::Screen(bool const _pretend)
            : 
#if BTG_DEBUG
            counter(0),
#endif
            pretend(_pretend),
            last_input()
         {
         }

         bool Screen::getLine()
         {
            if (pretend)
               {
                  return false;
               }

            std::string input;

            std::getline (std::cin, input);

            bool done = false;

            if (input.size() > 0)
               {
                  last_input = input;
                  done       = true;
               }

            return done;
         }

         std::string Screen::getInput()
         {
            return last_input;
         }

         void Screen::setOutput(std::string const& _s, bool const _appendNewline)
         {
            if (pretend)
               {
                  return;
               }
#if BTG_DEBUG
            std::cout << btg::core::convertToString<t_uint>(counter) << " ";
            counter++;

            if (counter > 254)
               {
                  counter = 0;
               }
#endif
            std::cout << _s;

            if (_appendNewline)
               {
                  std::cout << std::endl;
               }
         }

         void Screen::writeLog(std::string const& _string)
         {
            setOutput(_string);
         }

         bool Screen::isUserSure()
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
                  std::string input;
                  std::cin >> input;

                  if (input == "n" || input == "N")
                     {
                        status              = false;
                        gotExpectedResponse = true;
                     }
                  else if (input == "y" || input == "Y")
                     {
                        status              = true;
                        gotExpectedResponse = true;
                     }
               }

            return status;
         }

         Screen::~Screen()
         {
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

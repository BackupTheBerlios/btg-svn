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

#ifndef NSCREEN_H
#define NSCREEN_H

#include <vector>
#include <string>
#include <bcore/type.h>

#include <bcore/logger/logif.h>

namespace btg
{
   namespace UI
      {
         namespace cli
            {
               /**
                * \addtogroup cli
                */
               /** @{ */

               /// Implements a screen with two windows, one (small) for input, and
               /// another bigger for output. Handles input. All this is done using ncurses.
               class Screen: public btg::core::logger::logInterface
                  {
                  public:
                     /// Constructor.
                     /// @param [in] _pretend Only pretend, do no actual drawing.
                     Screen(bool const _pretend);

                     /// Handle input until the user presses enter.
                     /// @return True when user pressed enter.
                     bool getLine();

                     /// Query the user about if he is sure about
                     /// executing a command.
                     /// @return True, if user presses "Y". False if user presses "N".
                     bool isUserSure();

                     /// Get the entered line, after getKeys() returned true.
                     std::string getInput();

                     /// Write a string to the output window.
                     void setOutput(std::string const& _s);

                     /// Set text in the input window.
                     void setInput(std::string const& _s);

                     /// Write a string to the log.
                     void writeLog(std::string const& _string);

                     /// Destructor.
                     ~Screen();
                  private:
                     /// Print a string on the output window.
                     void toUser(std::string const& _s);

                     /// If true, do not output anything, only pretend.
                     bool const  pretend;

                     /// Written to the screen to init the command line.
                     std::string input_init_str;

                     /// Used by getInput() to return the last entered input.
                     std::string last_input;
                  private:
                     /// Copy constructor.
                     Screen(Screen const& _ns);
                     /// Assignment operator.
                     Screen& operator=(Screen const& _ns);
                  };

               /** @} */
            } // namespace cli
      } // namespace UI
} // namespace btg

#endif

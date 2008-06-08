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

#ifndef SCREEN_H
#define SCREEN_H

#include <iostream>
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
                     /// @param [in] _istream The stream to read the user input from
                     Screen(bool const _pretend, std::istream & _istream = std::cin);

                     /// Handle input until the user presses enter.
                     /// @return True when user pressed enter.
                     bool getLine();

                     /// Query the user about if he is sure about
                     /// executing a command.
                     /// @return True, if user presses "Y". False if user presses "N".
                     bool isUserSure();

                     /// Get the entered line, after getKeys() returned true.
                     std::string const& getInput();

                     /// Write a string to the output window.
                     void setOutput(std::string const& _s, bool const _appendNewline = true);

                     /// Write a string to the log.
                     void writeLog(std::string const& _string);
                     
                     /// Check whether the input stream is good for reading operations
                     /// @return false - impossible further reading from the stream, true - otherwise
                     /// @see std::istream::good()
                     bool good() const;

                     /// Destructor.
                     ~Screen();
                  private:
#if BTG_DEBUG
                     /// Command counter.
                     t_uint counter;
#endif
                     /// If true, do not output anything, only pretend.
                     bool const pretend;

                     /// Used by getInput() to return the last entered input.
                     std::string last_input;
                     
                     /// The stream to read user input from
                     std::istream & istream;
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

#endif // SCREEN_H

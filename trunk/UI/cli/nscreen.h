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
 * $Id: nscreen.h,v 1.11.4.10 2006/08/12 17:37:15 wojci Exp $
 */

#ifndef NSCREEN_H
#define NSCREEN_H

#include <vector>
#include <string>
#include <bcore/type.h>

#include <bcore/logger/logif.h>

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
               /**
                * \addtogroup cli
                */
               /** @{ */

               /// Implements a screen with two windows, one (small) for input, and
               /// another bigger for output. Handles input. All this is done using ncurses.
               class ncursesScreen: public btg::core::logger::logInterface
                  {
                  public:
                     /// Constructor.
                     /// @param [in] _pretend Only pretend, do no actual drawing.
                     ncursesScreen(bool const _pretend);

                     /// Events returned by the getKeys function.
                     enum EVENT
                        {
                           EVENT_KEY    = 1, //!< Got a string.
                           EVENT_NO_KEY = 2, //!< Got no string.
                           EVENT_RESIZE = 3  //!< Resize, the output should be discarded.
                        };
                     /// Handle input until the user presses enter.
                     /// @return True when user pressed enter.
                     ncursesScreen::EVENT getKeys();

                     /// Query the user about if he is sure about
                     /// executing a command.
                     /// @return True, if user presses "Y". False if user presses "N".
                     bool isUserSure();

                     /// Resize. Should be called when the screen size changes.
                     void resize();

                     /// Get the entered line, after getKeys() returned true.
                     std::string getInput();

                     /// Write a string to the output window.
                     void setOutput(std::string const& _s);

                     /// Set text in the input window.
                     void setInput(std::string const& _s);

                     /// Update all windows.
                     void refresh();

                     /// Clear the contents of all windows.
                     void clear();

                     /// Write a string to the log.
                     void writeLog(std::string const& _string);

                     /// Destructor.
                     ~ncursesScreen();
                  private:
                     /// Helper function, validate input.
                     bool inputValid(t_int const _c);

                     /// Helper function, moves the cursor to the left.
                     void moveLeft();

                     /// Helper function, moves the cursor to the right.
                     void moveRight();

                     /// Print a string on the output window.
                     void toUser(std::string const& _s);

                     /// Get the contents of the input window. Remove whitespace.
                     /// @param [out] _text  The resulting string.
                     /// @param [in]  _strip If true, strip the text.
                     bool getInputWindowText(std::string & _text, bool const _strip);

                     /// Modes. Either characters are inserted at the
                     /// position of the cursor, or the characters
                     /// overwrite the characters at the position of
                     /// the cursor.
                     enum IMODE
                        {
                           INSERT_MODE,   //!< Insert characters.
                           OVERWRITE_MODE //!< Overwrite characters.
                        };

                     /// Sets the insert mode.
                     void setInsertMode(IMODE const _mode);

                     /// Returns the insert mode.
                     IMODE getInsertMode() const;

                     /// If true, do not output anything, only pretend.
                     bool const  pretend;

                     /// The current insert mode.
                     IMODE       insert_mode;

                     /// The height of the display.
                     t_int       height;

                     /// Output window.
                     WINDOW*     main_window;

                     /// Written to the screen to init the command line.
                     std::string input_init_str;

                     /// Minimum input coord, x.
                     t_int       input_min;

                     /// Maximum input coord, x.
                     t_int       input_max;

                     /// Used internally.
                     t_strList   input;

                     /// Used by getInput() to return the last entered input.
                     std::string last_input;

                     /// Coordinate: the beginning of the input, y.
                     t_int       input_beg_y;
                     /// Coordinate: the beginning of the input, x.
                     t_int       input_beg_x;
                  private:
                     /// Copy constructor.
                     ncursesScreen(ncursesScreen const& _ns);
                     /// Assignment operator.
                     ncursesScreen& operator=(ncursesScreen const& _ns);
                  };

               /** @} */
            } // namespace cli
      } // namespace UI
} // namespace btg

#endif

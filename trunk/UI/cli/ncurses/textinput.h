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

#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include <string>
#include <vector>

#include <bcore/project.h>
#include <bcore/type_btg.h>

#include <bcore/status.h>

extern "C"
{
#include <ncurses.h>
}

#include "window.h"

namespace btg
{
   namespace UI
      {
         namespace cli
            {
	      /**
                * \addtogroup ncli
                */
               /** @{ */

               class mainWindow;

               /// Generic dialog used for entering text.
               class textInput: public baseWindow, public dialog
                  {
                  public:
                     /// Constructor.
                     textInput(keyMapping const& _kmap,
                               windowSize const& _ws,
                               std::string const& _topic,
                               std::string const& _text);
                     
                     void resize(windowSize const& _ws);

                     dialog::RESULT run();

                     /// Get the entered session name.
                     bool getText(std::string & _text) const;

                     /// Destructor.
                     virtual ~textInput();
                  private:
                     /// Size of this window.
                     windowSize  size_;

                     /// Title of the dialog.
                     std::string topic_;

                     /// Entered text.
                     std::string text_;

                     /// Line number.
                     t_uint      counter_;

                     /// Indicates if the text was changed.
                     bool        changed_;

                     /// Indicates if new text was entered.
                     bool        text_entered;

                     void refresh();

                     /// Draw the contents of this window.
                     void draw();

                     /// Add a counter topic to the window.
                     void addTopic(std::string const& _text);

                     /// Add a counter value to the window.
                     void addValue();

                     windowSize calculateDimenstions(windowSize const& _ws) const;

                     /// Enter a key.
                     void enterKey(t_uint const _key);

                     /// Delete the last entered key.
                     void deleteCurrent();

                     /// Show help.
                     /// @return True - the help window was resized, false otherwise.
                     bool showHelp();
                  };

	       /** @} */

            } // namespace cli
      } // namespace UI
} // namespace btg

#endif // TEXTINPUT_H


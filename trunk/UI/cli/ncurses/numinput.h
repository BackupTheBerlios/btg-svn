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

#ifndef NUMINPUT_H
#define NUMINPUT_H

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

               /// Dialog used to set the session name of the current
               /// session.
               class numberInputWindow: public baseWindow, public dialog
                  {
                  public:
                     /// Constructor.
                     numberInputWindow(keyMapping const& _kmap,
                                       windowSize const& _ws,
                                       std::string const& _topic,
                                       t_int const& _number,
                                       t_int const _min,
                                       t_int const _max);

                     void resize(windowSize const& _ws);

                     dialog::RESULT run();

                     bool changed() const;

                     /// Get the entered number.
                     t_int getNumber() const;

                     /// Destructor.
                     virtual ~numberInputWindow();
                  private:
                     /// Size of this window.
                     windowSize  size_;

                     /// Topic of this dialog.
                     std::string topic_;

                     /// The number entered.
                     t_int       number_;

                     /// Line number.
                     t_uint      counter_;

                     /// Indicates if the session name was changed.
                     bool        changed_;

                     std::string input_;

                     t_int const min_;
                     t_int const max_;

                     bool isNegative() const;
                     bool validate(std::string const& _input);
                     void stripLeadingZeros();

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

#endif // NUMINPUT_H


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

#ifndef BASEMENU_H
#define BASEMENU_H

#include <string>
#include <map>

#include <bcore/project.h>
#include <bcore/type_btg.h>

#include "window.h"
#include "statuswindow.h"

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

               /// A menu entry.
               class menuEntry
                  {
                  public:
                     /// Constructor.
                     menuEntry(t_uint _ID,
                               std::string _label,
                               std::string _descr);

                     /// ID.
                     t_uint      ID;
                     /// The label.
                     std::string label;
                     /// The description.
                     std::string descr;
                  };

               /// Base class for menus.
               ///
               /// One must call the run function to show the menu.
               ///
               /// \note While ncurses menu functions exist, the data
               /// they use is pure C and so its better to write a
               /// menu from scratch using windows and keyboard
               /// handling.
               class baseMenu: private baseWindow
                  {
                  public:
                     enum
                        {
                           BM_cancel = -1 //!< User cancelled menu selection.
                        };

                     /// Constructor.
                     baseMenu(keyMapping const& _kmap,
                              windowSize const& _ws,
                              std::string const& _title,
                              std::vector<menuEntry> const& _contents,
                              statusWindow & _statusWindow);

                     /// Show the menu.
                     ///
                     /// \note When K_QUIT or K_SELECT is pressed,
                     /// this function will return.
                     dialog::RESULT run();

                     /// Get the choosen element, or 0 if cancelled.
                     t_int getResult() const;

                     windowSize calculateDimenstions(windowSize const& _ws) const;

                     /// Destructor.
                     virtual ~baseMenu();
                  protected:
                     /// Size of this window.
                     windowSize             size_;

                     /// The title.
                     std::string            title_;

                     /// Menu entries.
                     std::vector<menuEntry> contents_;

                     statusWindow &         statusWindow_;

                     /// The line number of the selected text.
                     t_int                  selection_;

                     /// Indicates that something was selected.
                     bool                   selected_;

                     /// Draw the contents of this window.
                     void draw();

                     void resize(windowSize const& _ws);

                     void refresh();
                  };

	       /** @} */

            } // namespace cli
      } // namespace UI
} // namespace btg

#endif // BASEMENU_H


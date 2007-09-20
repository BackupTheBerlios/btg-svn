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
 * $Id: mainmenubar.h,v 1.13.4.7 2006/08/12 17:37:23 wojci Exp $
 */

#ifndef MAINMENUBAR_H
#define MAINMENUBAR_H

#include <gtkmm/menubar.h>
#include <bcore/type.h>

#include "sig_button_menu.h"

namespace btg
{
   namespace UI
      {
         namespace gui
            {
               /**
                * \addtogroup gui
                */
               /** @{ */

               class mainWindow;

               /// Implements a menu bar.
               class mainMenubar : public Gtk::MenuBar
                  {
                  public:
                     /// Constructor.
                     mainMenubar(mainWindow* _mainwindow);

                     /// Update the list of last opened files.
                     void updateLastFileList(t_strList const& _lastFileList);

                     /// Enable the control menu.
                     void enableControlFunction();

                     /// Disable the items used to control contexts.
                     /// \note There is no reason for them to be enabled
                     /// when there are no contexts.
                     void disableControlFunction(bool const _force = false);

                     /// Destructor.
                     virtual ~mainMenubar();
                  private:
                     /// The menuitem which is the control menu.
                     Gtk::MenuItem*              controlMenuItem;

                     /// Indicates if the control menu is enabled. It
                     /// can be disabled if no contexts are selected.
                     bool                        controlFunctionEnabled;

                     /// Seperator after the list of last files.
                     Gtk::MenuItem*              lastFiles_separator;

                     /// List of menu items representing last opened files.
                     std::vector<Gtk::MenuItem*> lastFiles;

                     /// Item used to open all last opened files.
                     Gtk::MenuItem*              open_all_last;

                     /// Item in the control menu.
                     Gtk::MenuItem*              startMenuitem;
                     /// Item in the control menu.
                     Gtk::MenuItem*              stopMenuitem;
                     /// Item in the control menu.
                     Gtk::MenuItem*              abortMenuitem;
                     /// Item in the control menu.
                     Gtk::MenuItem*              eraseMenuitem;
                     /// Item in the control menu.
                     Gtk::MenuItem*              cleanMenuitem;
                     /// Item in the control menu.
                     Gtk::MenuItem*              limitMenuitem;
                  private:
                     /// Copy constructor.
                     mainMenubar(mainMenubar const& _mm);
                     /// Assignment operator.
                     mainMenubar& operator=(mainMenubar const& _mm);
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif


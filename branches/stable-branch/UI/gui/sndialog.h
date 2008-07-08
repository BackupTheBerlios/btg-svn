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

#ifndef SESSIONNAMEDIALOG_H
#define SESSIONNAMEDIALOG_H

#include <gtkmm/dialog.h>

namespace Gtk
{
   class Button;
   class Label;
   class Entry;
   class Table;
}

namespace btg
{
   namespace UI
      {
         namespace gui
            {
               /// Dialog which allows the user to set the name of the
               /// session.
               class sessionNameDialog: public Gtk::Dialog
                  {
                  public:
                     /// Constructor.
                     sessionNameDialog(std::string const& _name);

                     /// Get the name of the session.
                     std::string getName() const;

                     /// Destructor.
                     virtual ~sessionNameDialog();
                  private:
                     /// Cancal button.
                     Gtk::Button* cancelbutton;
                     /// Ok button.
                     Gtk::Button* okbutton;
                     /// Label.
                     Gtk::Label*  nameLabel;

                     /// Name entry.
                     Gtk::Entry*  nameEntry;
                     /// Table used for layout.
                     Gtk::Table*  table;

                     /// Callback.
                     void on_button_pressed(int _button);

                     /// Callback.
                     void name_changed();

                  private:
                     /// Copy constructor.
                     sessionNameDialog(sessionNameDialog const& _ad);
                     /// Assignment operator.
                     sessionNameDialog& operator=(sessionNameDialog const& _ad);
                  };

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif // SESSIONNAMEDIALOG_H

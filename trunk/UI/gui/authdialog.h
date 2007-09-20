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
 * $Id: authdialog.h,v 1.1.4.4 2006/08/12 17:37:22 wojci Exp $
 */

#ifndef AUTHDIALOG_H
#define AUTHDIALOG_H

#include <gtkmm/dialog.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/table.h>
#include <gtkmm/box.h>

namespace btg
{
   namespace UI
      {
         namespace gui
            {

               /// Dialog which queries the user for username and
               /// password.
               class authDialog: public Gtk::Dialog
                  {
                  public:
                     /// Constructor.
                     authDialog();

                     /// Get the username and password. Return true if
                     /// the operations succeds.
                     bool getUsernameAndPassword(std::string & _username, std::string & _password);

                     /// Destructor.
                     virtual ~authDialog();
                  private:
                     /// Cancal button.
                     Gtk::Button* cancelbutton;
                     /// Ok button.
                     Gtk::Button* okbutton;
                     /// Label.
                     Gtk::Label*  userNameLabel;
                     /// Label.
                     Gtk::Label*  passwordLabel;
                     /// Text entry, use to enter password.
                     Gtk::Entry*  passwordEntry;
                     /// Text entry, used to enter username.
                     Gtk::Entry*  userNameEntry;
                     /// Table used for layout.
                     Gtk::Table*  table;

                     /// Callback.
                     void on_button_pressed(int _button);

                     /// Username was set.
                     bool username_ok;

                     /// Password was set.
                     bool password_ok;

                     /// Callback, called when the text entry used for the username was changed.
                     void username_changed();

                     /// Callback, called when the text entry used for the password was changed.
                     void password_changed();
                  private:
                     /// Copy constructor.
                     authDialog(authDialog const& _ad);
                     /// Assignment operator.
                     authDialog& operator=(authDialog const& _ad);
                  };

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif // AUTHDIALOG_H

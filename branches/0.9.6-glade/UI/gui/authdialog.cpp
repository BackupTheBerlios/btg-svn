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

#include "authdialog.h"

#include <bcore/logmacro.h>

namespace btg
{
   namespace UI
   {
      namespace gui
      {
         int const button_cancel = 1;
         int const button_ok     = 2;

         authDialog::authDialog()
            : cancelbutton(0),
              okbutton(0),
              userNameLabel(0),
              passwordLabel(0),
              passwordEntry(0),
              userNameEntry(0),
              table(0),
              username_ok(false),
              password_ok(false)
         {
            userNameLabel = Gtk::manage(new class Gtk::Label("User name"));
            passwordLabel = Gtk::manage(new class Gtk::Label("Password"));
            passwordEntry = Gtk::manage(new class Gtk::Entry());
            userNameEntry = Gtk::manage(new class Gtk::Entry());
            table         = Gtk::manage(new class Gtk::Table(2, 2, false));

            cancelbutton  = Gtk::manage(new class Gtk::Button("Cancel"));
            okbutton      = Gtk::manage(new class Gtk::Button("OK"));

            okbutton->set_flags(Gtk::CAN_FOCUS);
            okbutton->set_relief(Gtk::RELIEF_NORMAL);

            cancelbutton->set_flags(Gtk::CAN_FOCUS);
            cancelbutton->set_relief(Gtk::RELIEF_NORMAL);

            this->get_action_area()->property_layout_style().set_value(Gtk::BUTTONBOX_END);

            userNameLabel->set_alignment(0,0.5);
            userNameLabel->set_padding(15,15);
            userNameLabel->set_justify(Gtk::JUSTIFY_LEFT);
            userNameLabel->set_line_wrap(false);
            userNameLabel->set_use_markup(false);
            userNameLabel->set_selectable(false);

            passwordLabel->set_alignment(0,0.5);
            passwordLabel->set_padding(15,15);
            passwordLabel->set_justify(Gtk::JUSTIFY_LEFT);
            passwordLabel->set_line_wrap(false);
            passwordLabel->set_use_markup(false);
            passwordLabel->set_selectable(false);

            passwordEntry->set_flags(Gtk::CAN_FOCUS);
            passwordEntry->set_visibility(false);
            passwordEntry->set_invisible_char('*');
            passwordEntry->set_editable(true);
            passwordEntry->set_max_length(50);
            passwordEntry->set_text("");
            passwordEntry->set_has_frame(true);
            passwordEntry->set_activates_default(false);

            userNameEntry->set_flags(Gtk::CAN_FOCUS);
            userNameEntry->set_visibility(true);
            userNameEntry->set_editable(true);
            userNameEntry->set_max_length(50);
            userNameEntry->set_text("");
            userNameEntry->set_has_frame(true);
            userNameEntry->set_activates_default(false);

            table->set_row_spacings(0);
            table->set_col_spacings(0);
            table->attach(*userNameLabel, 0, 1, 0, 1, Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            table->attach(*passwordLabel, 0, 1, 1, 2, Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            table->attach(*passwordEntry, 1, 2, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            table->attach(*userNameEntry, 1, 2, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::AttachOptions(), 0, 0);

            this->get_vbox()->set_homogeneous(false);
            this->get_vbox()->set_spacing(0);
            this->get_vbox()->pack_start(*table);
            this->set_title("Authorization Required");
            this->set_modal(true);
            this->property_window_position().set_value(Gtk::WIN_POS_CENTER);
            this->set_resizable(true);
            this->property_destroy_with_parent().set_value(false);
            this->set_has_separator(true);

            userNameLabel->show();
            passwordLabel->show();
            passwordEntry->show();
            userNameEntry->show();
            table->show();

            // okbutton->show();
            cancelbutton->show();

            this->add_action_widget(*okbutton, -5);
            this->add_action_widget(*cancelbutton, -10);

            // Create buttons and connect their signals.
            userNameEntry->signal_changed().connect(sigc::mem_fun(*this, &authDialog::username_changed));
            passwordEntry->signal_changed().connect(sigc::mem_fun(*this, &authDialog::password_changed));

            this->show();
         }

         void authDialog::on_button_pressed(int _button)
         {
            switch(_button)
               {
               case button_cancel:
                  {
                     break;
                  }
               case button_ok:
                  {
                     if (username_ok && password_ok)
                        {
                           BTG_NOTICE("valid");
                           this->hide();
                        }
                     break;
                  }
               default:
                  {
                     BTG_NOTICE("invalid");
                  }
               }

            this->hide();
         }

         void authDialog::username_changed()
         {
            std::string text = userNameEntry->get_text();

            if (text.size() > 0)
               {
                  username_ok = true;
               }
            else
               {
                  username_ok = false;
               }

            if (username_ok && password_ok)
               {
                  okbutton->show();
               }
            else
               {
                  okbutton->hide();
               }
         }

         void authDialog::password_changed()
         {
            std::string text = passwordEntry->get_text();

            if (text.size() > 0)
               {
                  password_ok = true;
               }
            else
               {
                  password_ok = false;
               }

            if (username_ok && password_ok)
               {
                  okbutton->show();
               }
            else
               {
                  okbutton->hide();
               }
         }

         bool authDialog::getUsernameAndPassword(std::string & _username, std::string & _password)
         {
            bool status = false;

            if (username_ok && password_ok)
               {
                  status     = true;
                  _username  = userNameEntry->get_text();
                  _password  = passwordEntry->get_text();
               }

            return status;
         }

         authDialog::~authDialog()
         {
         }

      } // namespace gui
   } // namespace UI
} // namespace btg

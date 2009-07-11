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

#include "sndialog.h"

#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/table.h>
#include <gtkmm/box.h>

#include <bcore/logmacro.h>

namespace btg
{
   namespace UI
   {
      namespace gui
      {
         int const button_cancel = 1;
         int const button_ok     = 2;

         sessionNameDialog::sessionNameDialog(std::string const& _name)
            : cancelbutton(0),
              okbutton(0),
              nameLabel(0),
              nameEntry(0),
              table(0)
         {
            nameLabel = Gtk::manage(new class Gtk::Label("Session name"));
            nameEntry = Gtk::manage(new class Gtk::Entry());
            table         = Gtk::manage(new class Gtk::Table(2, 2, false));

            cancelbutton  = Gtk::manage(new class Gtk::Button("Cancel"));
            okbutton      = Gtk::manage(new class Gtk::Button("OK"));

            okbutton->set_flags(Gtk::CAN_FOCUS);
            okbutton->set_relief(Gtk::RELIEF_NORMAL);

            cancelbutton->set_flags(Gtk::CAN_FOCUS);
            cancelbutton->set_relief(Gtk::RELIEF_NORMAL);

            this->get_action_area()->property_layout_style().set_value(Gtk::BUTTONBOX_END);

            nameLabel->set_alignment(0,0.5);
            nameLabel->set_padding(15,15);
            nameLabel->set_justify(Gtk::JUSTIFY_LEFT);
            nameLabel->set_line_wrap(false);
            nameLabel->set_use_markup(false);
            nameLabel->set_selectable(false);

            nameEntry->set_flags(Gtk::CAN_FOCUS);
            nameEntry->set_visibility(true);
            nameEntry->set_editable(true);
            nameEntry->set_max_length(50);
            nameEntry->set_text(_name);
            nameEntry->set_has_frame(true);
            nameEntry->set_activates_default(false);

            table->set_row_spacings(0);
            table->set_col_spacings(0);
            table->attach(*nameLabel, 0, 1, 0, 1, Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            table->attach(*nameEntry, 1, 2, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::AttachOptions(), 0, 0);

            this->get_vbox()->set_homogeneous(false);
            this->get_vbox()->set_spacing(0);
            this->get_vbox()->pack_start(*table);
            this->set_title("Change session name");
            this->set_modal(true);
            this->property_window_position().set_value(Gtk::WIN_POS_CENTER);
            this->set_resizable(true);
            this->property_destroy_with_parent().set_value(false);
            this->set_has_separator(true);

            nameLabel->show();
            nameEntry->show();
            table->show();

            // okbutton->show();
            cancelbutton->show();

            this->add_action_widget(*okbutton, -5);
            this->add_action_widget(*cancelbutton, -10);

            // Create buttons and connect their signals.
            nameEntry->signal_changed().connect(sigc::mem_fun(*this, &sessionNameDialog::name_changed));
            this->show();
         }

         void sessionNameDialog::on_button_pressed(int _button)
         {
            /*
            switch(_button)
               {
               case button_cancel:
                  {
                     break;
                  }
               case button_ok:
                  {
                     break;
                  }
               default:
                  {
                     BTG_NOTICE("invalid");
                  }
               }
            */

            this->hide();
         }

         void sessionNameDialog::name_changed()
         {
            std::string text = nameEntry->get_text();

            if (text.size() > 0)
               {
                  okbutton->show();
               }
            else
               {
                  okbutton->hide();
               }
         }

         std::string sessionNameDialog::getName() const
         {
            return nameEntry->get_text();
         }

         sessionNameDialog::~sessionNameDialog()
         {
         }

      } // namespace gui
   } // namespace UI
} // namespace btg

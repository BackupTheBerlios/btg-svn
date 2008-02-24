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

#include "errordialog.h"
#include "gtkmm_ver.h"

#include <gdk/gdkkeysyms.h>
#include <gtkmm/accelgroup.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/label.h>
#include <gtkmm/textview.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         errorDialog::errorDialog(std::string const& _messageToShow)
         {
            Gtk::Button   *closeButton = Gtk::manage(new class Gtk::Button(Gtk::StockID("gtk-close")));
            Gtk::Label    *titleLabel = Gtk::manage(new class Gtk::Label("Btg Error Notice"));
            Gtk::TextView *errorMessageTextview = Gtk::manage(new class Gtk::TextView());
            Gtk::ScrolledWindow *textViewScrolledwindow = Gtk::manage(new class Gtk::ScrolledWindow());
            Gtk::VBox           *contentsVbox = Gtk::manage(new class Gtk::VBox(false, 0));
            closeButton->set_flags(Gtk::CAN_FOCUS);
            closeButton->set_flags(Gtk::CAN_DEFAULT);
            closeButton->set_relief(Gtk::RELIEF_NORMAL);

            get_action_area()->property_layout_style().set_value(Gtk::BUTTONBOX_END);

            titleLabel->set_alignment(0.5,0.5);
            titleLabel->set_padding(0,0);
            titleLabel->set_justify(Gtk::JUSTIFY_LEFT);
            titleLabel->set_line_wrap(false);
            titleLabel->set_use_markup(false);
            titleLabel->set_selectable(false);

            errorMessageTextview->set_flags(Gtk::CAN_FOCUS);
            errorMessageTextview->set_editable(false);
            errorMessageTextview->set_cursor_visible(false);
            errorMessageTextview->set_pixels_above_lines(10);
            errorMessageTextview->set_pixels_below_lines(10);
            errorMessageTextview->set_pixels_inside_wrap(0);
            errorMessageTextview->set_left_margin(10);
            errorMessageTextview->set_right_margin(10);
            errorMessageTextview->set_indent(5);
            errorMessageTextview->set_wrap_mode(Gtk::WRAP_WORD);
            errorMessageTextview->set_justification(Gtk::JUSTIFY_LEFT);

            // Sets the text which this dialog is showing.
            errorMessageTextview->get_buffer()->set_text(_messageToShow);

            textViewScrolledwindow->set_flags(Gtk::CAN_FOCUS);
            textViewScrolledwindow->set_shadow_type(Gtk::SHADOW_IN);
            textViewScrolledwindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
            textViewScrolledwindow->property_window_placement().set_value(Gtk::CORNER_TOP_LEFT);
            textViewScrolledwindow->add(*errorMessageTextview);

            contentsVbox->set_size_request(500,100);
            contentsVbox->pack_start(*titleLabel, Gtk::PACK_SHRINK, 0);
            contentsVbox->pack_start(*textViewScrolledwindow, Gtk::PACK_SHRINK, 0);

            get_vbox()->set_homogeneous(false);
            get_vbox()->set_spacing(0);
            get_vbox()->pack_start(*contentsVbox, Gtk::PACK_SHRINK, 0);
            set_border_width(5);
            set_title("Error notice");
            set_modal(true);
            property_window_position().set_value(Gtk::WIN_POS_CENTER);
            set_resizable(false);
            property_destroy_with_parent().set_value(false);
            set_has_separator(true);
            add_action_widget(*closeButton, -7);

            closeButton->show();
            titleLabel->show();
            errorMessageTextview->show();
            textViewScrolledwindow->show();
            contentsVbox->show();
            show();
            // closeButton->signal_clicked().connect(sigc::slot(*this, &errorDialog::on_closebutton_clicked), false);

            // Run untill button is pressed.
            run();
         }

         void errorDialog::showAndDie(std::string const& _messageToShow)
         {
            errorDialog* ed = new errorDialog(_messageToShow);
            delete ed;
            ed = 0;
         }

         void errorDialog::on_closebutton_clicked()
         {

         }

         errorDialog::~errorDialog()
         {
   
         }

         /** @} */

      } // namespace gui
   } // namespace UI
} // namespace btg

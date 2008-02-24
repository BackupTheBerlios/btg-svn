#include <gdk/gdkkeysyms.h>
#include <gtkmm/accelgroup.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/label.h>
#include <gtkmm/textview.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>

#include <string>

#include <bcore/project.h>
#include "aboutdialog.h"

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         aboutDialog::aboutDialog()
         {
            Gtk::Dialog *ad = this;

            Gtk::Button *okbutton    = Gtk::manage(new class Gtk::Button("ok"));
            Gtk::Label *aboutLabel   = Gtk::manage(new class Gtk::Label( "About " + GPD->sGUI_CLIENT() + " " + GPD->sFULLVERSION() + ", build " + GPD->sBUILD() ));
            Gtk::TextView *textview  = Gtk::manage(new class Gtk::TextView());
            Gtk::ScrolledWindow *scrolledwindow = Gtk::manage(new class Gtk::ScrolledWindow());
            Gtk::VBox *vbox                     = Gtk::manage(new class Gtk::VBox(false, 0));

            okbutton->set_flags(Gtk::CAN_FOCUS);
            okbutton->set_relief(Gtk::RELIEF_NORMAL);

            ad->get_action_area()->property_layout_style().set_value(Gtk::BUTTONBOX_END);
            ad->set_default_size(300, 200);

            aboutLabel->set_alignment(0.5,0.5);
            aboutLabel->set_padding(0,0);
            aboutLabel->set_justify(Gtk::JUSTIFY_LEFT);
            aboutLabel->set_line_wrap(false);
            aboutLabel->set_use_markup(false);
            aboutLabel->set_selectable(false);

            textview->set_flags(Gtk::CAN_FOCUS);
            textview->set_editable(true);
            textview->set_cursor_visible(true);
            textview->set_pixels_above_lines(0);
            textview->set_pixels_below_lines(0);
            textview->set_pixels_inside_wrap(0);
            textview->set_left_margin(0);
            textview->set_right_margin(0);
            textview->set_indent(0);
            textview->set_wrap_mode(Gtk::WRAP_NONE);
            textview->set_justification(Gtk::JUSTIFY_LEFT);

            std::string text;
            text += "btg Copyright (C) 2005 Michael Wojciechowski.";
            text += GPD->sNEWLINE();
            text += "This program is free software; you can redistribute it and/or modify";
            text += GPD->sNEWLINE();
            text += "it under the terms of the GNU General Public License as published by ";
            text += GPD->sNEWLINE();
            text += "the Free Software Foundation; either version 2 of the License, or ";
            text += GPD->sNEWLINE();
            text += "(at your option) any later version.";
            text += GPD->sNEWLINE();
            text += "This program is distributed in the hope that it will be useful, ";
            text += GPD->sNEWLINE();
            text += "but WITHOUT ANY WARRANTY; without even the implied warranty of ";
            text += GPD->sNEWLINE();
            text += "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the ";
            text += GPD->sNEWLINE();
            text += "GNU General Public License for more details.";
            text += GPD->sNEWLINE();
            text += "You should have received a copy of the GNU General Public License ";
            text += GPD->sNEWLINE();
            text += "along with this program; if not, write to the Free Software ";
            text += GPD->sNEWLINE();
            text += "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA";
            text += GPD->sNEWLINE();

            textview->get_buffer()->set_text(text);

            scrolledwindow->set_flags(Gtk::CAN_FOCUS);
            scrolledwindow->set_shadow_type(Gtk::SHADOW_IN);
            scrolledwindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
            scrolledwindow->property_window_placement().set_value(Gtk::CORNER_TOP_LEFT);
            scrolledwindow->add(*textview);
            vbox->pack_start(*aboutLabel, Gtk::PACK_SHRINK, 0);
            vbox->pack_start(*scrolledwindow);
            ad->get_vbox()->set_homogeneous(false);
            ad->get_vbox()->set_spacing(0);
            ad->get_vbox()->pack_start(*vbox);
            ad->set_title( GPD->sGUI_CLIENT() + " " + GPD->sFULLVERSION() + " / About" );
            ad->set_modal(true);
            ad->property_window_position().set_value(Gtk::WIN_POS_CENTER);
            ad->set_resizable(true);
            ad->property_destroy_with_parent().set_value(false);
            ad->set_has_separator(true);
            ad->add_action_widget(*okbutton, -5);
            okbutton->show();
            aboutLabel->show();
            textview->show();
            scrolledwindow->show();
            vbox->show();

            okbutton->signal_clicked().connect(sigc::mem_fun(*this, &aboutDialog::on_ok_clicked));
            // ad->show();
         }

         void aboutDialog::on_ok_clicked()
         {
            hide();
         }

         aboutDialog::~aboutDialog()
         {

         }

      } // namespace gui
   } // namespace UI
} // namespace btg

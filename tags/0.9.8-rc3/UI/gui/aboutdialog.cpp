#include <gdk/gdkkeysyms.h>
#include <gtkmm/accelgroup.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/label.h>
#include <gtkmm/textview.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/image.h>

#include <string>

#include <bcore/project.h>
#include "aboutdialog.h"
#include "aboutimage.h"

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         aboutDialog::aboutDialog()
         {
            Gtk::Button* okbutton    = Gtk::manage(new class Gtk::Button("ok"));
            Gtk::Label* aboutLabel   = Gtk::manage(new class Gtk::Label( "About " + btg::core::projectDefaults::sGUI_CLIENT() + " " + btg::core::projectDefaults::sFULLVERSION() + ", build " + btg::core::projectDefaults::sBUILD() ));
            Gtk::TextView* textview  = Gtk::manage(new class Gtk::TextView());
            Gtk::ScrolledWindow* scrolledwindow = Gtk::manage(new class Gtk::ScrolledWindow());
            Gtk::VBox* vbox                     = Gtk::manage(new class Gtk::VBox(false, 0));
            Glib::RefPtr<Gdk::Pixbuf> pixbufRef = Gdk::Pixbuf::create_from_xpm_data(aboutimage_xpm);
            Gtk::Image* image                   = Gtk::manage(new Gtk::Image(pixbufRef));

            okbutton->set_flags(Gtk::CAN_FOCUS);
            okbutton->set_relief(Gtk::RELIEF_NORMAL);

            get_action_area()->property_layout_style().set_value(Gtk::BUTTONBOX_END);
            set_default_size(400, 500);

            aboutLabel->set_alignment(0.5,0.5);
            aboutLabel->set_padding(0,0);
            aboutLabel->set_justify(Gtk::JUSTIFY_LEFT);
            aboutLabel->set_line_wrap(false);
            aboutLabel->set_use_markup(false);
            aboutLabel->set_selectable(false);

            textview->set_flags(Gtk::CAN_FOCUS);
            textview->set_editable(false);
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
            text += "btg Copyright (C) 2005 Michael Wojciechowski.\n";
            text += "This program is free software; you can redistribute it and/or modify\n";
            text += "it under the terms of the GNU General Public License as published by \n";
            text += "the Free Software Foundation; either version 2 of the License, or \n";
            text += "(at your option) any later version.\n";
            text += "This program is distributed in the hope that it will be useful, \n";
            text += "but WITHOUT ANY WARRANTY; without even the implied warranty of \n";
            text += "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \n";
            text += "GNU General Public License for more details.\n";
            text += "You should have received a copy of the GNU General Public License \n";
            text += "along with this program; if not, write to the Free Software \n";
            text += "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n";

            textview->get_buffer()->set_text(text);

            scrolledwindow->set_flags(Gtk::CAN_FOCUS);
            scrolledwindow->set_shadow_type(Gtk::SHADOW_IN);
            scrolledwindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
            scrolledwindow->property_window_placement().set_value(Gtk::CORNER_TOP_LEFT);
            scrolledwindow->add(*textview);

            vbox->pack_start(*aboutLabel, Gtk::PACK_SHRINK, 0);
            vbox->pack_start(*image);
            vbox->pack_start(*scrolledwindow);
            
            get_vbox()->set_homogeneous(false);
            get_vbox()->set_spacing(0);
            get_vbox()->pack_start(*vbox);
            set_title( btg::core::projectDefaults::sGUI_CLIENT() + " " + btg::core::projectDefaults::sFULLVERSION() + " / About" );
            set_modal(true);
            property_window_position().set_value(Gtk::WIN_POS_CENTER);
            set_resizable(true);
            property_destroy_with_parent().set_value(false);
            set_has_separator(true);
            add_action_widget(*okbutton, -5);
            okbutton->show();
            aboutLabel->show();
            textview->show();
            scrolledwindow->show();
            image->show();
            vbox->show();

            okbutton->signal_clicked().connect(sigc::mem_fun(*this, &aboutDialog::on_ok_clicked));
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

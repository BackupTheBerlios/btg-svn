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
 * $Id: aboutdialog.h 10 2007-09-22 14:18:40Z wojci $
 */

#include "urldialog.h"
#include <bcore/project.h>
#include <bcore/client/urlhelper.h>

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         urlDialog::urlDialog(std::string const& _url,
                              std::string const& _filename)
            : url_filename_filled(false),
              filename_filled(false)
         {  
            label1 = Gtk::manage(new class Gtk::Label("Enter URL and filename"));
            file_entry = Gtk::manage(new class Gtk::Entry());
            if (_filename != "")
               {
                  file_entry->set_text(_filename);
               }

            url_entry = Gtk::manage(new class Gtk::Entry());
            if (_url != "")
               {
                  url_entry->set_text(_url);
               }

            fn_label = Gtk::manage(new class Gtk::Label("Filename"));
            url_label = Gtk::manage(new class Gtk::Label("URL"));
            table1 = Gtk::manage(new class Gtk::Table(2, 2, true));
            vbox1 = Gtk::manage(new class Gtk::VBox(false, 0));
            cancelbutton = Gtk::manage(new class Gtk::Button(Gtk::StockID("gtk-cancel")));
            applybutton = Gtk::manage(new class Gtk::Button(Gtk::StockID("gtk-apply")));
            label1->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
            file_entry->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
            file_entry->set_flags(Gtk::CAN_FOCUS);
            file_entry->set_max_length(512);
            url_entry->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
            url_entry->set_flags(Gtk::CAN_FOCUS);
            url_entry->set_max_length(512);
            fn_label->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
            url_label->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
            table1->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
            table1->attach(*url_label,  0, 1, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::EXPAND|Gtk::FILL, 0, 0);
            table1->attach(*url_entry,  1, 2, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::EXPAND|Gtk::FILL, 0, 0);
            table1->attach(*fn_label,   0, 1, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::EXPAND|Gtk::FILL, 0, 0);
            table1->attach(*file_entry, 1, 2, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::EXPAND|Gtk::FILL, 0, 0);

            vbox1->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
            vbox1->pack_start(*label1);
            vbox1->pack_start(*table1, Gtk::PACK_EXPAND_PADDING, 0);
            cancelbutton->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
            cancelbutton->set_flags(Gtk::CAN_FOCUS);
            applybutton->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
            applybutton->set_flags(Gtk::CAN_FOCUS);
            get_action_area()->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
            get_action_area()->property_layout_style().set_value(Gtk::BUTTONBOX_CENTER);
            get_vbox()->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
            get_vbox()->set_spacing(2);
            get_vbox()->pack_start(*vbox1);
            set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
            set_border_width(5);
            property_window_position().set_value(Gtk::WIN_POS_CENTER_ON_PARENT);
            set_has_separator(false);
            add_action_widget(*cancelbutton, Gtk::RESPONSE_CANCEL);
            add_action_widget(*applybutton, Gtk::RESPONSE_APPLY);
            label1->show();
            file_entry->show();
            url_entry->show();
            fn_label->show();
            url_label->show();
            table1->show();
            vbox1->show();
            cancelbutton->show();
            // Start hidden, show when something has been entered in
            // both entries.
            applybutton->hide();

            file_entry->signal_changed().connect(sigc::mem_fun(*this, &urlDialog::file_changed));
            url_entry->signal_changed().connect(sigc::mem_fun(*this, &urlDialog::url_changed));

            set_title( GPD->sGUI_CLIENT() + " " + GPD->sFULLVERSION() + " / URL" );
            set_modal(true);
            property_window_position().set_value(Gtk::WIN_POS_CENTER);
            set_resizable(true);

            check_contents();

            show();
         }

         bool urlDialog::GetResult(std::string & _url,
                                   std::string & _filename)
         {
            _url      = url_entry->get_text();
            _filename = file_entry->get_text();

            return url_filename_filled;
         }

         void urlDialog::file_changed()
         {
            filename_filled = true;
            check_contents();
         }

         void urlDialog::url_changed()
         {
            check_contents();
         }
            
         void urlDialog::check_contents()
         {
            const std::string u  = url_entry->get_text();
            std::string f        = file_entry->get_text();

            if (!btg::core::client::isUrlValid(u))
               {
                  url_filename_filled = false;
                  applybutton->hide();
                  return;
               }

            if (!filename_filled)
               {
                  std::string url_filename;
                  if (btg::core::client::getFilenameFromUrl(u, url_filename))
                     {
                        file_entry->set_text(url_filename);
                        f = file_entry->get_text();
                     }
               }

            if ((f.size() > 0) && (u.size() > 0))
               {
                  url_filename_filled = true;
                  applybutton->show();
               }
            else
               {
                  url_filename_filled = false;
                  applybutton->hide();
               }
         }


         urlDialog::~urlDialog()
         {

         }

      } // namespace gui
   } // namespace UI
} // namespace btg

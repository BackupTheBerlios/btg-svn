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


#include <bcore/project.h>

#include "gtkmm_ver.h"

#include "progressdialog.h"

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         progressDialog::progressDialog(std::string _title)
            : title_label(0),
              label(0),
              progressbar(0),
              vbox(0)
         {
            title_label = Gtk::manage(new class Gtk::Label(_title));
            label       = Gtk::manage(new class Gtk::Label());
            progressbar = Gtk::manage(new class Gtk::ProgressBar());
            vbox        = get_vbox();

            label->set_alignment(0.5,0.5);
            label->set_padding(0,5);
            label->set_justify(Gtk::JUSTIFY_LEFT);
            label->set_line_wrap(false);
            label->set_use_markup(false);
            label->set_selectable(false);
#if GTKMM_2_6_OR_BETTER
            label->set_max_width_chars(50);
#endif // GTKMM_2_6_OR_BETTER

            title_label->set_alignment(0.5,0.5);
            title_label->set_padding(0,5);
            title_label->set_justify(Gtk::JUSTIFY_LEFT);
            title_label->set_line_wrap(false);
            title_label->set_use_markup(false);
            title_label->set_selectable(false);
#if GTKMM_2_6_OR_BETTER
            title_label->set_max_width_chars(50);
#endif // GTKMM_2_6_OR_BETTER

            vbox->pack_start(*title_label, Gtk::PACK_SHRINK, 0);
            vbox->pack_start(*label,       Gtk::PACK_SHRINK, 0);
            vbox->pack_start(*progressbar, Gtk::PACK_SHRINK, 0);

            property_window_position().set_value(Gtk::WIN_POS_CENTER);
            set_resizable(true);
            property_destroy_with_parent().set_value(false);
            set_has_separator(false);

            set_border_width(20);
            set_title(_title);
            set_modal(true);
            set_resizable(false);
            property_destroy_with_parent().set_value(false);

            title_label->show();
            label->show();
            progressbar->show();
            vbox->show();
         }

         void progressDialog::setText(std::string const& _text)
         {
            label->set_text(_text);
         }
            
         void progressDialog::updateProgress(t_uint const _percent, 
                                             std::string const& _text)
         {
            double d = _percent / 100.0;
            progressbar->set_fraction(d);
            setText(_text);

            // Enable a time out.
            this->setTimeout();
            this->run();
         }

         void progressDialog::setTimeout()
         {
            // Create/connect a timer used for updating the list of contexts.
            sigc::slot<bool> slot = sigc::bind(sigc::mem_fun(*this, &progressDialog::on_refresh_timeout), 0);
            Glib::signal_timeout().connect(slot, 64);
         }

         bool progressDialog::on_refresh_timeout(int _timeout)
         {
            this->response(1);
            return false;
         }

         progressDialog::~progressDialog()
         {

         }

         /** @} */

      } // namespace gui
   } // namespace UI
} // namespace btg

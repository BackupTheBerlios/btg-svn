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

#include "initwindow.h"

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         initWindow::initWindow()
            : title_label(0),
              label(0),
              progressbar(0),
              vbox(0)
         {
            title_label = Gtk::manage(new class Gtk::Label(
                                                           GPD->sGUI_CLIENT() + " " + GPD->sVERSION() +
                                                           ", build " + GPD->sBUILD()
                                                           )
                                      );
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
            set_title(
                      GPD->sGUI_CLIENT() + " " + GPD->sVERSION() +
                      ", build " + GPD->sBUILD() + " - initializing .."
                      );
            set_modal(true);
            set_resizable(false);
            property_destroy_with_parent().set_value(false);

            title_label->show();
            label->show();
            progressbar->show();
            vbox->show();
         }

         void initWindow::setText(std::string const& _text)
         {
            label->set_text(_text);
         }

         void initWindow::updateProgress(INITEVENT _event)
         {
            switch(_event)
               {
               case IEV_START:
                  progressbar->set_fraction(0.0);
                  setText("Started initialization");
                  break;
               case IEV_RCONF:
                  progressbar->set_fraction(0.15);
                  setText("Reading config");
                  break;
               case IEV_RCONF_DONE:
                  progressbar->set_fraction(0.30);
                  setText("Config read");
                  break;
               case IEV_CLA:
                  progressbar->set_fraction(0.45);
                  setText("Parsing arguments");
                  break;
               case IEV_CLA_DONE:
                  progressbar->set_fraction(0.50);
                  setText("Arguments parsed");
                  break;
               case IEV_TRANSP:
                  progressbar->set_fraction(0.65);
                  setText("Creating transport");
                  break;
               case IEV_TRANSP_DONE:
                  progressbar->set_fraction(0.75);
                  setText("Transport created");
                  break;
               case IEV_SETUP:
                  progressbar->set_fraction(0.85);
                  setText("Registering");
                  break;
               case IEV_SETUP_DONE:
                  progressbar->set_fraction(0.95);
                  setText("Registered");
                  break;
               case IEV_END:
                  progressbar->set_fraction(1.0);
                  setText("Initialization done");
                  break;
               }

            // Enable a time out.

            this->setTimeout();

            this->run();
         }

         void initWindow::setTimeout()
         {
            // Create/connect a timer used for updating the list of contexts.
            sigc::slot<bool> slot = sigc::bind(sigc::mem_fun(*this, &initWindow::on_refresh_timeout), 0);
            Glib::signal_timeout().connect(slot, 64);
         }

         bool initWindow::on_refresh_timeout(int _timeout)
         {
            this->response(1);
            return false;
         }

         initWindow::~initWindow()
         {

         }

         /** @} */

      } // namespace gui
   } // namespace UI
} // namespace btg

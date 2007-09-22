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

#include "limitdialog.h"
#include <gdk/gdkkeysyms.h>
#include <gtkmm/accelgroup.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/table.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/adjustment.h>

#include <bcore/project.h>

#include <string>
#include <bcore/t_string.h>

#include <bcore/logmacro.h>

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         limitDialog::limitDialog()
            : limit_interval(5),
              limit_selected(false),
              selected_upload_disable(false),
              selected_download_disable(false),
              selected_seed_percent_disable(false),
              selected_seed_timer_disable(false),
              uploadCombo(0),
              downloadCombo(0),
              seedPercentSpin(0),
              seedTimeSpin(0),
              selected_upload_limit(-1),
              selected_download_limit(-1)
         {
            Gtk::Label *uploadLabel   = Gtk::manage(new class Gtk::Label("Upload"));
            Gtk::Label *downloadLabel = Gtk::manage(new class Gtk::Label("Download"));
            uploadCombo               = Gtk::manage(new class Gtk::ComboBoxText());
            downloadCombo             = Gtk::manage(new class Gtk::ComboBoxText());

            Gtk::Label *seedPercentLabel = Gtk::manage(new class Gtk::Label("Seed %"));
            Gtk::Label *seedTimeLabel    = Gtk::manage(new class Gtk::Label("Seed Time"));

            Gtk::Adjustment *seedPercentAdjustment = Gtk::manage(new class Gtk::Adjustment(1, 0, 100, 1, 10, 10));
            seedPercentSpin = Gtk::manage(new class Gtk::SpinButton(*seedPercentAdjustment, 1, 0));

            Gtk::Adjustment *seedTimeAdjustment = Gtk::manage(new class Gtk::Adjustment(1, 0, 100, 1, 10, 10));
            seedTimeSpin    = Gtk::manage(new class Gtk::SpinButton(*seedTimeAdjustment, 1, 0));

            Gtk::Table *settingsTable = Gtk::manage(new class Gtk::Table(4, 2, false));
            Gtk::VBox *limitVbox      = Gtk::manage(new class Gtk::VBox(false, 10));

            uploadLabel->set_alignment(0,0.5);
            uploadLabel->set_padding(5,0);
            uploadLabel->set_justify(Gtk::JUSTIFY_LEFT);
            uploadLabel->set_line_wrap(false);
            uploadLabel->set_use_markup(false);
            uploadLabel->set_selectable(false);

            downloadLabel->set_alignment(0,0.5);
            downloadLabel->set_padding(5,0);
            downloadLabel->set_justify(Gtk::JUSTIFY_LEFT);
            downloadLabel->set_line_wrap(false);
            downloadLabel->set_use_markup(false);
            downloadLabel->set_selectable(false);

            seedPercentLabel->set_alignment(0,0.5);
            seedPercentLabel->set_padding(5,0);
            seedPercentLabel->set_justify(Gtk::JUSTIFY_LEFT);
            seedPercentLabel->set_line_wrap(false);
            seedPercentLabel->set_use_markup(false);
            seedPercentLabel->set_selectable(false);

            seedTimeLabel->set_alignment(0,0.5);
            seedTimeLabel->set_padding(5,0);
            seedTimeLabel->set_justify(Gtk::JUSTIFY_LEFT);
            seedTimeLabel->set_line_wrap(false);
            seedTimeLabel->set_use_markup(false);
            seedTimeLabel->set_selectable(false);

            seedPercentSpin->set_flags(Gtk::CAN_FOCUS);
            seedPercentSpin->set_update_policy(Gtk::UPDATE_ALWAYS);
            seedPercentSpin->set_numeric(false);
            seedPercentSpin->set_digits(0);
            seedPercentSpin->set_wrap(false);
            seedPercentSpin->set_range(0, MaxSeedProcent);

            seedTimeSpin->set_flags(Gtk::CAN_FOCUS);
            seedTimeSpin->set_update_policy(Gtk::UPDATE_ALWAYS);
            seedTimeSpin->set_numeric(false);
            seedTimeSpin->set_digits(0);
            seedTimeSpin->set_wrap(false);
            seedTimeSpin->set_range(0, MaxSeedTimeout);

            uploadCombo->append_text("disable");
            downloadCombo->append_text("disable");

            for (t_int i=limit_interval; i<=200; i=i+limit_interval)
               {
                  using namespace std;
                  using namespace btg::core;
                  string s = convertToString<int>(i) + " KiB/sec";
                  uploadCombo->append_text(s);
                  downloadCombo->append_text(s);
               }

            uploadCombo->set_active(0);
            downloadCombo->set_active(0);

            settingsTable->set_border_width(10);
            settingsTable->set_row_spacings(20);
            settingsTable->set_col_spacings(5);

            /* */
            settingsTable->attach(*uploadLabel, 0, 1, 0, 1, Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            settingsTable->attach(*downloadLabel, 0, 1, 1, 2, Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            settingsTable->attach(*seedPercentLabel, 0, 1, 2, 3, Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            settingsTable->attach(*seedTimeLabel, 0, 1, 3, 4, Gtk::FILL, Gtk::AttachOptions(), 0, 0);

            settingsTable->attach(*seedPercentSpin, 1, 2, 2, 3, Gtk::EXPAND|Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            settingsTable->attach(*seedTimeSpin,    1, 2, 3, 4, Gtk::EXPAND|Gtk::FILL, Gtk::AttachOptions(), 0, 0);

            settingsTable->attach(*uploadCombo, 1, 2, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            settingsTable->attach(*downloadCombo, 1, 2, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::AttachOptions(), 0, 0);

            limitVbox->pack_start(*settingsTable, Gtk::PACK_SHRINK, 0);

            set_title( GPD->sGUI_CLIENT() + " " + GPD->sVERSION() + " / Limit" );
            set_modal(true);
            property_window_position().set_value(Gtk::WIN_POS_CENTER);
            set_resizable(true);
            property_destroy_with_parent().set_value(false);
            set_has_separator(true);

            get_vbox()->pack_start(*limitVbox);

            set_default_size(300, 200);

            uploadLabel->show();
            downloadLabel->show();

            seedPercentLabel->show();
            seedTimeLabel->show();

            uploadCombo->show();
            downloadCombo ->show();

            seedPercentSpin->show();
            seedTimeSpin->show();

            settingsTable->show();
            limitVbox->show();

            // Create buttons and connect their signals.
            add_button("Set", 1);
            add_button("Cancel", 2);
            signal_response().connect(sigc::mem_fun(*this, &limitDialog::on_button_pressed));

            get_vbox()->show();
            show();
         }

         void limitDialog::update(std::string const& _filename,
                                  t_int const _currentUploadLimit,
                                  t_int const _currentDownloadLimit,
                                  t_int const  _currentSeedPercent,
                                  t_long const _currentSeedTime)
         {

            set_title( GPD->sGUI_CLIENT() + " " + GPD->sVERSION() + " / Limit: " + _filename );

            limit_selected = false;

            if (_currentUploadLimit > 0)
               {
                  t_int position = (_currentUploadLimit*KiB) / limit_interval;
                  uploadCombo->set_active(position);
               }
            else
               {
                  uploadCombo->set_active(0);
               }

            if (_currentDownloadLimit > 0)
               {
                  t_int position = (_currentDownloadLimit*KiB) / limit_interval;
                  downloadCombo->set_active(position);
               }
            else
               {
                  downloadCombo->set_active(0);
               }

            selected_upload_disable   = false;
            selected_upload_limit     = 0;

            selected_download_disable = false;
            selected_download_limit   = 0;

            selected_seed_percent_disable = false;
            selected_seed_timer_disable   = false;

            if (_currentSeedPercent >=0 && _currentSeedPercent <= MaxSeedProcent)
               {
                  seedPercentSpin->set_value(_currentSeedPercent);
               }
            else
               {
                  seedPercentSpin->set_value(0);
               }

            if (_currentSeedTime > 0)
               {
                  seedTimeSpin->set_value(_currentSeedTime);
               }
            else
               {
                  seedTimeSpin->set_value(0);
               }

            BTG_NOTICE("Update - limit, seed % " << seedPercentSpin->get_value() << " time = " << seedTimeSpin->get_value());
         }

         void limitDialog::on_button_pressed(int _button)
         {
            switch(_button)
               {
               case 1:
                  {
                     limit_selected = true;

                     int selection = uploadCombo->get_active_row_number();

                     BTG_NOTICE("selection = " << selection);

                     // Upload:
                     if (selection == 0)
                        {
                           // Disabled existing limit.
                           selected_upload_disable = true;
                        }

                     if (selection > 0)
                        {
                           // Set upload limit.
                           selected_upload_limit = (selection * limit_interval);
                        }
                     else if (selection < 0)
                        {
                           // Nothing selected, ignore.
                           selected_upload_limit = 0;
                        }

                     // Download:
                     selection = downloadCombo->get_active_row_number();
                     if (selection == 0)
                        {
                           // Disabled existing limit.
                           selected_download_disable = true;
                        }

                     if (selection > 0)
                        {
                           // Set upload limit.
                           selected_download_limit = (selection * limit_interval);
                        }
                     else if (selection < 0)
                        {
                           // Nothing selected, ignore.
                           selected_download_limit = 0;
                        }

                     // Seed percent:
                     if (seedPercentSpin->get_value() == 0)
                        {
                           selected_seed_percent_disable = true;
                        }

                     // Seed timer:
                     if (seedTimeSpin->get_value() == 0)
                        {
                           selected_seed_timer_disable = true;
                        }

                     break;
                  }
               case 2:
                  {
                     limit_selected = false;
                     break;
                  }
               }

            this->hide();
         }

         bool limitDialog::limitSelected() const
         {
            return limit_selected;
         }

         bool limitDialog::uploadLimitDisabled() const
         {
            return selected_upload_disable;
         }

         t_int limitDialog::getUploadLimit() const
         {
            return selected_upload_limit * KiB;
         }

         bool limitDialog::downloadLimitDisabled() const
         {
            return selected_download_disable;
         }

         t_int limitDialog::getDownloadLimit() const
         {
            return selected_download_limit * KiB;
         }

         bool limitDialog::seedPercentDisabled() const
         {
            return selected_seed_percent_disable;
         }

         t_int limitDialog::getSeedPercent() const
         {
            double percent = seedPercentSpin->get_value();
            return static_cast<t_int>(percent);
         }

         bool limitDialog::seedTimeDisabled() const
         {
            return selected_seed_timer_disable;
         }

         t_long limitDialog::getSeedTimeout() const
         {
            double timeout = seedTimeSpin->get_value();

            return static_cast<t_long>(timeout);
         }

         limitDialog::~limitDialog()
         {

         }

      } // namespace gui
   } // namespace UI
} // namespace btg

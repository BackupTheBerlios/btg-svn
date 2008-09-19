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
#include <gtkmm/comboboxtext.h>
#include <gtkmm/spinbutton.h>

#include <bcore/project.h>

#include <string>
#include <bcore/t_string.h>
#include <bcore/hrr.h>
#include <bcore/logmacro.h>

namespace btg
{
   namespace UI
   {
      namespace gui
      {
         const t_uint maxRateInBytes = (65*1024*1024);

         limitDialog::limitDialog(const char * _szUploadLabel, 
                                  const char * _szDownloadLabel, 
                                  const char * _szParam3Label, 
                                  const char * _szParam4Label)
            : Gtk::Dialog(btg::core::projectDefaults::sGUI_CLIENT() + " " + btg::core::projectDefaults::sFULLVERSION() + " / Limit", 
                          true /* modal */, 
                          true /* use_separator aka set_has_separator() */ ),
              limit_interval(5),
              limit_selected(false),
              selected_upload_disable(false),
              selected_download_disable(false),
              selected_param3_disable(false),
              selected_param4_disable(false),
              uploadCombo(0),
              downloadCombo(0),
              param3Spin(0),
              param4Spin(0),
              selected_upload_limit(-1),
              selected_download_limit(-1),
              limits()
         {
            Gtk::Label* uploadLabel   = Gtk::manage(new class Gtk::Label( _szUploadLabel ? _szUploadLabel : "Upload" ));
            Gtk::Label* downloadLabel = Gtk::manage(new class Gtk::Label( _szDownloadLabel ? _szDownloadLabel : "Download" ));
            uploadCombo               = Gtk::manage(new class Gtk::ComboBoxText());
            downloadCombo             = Gtk::manage(new class Gtk::ComboBoxText());

            Gtk::Label* seedPercentLabel = Gtk::manage(new class Gtk::Label( _szParam3Label ? _szParam3Label : "Seed %" ));
            Gtk::Label* seedTimeLabel    = Gtk::manage(new class Gtk::Label( _szParam4Label ? _szParam4Label : "Seed Time" ));

            Gtk::Adjustment* seedPercentAdjustment = Gtk::manage(new class Gtk::Adjustment(1, 0, 100, 1, 10, 10));
            param3Spin = Gtk::manage(new class Gtk::SpinButton(*seedPercentAdjustment, 1, 0));

            Gtk::Adjustment *seedTimeAdjustment = Gtk::manage(new class Gtk::Adjustment(1, 0, 100, 1, 10, 10));
            param4Spin    = Gtk::manage(new class Gtk::SpinButton(*seedTimeAdjustment, 1, 0));

            Gtk::Table* settingsTable = Gtk::manage(new class Gtk::Table(4, 2, false));
            Gtk::VBox* limitVbox      = Gtk::manage(new class Gtk::VBox(false, 10));

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

            param3Spin->set_flags(Gtk::CAN_FOCUS);
            param3Spin->set_update_policy(Gtk::UPDATE_ALWAYS);
            param3Spin->set_numeric(false);
            param3Spin->set_digits(0);
            param3Spin->set_wrap(false);
            param3Spin->set_range(0, MaxParam3Value);

            param4Spin->set_flags(Gtk::CAN_FOCUS);
            param4Spin->set_update_policy(Gtk::UPDATE_ALWAYS);
            param4Spin->set_numeric(false);
            param4Spin->set_digits(0);
            param4Spin->set_wrap(false);
            param4Spin->set_range(0, MaxParam4Value);

            uploadCombo->append_text("disable");
            downloadCombo->append_text("disable");

            for (t_uint i=1024; i<=maxRateInBytes; i*=2)
               {
                  using namespace btg::core;

                  humanReadableRate h = humanReadableRate::convert(i);

                  std::string s = h.toString();
                  uploadCombo->append_text(s);
                  downloadCombo->append_text(s);

                  limits.push_back(i);
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

            settingsTable->attach(*param3Spin, 1, 2, 2, 3, Gtk::EXPAND|Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            settingsTable->attach(*param4Spin,    1, 2, 3, 4, Gtk::EXPAND|Gtk::FILL, Gtk::AttachOptions(), 0, 0);

            settingsTable->attach(*uploadCombo, 1, 2, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            settingsTable->attach(*downloadCombo, 1, 2, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::AttachOptions(), 0, 0);

            limitVbox->pack_start(*settingsTable, Gtk::PACK_SHRINK, 0);

            property_window_position().set_value(Gtk::WIN_POS_CENTER);
            set_resizable(true);
            property_destroy_with_parent().set_value(false);

            get_vbox()->pack_start(*limitVbox);

            set_default_size(300, 200);

            // Create buttons and connect their signals.
            add_button("Set", 1);
            add_button("Cancel", 2);
            signal_response().connect(sigc::mem_fun(*this, &limitDialog::on_button_pressed));
            
            show_all(); // flag all inner elements as visible
            hide(); // hide toplevel window
         }

         void limitDialog::update(std::string const& _filename,
                                  t_int const _currentUploadLimit,
                                  t_int const _currentDownloadLimit,
                                  t_int const  _currentParam3,
                                  t_long const _currentParam4)
         {

            set_title( btg::core::projectDefaults::sGUI_CLIENT() + " " + btg::core::projectDefaults::sFULLVERSION() + " / Limit: " + _filename );

            limit_selected = false;

            if (_currentUploadLimit > 0)
               {
                  t_uint position = findClosestLimit(_currentUploadLimit);
                  uploadCombo->set_active(position);
               }
            else
               {
                  uploadCombo->set_active(0);
               }

            if (_currentDownloadLimit > 0)
               {
                  t_uint position = findClosestLimit(_currentDownloadLimit);
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

            selected_param3_disable = false;
            selected_param4_disable = false;

            if (_currentParam3 > 0 && _currentParam3 <= MaxParam3Value)
               {
                  param3Spin->set_value(_currentParam3);
               }
            else
               {
                  param3Spin->set_value(0);
               }

            if (_currentParam4 > 0 && _currentParam4 <= MaxParam4Value)
               {
                  param4Spin->set_value(_currentParam4);
               }
            else
               {
                  param4Spin->set_value(0);
               }
         }

         void limitDialog::on_button_pressed(int _button)
         {
            switch(_button)
               {
               case 1:
                  {
                     limit_selected = true;

                     int selection = uploadCombo->get_active_row_number();

                     // Upload:
                     if (selection == 0)
                        {
                           // Disabled existing limit.
                           selected_upload_disable = true;
                        }

                     if (selection > 0)
                        {
                           // Set upload limit.
                           selected_upload_limit = getValue(selection);
                           //(selection * limit_interval);
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
                           selected_download_limit = getValue(selection);
                           // (selection * limit_interval);
                        }
                     else if (selection < 0)
                        {
                           // Nothing selected, ignore.
                           selected_download_limit = 0;
                        }

                     // Seed percent:
                     if (param3Spin->get_value() == 0)
                        {
                           selected_param3_disable = true;
                        }

                     // Seed timer:
                     if (param4Spin->get_value() == 0)
                        {
                           selected_param4_disable = true;
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
            return selected_upload_limit;
         }

         bool limitDialog::downloadLimitDisabled() const
         {
            return selected_download_disable;
         }

         t_int limitDialog::getDownloadLimit() const
         {
            return selected_download_limit;
         }

         bool limitDialog::param3Disabled() const
         {
            return selected_param3_disable;
         }

         t_int limitDialog::getParam3() const
         {
            double percent = param3Spin->get_value();
            return static_cast<t_int>(percent);
         }

         bool limitDialog::param4Disabled() const
         {
            return selected_param4_disable;
         }

         t_long limitDialog::getParam4() const
         {
            double timeout = param4Spin->get_value();

            return static_cast<t_long>(timeout);
         }

         t_uint limitDialog::findClosestLimit(t_uint _limit) const
         {
            t_uint index = 0;
            std::vector<t_uint>::const_iterator iter;
            for (iter = limits.begin();
                 iter != limits.end();
                 iter++)
               {
                  if (*iter >= _limit)
                     {
                        break;
                     }
                  else
                     {
                        index++;
                     }
               }
            index++;
            return index;
         }

         t_uint limitDialog::getValue(t_uint _index) const
         {
            std::vector<t_uint>::const_iterator iter = limits.begin();
            iter += (_index-1);

            if (iter != limits.end())
               {
                  return *iter;
               }
            
            return 0;
         }

         limitDialog::~limitDialog()
         {

         }

      } // namespace gui
   } // namespace UI
} // namespace btg

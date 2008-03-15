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

#include "prefdialog.h"
#include <gdk/gdkkeysyms.h>
#include <gtkmm/accelgroup.h>
#include <gtkmm/label.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/table.h>
#include <gtkmm/box.h>
#include <gtkmm/alignment.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/filechooserdialog.h>

#include <bcore/project.h>
#include <bcore/logmacro.h>

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         preferencesDialog::preferencesDialog(int const _min_port, int const _max_port)
            : portrange_begin(0),
              beginPortRangeSpinbuttonAdj(0),
              portrange_end(0),
              endPortRangeSpinbuttonAdj(0),
              workPathEntry(0),
              outputPathEntry(0),
              leechModeCheckbutton(0),
              min_port(_max_port), max_port(_min_port),
              workPath(),
              outputPath(),
              leechmode(false),
              portrange_begin_intial_value(0),
              portrange_end_intial_value(0),
              workPath_initial_value(""),
              outputPath_initial_value(""),
              leechmode_initial_value(false),
              portrange_begin_signal(),
              portrange_end_signal(),
              save_btn_pressed(false)
         {
            Gtk::Label* workLabel      = Gtk::manage(new class Gtk::Label("Working directory"));
            Gtk::Label* outputLabel    = Gtk::manage(new class Gtk::Label("Output directory"));
            Gtk::Label* portRangeLabel = Gtk::manage(new class Gtk::Label("Port range"));
            Gtk::Label* leechModeLabel = Gtk::manage(new class Gtk::Label("Leech mode"));
            outputPathEntry            = Gtk::manage(new class Gtk::Entry());

            workPathEntry              = Gtk::manage(new class Gtk::Entry());

            leechModeCheckbutton       = Gtk::manage(new class Gtk::CheckButton("enabled"));
            Gtk::Table* settingsTable  = Gtk::manage(new class Gtk::Table(2, 2, false));

            // Ports:
            Gtk::Table* portTable       = Gtk::manage(new class Gtk::Table(2, 2, false));;
            Gtk::Label* beginPortLabel  = Gtk::manage(new class Gtk::Label("Begin Port"));
            Gtk::Label* endPortLabel    = Gtk::manage(new class Gtk::Label("End Port"));
            beginPortRangeSpinbuttonAdj = Gtk::manage(new class Gtk::Adjustment(_min_port, _min_port, _max_port, 1, 100, 10));
            portrange_begin             = Gtk::manage(new class Gtk::SpinButton(*beginPortRangeSpinbuttonAdj, 1, 0));
            endPortRangeSpinbuttonAdj   = Gtk::manage(new class Gtk::Adjustment(_min_port, _min_port, _max_port, 1, 100, 10));
            portrange_end               = Gtk::manage(new class Gtk::SpinButton(*endPortRangeSpinbuttonAdj, 1, 0));

            beginPortLabel->set_alignment(0,0.5);
            beginPortLabel->set_padding(5,5);
            beginPortLabel->set_justify(Gtk::JUSTIFY_LEFT);
            beginPortLabel->set_line_wrap(false);
            beginPortLabel->set_use_markup(false);
            beginPortLabel->set_selectable(false);

            endPortLabel->set_alignment(0,0.5);
            endPortLabel->set_padding(5,5);
            endPortLabel->set_justify(Gtk::JUSTIFY_LEFT);
            endPortLabel->set_line_wrap(false);
            endPortLabel->set_use_markup(false);
            endPortLabel->set_selectable(false);

            portrange_end->set_flags(Gtk::CAN_FOCUS);
            portrange_end->set_update_policy(Gtk::UPDATE_ALWAYS);
            portrange_end->set_numeric(false);
            portrange_end->set_digits(0);
            portrange_end->set_wrap(false);
            portrange_end->set_flags(Gtk::CAN_FOCUS);
            portrange_end->set_update_policy(Gtk::UPDATE_ALWAYS);
            portrange_end->set_numeric(true);
            portrange_end->set_digits(0);
            portrange_end->set_wrap(false);

            portrange_begin->set_flags(Gtk::CAN_FOCUS);
            portrange_begin->set_update_policy(Gtk::UPDATE_ALWAYS);
            portrange_begin->set_numeric(false);
            portrange_begin->set_digits(0);
            portrange_begin->set_wrap(false);
            portrange_begin->set_flags(Gtk::CAN_FOCUS);
            portrange_begin->set_update_policy(Gtk::UPDATE_ALWAYS);
            portrange_begin->set_numeric(true);
            portrange_begin->set_digits(0);
            portrange_begin->set_wrap(false);

            portTable->set_border_width(10);
            portTable->set_row_spacings(5);
            portTable->set_col_spacings(5);
            portTable->attach(*beginPortLabel, 0, 1, 0, 1, Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            portTable->attach(*endPortLabel, 0, 1, 1, 2, Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            portTable->attach(*portrange_end, 1, 2, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            portTable->attach(*portrange_begin, 1, 2, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::AttachOptions(), 0, 0);

            beginPortLabel->show();
            endPortLabel->show();
            portrange_begin->show();
            portrange_end->show();
            portTable->show();

            Gtk::VBox *preferencesVbox = Gtk::manage(new class Gtk::VBox(false, 10));

            outputLabel->set_alignment(0,0.5);
            outputLabel->set_padding(5,0);
            outputLabel->set_justify(Gtk::JUSTIFY_LEFT);
            outputLabel->set_line_wrap(false);
            outputLabel->set_use_markup(false);
            outputLabel->set_selectable(false);

            workLabel->set_alignment(0,0.5);
            workLabel->set_padding(5,0);
            workLabel->set_justify(Gtk::JUSTIFY_LEFT);
            workLabel->set_line_wrap(false);
            workLabel->set_use_markup(false);
            workLabel->set_selectable(false);



            portRangeLabel->set_alignment(0,0.5);
            portRangeLabel->set_padding(5,0);
            portRangeLabel->set_justify(Gtk::JUSTIFY_LEFT);
            portRangeLabel->set_line_wrap(false);
            portRangeLabel->set_use_markup(false);
            portRangeLabel->set_selectable(false);

            leechModeLabel->set_alignment(0,0.5);
            leechModeLabel->set_padding(5,0);
            leechModeLabel->set_justify(Gtk::JUSTIFY_LEFT);
            leechModeLabel->set_line_wrap(false);
            leechModeLabel->set_use_markup(false);
            leechModeLabel->set_selectable(false);

            Gtk::HBox* pathHbox     = Gtk::manage(new class Gtk::HBox(false, 10));
            Gtk::HBox* workPathHbox = Gtk::manage(new class Gtk::HBox(false, 10));

            outputPathEntry->set_flags(Gtk::CAN_FOCUS);
            outputPathEntry->set_visibility(true);
            outputPathEntry->set_editable(false);
            outputPathEntry->set_max_length(0);
            outputPathEntry->set_text("");
            outputPathEntry->set_has_frame(true);
            outputPathEntry->set_activates_default(false);

            workPathEntry->set_flags(Gtk::CAN_FOCUS);
            workPathEntry->set_visibility(true);
            workPathEntry->set_editable(false);
            workPathEntry->set_max_length(0);
            workPathEntry->set_text("");
            workPathEntry->set_has_frame(true);
            workPathEntry->set_activates_default(false);

            Gtk::Button* pathButton = Gtk::manage(new class Gtk::Button("Change Path"));
            Gtk::Button* workPathButton = Gtk::manage(new class Gtk::Button("Change Path"));

            pathHbox->pack_start(*outputPathEntry);
            pathHbox->pack_start(*pathButton, Gtk::PACK_SHRINK, 0);

            workPathHbox->pack_start(*workPathEntry);
            workPathHbox->pack_start(*workPathButton, Gtk::PACK_SHRINK, 0);

            leechModeCheckbutton->set_flags(Gtk::CAN_FOCUS);
            leechModeCheckbutton->set_relief(Gtk::RELIEF_NORMAL);
            leechModeCheckbutton->set_mode(true);
            leechModeCheckbutton->set_active(false);

            settingsTable->set_border_width(10);
            settingsTable->set_row_spacings(20);
            settingsTable->set_col_spacings(5);

            // left rigth top bottom

            /*
              table1->attach(*label1, 0, 1, 0, 1, Gtk::FILL, Gtk::AttachOptions(), 0, 0);
              table1->attach(*label2, 0, 1, 1, 2, Gtk::FILL, Gtk::AttachOptions(), 0, 0);
              table1->attach(*label3, 0, 1, 2, 3, Gtk::FILL, Gtk::AttachOptions(), 0, 0);
              table1->attach(*label4, 0, 1, 3, 4, Gtk::FILL, Gtk::AttachOptions(), 0, 0);
              table1->attach(*entry1, 1, 2, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::AttachOptions(), 0, 0);
              table1->attach(*entry2, 1, 2, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::AttachOptions(), 0, 0);
              table1->attach(*entry3, 1, 2, 2, 3, Gtk::EXPAND|Gtk::FILL, Gtk::AttachOptions(), 0, 0);
              table1->attach(*entry4, 1, 2, 3, 4, Gtk::EXPAND|Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            */

            settingsTable->attach(*workLabel,            0, 1, 0, 1, Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            settingsTable->attach(*outputLabel,          0, 1, 1, 2, Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            settingsTable->attach(*portRangeLabel,       0, 1, 2, 3, Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            settingsTable->attach(*leechModeLabel,       0, 1, 3, 4, Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            settingsTable->attach(*workPathHbox,         1, 2, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            settingsTable->attach(*pathHbox,             1, 2, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::AttachOptions(), 0, 0);
            settingsTable->attach(*portTable,            1, 2, 2, 3, Gtk::FILL, Gtk::FILL, 0, 0);
            settingsTable->attach(*leechModeCheckbutton, 1, 2, 3, 4, Gtk::FILL, Gtk::AttachOptions(), 0, 0);

            preferencesVbox->pack_start(*settingsTable, Gtk::PACK_SHRINK, 0);

            set_title( GPD->sGUI_CLIENT() + " " + GPD->sFULLVERSION() + " / Preferences" );
            set_modal(true);
            property_window_position().set_value(Gtk::WIN_POS_CENTER);
            set_resizable(true);
            property_destroy_with_parent().set_value(false);
            set_has_separator(true);

            get_vbox()->pack_start(*preferencesVbox);

            set_default_size(300, 200);

            outputLabel->show();
            workLabel->show();
            portRangeLabel->show();
            leechModeLabel->show();
            outputPathEntry->show();
            pathButton->show();
            pathHbox->show();

            workPathEntry->show();
            workPathButton->show();
            workPathHbox->show();

            leechModeCheckbutton->show();
            settingsTable->show();

            preferencesVbox->show();

            // Create buttons and connect their signals.
            add_button("Save", 1);
            add_button("Cancel", 2);

            signal_response().connect(sigc::mem_fun(*this, &preferencesDialog::on_dialog_button_pressed));

            pathButton->signal_clicked().connect(sigc::mem_fun(*this, &preferencesDialog::on_path_select_button_pressed));

            workPathButton->signal_clicked().connect(sigc::mem_fun(*this, &preferencesDialog::on_work_path_select_button_pressed));

            get_vbox()->show();
         }

         void preferencesDialog::setInitialValues(t_intPair const& _portInfo,
                                                  std::string const& _workpath,
                                                  std::string const& _outputpath,
                                                  bool const _leechmode)
         {
            beginPortRangeSpinbuttonAdj->set_value(_portInfo.first);
            endPortRangeSpinbuttonAdj->set_value(_portInfo.second);

            portrange_begin_intial_value = _portInfo.first;
            portrange_end_intial_value   = _portInfo.second;

            workPathEntry->set_text(_workpath);
            workPath_initial_value   = _workpath;

            outputPathEntry->set_text(_outputpath);
            outputPath_initial_value = _outputpath;

            leechModeCheckbutton->set_active(_leechmode);
            leechmode_initial_value  = _leechmode;
         }

         void preferencesDialog::on_dialog_button_pressed(int _button)
         {
            switch(_button)
               {
               case 1:
                  {
                     // Save.
                     save_btn_pressed = true;
                     break;
                  }
               case 2:
                  {
                     // Cancel.
                     save_btn_pressed = false;
                     break;
                  }
               }

            portrange_begin_signal.disconnect();
            portrange_end_signal.disconnect();

            this->hide();
         }

         void preferencesDialog::on_path_select_button_pressed()
         {
            std::string destinationDir;
            if (popUpDirSelection(outputPath_initial_value, destinationDir))
               {
                  outputPathEntry->set_text(destinationDir);
               }

         }

         void preferencesDialog::on_work_path_select_button_pressed()
         {
            std::string workDir;
            if (popUpDirSelection(workPath_initial_value, workDir))
               {
                  workPathEntry->set_text(workDir);
               }
         }

         void preferencesDialog::on_begin_port_changed()
         {
            if (beginPortRangeSpinbuttonAdj->get_value() > endPortRangeSpinbuttonAdj->get_value())
               {
                  beginPortRangeSpinbuttonAdj->set_value( endPortRangeSpinbuttonAdj->get_value() );
               }
         }

         void preferencesDialog::on_end_port_changed()
         {
            if (endPortRangeSpinbuttonAdj->get_value() < beginPortRangeSpinbuttonAdj->get_value())
               {
                  endPortRangeSpinbuttonAdj->set_value( beginPortRangeSpinbuttonAdj->get_value() );
               }
         }

         bool preferencesDialog::changed() const
         {
            bool status = false;

            if (beginPortRangeSpinbuttonAdj->get_value() != portrange_begin_intial_value)
               {
                  status = true;
               }

            if (endPortRangeSpinbuttonAdj->get_value() != portrange_end_intial_value)
               {
                  status = true;
               }

            if (workPathEntry->get_text() != workPath_initial_value)
               {
                  status = true;
               }

            if (outputPathEntry->get_text() != outputPath_initial_value)
               {
                  status = true;
               }

            if (leechModeCheckbutton->get_active() != leechmode_initial_value)
               {
                  status = true;
               }

            // If something was changed, but the user pressed cancel.
            if (!save_btn_pressed)
               {
                  status = false;
               }

            // BTG_NOTICE("save_btn_pressed = " << save_btn_pressed);

            return status;
         }

         void preferencesDialog::run()
         {
            save_btn_pressed = false;

            portrange_begin_signal = beginPortRangeSpinbuttonAdj->signal_value_changed().connect(sigc::mem_fun(*this, &preferencesDialog::on_begin_port_changed));
            portrange_end_signal   = endPortRangeSpinbuttonAdj->signal_value_changed().connect(sigc::mem_fun(*this, &preferencesDialog::on_end_port_changed));

            run();
         }

         bool preferencesDialog::popUpDirSelection(std::string const& _lastDir,
                                                   std::string & _destinationDir)
         {
            bool result = false;

            Gtk::FileChooserDialog dialog("Select a directory", Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);

            dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
            dialog.add_button("_OK",     Gtk::RESPONSE_OK);

            // Add filters, so that only certain file types can be selected:

            Gtk::FileFilter filter_any;
            filter_any.set_name("Any files");
            filter_any.add_pattern("*");
            dialog.add_filter(filter_any);

            // Set the directory, saved last time this dialog was
            // used.

            if (_lastDir.size() > 0)
               {
                  dialog.set_current_folder(_lastDir);
               }

            // Show the dialog and wait for a user response:
            int dresult = dialog.run();

            switch(dresult)
               {
               case(Gtk::RESPONSE_OK):
                  {
                     result          = true;
                     _destinationDir = dialog.get_current_folder();
                     break;
                  }
               case(Gtk::RESPONSE_CANCEL):
                  {
                     result          = false;
                     break;
                  }
               default:
                  {
                     result          = false;
                     break;
                  }
               } // switch

            return result;
         }

         t_intPair preferencesDialog::getPortRange() const
         {
            t_intPair result;

            result.first  = static_cast<int>(beginPortRangeSpinbuttonAdj->get_value());
            result.second = static_cast<int>(endPortRangeSpinbuttonAdj->get_value());

            return result;
         }

         std::string preferencesDialog::getDestinationDirectory() const
         {
            return outputPathEntry->get_text();
         }

         std::string preferencesDialog::getWorkingDirectory() const
         {
            return workPathEntry->get_text();
         }

         bool preferencesDialog::getLeechMode() const
         {
            return leechModeCheckbutton->get_active();
         }

         preferencesDialog::~preferencesDialog()
         {
            portrange_begin_signal.disconnect();
            portrange_end_signal.disconnect();
         }

      } // namespace gui
   } // namespace UI
} // namespace btg

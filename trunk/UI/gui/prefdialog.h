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
 * $Id: prefdialog.h,v 1.13.4.3 2006/08/12 17:37:23 wojci Exp $
 */

#ifndef PREFDIALOG_H
#define PREFDIALOG_H

#include <gtkmm/dialog.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/entry.h>

#include <bcore/type.h>

namespace btg
{
   namespace UI
      {
         namespace gui
            {
               /**
                * \addtogroup commands
                */
               /** @{ */

               /// Implements a dialog showing preferences.
               class preferencesDialog : public Gtk::Dialog
                  {
                  public:
                     /// Constructor.
                     /// @param [in] _min_port The minimum port that can be selected.
                     /// @param [in] _max_port The maximum port that can be selected.
                     preferencesDialog(int const _min_port, int const _max_port);

                     /// Setup this dialog.
                     /// @param [in] _portInfo   Port information.
                     /// @param [in] _workpath   Path to where downloaded data is held.
                     /// @param [in] _outputpath Path to where finished downloads are moved to.
                     /// @param [in] _leechmode  If true, seeding is disabled.
                     void setInitialValues(t_intPair const& _portInfo,
                                           std::string const& _workpath,
                                           std::string const& _outputpath,
                                           bool const _leechmode);

                     /// Get the selected port range.
                     t_intPair getPortRange() const;

                     /// Get the set work directory.
                     std::string getWorkingDirectory() const;

                     /// Get the set destination directory.
                     std::string getDestinationDirectory() const;

                     /// Get the set leech mode flag.
                     bool getLeechMode() const;

                     /// Run this dialog.
                     void run();

                     /// Returns true if the preferences were changed.
                     bool changed() const;

                     /// Destructor.
                     virtual ~preferencesDialog();
                  private:
                     /// Callback: called when one of the buttons are pressed.
                     /// @param [in] _button Which button was pressed. 1 - the save button. 0 - the cancel button.
                     void on_dialog_button_pressed(int _button);

                     /// Callback: called when the button used to popup a directory selection was pressed.
                     void on_path_select_button_pressed();

                     /// Callback: called when the button used to popup a directory selection was pressed.
                     void on_work_path_select_button_pressed();

                     /// Callback: called when the begin port was changed.
                     void on_begin_port_changed();

                     /// Callback: called when the end port was changed.
                     void on_end_port_changed();

                     /// Pop up a dir selection dialog.
                     /// @param [in]  _lastDir        The last used dir.
                     /// @param [out] _destinationDir Destination directory, if selected.
                     /// \return                      True, if a new directory was selected, false otherwise.
                     bool popUpDirSelection(std::string const& _lastDir,
                                            std::string & _destinationDir);

                     /// Pointer to a port range beginning.
                     Gtk::SpinButton*   portrange_begin;

                     /// Adjustment used for the above port range.
                     Gtk::Adjustment*   beginPortRangeSpinbuttonAdj;

                     /// Pointer to a port range ending.
                     Gtk::SpinButton*   portrange_end;

                     /// Adjustment used for the above port range.
                     Gtk::Adjustment*   endPortRangeSpinbuttonAdj;

                     /// Pointer to a text entry containing the work path.
                     Gtk::Entry*        workPathEntry;

                     /// Pointer to a text entry containing the output path.
                     Gtk::Entry*        outputPathEntry;

                     /// Pointer to a check button.
                     Gtk::CheckButton*  leechModeCheckbutton;

                     /// Minimum port that can be used.
                     int const          min_port;

                     /// Maximum port that can be used.
                     int const          max_port;

                     /// The work path.
                     std::string        workPath;

                     /// The output path.
                     std::string        outputPath;

                     /// Leech mode.
                     bool               leechmode;

                     /// Initial value: port range begin.
                     double             portrange_begin_intial_value;

                     /// Initial value: port range end.
                     double             portrange_end_intial_value;

                     /// Initial value: work path.
                     std::string        workPath_initial_value;

                     /// Initial value: output path.
                     std::string        outputPath_initial_value;

                     /// Initial value: seeding.
                     bool               leechmode_initial_value;

                     /// Signal.
                     sigc::connection   portrange_begin_signal;

                     /// Signal.
                     sigc::connection   portrange_end_signal;

                     /// True, if user pressed the save button.
                     bool               save_btn_pressed;
                  private:
                     /// Copy constructor.
                     preferencesDialog(preferencesDialog const& _p);
                     /// Assignment operator.
                     preferencesDialog& operator=(preferencesDialog const& _p);
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif

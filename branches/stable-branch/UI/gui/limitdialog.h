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

#ifndef LIMITDIALOG_H
#define LIMITDIALOG_H

#include <gtkmm/dialog.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/spinbutton.h>

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

               /// Implements a dialog used to set bandwidth limits.
               class limitDialog : public Gtk::Dialog
                  {
                  public:
                     /// Constructor.
                     /// @param [in] _szUploadLabel 1st label text (default "Upload")
                     /// @param [in] _szDownloadLabel 2nd label text (default "Download")
                     /// @param [in] _szSeedpercUploadscntLabel 3rd label text (default "Seed %")
                     /// @param [in] _szSeedtimeConnectionscntLabel 4th label text (default "Seed Time")
                     limitDialog(const char * _szUploadLabel = NULL, const char * _szDownloadLabel = NULL, const char * _szParam3Label = NULL, const char * _szParam4Label = NULL);

                     /// Update this dialog. Should be called before
                     /// the run method is called.
                     /// @param [in] _filename             Used as a part of the title.
                     /// @param [in] _currentUploadLimit   Choosen upload limit, in KiB/sec.
                     /// @param [in] _currentDownloadLimit Choosen download limit, in KiB/sec.
                     /// @param [in] _currentSeedPercent Choosen seed percent.
                     /// @param [in] _currentSeedTime    Choosen seed timeout, in minutes.
                     void update(std::string const& _filename,
                                 t_int const  _currentUploadLimit,
                                 t_int const  _currentDownloadLimit,
                                 t_int const  _currentParam3,
                                 t_long const _currentParam4
                                 );

                     /// Returns true if the user selected an option
                     /// from either one of the drop down boxes.
                     bool limitSelected() const;

                     /// Returns true if the user wants to disable the upload limit.
                     bool uploadLimitDisabled() const;

                     /// Returns the selected upload limit in B/sec.
                     t_int getUploadLimit() const;

                     /// Returns true if the user wants to disable the download limit.
                     bool downloadLimitDisabled() const;

                     /// Returns the selected download limit in B/sec.
                     t_int getDownloadLimit() const;

                     /// Returns true, if seed percent/max uploads is 0, false otherwise.
                     bool param3Disabled() const;

                     /// Get seed percent/max uploads.
                     t_int getParam3() const;

                     /// Returns true, if seed time/max connections is 0, false otherwise.
                     bool param4Disabled() const;

                     /// Get seed timeout/max connections.
                     t_long getParam4() const;

                     /// Destructor.
                     virtual ~limitDialog();
                  private:
                     enum
                        {
                           KiB            = 1024,   //!< Definition of KiB.
                           MaxParam3Value = 102400, //!< Maximum seed percent/Maximum uploads. (taken out of thin air)
                           MaxParam4Value = 102400  //!< Maximum seed timeout/Maximum connections. (taken out of thin air)
                        };

                     /// The interval in which rates are in.
                     const t_int        limit_interval;

                     /// True if user pressed the "set" button.
                     bool               limit_selected;

                     /// True if user selected the option to disable limit on upload.
                     bool               selected_upload_disable;

                     /// True if user selected the option to disable limit on download.
                     bool               selected_download_disable;

                     /// True, if seed percent/max uploads is 0.
                     bool               selected_param3_disable;

                     /// True, if seed timeout/max connections is 0.
                     bool               selected_param4_disable;

                     /// Drop down box showing upload rates.
                     Gtk::ComboBoxText* uploadCombo;

                     /// Drop down box showing download rates.
                     Gtk::ComboBoxText* downloadCombo;

                     /// Spinbutton used to show seed percent or max uploads.
                     Gtk::SpinButton*   param3Spin;

                     /// Spinbutton used to show seed timeout or max connections.
                     Gtk::SpinButton*   param4Spin;

                     /// Upload limit in KiB/sec.
                     t_int              selected_upload_limit;

                     /// Download limit in KiB/sec.
                     t_int              selected_download_limit;

                     /// Callback: called when one of the buttons are pressed.
                     /// @param [in] _button Which button was pressed. 1 - the set button. 0 - the cancel button.
                     void on_button_pressed(int _button);
                  private:
                     /// Copy constructor.
                     limitDialog(btg::UI::gui::limitDialog const& _ld);
                     /// Assignment operator.
                     limitDialog& operator=(limitDialog const& _ld);
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif

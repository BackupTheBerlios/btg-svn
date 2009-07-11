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

#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <bcore/type.h>

#include <gtkmm.h>

namespace Gtk
{
   class Button;
   class Label;
   class ProgressBar;
   class VBox;
}

namespace btg
{
   namespace UI
      {
         namespace gui
            {
               /**
                * \addtogroup gui
                */
               /** @{ */

               /// Generic dialog used for showing progress.
               class progressDialog : public Gtk::Dialog
                  {

                  public:
                     /// Constructor.
                     progressDialog(std::string _title,
                                    bool const _cancel);

                     /// Update progress.
                     /// @param _percent Percent.
                     /// @param _text    Text to display.
                     void updateProgress(t_uint const _percent, 
                                         std::string const& _text);

                     /// Indicates if user pressed cancel,
                     bool cancelPressed() const;

                     /// Destructor.
                     virtual ~progressDialog();

                  private:
                     /// Set the text of the contained label.
                     void setText(std::string const& _text);

                     /// Enable a timeout. Called by updateProgress so that one can call
                     /// the run function of the dialog class and return control after a
                     /// timer expires.
                     void setTimeout();

                     /// User clicked cancel, callback.
                     void on_cancel_clicked();

                     /// Callback.
                     bool on_refresh_timeout(int _timeout);

                     /// Canceling is enabled.
                     bool              cancel_enabled;

                     /// Cancel was pressed.
                     bool              cancel_pressed;

                     /// The cancel button.
                     Gtk::Button*      cancelbutton;

                     /// Label showing a title.
                     Gtk::Label*       title_label;

                     /// Label showing a text message.
                     Gtk::Label*       label;

                     /// Progress bar.
                     Gtk::ProgressBar* progressbar;

                     /// Vbox.
                     Gtk::VBox*        vbox;
                  private:
                     /// Copy constructor.
                     progressDialog(progressDialog const& _iw);
                     /// Assignment operator.
                     progressDialog& operator=(progressDialog const& _iw);
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif // PROGRESSDIALOG_H

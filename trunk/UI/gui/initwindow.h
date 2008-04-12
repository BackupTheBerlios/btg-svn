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

#ifndef INIT_WINDOW_H
#define INIT_WINDOW_H

#include <gtkmm/dialog.h>

#include <string>

namespace Gtk
{
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

               /// Used to show an initial window which displays setup progress.
               class initWindow : public Gtk::Dialog
                  {

                  public:
                     /// Events.
                     enum INITEVENT
                        {
                           IEV_START,       //!< Start.
                           IEV_RCONF,       //!< Reading configuration.
                           IEV_RCONF_DONE,  //!< Reading configuration done.
                           IEV_CLA,         //!< Parsing command line arguments.
                           IEV_CLA_DONE,    //!< Parsing command line arguments done.
                           IEV_TRANSP,      //!< Setting up transport.
                           IEV_TRANSP_DONE, //!< Setting up transport done.
                           IEV_SETUP,       //!< Registering client.
                           IEV_SETUP_DONE,  //!< Registering done.
                           IEV_END          //!< Finished.
                        };

                     /// Constructor.
                     initWindow();

                     /// Update the progress bar contained in this window.
                     void updateProgress(INITEVENT _event);

                     /// Destructor.
                     virtual ~initWindow();

                  private:
                     /// Set the text of the contained label.
                     void setText(std::string const& _text);

                     /// Enable a timeout. Called by updateProgress so that one can call
                     /// the run function of the dialog class and return control after a
                     /// timer expires.
                     void setTimeout();

                     /// Callback.
                     bool on_refresh_timeout(int _timeout);

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
                     initWindow(initWindow const& _iw);
                     /// Assignment operator.
                     initWindow& operator=(initWindow const& _iw);
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif

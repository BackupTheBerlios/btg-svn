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
 * $Id: initwindow.h,v 1.1.2.4 2007/01/16 20:26:36 wojci Exp $
 */

#ifndef INIT_WINDOW_H
#define INIT_WINDOW_H

#include "window.h"

#include <string>

namespace btg
{
   namespace UI
      {
         namespace cli
            {
               /**
                * \addtogroup ncli
                */
               /** @{ */

               /// Used to show an initial window which displays setup
               /// progress.
               class initWindow : public baseWindow
                  {

                  public:
                     /// Events.
                     enum INITEVENT
                        {
                           IEV_START,       //!< Start.
                           IEV_RCONF,       //!< Reading configuration.
                           IEV_RCONF_KEYS,  //!< Reading keys.
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
                     initWindow(keyMapping const& _kmap);

                     /// Initialize ncurses, show the window.
                     void show();

                     /// Hide the window, deinit ncurses.
                     void hide();

                     /// Update the progress bar contained in this window.
                     void updateProgress(INITEVENT _event);

                     /// Show an error message, and wait for the user
                     /// to press any key.
                     void showError(std::string const& _error);

                     /// Destructor.
                     virtual ~initWindow();
                  private:
                     /// Indicates that this window was hidden.
                     bool  hidden;

                     /// The middle x position for this window.
                     t_int middle_x;

                     /// The middle y position for this window.
                     t_int middle_y;

                     /// Set the text to be used.
                     void setText(std::string const& _text);

                     /// Set the percent of the initialization progress.
                     void setPercent(t_uint const _percent);

                     void resize(windowSize const& _ws);

                     windowSize calculateDimenstions(windowSize const& _ws) const;

                     void refresh();

                  private:
                     /// Copy constructor.
                     initWindow(initWindow const& _iw);
                     /// Assignment operator.
                     initWindow& operator=(initWindow const& _iw);
                  };

               /** @} */

            } // namespace cli
      } // namespace UI
} // namespace btg

#endif

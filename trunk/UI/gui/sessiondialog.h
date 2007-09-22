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

#ifndef SESSION_DIALOG_H
#define SESSION_DIALOG_H

#include <gtkmm/dialog.h>
#include <gtkmm/comboboxtext.h>

#include <bcore/type.h>

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

               /// Implements a dialog which is used to either create
               /// a new session or to attach to an existing one.
               class sessionDialog: public Gtk::Dialog
                  {
                  public:
		    /// The state when this dialog quits.
                     enum QUIT_STATE
                        {
			  QUIT         = 1, //!< Quit, nothing selected.
			  NEW_SESSION,      //!< Create a new session.
			  SELECTED          //!< A session was selected - attach to it.
                        };

                     /// Constructor.
                     /// @param [in] _sessionlist      List of sessions.
                     sessionDialog(t_longList const& _sessionlist);

                     /// Use this function to get the result of the
                     /// dialog, after it was terminated.
		     /// @return State.
                     sessionDialog::QUIT_STATE getResult();

                     /// Returns true if the user selected a session -
                     /// also stores the session id in _session.
		     /// @return True - got session ID. False - otherwise.
                     bool getSelectedSession(t_long & _session);

                     /// Destructor.
                     virtual ~sessionDialog();
                  private:
                     /// Callback: user clicked ok.
                     void on_ok_clicked();

                     /// Callback: user clicked create session.
                     void on_create_session_clicked();

                     /// Callback: user clicked quit.
                     void on_quit_clicked();

		     /// Callback: user changed the combo box.
                     void on_combo_changed();

		     /// The state of this dialog after it terminated.
                     QUIT_STATE         quitstate;

                     /// List of sessions.
                     t_longList         sessionlist;

                     /// True, if a session was selected.
                     bool               selected;

                     /// The selected session.
                     t_long             session;

                     /// Truem if user requested new session.
                     bool               create_session;

                     /// Pointer to the combo used to display
                     /// sessions.
                     Gtk::ComboBoxText* cbt;

		     /// Button used to attach to a session.
                     Gtk::Button*       attachbutton;
                  private:
                     /// Copy constructor.
                     sessionDialog(sessionDialog const& _ssd);
                     /// Assignment operator.
                     sessionDialog& operator=(sessionDialog const& _ssd);
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif // SESSION_DIALOG_H


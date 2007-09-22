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

#ifndef SESSION_SELECTION_H
#define SESSION_SELECTION_H

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

               /// Implements a dialog which pop ups when the user
               /// should select a session which he wishes to attach to.
               class sessionSelectionDialog: public Gtk::Dialog
                  {
                  public:
                     /// Constructor.
                     /// @param [in] _sessionlist      List of sessions.
                     /// @param [in] _disableSelection False, selection of a session disabled. True, selection of a session is enabled.
                     sessionSelectionDialog(t_longList const& _sessionlist, bool const _disableSelection = false);

                     /// Returns true if the user selected a session - also stores the session id in _session.
                     bool getSelectedSession(t_long & _session);

                     /// Destructor.
                     virtual ~sessionSelectionDialog();
                  private:
                     /// Callback: user clicked ok.
                     void on_ok_clicked();

                     /// Callback: user clicked cancel.
                     void on_cancel_clicked();

                     /// List of sessions.
                     t_longList         sessionlist;

                     /// False, selection of a session disabled.
                     /// True, selection of a session is enabled.
                     bool               disableSelection_;

                     /// True, if a session was selected.
                     bool               selected;

                     /// The selected session.
                     t_long             session;

                     /// Pointer to the combo used to display sessions.
                     Gtk::ComboBoxText* cbt;
                  private:
                     /// Copy constructor.
                     sessionSelectionDialog(sessionSelectionDialog const& _ssd);
                     /// Assignment operator.
                     sessionSelectionDialog& operator=(sessionSelectionDialog const& _ssd);
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif

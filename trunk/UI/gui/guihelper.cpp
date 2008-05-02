/*
 * btg Copyright (C) 2008 Michael Wojciechowski.
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

#include "guihelper.h"

#include <bcore/util.h>
#include <bcore/t_string.h>

#include <gtkmm/main.h>

#include "sessionselection.h"
#include "sessiondialog.h"
#include "mainstatusbar.h"

#include "authdialog.h"

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         using namespace btg::core;
         using namespace btg::core::client;

         guiStartupHelper::guiStartupHelper(btg::core::LogWrapperType _logwrapper,
                                            btg::core::client::clientConfiguration&        _config,
                                            btg::core::client::commandLineArgumentHandler& _clah,
                                            btg::core::messageTransport&                   _messageTransport,
                                            btg::core::client::clientHandler&              _handler)
            : btg::core::client::startupHelper(_logwrapper,
                                               GPD->sGUI_CLIENT(),
                                               _config,
                                               _clah,
                                               _messageTransport,
                                               _handler,
                                               *this)
         {

         }

         bool guiStartupHelper::AuthQuery()
         {
            bool status = false;

            authDialog* authdialog = new authDialog();

            if (authdialog->run() == Gtk::RESPONSE_OK)
               {
                  std::string username;
                  std::string password;

                  if (authdialog->getUsernameAndPassword(username, password))
                     {
                        setAuth(username, password);

                        status = true;
                     }
               }

            delete authdialog;
            authdialog = 0;

            return status;
         }

         bool guiStartupHelper::AttachSessionQuery(t_longList const& _sessionsIDs,
                                                   t_strList const& _sessionNames,
                                                   t_long & _session)
         {
            // Assumption: An instance of Gtk::Main was constructed before the call to
            // sessionSelectionDialog::run().

            sessionSelectionDialog* ssd = new sessionSelectionDialog("Select session to attach to", 
                                                                     _sessionsIDs, 
                                                                     _sessionNames);
            ssd->run();

            if (!ssd->getSelectedSession(_session))
               {
                  // Pressed cancel.
                  BTG_FATAL_ERROR(logWrapper(), 
                                  GPD->sGUI_CLIENT(), "Cancelled.");

                  _session = Command::INVALID_SESSION;
                  return false;
               }

            delete ssd;
            ssd = 0;

            return true;
         }
                     
         void guiStartupHelper::ListSessions(t_longList const& _sessions,
                                             t_strList const& _sessionNames)
         {
            sessionSelectionDialog* ssd = new sessionSelectionDialog("Available sessions",
                                                                     _sessions, 
                                                                     _sessionNames, 
                                                                     true /* no selection. */);
            ssd->run();

            delete ssd;
            ssd = 0;
         }

         
         bool guiStartupHelper::DefaultAction(t_longList const& _sessions,
                                              t_strList const& _sessionNames,
                                              bool & _attach,
                                              t_long & _sessionId)
         {
            // Not attaching to a session. Show a dialog that lets one
            // select which session to use or to create a new one.
            bool status = false;

            sessionDialog sd(_sessions, _sessionNames);
            sd.run();

            switch (sd.getResult())
               {
               case sessionDialog::QUIT:
                  {
                     status = false;
                     break;
                  }
               case sessionDialog::NEW_SESSION:
                  {
                     status = true;
                     _attach = false;
                     break;
                  }
               case sessionDialog::SELECTED:
                  {
                     status = true;
                     _attach = true;
                     sd.getSelectedSession(_sessionId);
                     break;
                  }
               }
            return status;
         }

         guiStartupHelper::~guiStartupHelper()
         {

         }

      } // namespace gui
   } // namespace UI
} // namespace btg

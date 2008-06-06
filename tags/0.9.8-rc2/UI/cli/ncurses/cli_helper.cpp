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

#include "cli_helper.h"

#include <iostream>

#include <bcore/command_factory.h>
#include <bcore/util.h>
#include <bcore/t_string.h>

#include <bcore/hru.h>
#include <bcore/hrr.h>

#include <bcore/os/stdin.h>

#include <bcore/command/limit_base.h>

namespace btg
{
   namespace UI
   {
      namespace cli
      {

         using namespace btg::core;
         using namespace btg::core::client;

         cliStartupHelper::cliStartupHelper(btg::core::LogWrapperType _logwrapper,
                                            btg::core::client::clientConfiguration&        _config,
                                            btg::core::client::commandLineArgumentHandler& _clah,
                                            btg::core::messageTransport&                   _messageTransport,
                                            btg::core::client::clientHandler&              _handler)
            : btg::core::client::startupHelper(_logwrapper,
                                               btg::core::projectDefaults::sCLI_CLIENT(),
                                               _config,
                                               _clah,
                                               _messageTransport,
                                               _handler,
                                               dynamic_cast<btg::core::client::startupHelperIf&>(*this))
         {
         }

         bool cliStartupHelper::AuthQuery()
         {
            bool status = true;

            std::cout << "Username:" << std::endl;

            std::string username;

            std::cin >> username;

            std::cout << "Password:" << std::endl;

            std::string password;

            if (!btg::core::os::stdIn::getPassword(password))
               {
                  std::cout << "Unable to get password." << std::endl;

                  status = false;
                  return status;
               }

            setAuth(username, password);

            return status;
         }

         bool cliStartupHelper::AttachSessionQuery(t_longList const& _sessionsIDs,
                                                   t_strList const& _sessionNames,
                                                   t_long & _session)
         {
            std::cout << "Session(s):" << std::endl;

            t_strListCI sessionNameIter = _sessionNames.begin();
            t_int session_no          = 0;
            for (t_longListCI vlci = _sessionsIDs.begin();
                 vlci != _sessionsIDs.end();
                 vlci++)
               {
                  std::cout << session_no << ": " << *vlci << " (" << *sessionNameIter << ")" << std::endl;
                  sessionNameIter++;
                  session_no++;
               }

            // Get user input.
            bool done = false;
            t_int input = -1;
            while (done != true)
               {
                  if ((session_no-1) > 0)
                     {
                        std::cout << "Enter a number (0-" << (session_no-1) << "): " << std::endl;
                     }
                  else
                     {
                        std::cout << "Enter a number: " << std::endl;
                     }

                  if(!(std::cin >> input))
                     {
                        std::cin.clear();
                        std::cin.ignore(254, '\n');
                     }
                  else
                     {
                        if (input >= 0 && input < session_no)
                           {
                              done = true;
                           }
                     }
               }

            _session = _sessionsIDs.at(input);
            return true;
         }

         void cliStartupHelper::ListSessions(t_longList const& _sessionsIDs,
                                             t_strList const& _sessionNames)
         {
            t_strListCI sessionNameIter = _sessionNames.begin();

            if (_sessionsIDs.size() > 0)
               {
                  std::cout << "Session(s):" << std::endl;

                  for (t_longListCI vlci = _sessionsIDs.begin();
                       vlci != _sessionsIDs.end();
                       vlci++)
                     {
                        std::cout << *vlci << " (" << *sessionNameIter << ")" << std::endl;
                        sessionNameIter++;
                     }
               }
            else
               {
                  std::cout << "No sessions to list." << std::endl;
               }
         }

         bool cliStartupHelper::DefaultAction(t_longList const& _sessionsIDs,
                                              t_strList const& _sessionNames,
                                              bool & _attach,
                                              t_long & _sessionId)
         {
            std::cout << "0: create new session." << std::endl;

            t_strListCI sessionNameIter = _sessionNames.begin();
            t_int session_no          = 1;
            for (t_longListCI vlci = _sessionsIDs.begin();
                 vlci != _sessionsIDs.end();
                 vlci++)
               {
                  std::cout << session_no << ": " << *vlci << " (" << *sessionNameIter << ")" << std::endl;
                  sessionNameIter++;
                  session_no++;
               }

            // Get user input.
            bool done = false;
            t_int input = -1;
            while (done != true)
               {
                  std::cout << "Enter a number: " << std::endl;

                  if (!(std::cin >> input))
                     {
                        std::cin.clear();
                        std::cin.ignore(254, '\n');
                     }
                  else
                     {
                        if (input >= 1 && input < session_no)
                           {
                              done       = true;
                              _attach    = true;
                              _sessionId = _sessionsIDs[input-1];
                           }
                        else if (input == 0)
                           {
                              done = true;
                           }
                     }
               }

            return true;
         }

         cliStartupHelper::~cliStartupHelper()
         {

         }

      } // namespace cli
   } // namespace UI
} // namespace btg

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

#ifndef CLI_HELPER_H
#define CLI_HELPER_H

#include <string>
#include <vector>

#include <bcore/project.h>
#include <bcore/type_btg.h>
#include <bcore/command/command.h>
#include <bcore/transport/transport.h>
#include <bcore/dbuf.h>
#include <bcore/client/statemachine.h>
#include <bcore/client/clientcallback.h>
#include <bcore/client/clienthandler.h>
#include <bcore/client/configuration.h>

#include <bcore/client/helper.h>
#include <bcore/client/handlerthr.h>

#include <bcore/file_info.h>

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

               /// Helper: executes a number of tasks when this client starts.
               class cliStartupHelper: public btg::core::client::startupHelper, 
                  public btg::core::client::startupHelperIf
                  {
                  public:
                     /// Constructor.
                     cliStartupHelper(btg::core::LogWrapperType _logwrapper,
                                      btg::core::client::clientConfiguration&        _config,
                                      btg::core::client::commandLineArgumentHandler& _clah,
                                      btg::core::messageTransport&                   _messageTransport,
                                      btg::core::client::clientHandler&              _handler);

                     bool AuthQuery();

                     bool AttachSessionQuery(t_longList const& _sessionsIDs,
                                             t_strList const& _sessionNames,
                                             t_long & _session);

                     void ListSessions(t_longList const& _sessions,
                                       t_strList const& _sessionNames);
                     
                     bool DefaultAction(t_longList const& _sessions,
                                        t_strList const& _sessionNames,
                                        bool & _attach,
                                        t_long & _sessionId);

                     /// Destructor.
                     virtual ~cliStartupHelper();
                  };

               /** @} */

            } // namespace cli
      } // namespace UI
} // namespace btg

#endif // CLI_HELPER_H


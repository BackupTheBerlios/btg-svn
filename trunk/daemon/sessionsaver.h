/*
 * btg Copyright (C) 2005 Michael Wojciechowski.
 *
 * Session saving part written by Johan Ström.
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

#ifndef SESSIONSAVER_H
#define SESSIONSAVER_H

#include <bcore/command/setup.h>
#include <bcore/addrport.h>
#include "portmgr.h"
#include "limitmgr.h"

namespace btg
{
   namespace daemon
      {
         class daemonData;
         class eventHandler;
         class sessionList;

         /// This class is responsible for saving the daemon state
         /// into a file which can be read again to recover the saved
         /// state.
         class SessionSaver
            {
            public:
               /// Constructor.
               /// @param _verboseFlag   Indicates that the session saver should log verbose messages.
               /// @param _portManager   Reference to the port manager.
               /// @param _limitManager  Reference to the limit manager.
               /// @param _sessionlist   Reference to a list of sessions.
               /// @param _dd            Data used to initialize the daemon.
               SessionSaver(bool const _verboseFlag,
                            portManager & _portManager,
                            limitManager & _limitManager,
                            sessionList & _sessionlist,
                            daemonData & _dd);

               /// Load sessions from a file.
               t_int loadSessions(std::string const& _filename
#ifdef OLD_FORMAT
							, bool useBinaryFormat = false
#endif
							);

               /// Save sessions to a file.
               bool saveSessions(std::string const& _filename, bool const _dumpFastResume);
            private:
               /// Indicates that the session saver should log verbose
               /// messages.
               bool const verboseFlag_;

               /// Handle reading a saved session.
               /// @param [in] _filename Filename to read from.
               /// @param [in] _e        Pointer to externalization interface to use.
               /// @param [in] _version  Version of the file as indicated by the signature bytes
               t_int handleSaved(std::string const& _filename,
                                 btg::core::externalization::Externalization* _e,
                                 t_uint _version);

               /// Create a session.
               /// @param [in] _username     The username.
               /// @param [in] _session_id   The session ID.
               /// @param [in] _seed_limit   Seed limit.
               /// @param [in] _seed_timeout Seed timeout.
               /// @return Pointer to an event handler or 0 on failture.
               btg::daemon::eventHandler* createSession(std::string const & _username,
                                                        t_long const _session_id,
                                                        std::string const & _session_name,
                                                        t_int const _seed_limit,
                                                        t_int const _seed_timeout);

               /// Reference to the port manager.
               ///
               /// Access to network ports goes trough it.
               portManager & portManager_;

               /// Reference to the limit manager.
               limitManager & limitManager_;

               /// Reference to a map (sessions / eventhandlers).
               sessionList & sessionlist_;

               /// Data used to initialize the daemon.
               daemonData & dd;
            };
      } // namespace daemon
} // namespace btg

#endif // SESSIONSAVER_H

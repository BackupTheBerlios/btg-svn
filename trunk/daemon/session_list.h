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

#ifndef SESSION_LIST_H
#define SESSION_LIST_H

#include <bcore/type.h>
#include <bcore/logable.h>

#include "eventhandler.h"
#include <map>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

namespace btg
{
   namespace daemon
      {
         class IpFilterIf;

         /// List of sessions.
         /// Maps session IDs to eventhandler instances.
         class sessionList: public btg::core::Logable
            {
            public:
               /// Constructor.
               /// @param [in] _logwrapper  Pointer used to send logs to.
               /// @param [in] _verboseFlag Indicates that verbose logging should be done.
               /// @param [in] _maxSessions The max number of sessions.
               sessionList(btg::core::LogWrapperType _logwrapper,
                           bool const _verboseFlag,
                           t_uint const _maxSessions);

               /// Get a new session id.
               void new_session(t_long & _new_session);

               /// Add a new session together with an eventhandler.
               /// @param [in] _session      The session ID to use.
               /// @param [in] _eventhandler The eventhandler to use.
               /// @param [in] _resetCounter If true, the internal session counter will be adjusted.
               void add(t_long const _session, eventHandler* _eventhandler, bool _resetCounter = false);

               /// Get a pointer to a eventhandler for a specific session.
               /// @param [in] _session       The session ID to use.
               /// @param [out] _eventhandler Reference to a pointer to an eventhandler.
               /// @return True - success. False otherwise.
               bool get(t_long const & _session, eventHandler* & _eventhandler);

#if BTG_OPTION_SAVESESSIONS
               /// Serialize the contents.
               /// Deserialization is done by the session saver.
               void serialize(btg::core::externalization::Externalization* _e, 
                              bool const _dumpFastResume);
#endif

               /// Delete the eventhandler belonging to session.
               void erase(t_long const & _session);

               /// Get all contained session IDs belonging to an user.
               void getIds(std::string const& _username, 
                           std::vector<t_long> & _sessions);

               /// Get all contained session IDs.
               void getIds(std::vector<t_long> & _sessions);

               /// Get the names of the contained sessions belonging
               /// to an user.
               void getNames(std::string const& _username, 
                             std::vector<std::string> & _names);

               /// Get the names of the contained sessions. 
               void getNames(std::vector<std::string> & _names);

               /// For each event handler contained in an instance of
               /// this class, check their limits and check for
               /// alerts.
               void checkLimitsAndAlerts();

               /// For each session, update the contained elapsed or
               /// seed counter.
               void updateElapsedOrSeedCounter();

               /// Delete all eventhandlers.
               void deleteAll();

               /// Tell the sessions to shutdown themselves.
               ///
               /// This was done to avoid waiting for many sessions to
               /// shutdown (first implemented in libtorrent 0.11).
               void shutdown();

               /// Update the IP filter used.
               void updateFilter(IpFilterIf* _filter);

               /// Destructor.
               virtual ~sessionList();
            private:
               /// Get a session ID.
               void get_session(t_long & _new_session);

               /// Check that a session ID is unique.
               bool check_session(t_long const _session) const;

               /// Get the size of the contained list of eventhandlers.
               t_uint size() const;

               /// Indicates that verbose logging should be done.
               bool const                      verboseFlag_;

               /// Max number of sessions (IDs).
               t_uint const                    maxSessions_;

               /// The current session ID.
               t_long                          current_id_;

               /// Maps session IDs to eventhandlers.
               std::map<t_long, eventHandler*> eventhandlers_;
            private:
               /// Mutex used to control access to the members
               /// of this class from the outside.
               boost::mutex interfaceMutex_;
            };

      } // namespace daemon
} // namespace btg

#endif // SESSION_LIST_H


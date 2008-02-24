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

#ifndef LIMIT_MGR_H
#define LIMIT_MGR_H

#include <bcore/type.h>
#include <vector>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include "limitval.h"

namespace libtorrent
{
   class session;
}

namespace btg
{
   namespace daemon
      {

         /**
          * \addtogroup daemon Daemon
          */
         /** @{ */

         /// Thread which executes the limit operations.
         ///
         /// Implemented to avoid blocking the main thread.
         ///
         /// \note The scenario where an update is running, and a
         /// torrent is removed is handled, because the add/remove
         /// operations block on the same interface mutex.
         class limitManager
            {
            public:
               /// Actions performed by the thread.
               enum ACTION
                  {
                     UNDEF = -1, //<! Undefined action.
                     UPDATE      //<! Update limits set on every session.
                  };

               /// Constructor.
               /// @param [in] _verboseFlag Indicates if verbose logging should be performed.
               /// @param [in] _interval    How often the update function is called. In seconds.
               /// @param [in] _upload_rate_limit      Initial max upload rate for this session.
               /// @param [in] _download_rate_limit    Initial max upload rate for this session.
               /// @param [in] _max_uploads            Initial max number of uploads for this session.
               /// @param [in] _max_connections        Initial max number of connections for this session.
               limitManager(bool const _verboseFlag,
                            t_uint const _interval,
                            t_int const _upload_rate_limit,
                            t_int const _download_rate_limit,
                            t_int const _max_uploads,
                            t_long const _max_connections);

               /// Set global limits.
               /// @param [in] _upload_rate_limit      Max upload rate for this session.
               /// @param [in] _download_rate_limit    Max upload rate for this session.
               /// @param [in] _max_uploads            Max number of uploads for this session.
               /// @param [in] _max_connections        Max number of connections for this session.
               /// \note This operation blocks.
               void set(t_int const _upload_rate_limit,
                        t_int const _download_rate_limit,
                        t_int const _max_uploads,
                        t_long const _max_connections);

               /// Get global limits.
               /// @param [in] _upload_rate_limit      Reference to max upload rate.
               /// @param [in] _download_rate_limit    Reference to max upload rate.
               /// @param [in] _max_uploads            Reference to max number of uploads.
               /// @param [in] _max_connections        Reference to max number of connections.
               /// \note This operation blocks.
               void get(t_int & _upload_rate_limit,
                        t_int & _download_rate_limit,
                        t_int & _max_uploads,
                        t_long & _max_connections);

               /// Update the limits set on all contained sessions.
               ///
               /// \note This operation does not block.
               void update();

               /// Add a session.
               /// \note This operation blocks.
               void add(libtorrent::session* _session);

               /// Remove a session.
               /// \note This operation blocks.
               void remove(libtorrent::session* _session);

               /// Make this thread stop.
               void stop();

               /// Destructor.
               ~limitManager();

            private:
               /// Indicates if this class shall be verbose.
               bool const verboseFlag_;

               /// How often the update function is called. In seconds.
               t_uint const interval_;

               /// List of sessions.
               std::vector<sessionData> sessions_;

               /// Limit: the upload rate.
               t_int upload_rate_limit_;

               /// Limit: the download rate.
               t_int download_rate_limit_;

               /// Limit: max uploads.
               t_int max_uploads_;

               /// Limit: max connections.
               t_long max_connections_;

               /// What action the thread should execute next.
               ACTION           action_;

               /// Mutex used to control access to the members
               /// of this class from the outside.
               boost::mutex     interfaceMutex_;

               /// Condition variable used for communication of
               /// results from an action executed by the
               /// thread.
               boost::condition interfaceCondition_;

               /// True if the thread should terminate itself.
               bool             die_;

               // Parameters.
               /// Parameter: Upload rate.
               t_int               param_upload_rate_limit;

               /// Parameter: Download rate.
               t_int               param_download_rate_limit;

               /// Parameter: Max uploads.
               t_int               param_max_uploads;

               /// Parameter: Max connections.
               t_long              param_max_connections;

               /// Parameter: Pointer to current libtorrent session.
               libtorrent::session* param_session;

               /// The thread used by this class.
               boost::thread        thread_;

               /// Update the used field of ressource_request members
               /// of the contained sessions.
               void updateUsed();

               /// Get a vector of session pointers, which is used as
               /// an argument to allocate_resources.
               void getSessions(std::vector<libtorrent::session*> & _sessions) const;

               /// Function used for the thread.
               void work();

               /// Set limits.
               void work_set();

               /// Get limits.
               void work_get(t_int & _upload_rate_limit,
                             t_int & _download_rate_limit,
                             t_int & _max_uploads,
                             t_long & _max_connections);

               /// Add a libtorrent session.
               void work_add();

               /// Remove a libtorrent session.
               void work_remove();

               /// Update limits set on the contained libtorrent
               /// sessions.
               void work_update();
            };

         /** @} */

      } // namespace daemon
} // namespace btg

#endif // LIMIT_MGR_H


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

#ifndef CALLBACKMGR_H
#define CALLBACKMGR_H

#include <bcore/type.h>

#include <string>
#include <map>
#include <vector>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

namespace btg
{
   namespace daemon
      {

         /**
          * \addtogroup daemon Daemon
          */
         /** @{ */

         class callbackManager
            {
            public:
               /// The events reported by btgdaemon.
               enum EVENT
                  {
                     CBM_UNDEF     = 0, //!< Undefined.
                     CBM_ADD,           //!< A torrent was added.
                     CBM_REM,           //!< A torrent was removed.
                     CBM_FINISHED,      //!< A torrent finished. Not seeding, stopped.
                     CBM_SEED,          //!< A torrent began seeding.
                     CBM_CLEAN          //!< A torrent was cleaned.
                  };

               /// Constructor.
               callbackManager(bool const _verboseFlag);

               /// Add an user with a callback.
               bool add(std::string const& _username,
                        std::string const& _callback);

               /// Remove a callback.
               bool remove(std::string const& _username);

               /// Report an event.
               /// The _arguments list contains the arguments given to
               /// the script executed after the type of the event
               /// being reported.
               void event(std::string const& _username,
                          EVENT const _event,
                          std::vector<std::string> const& _arguments);

               /// Destructor.
               ~callbackManager();
            private:
               bool                                verboseFlag;
               /// Maps user to callback.
               std::map<std::string, std::string > userCallbackMap;

               /// Maps event ID to event name.
               std::map<t_int, std::string>        eventNameMap;

               /// Mutex used to control access to the members
               /// of this class from the outside.
               boost::mutex                        interfaceMutex_;

               /// Condition variable used for communication of
               /// results from an action executed by the
               /// thread.
               boost::condition                    interfaceCondition_;

               /// True if the thread should terminate itself.
               bool                                die;

               struct threadData
               {
                  std::string username;
                  EVENT event;
                  std::vector<std::string> arguments;
               };

               std::vector<threadData> threaddata;

               /// The thread used by this class.
               boost::thread                       thread;

               /// Function used for the thread.
               void work();

               void work_pop();
            };

         /** @} */

      } // namespace daemon
} // namespace btg

#endif // CALLBACKMGR_H


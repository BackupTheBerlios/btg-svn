/*
 * btg Copyright (C) 2009 Michael Wojciechowski.
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

#ifndef ALERTHANDLER_THR_H
#define ALERTHANDLER_THR_H

#include <bcore/logable.h>

#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include <bcore/os/timer.h>
#include <bcore/os/fstream.h>

namespace btg
{
   namespace daemon
   {
      class sessionList;

      /// Thread handling alerts.
      class alertHandlerThread: public btg::core::Logable
      {
      public:
         /// Constructor.
         alertHandlerThread(btg::core::LogWrapperType _logwrapper,
                            sessionList*  _sessionlist,
                            bool const _verboseFlag);
         
         /// Destructor.
         ~alertHandlerThread();

         /// Make the thread terminate.
         void stop();

      protected:
         /// Function used for the thread.
         void work();

         /// Indicates if the thread should terminate.
         bool die_;
      protected:
         /// Function which implements checks of the different timers
         /// used by the daemon.
         void checkTimeout();
      protected:
         /* Handlers */

         /* Timers */

         /// The timer used for checking alerts for
         /// the contained sessions.
         btg::core::os::Timer            session_timer_;
         
         /// Indicates that the session timer has expired.
         bool                            session_timer_trigger_;

      protected:
         /// List of contained sessions.
         sessionList*                    sessionlist_;

      protected:
         /// Mutex used to control access to the members
         /// of this class from the outside.
         boost::mutex          interfaceMutex_;
         
         /// The thread used by this class.
         boost::thread         thread_;

         /// Indicates if the logging done by this module shall be
         /// verbose.
         bool const verboseFlag_;

      };

   } // namespace daemon
} // namespace btg

#endif // ALERTHANDLER_THR_H

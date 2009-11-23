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

#ifndef DAEMONHANDLER_THR_H
#define DAEMONHANDLER_THR_H

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
      class SessionSaver;
      class limitManager;
      class urlManager;
      class fileManager;

      /// Daemon handler thread.
      /// Responsible for running periodic tasks like session saving,
      /// downloading files etc..
      class daemonHandlerThread: public btg::core::Logable
      {
      public:
         /// Constructor.
         daemonHandlerThread(btg::core::LogWrapperType _logwrapper,
                             bool const _ss_enable,
#if BTG_OPTION_SAVESESSIONS
                             btg::core::os::fstream & _ss_file,
#endif
                             t_int const _SSTimeout,
                             sessionList*  _sessionlist,
#if BTG_OPTION_SAVESESSIONS
                             SessionSaver* _sessionsaver,
#endif
                             limitManager*  _limitManager,
                             t_int const    _limitTimerMax,
                             urlManager*  _urlmgr,
                             fileManager* _filemgr,
                             bool const _verboseFlag);
         
         /// Destructor.
         ~daemonHandlerThread();

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

         /// Every few seconds, check if any of the file uploads
         /// changed state.
         void handleFileDownloads();

         /// Once every few seconds, see if any of the URLs
         /// downloading changed state.
         void handleUrlDownloads();

         /// Executed when session timer times out.
         void handleSessions();

         /// Executed when periodic session save timer times out.
         void handlePeriodicSessions();

         /// Executed when limit timer times out.
         void handleLimits();

         /// Executed when elapsed timer times out.
         /// This checks for example if any seeds are going to be
         /// stopped because of user set limits.
         void handleElapsed();

         /// Session saving enabled?
         bool ss_enable_;
         /// Session saved to this file if enabled.
#if BTG_OPTION_SAVESESSIONS
         btg::core::os::fstream & ss_file_;
#endif
         /* Timers */

         /// The timer used for checking limits and alerts for
         /// the contained sessions.
         btg::core::os::Timer            session_timer_;
         
         /// Indicates that the session timer has expired.
         bool                            session_timer_trigger_;
#if BTG_OPTION_URL
         /// Timer used to check for completed URL downloads.
         btg::core::os::Timer            url_timer_;
         /// Indicates that the URL timer timed out.
         bool                            url_timer_trigger_;
#endif // BTG_OPTION_URL
         
         /// Timer used to check for completed URL downloads.
         btg::core::os::Timer            file_timer_;
         /// Indicates that the URL timer timed out.
         bool                            file_timer_trigger_;
         /// Timer used for setting global limits.
         btg::core::os::Timer            limit_timer_;

         /// Indicates that the limit timer has expired.
         bool                            limit_timer_trigger_;
         
         /// Timer used for increasing the elapsed or seed time
         /// for torrents.
         btg::core::os::Timer            elapsed_seed_timer_;
         
         /// Indicates that the elapsed/seed timer has expired.
         bool                            elapsed_timer_trigger_;
         
         /// Timer used for timed session saving.
         btg::core::os::Timer            periodic_ssave_timer_;
         
         /// Indicates that the periodic session save timer has
         /// expired.
         bool                            periodic_ssave_timer_trigger_;
      protected:
         /// List of contained sessions.
         sessionList*                    sessionlist_;

#if BTG_OPTION_SAVESESSIONS
         /// Instance of the class used for saving sessions to a file.
         SessionSaver*                   sessionsaver_;
#endif // BTG_OPTION_SAVESESSIONS

         /// Entity used for setting global limits.
         limitManager*                   limitManager_;

         /// Pointer to URL manager.
         btg::daemon::urlManager*        urlmgr_;

         /// Pointer to file manager.
         btg::daemon::fileManager*       filemgr_;
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

#endif // DAEMONHANDLER_THR_H

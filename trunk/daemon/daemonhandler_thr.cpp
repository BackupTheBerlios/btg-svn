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

#include "daemonhandler_thr.h"
#include <boost/bind.hpp>
#include <daemon/modulelog.h>

#include "session_list.h"
#include "sessionsaver.h"
#include "limitmgr.h"
#include "urlmgr.h"
#include "filemgr.h"

#include <bcore/os/sleep.h>

namespace btg
{
   namespace daemon
   {
      const std::string moduleName("hdlthr");

      daemonHandlerThread::daemonHandlerThread(btg::core::LogWrapperType _logwrapper,
                                               bool const _ss_enable,
                                               btg::core::os::fstream & _ss_file,
                                               t_int const _SSTimeout,
                                               sessionList*  _sessionlist,
                                               SessionSaver* _sessionsaver,
                                               limitManager* _limitManager,
                                               t_int const _limitTimerMax,
                                               urlManager*  _urlmgr,
                                               fileManager* _filemgr,
                                               bool const _verboseFlag)
         : btg::core::Logable::Logable(_logwrapper),
           die_(false),

           ss_enable_(_ss_enable),
           ss_file_(_ss_file),
          
           session_timer_(10), /* 10 seconds. */
           session_timer_trigger_(false),
#if BTG_OPTION_URL
           url_timer_(5), /* 5 seconds. */
           url_timer_trigger_(false),
#endif
           file_timer_(5), /* 5 seconds. */
           file_timer_trigger_(false),
           limit_timer_(_limitTimerMax), /* x seconds. */
           limit_timer_trigger_(false),
           elapsed_seed_timer_(60), /* 1 minute. */
           elapsed_timer_trigger_(false),
           periodic_ssave_timer_(_SSTimeout),
           periodic_ssave_timer_trigger_(false),

           sessionlist_(_sessionlist),
           sessionsaver_(_sessionsaver),
           limitManager_(_limitManager),

           urlmgr_(_urlmgr),
           filemgr_(_filemgr),

           thread_(
                   boost::bind(&daemonHandlerThread::work, boost::ref(*this))
                   ),
           verboseFlag_(_verboseFlag)
      {
      }

      daemonHandlerThread::~daemonHandlerThread()
      {
         if (!die_)
         {
            stop();
         }
         BTG_MNOTICE(logWrapper(), "destroyed");
      }

      void daemonHandlerThread::stop()
      {
         {
            boost::mutex::scoped_lock interface_lock(interfaceMutex_);
            die_ = true;
         }

         thread_.join();

         BTG_MNOTICE(logWrapper(), "stopped");
      }

      void daemonHandlerThread::work()
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "started.");

         while (!die_)
         {
            // Check if timeouts occured every few seconds.
            checkTimeout();
            btg::core::os::Sleep::sleepSecond(1);
         }
      }

      void daemonHandlerThread::handleFileDownloads()
      {
         if (filemgr_->size() > 0)
         {
            MVERBOSE_LOG(logWrapper(), verboseFlag_, "Checking file downloads (" << filemgr_->size() << ").");
         }
         filemgr_->updateAge();
         filemgr_->removeDead();
      }

      void daemonHandlerThread::handleUrlDownloads()
      {
#if BTG_OPTION_URL
         if (urlmgr_->size() > 0)
         {
            MVERBOSE_LOG(logWrapper(), verboseFlag_, "Checking URL downloads(" << urlmgr_->size() << ").");
            urlmgr_->checkUrlDownloads();
         }
#endif
      }

      void daemonHandlerThread::handleSessions()
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "Checking limits.");
         sessionlist_->checkLimits();
         session_timer_trigger_ = false;
         session_timer_.Reset();
      }

      void daemonHandlerThread::handlePeriodicSessions()
      {
#if BTG_OPTION_SAVESESSIONS
         if (ss_enable_)
            {
               BTG_MNOTICE(logWrapper(), "Periodically saving sessions");
               MVERBOSE_LOG(logWrapper(), verboseFlag_, "Periodically saving sessions.");
               sessionsaver_->saveSessions(ss_file_, true /* also save fast-resume data */);
            }
#endif
      }

      void daemonHandlerThread::handleLimits()
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "Updating limits.");
         limitManager_->update();
      }

      void daemonHandlerThread::handleElapsed()
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "Updating seed counters.");
         sessionlist_->updateElapsedOrSeedCounter();
      }

      void daemonHandlerThread::checkTimeout()
      {
         if (file_timer_trigger_)
            {
               file_timer_trigger_ = false;
               file_timer_.Reset();
               handleFileDownloads();
            }
         
#if BTG_OPTION_URL
         if (url_timer_trigger_)
            {
               url_timer_trigger_ = false;
               url_timer_.Reset();

               handleUrlDownloads();
            }
#endif
         if (session_timer_trigger_)
            {
               handleSessions();
               return;
            }

         if (periodic_ssave_timer_trigger_)
            {
               handlePeriodicSessions();

               periodic_ssave_timer_trigger_ = false;
               periodic_ssave_timer_.Reset();
               return;
            }

         if (limit_timer_trigger_)
            {
               handleLimits();

               limit_timer_.Reset();
               limit_timer_trigger_ = false;
               return;
            }

         if (elapsed_timer_trigger_)
            {
               handleElapsed();

               elapsed_timer_trigger_ = false;
               elapsed_seed_timer_.Reset();
               return;
            }

         if (file_timer_.Timeout())
            {
               file_timer_trigger_ = true;
            }
#if BTG_OPTION_URL
         if (url_timer_.Timeout())
            {
               url_timer_trigger_ = true;
            }
#endif
         if (session_timer_.Timeout())
            {
               session_timer_trigger_ = true;
            }

         if (periodic_ssave_timer_.Timeout())
            {
               periodic_ssave_timer_trigger_ = true;
            }

         if (limit_timer_.Timeout())
            {
               limit_timer_trigger_ = true;
            }

         if (elapsed_seed_timer_.Timeout())
            {
               elapsed_timer_trigger_ = true;
            }
      }

   } // namespace daemon
} // namespace btg



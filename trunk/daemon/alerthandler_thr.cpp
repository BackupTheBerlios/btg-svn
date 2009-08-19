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

#include "alerthandler_thr.h"
#include <boost/bind.hpp>
#include <daemon/modulelog.h>

#include "session_list.h"

#include <bcore/os/sleep.h>

namespace btg
{
   namespace daemon
   {
      const std::string moduleName("alertthr");

      alertHandlerThread::alertHandlerThread(btg::core::LogWrapperType _logwrapper,
                                             sessionList*  _sessionlist,
                                             bool const _verboseFlag)
         : btg::core::Logable::Logable(_logwrapper),
           die_(false),

           session_timer_(1), /* 1 second. */
           session_timer_trigger_(false),
           sessionlist_(_sessionlist),

           thread_(
                   boost::bind(&alertHandlerThread::work, boost::ref(*this))
                   ),
           verboseFlag_(_verboseFlag)
      {
      }

      alertHandlerThread::~alertHandlerThread()
      {
         if (!die_)
         {
            stop();
         }
         BTG_MNOTICE(logWrapper(), "destroyed");
      }

      void alertHandlerThread::stop()
      {
         {
            boost::mutex::scoped_lock interface_lock(interfaceMutex_);
            die_ = true;
         }

         thread_.join();

         BTG_MNOTICE(logWrapper(), "stopped");
      }

      void alertHandlerThread::work()
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "started.");

         while (!die_)
         {
            // Check if timeouts occured.
            checkTimeout();
            btg::core::os::Sleep::sleepSecond(1);
         }
      }

      void alertHandlerThread::checkTimeout()
      {
         if (session_timer_trigger_)
            {
               MVERBOSE_LOG(logWrapper(), verboseFlag_, "Checking alerts.");
               sessionlist_->handleAlerts();
               session_timer_trigger_ = false;
               session_timer_.Reset();
               
               return;
            }

         if (session_timer_.Timeout())
            {
               session_timer_trigger_ = true;
            }
      }

   } // namespace daemon
} // namespace btg



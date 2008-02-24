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

#include "handlerthr.h"

#include <boost/bind.hpp>

#include <bcore/os/sleep.h>
#include <bcore/command/limit_base.h>

#include <bcore/verbose.h>
#include <bcore/logmacro.h>

namespace btg
{
   namespace core
   {
      namespace client
      {

         using namespace btg::core;

         handlerThread::handlerThread(LogWrapperType _logwrapper,
                                      bool const _verboseFlag,
                                      handlerThreadIf* _handler)
            : Logable(_logwrapper),
              verboseFlag_(_verboseFlag),
              interfaceMutex_(),
              die_(false),
              handler_(_handler),
              timer_(3 /* seconds */),
              thread_(
                      boost::bind(&handlerThread::work, boost::ref(*this))
                      )
         {
         }

         struct null_deleter
         {
            void operator()(void const *) const
            {
            }
         };

         boost::shared_ptr<boost::mutex> handlerThread::mutex()
         {
            return boost::shared_ptr<boost::mutex>(&interfaceMutex_, null_deleter());
         }

         void handlerThread::forceUpdate()
         {
            boost::mutex::scoped_lock interface_lock(interfaceMutex_);

            BTG_NOTICE(logWrapper(), "Updating status");
            timer_.Reset();
            handler_->resetStatusList();
            handler_->reqStatus(-1, true);
         }

         void handlerThread::work()
         {
            // Do an initial update first.
            {
               boost::mutex::scoped_lock interface_lock(interfaceMutex_);
               handler_->resetStatusList();
               handler_->reqStatus(-1, true);
               BTG_NOTICE(logWrapper(), "Initial update.")
            }

            // Keep on doing updates, dictated by a timer.
            while (!die_)
               {
                  {
                     boost::mutex::scoped_lock interface_lock(interfaceMutex_);
                     scopedLockPasser lock_setter(*handler_, interface_lock);
                     
                     if (timer_.Timeout())
                        {
                           timer_.Reset();

                           // Update contexts.

                           if (!handler_->statusListUpdated())
                              {
                                 // Request status of all contexts.
                                 BTG_NOTICE(logWrapper(), "Updating status");
                                 handler_->reqStatus(-1, true);
                              }
                        }
                  } // interface locked

                  if (!die_)
                     {
                        btg::core::os::Sleep::sleepMiliSeconds(64);
                     }
               } // while !die
         }

         handlerThreadIf* handlerThread::handler() const 
         {
            return handler_;
         }

         handlerThread::~handlerThread()
         {
            die_ = true;
            thread_.join();

            BTG_NOTICE(logWrapper(), "Thread stopped.");
         }

         /* */

         handlerThreadIf::handlerThreadIf(LogWrapperType _logwrapper,
                                          btg::core::externalization::Externalization* _e,
                                          btg::core::messageTransport*            _transport,
                                          btg::core::client::clientConfiguration* _config,
                                          btg::core::client::lastFiles*           _lastfiles,
                                          bool const _verboseFlag,
                                          bool const _autoStartFlag)
            : clientCallback(),
              clientHandler(_logwrapper,
                            _e,
                            this,
                            _transport,
                            _config,
                            _lastfiles,
                            _verboseFlag,
                            _autoStartFlag),
              statusList_(),
              statusListUpdated_(false),
              statusSize_(0),
              threadMutexLock(NULL)
         {

         }

         bool handlerThreadIf::statusListUpdated() const
         {
            return statusListUpdated_;
         }

         t_uint handlerThreadIf::statusSize() const
         {
            return statusSize_;
         }

         void handlerThreadIf::resetStatusList()
         {
            statusSize_ = 0;
         }

         void handlerThreadIf::getStatusList(t_statusList & _statusList)
         {
            _statusList        = statusList_;
            statusListUpdated_ = false;
         }
         
         void handlerThreadIf::triggerThreadMutexLock()
         {
            if (threadMutexLock)
               {
                  threadMutexLock->unlock();
                  threadMutexLock->lock();
               }
         }

         handlerThreadIf::~handlerThreadIf()
         {

         }
         
         scopedLockPasser::scopedLockPasser(handlerThreadIf & htif, boost::mutex::scoped_lock & lock)
         : pthreadMutexLock(& htif.threadMutexLock)
         {
            if (pthreadMutexLock)
            {
               * pthreadMutexLock = & lock;
            }
         }
         
         scopedLockPasser::~scopedLockPasser()
         {
            if (pthreadMutexLock)
            {
               * pthreadMutexLock = NULL;
            }
         }

      } // namespace client
   } // namespace core
} // namespace btg



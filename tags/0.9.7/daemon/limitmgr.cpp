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

#include "limitmgr.h"

#include <libtorrent/session.hpp>

#include <boost/bind.hpp>

#include <bcore/os/sleep.h>
#include <bcore/command/limit_base.h>

#include <bcore/verbose.h>

#include "lt_version.h"

#include "modulelog.h"

#include <daemon/resource/allocate_resources.h>

namespace btg
{
   namespace daemon
   {

      using namespace btg::core;

      const std::string moduleName("lim");

      /* */
      /* */
      /* */

      limitManager::limitManager(btg::core::LogWrapperType _logwrapper,
                                 bool const _verboseFlag,
                                 t_uint const _interval,
                                 t_int const _upload_rate_limit,
                                 t_int const _download_rate_limit,
                                 t_int const _max_uploads,
                                 t_long const _max_connections)
         : btg::core::Logable(_logwrapper),
           verboseFlag_(_verboseFlag),
           interval_(_interval),
           sessions_(),
           upload_rate_limit_(_upload_rate_limit),
           download_rate_limit_(_download_rate_limit),
           max_uploads_(_max_uploads),
           max_connections_(_max_connections),
           action_(limitManager::UNDEF),
           interfaceMutex_(),
           interfaceCondition_(),
           die_(false),
           param_upload_rate_limit(limitBase::LIMIT_DISABLED),
           param_download_rate_limit(limitBase::LIMIT_DISABLED),
           param_max_uploads(limitBase::LIMIT_DISABLED),
           param_max_connections(limitBase::LIMIT_DISABLED),
           param_session(0),
           thread_(
                   boost::bind(&limitManager::work, boost::ref(*this))
                   )
      {
         BTG_MNOTICE(logWrapper(), "constructed");
         BTG_MNOTICE(logWrapper(), 
                     "limits: " << upload_rate_limit_ << ", " <<
                     download_rate_limit_ << ", " << max_uploads_ << ", " <<
                     max_connections_);
      }

      void limitManager::set(t_int const _upload_rate_limit,
                             t_int const _download_rate_limit,
                             t_int const _max_uploads,
                             t_long const _max_connections)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         param_upload_rate_limit    = _upload_rate_limit;
         param_download_rate_limit  = _download_rate_limit;
         param_max_uploads          = _max_uploads;
         param_max_connections      = _max_connections;

         work_set();
      }

      void limitManager::get(t_int & _upload_rate_limit,
                             t_int & _download_rate_limit,
                             t_int & _max_uploads,
                             t_long & _max_connections)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         work_get(_upload_rate_limit,
                  _download_rate_limit,
                  _max_uploads,
                  _max_connections);
      }

      void limitManager::add(libtorrent::session* _session)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);
         param_session = _session;
         work_add();
      }

      void limitManager::remove(libtorrent::session* _session)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);
         param_session = _session;
         work_remove();
      }

      void limitManager::update()
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);
         action_ = limitManager::UPDATE;
         interfaceCondition_.notify_one();
      }

      void limitManager::work()
      {
         BTG_MNOTICE(logWrapper(), "thread started");

         while (!die_)
            {
               {
                  boost::mutex::scoped_lock interface_lock(interfaceMutex_);

                  interfaceCondition_.wait(interface_lock);

                  // React on interface calls.
                  //
                  // In the following, a calling thread is unblocked
                  // as soon as a function transfered its arguments to
                  // the thread.
                  switch (action_)
                     {
                     case limitManager::UPDATE:
                        {
                           work_update();
                           action_  = limitManager::UNDEF;
                           break;
                        }
                     case limitManager::UNDEF:
                        {
                           // BTG_NOTICE("limitManager, sleep.");
                           btg::core::os::Sleep::sleepMiliSeconds(64);
                           break;
                        }
                     }
               } // interface locked
            } // while !die
      }

      void limitManager::work_set()
      {
         VERBOSE_LOG(logWrapper(), 
                     verboseFlag_, moduleName << ":"
                     " setting limits, upload = " << param_upload_rate_limit << ", " <<
                     " download = " << param_download_rate_limit << ", " <<
                     " max upload = " << param_max_uploads << ", " <<
                     " max connections = " << param_max_connections << ".");

         upload_rate_limit_   = param_upload_rate_limit;
         download_rate_limit_ = param_download_rate_limit;
         max_uploads_         = param_max_uploads;
         max_connections_     = param_max_connections;
      }

      void limitManager::work_get(t_int & _upload_rate_limit,
                                  t_int & _download_rate_limit,
                                  t_int & _max_uploads,
                                  t_long & _max_connections)
      {
         _upload_rate_limit   = upload_rate_limit_;
         _download_rate_limit = download_rate_limit_;
         _max_uploads         = max_uploads_;
         _max_connections     = max_connections_;
      }

      void limitManager::work_add()
      {
         // A session was just added.
         // It does not use any ressources yet.
         sessions_.push_back(sessionData(param_session));

         param_session = 0;
      }
      void limitManager::work_remove()
      {
         std::vector<sessionData>::iterator sessionIter;
         for (sessionIter = sessions_.begin();
              sessionIter != sessions_.end();
              sessionIter++)
            {
               if (param_session == sessionIter->session())
                  {
                     sessions_.erase(sessionIter);
                     break;
                  }
            }

         param_session = 0;
      }

      void limitManager::work_update()
      {
         // For each session, find out the actal bandwidth used, and
         // update the session's used fields.
         updateUsed();

         // Tick interval in seconds.
         t_int tick_interval = interval_;

         t_int resources     = limitValue::inf;

         // Upload rate.
         if (upload_rate_limit_ == limitBase::LIMIT_DISABLED)
            {
               // resources = std::numeric_limits<int>::max();
               resources = limitValue::inf;
            }
         else
            {
               resources = upload_rate_limit_ * tick_interval;
            }

         BTG_MNOTICE(logWrapper(), "distributing resources (ul): " << resources);

         allocate_resources(resources,
                            sessions_,
                            &sessionData::val_ul_rate);

         // Download rate.
         if (download_rate_limit_ == limitBase::LIMIT_DISABLED)
            {
               // resources = std::numeric_limits<int>::max();
               resources = limitValue::inf;
            }
         else
            {
               resources = download_rate_limit_ * tick_interval;
            }

         BTG_MNOTICE(logWrapper(), "distributing resources (dl): " << resources);

         allocate_resources(resources,
                            sessions_,
                            &sessionData::val_dl_rate);

         // Max uploads.
         if (max_uploads_ == limitBase::LIMIT_DISABLED)
            {
               resources = limitValue::inf;
            }
         else
            {
               resources = max_uploads_ * tick_interval;
            }

         BTG_MNOTICE(logWrapper(), "distributing resources (max ul): " << resources);
         
         allocate_resources(resources,
                            sessions_,
                            &sessionData::val_uploads);

         // Max connections.

         if (max_connections_ == limitBase::LIMIT_DISABLED)
            {
               resources = limitValue::inf;
            }
         else
            {
               resources = max_connections_ * tick_interval;
            }

         BTG_MNOTICE(logWrapper(), "distributing resources (max conn): " << resources);

         allocate_resources(resources,
                            sessions_,
                            &sessionData::val_connections);

         // The limits were distributed above, now set them.

         std::vector<sessionData>::iterator sessionIter;
         for (sessionIter = sessions_.begin();
              sessionIter != sessions_.end();
              sessionIter++)
            {
               sessionIter->set();
            }
      }

      void limitManager::updateUsed()
      {
         std::vector<sessionData>::iterator sessionIter;
         for (sessionIter = sessions_.begin();
              sessionIter != sessions_.end();
              sessionIter++)
            {
               sessionData & sessiondata = *sessionIter;

               // BTG_MNOTICE(logWrapper(), "rates: " << upload_rate_limit_ << ", " << download_rate_limit_ << ", " << max_uploads_ << ", " << max_connections_);

               sessiondata.update(upload_rate_limit_,
                                  download_rate_limit_,
                                  max_uploads_,
                                  max_connections_,
                                  interval_);
            }
      }

      void limitManager::getSessions(std::vector<libtorrent::session*> & _sessions) const
      {
         std::vector<sessionData>::const_iterator sessionIter;
         for (sessionIter = sessions_.begin();
              sessionIter != sessions_.end();
              sessionIter++)
            {
               _sessions.push_back(sessionIter->session());
            }
      }

      void limitManager::stop()
      {
         {
            boost::mutex::scoped_lock interface_lock(interfaceMutex_);
            die_ = true;
         }

         interfaceCondition_.notify_one();

         thread_.join();

         BTG_MNOTICE(logWrapper(), "stopped");
      }

      limitManager::~limitManager()
      {
         if (!die_)
            {
               stop();
            }
         BTG_MNOTICE(logWrapper(), "destroyed");
      }

   } // namespace daemon
} // namespace btg



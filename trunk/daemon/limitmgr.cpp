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

#include <libtorrent/allocate_resources.hpp>
#include <libtorrent/session.hpp>

#include <boost/bind.hpp>

#include <bcore/os/sleep.h>
#include <bcore/command/limit_base.h>

#include <bcore/verbose.h>

#include "lt_version.h"

#include "modulelog.h"

namespace btg
{
   namespace daemon
   {

      using namespace btg::core;

      const std::string moduleName("lim");

      sessionData::sessionData()
         : session_(0),
           upload_bytes_(0),
           upload_bytes_last_(0),
           download_bytes_(0),
           download_bytes_last_(0),
           connections_(0),
           connections_last_(0),
           uploads_(0),
           uploads_last_(0),
           interval_(0)
      {
      }

      sessionData::sessionData(libtorrent::session* _session)
         : session_(_session),
           upload_bytes_(0),
           upload_bytes_last_(0),
           download_bytes_(0),
           download_bytes_last_(0),
           connections_(0),
           connections_last_(0),
           uploads_(0),
           uploads_last_(0),
           interval_(0)
      {

      }

      sessionData::sessionData(sessionData const& _sd)
         : session_(_sd.session_),
           upload_bytes_(_sd.upload_bytes_),
           upload_bytes_last_(_sd.upload_bytes_last_),
           download_bytes_(_sd.download_bytes_),
           download_bytes_last_(_sd.download_bytes_last_),
           connections_(_sd.connections_),
           connections_last_(_sd.connections_last_),
           uploads_(_sd.uploads_),
           uploads_last_(_sd.uploads_last_),
           interval_(_sd.interval_)
      {
      }

      bool sessionData::operator== (sessionData const& _sd) const
      {
         if ((_sd.session_             != session_)             ||
             (_sd.upload_bytes_        != upload_bytes_)        ||
             (_sd.upload_bytes_last_   != upload_bytes_last_)   ||
             (_sd.download_bytes_      != download_bytes_)      ||
             (_sd.download_bytes_last_ != download_bytes_last_) ||
             (_sd.connections_         != connections_)         || 
             (_sd.connections_last_    != connections_last_)    || 
             (_sd.uploads_             != uploads_)             ||
             (_sd.uploads_last_        != uploads_last_)        ||
             (_sd.interval_            != interval_))
            {
               return false;
            }

         return true;
      }

      bool sessionData::operator== (const libtorrent::session* _session) const
      {
         if (_session != session_)
            {
               return false;
            }

         return true;
      }

      bool sessionData::operator!= (sessionData const& _sd) const
      {
         bool status = (_sd == *this);

         return !status;
      }

      sessionData& sessionData::operator= (sessionData const& _sd)
      {
         if (_sd != *this)
            {
               session_             = _sd.session_;
               upload_bytes_        = _sd.upload_bytes_;
               upload_bytes_last_   = _sd.upload_bytes_last_;
               download_bytes_      = _sd.download_bytes_;
               download_bytes_last_ = _sd.download_bytes_last_;
               connections_         = _sd.connections_;
               connections_last_    = _sd.connections_last_;
               uploads_             = _sd.uploads_;
               uploads_last_        = _sd.uploads_last_;
               interval_            = _sd.interval_;
            }
         return *this;
      }

      void sessionData::update(t_int const _upload_rate_limit,
                               t_int const _download_rate_limit,
                               t_int const _max_uploads,
                               t_long const _max_connections,
                               t_int const _interval)
      {
         interval_ = _interval;

         const std::vector<libtorrent::torrent_handle> torrents = session_->get_torrents();

         std::vector<libtorrent::torrent_handle>::const_iterator iter;

         t_uint upl = 0;
         t_uint dnl = 0;

         for (iter = torrents.begin();
              iter != torrents.end();
              iter++)
            {
               const libtorrent::torrent_handle handle = *iter;
               const libtorrent::torrent_status status = handle.status();

               upl += status.total_upload;
               dnl += status.total_download;
            }

         download_bytes_      = dnl - download_bytes_last_;
         download_bytes_last_ = dnl;

         upload_bytes_        = upl - upload_bytes_last_;
         upload_bytes_last_   = upl;

         BTG_MNOTICE("setting counters, dnl " << download_bytes_ << ", upl " << upload_bytes_);

         session_->m_ul_bandwidth_quota.min  = 1;
         session_->m_ul_bandwidth_quota.used = upload_bytes_;
         if (_upload_rate_limit == limitBase::LIMIT_DISABLED)
            {
               session_->m_ul_bandwidth_quota.max  = libtorrent::resource_request::inf;
            }
         else
            {
               session_->m_ul_bandwidth_quota.max  = _upload_rate_limit * interval_;
            }

         session_->m_dl_bandwidth_quota.min  = 1;
         session_->m_dl_bandwidth_quota.used = download_bytes_;
         if (_download_rate_limit == limitBase::LIMIT_DISABLED)
            {
               session_->m_dl_bandwidth_quota.max  = libtorrent::resource_request::inf;
            }
         else
            {
               session_->m_dl_bandwidth_quota.max  = _download_rate_limit * interval_;
            }

         // Number of connections.
         t_int connections   = session_->num_uploads();

         connections_        = connections - connections_last_;
         connections_last_   = connections;

         session_->m_connections_quota.min  = 1;
         session_->m_connections_quota.used = connections_;
         if (_max_connections == limitBase::LIMIT_DISABLED)
            {
               session_->m_connections_quota.max  = libtorrent::resource_request::inf;
            }
         else
            {
               session_->m_connections_quota.max  = _max_connections * interval_;
            }

         // Number of uploads.
         t_int uploads       = session_->num_connections();

         uploads_      = uploads - uploads_last_;
         uploads_last_ = uploads;

         session_->m_uploads_quota.min  = 1;
         session_->m_uploads_quota.used = uploads_;
         if (_max_uploads == limitBase::LIMIT_DISABLED)
            {
               session_->m_uploads_quota.max  = libtorrent::resource_request::inf;
            }
         else
            {
               session_->m_uploads_quota.max  = _max_uploads * interval_;
            }
      }

      void sessionData::set()
      {
         t_int ul = session_->m_ul_bandwidth_quota.given / interval_;
         t_int dl = session_->m_dl_bandwidth_quota.given / interval_;

         t_int connections = session_->m_uploads_quota.given / interval_;
         t_int uploads     = session_->m_connections_quota.given / interval_;

         if (ul == 0)
            {
               ul++;
            }

         if (dl == 0)
            {
               dl++;
            }

         if (connections == 0)
            {
               connections++;
            }

         if (uploads == 0)
            {
               uploads++;
            }

         BTG_MNOTICE("Setting limit:");
         BTG_MNOTICE("ul = " << ul << " bytes/sec, dl = " << dl << " bytes/sec.");
         BTG_MNOTICE("connections = " << connections << ", uploads = " << uploads << ".");

         session_->set_upload_rate_limit(ul);
         session_->set_download_rate_limit(dl);

         session_->set_max_uploads(uploads);
         session_->set_max_connections(connections);
      }

      libtorrent::session* sessionData::session() const
      {
         return session_;
      }

      sessionData::~sessionData()
      {

      }

      /* */
      /* */
      /* */

      limitManager::limitManager(bool const _verboseFlag,
                                 t_uint const _interval,
                                 t_int const _upload_rate_limit,
                                 t_int const _download_rate_limit,
                                 t_int const _max_uploads,
                                 t_long const _max_connections)
         : verboseFlag_(_verboseFlag),
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
         BTG_MNOTICE("constructed");
         BTG_MNOTICE("limits: " << upload_rate_limit_ << ", " <<
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
         BTG_MNOTICE("thread started");

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
         VERBOSE_LOG(verboseFlag_, moduleName << ":"
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
         param_session->m_ul_bandwidth_quota.used = 0;
         param_session->m_ul_bandwidth_quota.min  = 1;
         param_session->m_ul_bandwidth_quota.max  = 0;

         param_session->m_dl_bandwidth_quota.used = 0;
         param_session->m_dl_bandwidth_quota.min  = 1;
         param_session->m_dl_bandwidth_quota.max  = 0;

         param_session->m_uploads_quota.used      = 0;
         param_session->m_uploads_quota.min       = 1;
         param_session->m_uploads_quota.max       = 0;

         param_session->m_connections_quota.used  = 0;
         param_session->m_connections_quota.min   = 1;
         param_session->m_connections_quota.max   = 0;

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

         std::vector<libtorrent::session*> sessions;
         getSessions(sessions);

         // Tick interval in seconds.
         t_int tick_interval = interval_;

         t_int resources     = -1;

         // Upload rate.
         if (upload_rate_limit_ == limitBase::LIMIT_DISABLED)
            {
               // resources = std::numeric_limits<int>::max();
               resources = libtorrent::resource_request::inf;
            }
         else
            {
               resources = upload_rate_limit_ * tick_interval;
            }

         BTG_MNOTICE("distributing resources (ul): " << resources);

         libtorrent::allocate_resources(resources,
                                        sessions,
                                        &libtorrent::session::m_ul_bandwidth_quota);

         // Download rate.
         if (download_rate_limit_ == limitBase::LIMIT_DISABLED)
            {
               // resources = std::numeric_limits<int>::max();
               resources = libtorrent::resource_request::inf;
            }
         else
            {
               resources = download_rate_limit_ * tick_interval;
            }

         BTG_MNOTICE("distributing resources (dl): " << resources);

         libtorrent::allocate_resources(resources,
                                        sessions,
                                        &libtorrent::session::m_dl_bandwidth_quota);

         // Max uploads.
         if (max_uploads_ == limitBase::LIMIT_DISABLED)
            {
               resources = libtorrent::resource_request::inf;
            }
         else
            {
               resources = max_uploads_ * tick_interval;
            }

         BTG_MNOTICE("distributing resources (max ul): " << resources);
         
         libtorrent::allocate_resources(resources,
                                        sessions,
                                        &libtorrent::session::m_uploads_quota);

         // Max connections.

         if (max_connections_ == limitBase::LIMIT_DISABLED)
            {
               resources = libtorrent::resource_request::inf;
            }
         else
            {
               resources = max_connections_ * tick_interval;
            }

         BTG_MNOTICE("distributing resources (max conn): " << resources);

         libtorrent::allocate_resources(resources,
                                        sessions,
                                        &libtorrent::session::m_connections_quota);

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

               // BTG_MNOTICE("rates: " << upload_rate_limit_ << ", " << download_rate_limit_ << ", " << max_uploads_ << ", " << max_connections_);

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

         BTG_MNOTICE("stopped");
      }

      limitManager::~limitManager()
      {
         if (!die_)
            {
               stop();
            }
         BTG_MNOTICE("destroyed");
      }

   } // namespace daemon
} // namespace btg



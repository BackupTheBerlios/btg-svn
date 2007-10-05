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

#include "limitval.h"

#include <libtorrent/session.hpp>

#include <bcore/command/limit_base.h>

#include "modulelog.h"

namespace btg
{
   namespace daemon
   {

      using namespace btg::core;

      const std::string moduleName("lim");

      limitValue::limitValue()
         : min(0),
           max(0),
           used(0),
           given(0),
           leftovers(0)
      {
      }

      t_int limitValue::left() const
		{
			return (std::max)(given - used, 0);
		}
		
		void limitValue::reset()
      { 
         used = leftovers; 
         leftovers = 0; 
      }

      bool limitValue::operator== (limitValue const& _lv) const
      {
         bool eq = true;

         if (
             (min   != _lv.min)  || 
             (max   != _lv.max)  || 
             (used  != _lv.used) || 
             (given != _lv.given)
             )
            {
               eq = false;
            }

         return eq;
      }

      bool limitValue::operator!= (limitValue const& _lv) const
      {
         return !(*this == _lv);
      }

      limitIf::limitIf()
         : val_dl_rate(),
           val_ul_rate(),
           val_connections(),
           val_uploads()
      {
      }

      limitIf::limitIf(limitIf const& _li)
      : val_dl_rate(_li.val_dl_rate),
        val_ul_rate(_li.val_ul_rate),
        val_connections(_li.val_connections),
        val_uploads(_li.val_uploads)
      {

      }

      bool limitIf::operator== (limitIf const& _li) const
      {
         bool eq = true;

         if (
             (val_dl_rate     != _li.val_dl_rate) ||
             (val_ul_rate     != _li.val_ul_rate) ||
             (val_connections != _li.val_connections) ||
             (val_uploads     != _li.val_uploads)
             )
            {
               eq = false;
            }

         return eq;
      }

      bool limitIf::operator!= (limitIf const& _li) const
      {
         return !(*this == _li);
      }

      limitIf& limitIf::operator= (limitIf const& _li)
      {
         bool eq = (*this == _li);

         if (!eq)
            {
               val_dl_rate = _li.val_dl_rate;
               val_ul_rate   = _li.val_ul_rate;
               val_connections   = _li.val_connections;
               val_uploads       = _li.val_uploads;
            }
         return *this;
      }

      limitIf::~limitIf()
      {

      }

      const t_uint sessionData::inf = -1;

      sessionData::sessionData()
         : limitIf(),
           session_(0),
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
         : limitIf(),
           session_(_session),
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
         : limitIf(_sd),
           session_(_sd.session_),
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
         if (
             (!limitIf::operator==(_sd))                        ||
             (_sd.session_             != session_)             ||
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
               limitIf::operator=(_sd);
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

         val_ul_rate.min  = 1;
         val_ul_rate.used = upload_bytes_;
         // session_->m_ul_bandwidth_quota.min  = 1;
         // session_->m_ul_bandwidth_quota.used = upload_bytes_;
         if (_upload_rate_limit == limitBase::LIMIT_DISABLED)
            {
               // session_->m_ul_bandwidth_quota.max  = libtorrent::resource_request::inf;
               val_ul_rate.max = sessionData::inf;
            }
         else
            {
               // session_->m_ul_bandwidth_quota.max  = _upload_rate_limit * interval_;
               val_ul_rate.max = _upload_rate_limit * interval_;
            }

         //session_->m_dl_bandwidth_quota.min  = 1;
         //session_->m_dl_bandwidth_quota.used = download_bytes_;
         val_dl_rate.min  = 1;
         val_dl_rate.used = download_bytes_;

         if (_download_rate_limit == limitBase::LIMIT_DISABLED)
            {
               // session_->m_dl_bandwidth_quota.max  = libtorrent::resource_request::inf;
               val_dl_rate.max = sessionData::inf;
            }
         else
            {
               // session_->m_dl_bandwidth_quota.max  = _download_rate_limit * interval_;
               val_dl_rate.max = _download_rate_limit * interval_;
            }

         // Number of connections.
         t_int connections   = session_->num_uploads();

         connections_        = connections - connections_last_;
         connections_last_   = connections;

         //session_->m_connections_quota.min  = 1;
         //session_->m_connections_quota.used = connections_;
         val_connections.min  = 1;
         val_connections.used = connections_;

         if (_max_connections == limitBase::LIMIT_DISABLED)
            {
               //session_->m_connections_quota.max  = libtorrent::resource_request::inf;
               val_connections.max = sessionData::inf;
            }
         else
            {
               // session_->m_connections_quota.max  = _max_connections * interval_;
               val_connections.max = _max_connections * interval_;
            }

         // Number of uploads.
         t_int uploads = session_->num_connections();

         uploads_      = uploads - uploads_last_;
         uploads_last_ = uploads;

         // session_->m_uploads_quota.min  = 1;
         // session_->m_uploads_quota.used = uploads_;

         val_uploads.min  = 1;
         val_uploads.used = uploads_;

         if (_max_uploads == limitBase::LIMIT_DISABLED)
            {
               // session_->m_uploads_quota.max  = libtorrent::resource_request::inf;
               val_uploads.max = sessionData::inf;
            }
         else
            {
               // session_->m_uploads_quota.max  = _max_uploads * interval_;
               val_uploads.max = _max_uploads * interval_;
            }
      }

      void sessionData::set()
      {
         /*
         t_int ul = session_->m_ul_bandwidth_quota.given / interval_;
         t_int dl = session_->m_dl_bandwidth_quota.given / interval_;

         t_int connections = session_->m_uploads_quota.given / interval_;
         t_int uploads     = session_->m_connections_quota.given / interval_;
         */
         t_int ul = val_ul_rate.given / interval_;
         t_int dl = val_dl_rate.given / interval_;

         t_int connections = val_connections.given / interval_;
         t_int uploads     = val_uploads.given     / interval_;

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

   } // namespace daemon
} // namespace btg


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

#ifndef LIMITVAL_H
#define LIMITVAL_H

#include <bcore/type.h>
#include <boost/integer_traits.hpp>

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

         struct limitValue
         {
            limitValue();

            t_int left() const;
		
            void reset();

            /// The equality operator.
            bool operator== (limitValue const& _lv) const;

            /// The not equal operator.
            bool operator!= (limitValue const& _lv) const;

            t_int min;
            t_int max;
            t_int used;
            t_int given;
            t_int leftovers;

            static const int inf = boost::integer_traits<int>::const_max;
         };

         class limitIf
         {
         public:
            /// Default constructor.
            limitIf();

            /// Copy constructor.
            limitIf(limitIf const& _li);

            /// The equality operator.
            bool operator== (limitIf const& _li) const;

            /// The not equal operator.
            bool operator!= (limitIf const& _li) const;

            /// The assignment operator.
            limitIf& operator= (limitIf const& _li);

            /// Destructor.
            ~limitIf();
         public:
            limitValue val_dl_rate;
            limitValue val_ul_rate;
            limitValue val_connections;
            limitValue val_uploads;
         };

         /// Session and data used for setting limits on a libtorrent
         /// session.
         class sessionData: public limitIf
            {
            public:
               /// Default Constructor.
               sessionData();

               /// Constructor.
               sessionData(libtorrent::session* _session);

               /// Copy constructor.
               sessionData(sessionData const& _sd);

               /// The equality operator.
               bool operator== (sessionData const& _sd) const;

               /// The equality operator.
               ///
               /// Used for comparing to a session, which is also
               /// contained in this class.
               bool operator== (const libtorrent::session* _session) const;

               /// The not equal operator.
               bool operator!= (sessionData const& _sd) const;

               /// The assignment operator.
               sessionData& operator= (sessionData const& _sd);

               /// Update the counters.
               ///
               /// Update the session's resource_request members using
               /// this information.
               void update(t_int const _upload_rate_limit,
                           t_int const _download_rate_limit,
                           t_int const _max_uploads,
                           t_long const _max_connections,
                           t_int const _interval);

               /// Set the limits, which are stored in the contained
               /// session.
               void set();

               /// Get the contained session.
               libtorrent::session* session() const;

               static const t_uint inf;

               /// Destructor.
               ~sessionData();
            private:
               /// Pointer to libtorrent session.
               libtorrent::session*     session_;

               /// The number of bytes uploaded. This is a difference
               /// since last time update was called.
               t_uint                   upload_bytes_;
               /// The number of bytes uploaded, when update was
               /// last called.
               t_uint                   upload_bytes_last_;

               /// The number of bytes downloaded. This is a difference
               /// since last time update was called.
               t_uint                   download_bytes_;
               /// The number of bytes downloaded, when update was
               /// last called.
               t_uint                   download_bytes_last_;

               /// The number of connections used. This is a
               /// difference since last time update was called.
               t_uint                   connections_;

               /// The number of connections used, when update was
               /// last called.
               t_uint                   connections_last_;

               /// The number of uploads used. This is a difference
               /// since last time update was called.
               t_uint                   uploads_;

               /// The number of uploads used, when update was last
               /// called.
               t_uint                   uploads_last_;

               /// The interval in seconds. This is the time between
               /// calls to the update function.
               t_int                    interval_;
            };

      } // namespace daemon
} // namespace btg

#endif


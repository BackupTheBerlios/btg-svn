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

#ifndef CLIENT_PROGRESS_H
#define CLIENT_PROGRESS_H

#include <string>

#include <bcore/type.h>

namespace btg
{
   namespace daemon
      {
         /**
          * \addtogroup daemon Daemon
          */
         /** @{ */

         class progressData;
         class timeSpecification;
         /// Monitors contexts and _guesses_ on when they are
         /// going to finish.

         class Progress
            {
            public:
               /// Constructor.
               Progress();

               /// Add a context.
               void add(t_int const _torrent_id, t_ullong const _filesizeInBytes);

               /// Check if a torrent id is being tracked by an
               /// instance of this class.
               bool exists(t_int const _torrent_id);

               /// Update a context.
               void update(t_int const _torrent_id, t_ulong const _rateInBytes);

               /// Remove a context.
               void remove(t_int const _torrent_id);

               /// Get a time for when a context is finished.
               timeSpecification getTime(t_int const _torrent_id, t_ulong const _percent) const;

               /// Destructor.
               ~Progress();
            private:
               /// Maps a context id to rate and filesize;
               std::map<t_int, progressData> progressMap;
            };

         /// Class containing the progress data.
         class progressData
            {
            public:
               /// The file size.
               t_ullong filesize;
               /// Download rate.
               t_ulong  rate;
               /// Counter.
               t_uint   updateCounter;
            };

         /// Represents the time left of a download.
         class timeSpecification
            {
            public:
               /// Constructor.
               timeSpecification();

               /// Set the hour, minute and second fields using
               /// a time interval in seconds.
               void set(t_ulong const _seconds);

               /// Mark this time spec as being undefined.
               void setUndefined(bool const _value = true);

               /// Returns true if this time spec is undefined.
               bool isUndefined() const;

               /// Convert a time interval to a h:m:s, where all
               /// numbers are prefixed with zeros, if needed.
               std::string toString();

               /// Hours.
               t_int day() const { return day_; };
               /// Hours.
               t_ulong hour() const { return hour_; };
               /// Minutes.
               t_uint minute() const { return minute_; };
               /// Seconds.
               t_uint second() const { return second_; };

               /// Destructor.
               ~timeSpecification();
            private:
               /// Days.
               t_int day_;
               /// Hours.
               t_ulong hour_;
               /// Minutes.
               t_uint minute_;
               /// Seconds.
               t_uint second_;

               /// flag, indicates that a download never is
               /// going to complete, since the dl rate is 0.
               bool undefined;
            };

      } // namespace daemon
} // namespace btg

#endif

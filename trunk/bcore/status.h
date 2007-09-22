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

#ifndef STATUS_H
#define STATUS_H

#include <bcore/serializable.h>
#include <bcore/printable.h>
#include <bcore/trackerstatus.h>

#include <string>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup core
          */
         /** @{ */

         /// Used for transfering status information about torrent-contexts.
         class Status: public Serializable, public Printable
            {
            public:

               /// Represents the status of a torrent.
               enum torrent_status
                  {
                     ts_undefined = 0, //!< Not defined.
                     ts_queued    = 1, //!< In que for downloading.
                     ts_checking,      //!< The client is hash-checking the data.
                     ts_connecting,    //!< The client is connecting.
                     ts_downloading,   //!< The client is downloading.
                     ts_seeding,       //!< Download is complete, upload only.
                     ts_stopped,       //!< The client stopped downloading.
                     ts_finished       //!< The client finished downloading and is not seeding.
                  };

               /// Default constructor.
               Status();

               /// Constructor.
               /// @param [in] _contextID    The context ID.
               /// @param [in] _filename     The filename.
               /// @param [in] _status       The status of the torrent.
               /// @param [in] _dn_total     Total bytes downloaded.
               /// @param [in] _ul_total     Total bytes uploaded.
               /// @param [in] _failed_bytes Total failed bytes.
               /// @param [in] _dn_rate      Download rate in bytes per second.
               /// @param [in] _ul_rate      Upload rate in bytes per second.
               /// @param [in] _done         Percent done [0-100%].
               /// @param [in] _filesize     Filesize in bytes.
               /// @param [in] _leechers     Number of leeches.
               /// @param [in] _seeders      Number of seeders.
               /// @param [in] _time_left_d  Estimated days left.
               /// @param [in] _time_left_h  Estimated hours left.
               /// @param [in] _time_left_m  Estimated minutes left.
               /// @param [in] _time_left_s  Estimated seconds left.
               /// @param [in] _trackerStatus Tracker status object.
               /// @param [in] _activityCounter Activity counter.
               Status(t_int const _contextID,
                      std::string const& _filename,
                      torrent_status const _status,
                      t_ulong const _dn_total,
                      t_ulong const _ul_total,
                      t_ulong const _failed_bytes,
                      t_ulong const _dn_rate,
                      t_ulong const _ul_rate,
                      t_int const _done,
                      t_ulong const _filesize,
                      t_int const _leechers,
                      t_int const _seeders,
                      t_int const _time_left_d, 
                      t_ulong const _time_left_h, 
                      t_int const _time_left_m, 
                      t_int const _time_left_s,
                      btg::core::trackerStatus _trackerStatus,
                      t_ulong const _activityCounter);

               /// Copy constructor.
               /// @param [in] _status       Reference to the object to copy.
               Status(Status const& _status);

               /// Set all the contained values.
               /// @param [in] _contextID    The context ID.
               /// @param [in] _filename     The filename.
               /// @param [in] _status       The status of the torrent.
               /// @param [in] _dn_total     Total bytes downloaded.
               /// @param [in] _ul_total     Total bytes uploaded.
               /// @param [in] _failed_bytes Total failed bytes.
               /// @param [in] _dn_rate      Download rate in bytes per second.
               /// @param [in] _ul_rate      Upload rate in bytes per second.
               /// @param [in] _done         Percent done [0-100%].
               /// @param [in] _filesize     Filesize in bytes.
               /// @param [in] _leechers     Number of leeches.
               /// @param [in] _seeders      Number of seeders.
               /// @param [in] _time_left_d  Estimated days left.
               /// @param [in] _time_left_h  Estimated hours left.
               /// @param [in] _time_left_m  Estimated minutes left.
               /// @param [in] _time_left_s  Estimated seconds left.
               /// @param [in] _trackerStatus Tracker status object.
               /// @param [in] _activityCounter Activity counter.
               void set(t_int const _contextID,
                        std::string const& _filename,
                        torrent_status const _status,
                        t_ulong const _dn_total,
                        t_ulong const _ul_total,
                        t_ulong const _failed_bytes,
                        t_ulong const _dn_rate,
                        t_ulong const _ul_rate,
                        t_int const _done,
                        t_ulong const _filesize,
                        t_int const _leechers,
                        t_int const _seeders,
                        t_int const _time_left_d, 
                        t_ulong const _time_left_h, 
                        t_int const _time_left_m, 
                        t_int const _time_left_s,
                        btg::core::trackerStatus _trackerStatus,
                        t_ulong const _activityCounter);

               /// The equality operator.
               bool operator== (Status const& _status) const;

               /// The assignment operator.
               Status& operator= (Status const& _status);

               /// Get the context ID.
               t_int contextID()         const { return contextID_; }

               /// Get the filename;
               std::string filename() const { return filename_; }

               /// Get the status.
               t_int status()            const { return status_; }

               /// Return total bytes downloaded
               t_ulong downloadTotal()   const { return downloadTotal_; }

               /// Return total bytes uploaded
               t_ulong uploadTotal()     const { return uploadTotal_; }

               /// Return total failed bytes
               t_ulong failedBytes()     const { return failedBytes_; }

               /// Return download rate in bytes per second.
               t_ulong downloadRate()    const { return downloadRate_; }

               /// Return upload rate in bytes per second
               t_ulong uploadRate()      const { return uploadRate_; }

               /// Return percent done [0-100%].
               t_int done()              const { return done_; }

               /// Return the total filesize in bytes.
               t_ulong filesize()        const { return filesize_; }

               /// Return the number of seeders.
               t_int seeders()           const { return seeders_; };

               /// Return the number of leechers.
               t_int leechers()          const { return leechers_; };

               std::string toString()    const;

               /// Get the time left for the download to complete,
               /// days.
               t_int time_left_d()       const { return time_left_d_; };

               /// Get the time left for the download to complete,
               /// hours.
               t_ulong time_left_h()     const { return time_left_h_; };

               /// Get the time left for the download to complete,
               /// minutes.
               t_int time_left_m()       const { return time_left_m_; };

               /// Get the time left for the download to complete,
               /// seconds.
               t_int time_left_s()       const { return time_left_s_; };

               /// Get the time left for the download to complete,
               /// seconds.
               btg::core::trackerStatus trackerStatus() const { return trackerStatus_;  }

               /// Get the activity counter.
               t_ulong activityCounter() const { return activityCounter_; };

               /// Used to convert this object to a byte array. This does not
               /// create a valid representation, since the session information
               /// and command type is not included, only the data from this
               /// object.
               bool serialize(btg::core::externalization::Externalization* _e) const;

               /// Setup this object with data contained in the byte array.
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Indicate if the contained time left is valid.
               /// @return True - valid, false - invalid (zeros).
               bool validTimeLeft() const;

               /// Convert the contained time left (days, hour,
               /// minute, second) to a string.
               /// @param [out] _output     Reference to string used for output.
               /// @param [in] _shortFormat True - short format, false - wide format.
               void timeLeftToString(std::string & _output, 
                                     bool const _shortFormat = false) const;

               /// Destructor.
               ~Status();
            private:
               /// The context ID.
               t_int             contextID_;

               /// The filename.
               std::string       filename_;

               /// The status.
               torrent_status    status_;

               /// Total downloaded bytes.
               t_ulong           downloadTotal_;

               /// Total uploaded bytes.
               t_ulong           uploadTotal_;

               /// Total failed bytes.
               t_ulong           failedBytes_;

               /// Download rate.
               t_ulong           downloadRate_;

               /// Upload rate.
               t_ulong           uploadRate_;

               /// Done in percent.
               t_int             done_;

               /// Total filesize in bytes.
               t_ulong           filesize_;

               /// Number of leeches.
               t_int             leechers_;

               /// Number of seeders.
               t_int             seeders_;

               /// Time left for the download to complete, hours.
               t_int             time_left_d_;

               /// Time left for the download to complete, hours.
               t_ulong           time_left_h_;

               /// Time left for the download to complete, minutes.
               t_int             time_left_m_;

               /// Time left for the download to complete, seconds.
               t_int             time_left_s_;

               /// Tracker status
               btg::core::trackerStatus trackerStatus_;

               /// Either:
               /// Time in minutes this torrent has been downloading.
               /// or 
               /// Time in minutes this torrent has been seeding.
               /// 
               /// The meaning depends on the status_ member.
               /// ts_downloading - downloading time.
               /// ts_seeding     - seeding time.
               t_ulong                  activityCounter_;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

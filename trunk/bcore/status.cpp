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

#include "status.h"
#include <bcore/t_string.h>
#include <bcore/hru.h>
#include <bcore/hrr.h>

#include <bcore/helpermacro.h>

namespace btg
{
   namespace core
   {

      using namespace std;

      Status::Status()
         : Serializable(),
           Printable(),
           contextID_(-1),
           filename_("undefined"),
           status_(ts_undefined),
           downloadTotal_(0),
           uploadTotal_(0),
           failedBytes_(0),
           downloadRate_(0),
           uploadRate_(0),
           done_(0),
           filesize_(0),
           leechers_(0),
           seeders_(0),
           time_left_d_(0),
           time_left_h_(0),
           time_left_m_(0),
           time_left_s_(0),
           trackerStatus_(),
	   activityCounter_(0)
      {}

      Status::Status(t_int const _contextID,
                     string const& _filename,
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
                     t_int _time_left_s,
                     btg::core::trackerStatus _trackerStatus,
		     t_ulong const _activityCounter)
         : Serializable(),
           Printable(),
           contextID_(_contextID),
           filename_(_filename),
           status_(_status),
           downloadTotal_(_dn_total),
           uploadTotal_(_ul_total),
           failedBytes_(_failed_bytes),
           downloadRate_(_dn_rate),
           uploadRate_(_ul_rate),
           done_(_done),
           filesize_(_filesize),
           leechers_(_leechers),
           seeders_(_seeders),
           time_left_d_(_time_left_d),
           time_left_h_(_time_left_h),
           time_left_m_(_time_left_m),
           time_left_s_(_time_left_s),
           trackerStatus_(_trackerStatus),
	   activityCounter_(_activityCounter)
      {}

      Status::Status(Status const& _status)
         : Serializable(),
           Printable(),
           contextID_(_status.contextID_),
           filename_(_status.filename_),
           status_(_status.status_),
           downloadTotal_(_status.downloadTotal_),
           uploadTotal_(_status.uploadTotal_),
           failedBytes_(_status.failedBytes_),
           downloadRate_(_status.downloadRate_),
           uploadRate_(_status.uploadRate_),
           done_(_status.done_),
           filesize_(_status.filesize_),
           leechers_(_status.leechers_),
           seeders_(_status.seeders_),
           time_left_d_(_status.time_left_d_),
           time_left_h_(_status.time_left_h_),
           time_left_m_(_status.time_left_m_),
           time_left_s_(_status.time_left_s_),
           trackerStatus_(_status.trackerStatus_),
	   activityCounter_(_status.activityCounter_)
      {}

      void Status::set(t_int const _contextID,
                       string const& _filename,
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
                       t_int _time_left_s,
                       btg::core::trackerStatus _trackerStatus,
		       t_ulong const _activityCounter)
      {
         contextID_         = _contextID;
         filename_          = _filename;
         status_            = _status;
         downloadTotal_     = _dn_total;
         uploadTotal_       = _ul_total;
         failedBytes_       = _failed_bytes;
         downloadRate_      = _dn_rate;
         uploadRate_        = _ul_rate;
         done_              = _done;
         filesize_          = _filesize;
         leechers_          = _leechers;
         seeders_           = _seeders;
         time_left_d_       = _time_left_d;
         time_left_h_       = _time_left_h;
         time_left_m_       = _time_left_m;
         time_left_s_       = _time_left_s;
         trackerStatus_     = _trackerStatus;
	 activityCounter_   = _activityCounter;
      }

      bool Status::serialize(btg::core::externalization::Externalization* _e) const
      {
         // int, context id;
         t_int temp_contextid  = this->contextID_;
         BTG_RCHECK( _e->intToBytes(&temp_contextid) );

         // string, filename;
         BTG_RCHECK( _e->stringToBytes(&this->filename_) );

         // int, status;
         t_int temp_status  = this->status_;
         BTG_RCHECK( _e->intToBytes(&temp_status) );

         //long, dn_total;
         BTG_RCHECK( _e->uLongToBytes(&this->downloadTotal_) );

         //long, ul_total;
         BTG_RCHECK( _e->uLongToBytes(&this->uploadTotal_) );

         //long, failed_bytes;
         BTG_RCHECK( _e->uLongToBytes(&this->failedBytes_) );

         //long, dn_rate;
         BTG_RCHECK( _e->uLongToBytes(&this->downloadRate_) );

         //long, ul_rate;
         BTG_RCHECK( _e->uLongToBytes(&this->uploadRate_) );

         //int, done;
         BTG_RCHECK( _e->intToBytes(&this->done_) );

         //long, filesize;
         BTG_RCHECK( _e->uLongToBytes(&this->filesize_) );

         //int, leeches;
         BTG_RCHECK( _e->intToBytes(&this->leechers_) );

         //int, seeders;
         BTG_RCHECK( _e->intToBytes(&this->seeders_) );

         //long, time left, days;
         BTG_RCHECK( _e->intToBytes(&this->time_left_d_) );

         //long, time left, hours;
         BTG_RCHECK( _e->uLongToBytes(&this->time_left_h_) );

         //int, time left, minutes;
         BTG_RCHECK( _e->intToBytes(&this->time_left_m_) );

         //int, time left, seconds;
         BTG_RCHECK( _e->intToBytes(&this->time_left_s_) );

         //tracker status;
         BTG_RCHECK( trackerStatus_.serialize(_e) );

	 // unsigned long, counter, minutes;
         BTG_RCHECK( _e->uLongToBytes(&this->activityCounter_) );

         return true;
      }

      bool Status::deserialize(btg::core::externalization::Externalization* _e)
      {
         // int, context id;
         t_int temp_contextid  = 0;
         BTG_RCHECK( _e->bytesToInt(&temp_contextid) );

         this->contextID_ = temp_contextid;

         // string, filename;
         BTG_RCHECK( _e->bytesToString(&this->filename_) );

         // int, status;
         t_int temp_status  = 0;
         BTG_RCHECK( _e->bytesToInt(&temp_status) );

         // Check that the input integer is a status.
         switch(temp_status)
            {
            case Status::ts_undefined:
            case Status::ts_queued:
            case Status::ts_checking:
            case Status::ts_connecting:
            case Status::ts_downloading:
            case Status::ts_seeding:
            case Status::ts_stopped:
            case Status::ts_finished:
               break;
            default:
               BTG_RCHECK(false);
            }

         this->status_ = static_cast<torrent_status>(temp_status);

         //long, dn_total;
         BTG_RCHECK( _e->bytesToULong(&this->downloadTotal_) );

         //long, ul_total;
         BTG_RCHECK( _e->bytesToULong(&this->uploadTotal_) );

         //long, failed_bytes;
         BTG_RCHECK( _e->bytesToULong(&this->failedBytes_) );

         //long, dn_rate;
         BTG_RCHECK( _e->bytesToULong(&this->downloadRate_) );

         //long, ul_rate;
         BTG_RCHECK( _e->bytesToULong(&this->uploadRate_) );

         //int, done;
         BTG_RCHECK( _e->bytesToInt(&this->done_) );

         //long, filesize;
         BTG_RCHECK( _e->bytesToULong(&this->filesize_) );

         //int, leeches;
         BTG_RCHECK( _e->bytesToInt(&this->leechers_) );

         //int, seeders;
         BTG_RCHECK( _e->bytesToInt(&this->seeders_) );

         //int, time left, days;
         BTG_RCHECK( _e->bytesToInt(&this->time_left_d_) );

         //long, time left, hours;
         BTG_RCHECK( _e->bytesToULong(&this->time_left_h_) );

         //int, time left, minutes;
         BTG_RCHECK( _e->bytesToInt(&this->time_left_m_) );

         //int, time left, seconds;
         BTG_RCHECK( _e->bytesToInt(&this->time_left_s_) );

         // Trackerstatus
         BTG_RCHECK( trackerStatus_.deserialize(_e));

	 // unsigned long, counter, minutes;
         BTG_RCHECK( _e->bytesToULong(&this->activityCounter_) );

         return true;
      }

      string Status::toString() const
      {
         string output("");

         output += "contex id: "    + convertToString<t_int>(contextID_) + ", ";
         output += "filename: '"    + filename_ + "', ";
         output += "status: "       + convertToString<t_int>(status_) + ", ";
         output += "trackerstatus: "+ trackerStatus_.toString() + ", ";
         output += "dn_total: "     + convertToString<t_ulong>(downloadTotal_) + ", ";
         output += "ul_total: "     + convertToString<t_ulong>(uploadTotal_) + ", ";
         output += "failed_bytes: " + convertToString<t_ulong>(failedBytes_) + ", ";
         output += "dn_rate: "      + convertToString<t_ulong>(downloadRate_) + ", ";
         output += "ul_rate: "      + convertToString<t_ulong>(uploadRate_) + ", ";
         output += "done: "         + convertToString<t_int>(done_) + ", ";
         output += "filesize: "     + convertToString<t_ulong>(filesize_) + ", ";
         output += "leechers: "      + convertToString<t_int>(leechers_) + ", ";
         output += "seeders: "      + convertToString<t_int>(seeders_) + ", ";

         output += "progress: "     + convertToString<t_int>(time_left_d_) + ":" + convertToString<t_ulong>(time_left_h_) + ":" +
            convertToString<t_int>(time_left_m_) + ":" +
            convertToString<t_int>(time_left_s_) + ", ";

	 output += "activity counter:" + convertToString<t_ulong>(activityCounter_);

         return output;
      }

      bool Status::operator== (Status const& _status) const
      {
         bool op_status = true;

         if ((this->contextID_ != _status.contextID_) ||
	     (this->filename_ != _status.filename_) || 
	     (this->status_ != _status.status_) || 
	     (this->downloadTotal_ != _status.downloadTotal_) || 
	     (this->uploadTotal_ != _status.uploadTotal_) || 
	     (this->failedBytes_ != _status.failedBytes_) || 
	     (this->downloadRate_ != _status.downloadRate_) || 
	     (this->uploadRate_ != _status.uploadRate_) ||
	     (this->done_ != _status.done_) ||
	     (this->filesize_ != _status.filesize_) ||
	     (this->leechers_ != _status.leechers_) ||
	     (this->seeders_ != _status.seeders_) ||
	     (this->time_left_d_ != _status.time_left_d_) ||
	     (this->time_left_h_ != _status.time_left_h_) ||
	     (this->time_left_m_ != _status.time_left_m_) ||
	     (this->time_left_s_ != _status.time_left_s_) ||
	     (this->trackerStatus_ != _status.trackerStatus_) ||
	     (activityCounter_ != _status.activityCounter_))
	   {
	     op_status = false;
	   }

         return op_status;
      }

      Status& Status::operator= (Status const& _status)
      {
         bool eq = (_status == *this);

         if (!eq)
            {
               contextID_      = _status.contextID_;
               filename_       = _status.filename_;
               status_         = _status.status_;
               downloadTotal_  = _status.downloadTotal_;
               uploadTotal_    = _status.uploadTotal_;
               failedBytes_    = _status.failedBytes_;
               downloadRate_   = _status.downloadRate_;
               uploadRate_     = _status.uploadRate_;
               done_           = _status.done_;
               filesize_       = _status.filesize_;
               leechers_       = _status.leechers_;
               seeders_        = _status.seeders_;
               time_left_d_    = _status.time_left_d_;
               time_left_h_    = _status.time_left_h_;
               time_left_m_    = _status.time_left_m_;
               time_left_s_    = _status.time_left_s_;
               trackerStatus_   = _status.trackerStatus_;
	       activityCounter_ = _status.activityCounter_;
            }

         return *this;
      }

     bool Status::validTimeLeft() const
     {
       bool status = false;

       if ((time_left_d_ != 0) || 
	   (time_left_h_ != 0) ||
	   (time_left_m_ != 0) || 
	   (time_left_s_ != 0))
	 {
	   status = true;
	 }

       return status;
     }

     void Status::timeLeftToString(std::string & _output, 
				   bool const _shortFormat) const
     {
       if (time_left_d_ > 0)
	 {
	   // Show only days.
	   _output += convertToString<t_ulong>(time_left_d_);

	   if (!_shortFormat)
	     {
	       if (time_left_d_ > 1)
		 {
		   _output += " days";
		 }
	       else
		 {
		   _output += " day";
		 }
	     }
	   else
	     {
	       _output += " d";
	     }
	 }
       else
	 {
	   // Show only h:m:s.

	   if (time_left_h_ < 10)
	     {
	       _output += "0";
	     }
	   _output += convertToString<t_ulong>(time_left_h_) + ":";

	   if (time_left_m_ < 10)
	     {
	       _output += "0";
	     }
	   _output += convertToString<t_int>(time_left_m_) + ":";
	   
	   if (time_left_s_ < 10)
	     {
	       _output += "0";
	     }
	   _output += convertToString<t_int>(time_left_s_);
	 }	  
     }

      Status::~Status()
      {}

   } // namespace core
} // namespace btg

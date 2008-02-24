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

#include <bcore/t_string.h>
#include "progress.h"

namespace btg
{
   namespace daemon
   {
      Progress::Progress()
         : progressMap()
      {
      }

      void Progress::add(t_int const _torrent_id, t_ullong const _filesizeInBytes)
      {
         std::map<t_int, progressData>::iterator iter;
         // std::map<int, t_ullong>::iterator iter;

         iter = progressMap.find(_torrent_id);

         if (iter == progressMap.end())
            {
               progressData pd;
               pd.filesize              = _filesizeInBytes;
               pd.rate                  = 0;
               pd.updateCounter         = 0;
               progressMap[_torrent_id] = pd;
            }
      }

      bool Progress::exists(t_int const _torrent_id)
      {
         bool status = false;

         std::map<t_int, progressData>::iterator iter = progressMap.find(_torrent_id);

         if (iter != progressMap.end())
            {
               status = true;
            }

         return status;
      }

      void Progress::update(t_int const _torrent_id, t_ulong const _rateInBytes)
      {
         std::map<t_int, progressData>::iterator iter = progressMap.find(_torrent_id);

         if (iter == progressMap.end())
            {
               return;
            }

         progressData & pd = progressMap[_torrent_id];

         t_ulong value     = (pd.rate + _rateInBytes) / 2;
         pd.rate           = value;
         pd.updateCounter += 1;
      }

      void Progress::remove(t_int const _torrent_id)
      {
         std::map<t_int, progressData>::iterator iter = progressMap.find(_torrent_id);

         if (iter != progressMap.end())
            {
               progressMap.erase(iter);
            }
      }

      timeSpecification Progress::getTime(t_int const _torrent_id, t_ulong const _percent) const
      {
         timeSpecification ts;

         std::map<t_int, progressData>::const_iterator iter = progressMap.find(_torrent_id);

         t_ullong size = iter->second.filesize;
         t_ulong  rate = iter->second.rate;

         if (rate <= 0)
            {
               // No download, mark as underfined.
               ts.setUndefined(true);
               return ts;
            }

         t_ulong value = (100 - _percent) * (size / rate) / 100;

         ts.set(value);
         // Mark as not underfined.
         ts.setUndefined(false);

         return ts;
      }

      Progress::~Progress()
      {

      }

      timeSpecification::timeSpecification()
         : day_(0),
           hour_(0),
           minute_(0),
           second_(0),
           undefined(true)
      {
      }

      void timeSpecification::set(t_ulong const _seconds)
      {
         const t_int secPerDay = 86400;
         const t_int secPerHour   = 3600;
         const t_int secPerMinute = 60;

         this->day_   = _seconds / secPerDay;
         t_ulong left  = _seconds - (this->day_ * secPerDay);

         if (this->day_ == 0)
            {
               left = _seconds;
            }

         this->hour_   = left / secPerHour;
         left  = left - (this->hour_ * secPerHour);

         if (this->day_ == 0 && this->hour_ == 0)
            {
               left = _seconds;
            }

         this->minute_ = left / secPerMinute;

         if ((this->day_ == 0) && (this->hour_ == 0) && (this->minute_ == 0))
            {
               left = _seconds;
            }
         else
            {
               left = left - (this->minute_ * secPerMinute);
            }

         this->second_ = left;
      }

      void timeSpecification::setUndefined(bool const _value)
      {
         this->undefined = _value;
      }

      bool timeSpecification::isUndefined() const
      {
         return this->undefined;
      }

      std::string timeSpecification::toString()
      {
         using namespace btg::core;

         std::string output;

         if (this->day_ < 10)
            {
               output += "0";
            }

         output += convertToString<t_int>(this->day_) + ":";

         if (this->hour_ < 10)
            {
               output += "0";
            }

         output += convertToString<t_ulong>(this->hour_) + ":";

         if (this->minute_ < 10)
            {
               output += "0";
            }

         output += convertToString<t_uint>(this->minute_) + ":";

         if (this->second_ < 10)
            {
               output += "0";
            }

         output += convertToString<t_uint>(this->second_);

         return output;
      }

      timeSpecification::~timeSpecification()
      {

      }

   } // namespace daemon
} // namespace btg

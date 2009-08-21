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

#include "filetrackdata.h"

#include <libtorrent/torrent_info.hpp>

#include <bcore/logmacro.h>

#include "modulelog.h"

namespace btg
{
   namespace daemon
   {

      const std::string moduleName("fid");

      fileTrackData::fileTrackData()
         : torrent_filename_(),
           entries_()
      {

      }

      fileTrackData::fileTrackData(std::string const& _filename)
         : torrent_filename_(_filename),
           entries_()
      {

      }

      fileTrackData::fileTrackData(fileTrackData const& _ftd)
         : torrent_filename_(_ftd.torrent_filename_),
           entries_(_ftd.entries_)
      {

      }

      fileTrackData& fileTrackData::operator= (fileTrackData const& _ftd)
      {
         if (*this != _ftd)
            {
               torrent_filename_ = _ftd.torrent_filename_;
               entries_          = _ftd.entries_;
            }

         return *this;
      }

      bool fileTrackData::operator== (fileTrackData const& _ftd) const
      {
         bool status = false;
         if (
             (_ftd.torrent_filename_ == torrent_filename_) &&
             (_ftd.entries_          == entries_)
             )
            {
               status = true;
            }
         return status;
      }

      bool fileTrackData::operator== (std::string const& _filename) const
      {
         bool status = false;
         if (_filename == torrent_filename_)
            {
               status = true;
            }

         return status;
      }

      bool fileTrackData::operator!= (fileTrackData const& _ftd) const
      {
         return !(_ftd == *this);
      }

      bool fileTrackData::isUnique(std::vector<std::string> const& _entries) const
      {
         std::vector<std::string>::const_iterator arg_iter;
         std::vector<std::string>::const_iterator entries_iter;

         /*
         BTG_MNOTICE("isUnique, argument entries:");
         for (arg_iter = _entries.begin();
              arg_iter != _entries.end();
              arg_iter++)
            {
               BTG_MNOTICE("entry = " << "'" << *arg_iter << "'.");
            }

         BTG_MNOTICE("isUnique, contents:");
         for (arg_iter = entries_.begin();
              arg_iter != entries_.end();
              arg_iter++)
            {
               BTG_MNOTICE("entry = " << "'" << *arg_iter << "'.");
            }
         */

         for (arg_iter = _entries.begin();
              arg_iter != _entries.end();
              arg_iter++)
            {

               entries_iter = std::find(entries_.begin(),
                                        entries_.end(),
                                        *arg_iter);
               if (entries_iter != entries_.end())
                  {
                     // Not unique.
                     // BTG_MNOTICE("isUnique, false.");
                     return false;
                  }
            }
	  
         // BTG_MNOTICE("isUnique, true.");
         return true;
      }

      void fileTrackData::setFiles(std::vector<std::string> const& _files)
      {
         // BTG_MNOTICE("setFiles, entries:");

         std::vector<std::string>::const_iterator arg_iter;

         /*
         for (arg_iter = _files.begin();
              arg_iter != _files.end();
              arg_iter++)
            {
               BTG_MNOTICE("entry = " << "'" << *arg_iter << "'.");
            }
         */

         entries_ = _files;
      }

      std::vector<std::string> const& fileTrackData::entries() const
      {
         return entries_;
      }

      fileTrackData::~fileTrackData()
      {
         entries_.clear();
      }

   } // namespace daemon
} // namespace btg


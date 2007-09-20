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
 * $Id: filetrack.cpp,v 1.1.2.5 2007/02/17 22:05:31 wojci Exp $
 */

#include "filetrack.h"
#include <bcore/logmacro.h>

#include "modulelog.h"

namespace btg
{
   namespace daemon
   {
      const std::string moduleName("fit");

      fileTrack::fileTrack()
         : files_(0)
      {
      }

      bool fileTrack::add(std::string const& _filename)
      {
         BTG_MNOTICE("adding filename '" << _filename << "'.");

         bool status = false;

         std::vector<fileTrackData>::iterator iter = check(_filename);

         if (iter == files_.end())
            {
               status = true;
               files_.push_back(_filename);
            }

         return status;
      }

     bool fileTrack::setFiles(std::string const& _filename, 
			      libtorrent::entry const& _torrent_entry)
     {
         std::vector<fileTrackData>::iterator iter = check(_filename);

         if (iter == files_.end())
	   {
	     return false;
	   }

	 if (!iter->setFiles(_torrent_entry))
	   {
	     // Unable to set the files, probably because the torrent
	     // entry cannot be converted into a torrent_info used by
	     // libtorrent.
	     return false;
	   }

	 // Find out if the files which were set are unique.
	 
	 std::vector<fileTrackData>::iterator uiter;

	 t_uint matches = 0;

	 for (uiter = files_.begin();
	      uiter != files_.end();
	      uiter++)
	   {
	     // If a single file matches another torrent, we got a match.
	     if (!uiter->isUnique(iter->entries()))
	       {
		 matches++;
	       }
	   }
	 
	 if (matches > 1)
	   {
	     // More than one torrent contains one or more file
	     // contained in the torrent which is used for compartion.
	     return false;
	   }

	 // The files added are unique.
	 return true;
     }

      void fileTrack::remove(std::string const& _filename)
      {
         BTG_MNOTICE("removing filename '" << _filename << "'.");

         std::vector<fileTrackData>::iterator iter = check(_filename);

         if (iter == files_.end())
            {
               BTG_MNOTICE("filename '" << _filename << "' was not found.");
               return;
            }

         files_.erase(iter);
      }

      bool fileTrack::exists(std::string const& _filename)
      {
         if (check(_filename) == files_.end())
	   {
	     return false;
	   }
         return true;
      }

      std::vector<fileTrackData>::iterator fileTrack::check(std::string const& _filename)
      {
         std::vector<fileTrackData>::iterator iter;

         for (iter = files_.begin();
              iter != files_.end();
              iter++)
            {
               if (*iter == _filename)
                  {
                     return iter;
                  }
            }
         return iter;
      }

      fileTrack::~fileTrack()
      {
	files_.clear();
      }

   } // namespace daemon
} // namespace btg


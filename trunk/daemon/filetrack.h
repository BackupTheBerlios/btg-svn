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

#ifndef FILETRACK_H
#define FILETRACK_H

#include <vector>
#include <string>

#include <libtorrent/entry.hpp>

#include "filetrackdata.h"

namespace btg
{
   namespace daemon
      {

         /**
          * \addtogroup daemon Daemon
          */
         /** @{ */

         /// Keep track of files added and removed. Used to prohibit
         /// adding the same torrent more than once.
         class fileTrack
            {
            public:
               /// Constructor.
               fileTrack();

               /// Add a filename. Return true if it can be added,
               /// false otherwise.
               bool add(std::string const& _filename);

	       /// Set the contents of a torrent.
	       /// @return True - contents are unique and was set. False otherwise.
	       bool setFiles(std::string const& _filename, 
			     libtorrent::entry const& _torrent_entry);

               /// Remove a filename.
               void remove(std::string const& _filename);

	       /// Check if the specified file is added or not.
	       /// @param [in] _filename Filename to check.
	       /// @return True - file exists. False otherwise.
               bool exists(std::string const& _filename);

               /// Destructor.
               ~fileTrack();
            private:
               /// Return an iterator or files_.end() if the filename
               /// in argument is not present in the list of
               /// filenames.
               std::vector<fileTrackData>::iterator check(std::string const& _filename);

               /// List of files.
               std::vector<fileTrackData> files_;
            };

         /** @} */

      } // namespace daemon
} // namespace btg

#endif // FILETRACK_H


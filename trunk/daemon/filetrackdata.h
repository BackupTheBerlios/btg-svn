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

#ifndef FILETRACKDATA_H
#define FILETRACKDATA_H

#include <vector>
#include <string>

#include <libtorrent/entry.hpp>

namespace btg
{
   namespace daemon
      {

         /**
          * \addtogroup daemon Daemon
          */
         /** @{ */

	/// Represents data kept for each torrent.
	class fileTrackData
	  {
	  public:
	    /// Default constructor.
	    fileTrackData();

	    /// Constructor.
	    fileTrackData(std::string const& _filename);

	    /// Copy constructor.
	    fileTrackData(fileTrackData const& _ftd);

	    /// The assignment operator.
	    fileTrackData& operator= (fileTrackData const& _ftd);

	    /// The equality operator.
	    bool operator== (fileTrackData const& _ftd) const;

	    /// The equality operator.
	    /// Used to compare against filename only.
	    bool operator== (std::string const& _filename) const;

	    /// The equality operator.
	    bool operator!= (fileTrackData const& _ftd) const;

	    /// Set the files belonging to a torrent being tracked.
	    bool setFiles(libtorrent::entry const& _torrent_entry);

	    /// Compare a list of files agains the list of files
	    /// contained in this class.
	    /// 
	    /// @return True - unique. False - not unique.
	    bool isUnique(std::vector<std::string> const& _entries) const;
	    
	    /// Get a reference to the entries contained in this
	    /// class.
	    std::vector<std::string> const& entries() const;

	    /// Destructor.
	    ~fileTrackData();
	  private:
	    /// The filename of the torrent.
	    std::string              torrent_filename_;
	    /// List of files contained in the torrent.
	    std::vector<std::string> entries_;
	  };

         /** @} */

      } // namespace daemon
} // namespace btg

#endif // FILETRACKDATA_H


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

#ifndef FILELIST_H
#define FILELIST_H

#include <map>
#include <string>
#include <vector>

#include <bcore/project.h>
#include <bcore/type_btg.h>

#include <bcore/status.h>

extern "C"
{
#include <ncurses.h>
}

#include "window.h"

#include <boost/filesystem/path.hpp>

namespace btg
{
   namespace UI
      {
         namespace cli
            {
               /**
                * \addtogroup ncli
                */
               /** @{ */

               namespace fs = boost::filesystem;

               /// Show a list of files and directories, used for
               /// selecting a torrent file to load.
               class fileList: public baseWindow, public dialog
                  {
                  public:
                     /// Constructor.
                     fileList(keyMapping const& _kmap, 
                              windowSize const& _ws,
                              std::string const& _directory = "");

                     /// Show the list.
                     dialog::RESULT run();

                     /// If a file was choosen, assign its filename to _file and return true.
                     /// If no file was choosen, return false.
                     bool getFile(std::string & _file) const;

                     /// Get the last directory which was browsed.
                     std::string getLastDirectory() const;

                     /// Destructor.
                     virtual ~fileList();
                  private:
                     /// Size of this window.
                     windowSize                           size_;

                     /// Current directory.
                     std::string                          currentDirectory_;

                     /// Currently selected file.
                     std::string                          currentFile_;

                     /// The entries in the current directory.
                     std::vector<boost::filesystem::path> entries_;

                     /// Number of lines this window can show.
                     t_int                                numberOfLines_;

                     /// Start of the window used to shown a part of
                     /// the files, in case not all can be shown at
                     /// the same time.
                     t_int                                positionWindowStart_;

                     /// End of the window used to shown a part of
                     /// the files, in case not all can be shown at
                     /// the same time.
                     t_int                                positionWindowEnd_;

                     /// Current position.
                     t_int                                currentPosition_;

                     /// The current ID.
                     t_int                                currentId_;

                     bool init(windowSize const& _ws);

                     void resize(windowSize const& _ws);

                     bool destroy();

                     void refresh();

                     /// Given the display size, adjust two iterators
                     /// to point at the beginning/end of the window
                     /// used to display a part of the list of entries
                     /// this window shows.
                     void fillIterator(std::vector<fs::path>::const_iterator & _startIter,
                                       std::vector<fs::path>::const_iterator & _endIter);

                     /// Draw the contained list.
                     void drawList();

                     /// Fill entries_ with the contents of the current directory.
                     void getDirectory();

                     /// Get the selected file or directory.
                     bool getSelection(boost::filesystem::path & _p) const;

                     ///  Move down the list of files.
                     void moveDown();

                     ///  Move up the list of files.
                     void moveUp();

                     ///  Move to the beginning of the list of files.
                     void toStart();

                     ///  Move to the end of the list of files.
                     void toEnd();

                     windowSize calculateDimenstions(windowSize const& _ws) const;

                     /// Show help for this dialog.
                     dialog::RESULT showHelp();

                     /// Move the a file or directory which name
                     /// begins with a certain character.
                     void moveTo(t_int const _key);
                  };

               /** @} */

            } // namespace cli
      } // namespace UI
} // namespace btg

#endif // FILELIST_H


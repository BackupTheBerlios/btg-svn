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
 * $Id: fileselect.h,v 1.1.2.2 2007/07/22 11:53:16 wojci Exp $
 */

#ifndef FILESELECT_H
#define FILESELECT_H

#include <bcore/project.h>
#include <bcore/type_btg.h>

#include <bcore/status.h>

extern "C"
{
#include <ncurses.h>
}

#include "window.h"

#include <bcore/file_info.h>

#include <string>
#include <vector>

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

               /// Window used to select files contained in a torrent.
               class fileSelect: public baseWindow, public dialog
                  {
                  public:
                     /// Constructor, view files.
                     fileSelect(keyMapping const& _kmap, 
                                windowSize const& _ws,
                                btg::core::selectedFileEntryList const& _list);

                     /// Show the list.
                     dialog::RESULT run();

                     /// Returns true, if user selected one or more
                     /// files.
                     bool pressed_select() const;

                     /// Return true if the list of files was changed.
                     bool changed() const;

                     /// Get the selected files.
                     void getFiles(btg::core::selectedFileEntryList & _list);

                     /// Destructor.
                     virtual ~fileSelect();
                  private:
                     /// Size of this window.
                     windowSize                                      size_;

                     /// The original list, used for comparing.
                     const std::vector<btg::core::selectedFileEntry> original_files_;

                     /// List of selected files.
                     std::vector<btg::core::selectedFileEntry>       selected_files_;

                     /// Number of lines this window can contain.
                     t_int                                           numberOfLines_;

                     /// Start of the window used to shown a part of
                     /// the files, in case not all can be shown at
                     /// the same time.
                     t_int                                           positionWindowStart_;

                     /// End of the window used to shown a part of the
                     /// files, in case not all can be shown at the
                     /// same time.
                     t_int                                           positionWindowEnd_;

                     /// The current position.
                     t_int                                           currentPosition_;

                     /// The current ID.
                     t_int                                           currentId_;

                     /// Indicates that the user selected files.
                     bool                                            pressed_select_key_;

                     bool init(windowSize const& _ws);

                     void resize(windowSize const& _ws);

                     bool destroy();

                     void refresh();

                     /// Draw the contained list.
                     void drawList();

                     /// Move down the list of files.
                     void moveDown();

                     /// Move up the list of files.
                     void moveUp();

                     /// Move to the beginning of the list of files.
                     void toStart();

                     /// Move to the end of the list of files.
                     void toEnd();

                     windowSize calculateDimenstions(windowSize const& _ws) const;

                     /// Show help for this window.
                     dialog::RESULT showHelp();

                     /// Split a string containing a full path to a
                     /// filename into the path and filename.
                     void splitPath(std::string const& _input,
                                    std::string & _path,
                                    std::string & _filename) const;
                     
                     /// Handle marking/unmarking one file.
                     void handleMark();

                     /// Handle marking/unmarking all files.
                     void handleMarkAll();
                  };

               /** @} */

            } // namespace cli
      } // namespace UI
} // namespace btg

#endif // FILESELECT_H


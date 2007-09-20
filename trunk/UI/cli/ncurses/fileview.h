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
 * $Id: fileview.h,v 1.1.2.5 2007/01/16 20:26:36 wojci Exp $
 */

#ifndef FILEVIEW_H
#define FILEVIEW_H

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

#include <bcore/file_info.h>

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

	      /// Window used to view files contained in a torrent.
               class fileView: public baseWindow, public dialog
                  {
                  public:
                     /// Constructor.
                     fileView(keyMapping const& _kmap, 
                              windowSize const& _ws,
                              std::vector<btg::core::fileInformation> const& _fi);

                     /// Show the list.
                     dialog::RESULT run();

                     /// Destructor.
                     virtual ~fileView();
                  private:
                     /// Size of this window.
                     windowSize                              size_;

                     /// List of file information objects shown by
                     /// this window.
                     std::vector<btg::core::fileInformation> info_;

                     /// Number of lines this window can contain.
                     t_int                                numberOfLines_;

                     /// Start of the window used to shown a part of
                     /// the files, in case not all can be shown at
                     /// the same time.
                     t_int                                positionWindowStart_;

                     /// End of the window used to shown a part of the
                     /// files, in case not all can be shown at the
                     /// same time.
                     t_int                                positionWindowEnd_;

                     /// The current position.
                     t_int                                currentPosition_;

                     /// The current ID.
                     t_int                                currentId_;

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

                     /// Based on the piece information for a certain
                     /// file, find out how much of it was downloaded.
                     void genPieces(btg::core::fileInformation const& _fi, std::string & _output);
                  };

	       /** @} */

            } // namespace cli
      } // namespace UI
} // namespace btg

#endif // FILEVIEW_H


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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include "se.h"
#include "dm.h"

namespace btg
{
   namespace UI
   {
      namespace cli
      {
         class UI;

         /// Window showing a list of torrents.
         class mainWindow: 
            private btg::core::Logable,
            public baseWindow, 
            public DisplayModelIf
         {
         public:
            /// Constructor.
            mainWindow(btg::core::LogWrapperType _logwrapper,
                       keyMapping const& _kmap, 
                       UI & _ui);

            bool init(windowSize const& _ws);

            void resize(windowSize const& _ws);

            void refresh();

            /// Update the list of contexts with a list of
            /// status objects.
            void update(std::vector<btg::core::Status> const& _list);

            /// Remove a number of IDs from the contained list.
            void remove(std::vector<t_int> const& _id_list);
		     
            /// Remove all entries.
            void clearContents();

            /// Mark the currently selected torrent.
            void mark();
                     
            /// Mark all torrents.
            void markAll();

            /// Get a list of marked torrents.
            void getMarked(std::vector<t_int> & _id_list) const;
		     
            /// Move down the list of torrents.
            void moveDown();

            /// Move up the list of torrents.
            void moveUp();

            /// Move to the (visible) start of the list.
            void toStart();

            /// Move to the (visible) end of the list.
            void toEnd();

            /// Get the selected entry.
            bool getSelection(btg::core::Status & _status);

            /// Get the selected entry.
            bool getSelection(btg::core::Status & _status, 
                              t_strList & _trackers);

            /// Get the number of selected entries.
            t_uint selected() const;

            /// Get the number of  entries.
            t_uint entries() const;

            /// Get a status, identified by a context id.
            bool get(t_int const _context_id, btg::core::Status & _status) const;

            /// Set the method used for sorting this list.
            void setSortBy(DisplayModel::sortBy const _sortby);

            windowSize calculateDimenstions(windowSize const& _ws) const;

            void pstart();

            void paint(const t_uint _number, 
                       const bool _current,
                       const statusEntry & _se,
                       const t_uint _total);

            void findSizes(std::vector<statusEntry> const& _l);

            void pend();

            /// Destructor.
            virtual ~mainWindow();
         private:
            /// Draw the contained list.
            void drawList();

            /// Given a torrent's status, convert it into a string 
            /// describing the torrent's progress.
            bool statusToProgress(btg::core::Status const& _s, 
                                  std::string & _output) const;

            /// Convenience function used to add a number of spaces 
            /// to a string.
            void addSpace(t_uint const _number,
                          std::string & _output) const;

            /// The number of lines that is available for
            /// this window to work with.
            t_int      numberOfLines_;

            /// The ID of the currently selected torrent.
            t_int      currentId_;

            /// Model used to display a list of entries.
            DisplayModel dm_;

            t_uint lcounter;
            t_uint max_filename_size;
            t_uint max_progress_size;
            t_uint max_stat_size;
            t_uint max_perc_size;
            t_uint max_peers_size;
            t_uint extra_space;

            /// Reference to the UI object used.
            UI &       ui_;
         };

         /** @} */

      } // namespace cli
   } // namespace UI
} // namespace btg

#endif // MAINWINDOW_H


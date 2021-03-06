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

         /// Status and flag for marking torrents.
         class statusEntry
         {
         public:
            /// Constructor used for adding new torrent. 
            /// Sets the updated flag.
            statusEntry(btg::core::Status const& _s);

            /// Default constructor.
            statusEntry();
         public:
            /// The status.
            btg::core::Status status;

            /// Flag: this torrent is marked.
            bool              marked;

            /// Flag: updated.
            bool              updated;
         };

         /// List of context status objects.
         /// Status objects in this list can be marked.
         /// 
         /// This is reflected in the display as torrents marked
         /// with a different color than the unmarked torrents.
         class statusList
         {
         public:
            /// Constructor. Creates an empty list.
            statusList();

            /// Get the list of status objects.
            void get(std::vector<statusEntry> & _list) const;

            /// Get a status object, based in its context ID.
            bool get(t_uint const _context_id,
                     btg::core::Status & _status) const;

            /// Get a status, based on the position in the
            /// list.
            bool getAtPosition(t_uint const _position,
                               btg::core::Status & _status) const;

            /// Mark an entry, based on its position.
            void mark(t_uint const _position);

            /// Mark all entries.
            void markAll();

            /// Get a list of marked entries.
            void getMarked(std::vector<t_int> & _id_list) const;

            /// Clear any marks.
            void clearMark();

            /// Update the contained list.
            /// Sets the changed flag, if new status objects were added.
            void update(std::vector<btg::core::Status> const& _list);

            /// Remove status objects maching the IDs in the list.
            /// Sets the changed flag, if status objects were removed.
            void remove(std::vector<t_int> const& _id_list);

            /// Remove all entries.
            void clear();

            /// Sets the changed flag to false.
            void resetChanged();

            /// Was the contained list changed by the last
            /// call to update or remove.
            ///
            /// \return True - if the contained list was
            /// updated by the last call to the update
            /// function. False otherwise.
            ///
            /// \note That true is returned when the number
            /// of contexts change and not when contexts get
            /// updated.
            bool changed() const;

            /// Get the number of status objects contained in
            /// this list.
            t_uint size() const;

            /// Destructor.
            ~statusList();
         private:
            /// Indicates that the list was changed by a call
            /// to update or remove.
            bool                               changed_;
            /// Maps context ID to entries.
            std::map<t_int, statusEntry> statusList_;

            /// Reset the updated flag for the contained
            /// entries.
            void resetUpdated();

            /// Remove dead entries.
            void removeDead();
         };

         /// Window showing a list of torrents.
         class mainWindow: public baseWindow
         {
         public:
            /// Constructor.
            mainWindow(keyMapping const& _kmap);

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
		     
            /// Clear all markings.
            void clearMark();

            /// Move down the list of torrents.
            void moveDown();

            /// Move up the list of torrents.
            void moveUp();

            /// Move to the (visible) start of the list.
            void toStart();

            /// Move to the (visible) end of the list.
            void toEnd();

            /// Get the selected entry.
            bool getSelection(btg::core::Status & _status) const;

            /// Get a status, identified by a context id.
            bool get(t_int const _context_id, btg::core::Status & _status) const;

            windowSize calculateDimenstions(windowSize const& _ws) const;

            /// Destructor.
            virtual ~mainWindow();
         private:
            /// Draw the contained list.
            void drawList();

            bool statusToProgress(btg::core::Status const& _s, 
                                  std::string & _output) const;

            void addSpace(t_uint const _number,
                          std::string & _output) const;

            /// The number of lines that is available for
            /// this window to work with.
            t_int      numberOfLines_;

            /// List of status objects.
            statusList list_;

            /// Window used to view torrents, when more
            /// torrents than can be displayed on the screen
            /// at the same time is present.
            t_int      positionWindowStart_;

            /// Window used to view torrents, when more
            /// torrents than can be displayed on the screen
            /// at the same time is present.
            t_int      positionWindowEnd_;

            /// The currently selected torrent.
            t_int      currentPosition_;

            /// The ID of the currently selected torrent.
            t_int      currentId_;
         };

         /** @} */

      } // namespace cli
   } // namespace UI
} // namespace btg

#endif // MAINWINDOW_H


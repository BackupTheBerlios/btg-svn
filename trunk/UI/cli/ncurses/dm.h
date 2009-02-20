/*
 * btg Copyright (C) 2009 Michael Wojciechowski.
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

#ifndef DISPLAY_MODEL_H
#define DISPLAY_MODEL_H

#include <string>
#include <vector>

#include <bcore/project.h>
#include <bcore/type_btg.h>

#include <bcore/status.h>

#include "se.h"

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

         class DisplayModelIf
            {
            public:
               /// Start painting.
               virtual void pstart() = 0;
               /// Paint.
               virtual void paint(const t_uint _number, 
                                  const bool _current,
                                  const statusEntry & _se,
                                  const t_uint _total) = 0;
               /// Find max sizes of the list of status entries.
               virtual void findSizes(std::vector<statusEntry> const& _l) = 0;
               /// End painting.
               virtual void pend() = 0;
            };

         /// Model used for showing a list of items, scrolling down/up
         /// the list, going to begin/end and selecting entries in the
         /// list.
         class DisplayModel
         {
         public:
            /// Different ways of sorting this list.
            enum sortBy
            {
               sB_Id = 0,   /// Bu Id.
               sB_Name,     /// By name.
               sB_Size,     /// By file size.
               sB_UlSpeed,  /// By upload speed.
               sB_DlSpeed,  /// By download speed.
               sB_Peers,    /// By number of peers.
               sB_Done      /// By percent done.
            };
         public:
            /// Constructor.
            DisplayModel(t_uint _visible, 
                         DisplayModelIf* _displayModelIf);
         public:
            /// Draw the list.
            void draw();

         public:
            /// Move down the list.
            void down();

            /// Move up the list.
            void up();

            /// Scroll to begninning.
            void begin();
            
            /// Scroll to end.
            void end();

            /// Select current.
            void mark();

            /// Select all.
            void markAll();

            /// Select none.
            void markNone();

            /// Get the IDs of the selected entries.
            void getSelected(std::vector<t_int> & _id_list) const;

            /// Get the number of selected entries.
            t_uint selected() const;

            /// Resize visible area.
            void resize(t_uint _visible);

            /// Size of the contained entries.
            t_uint size() const;

            /// Indicates if the size of the list was changed adding
            /// or removing elements.
            bool sizeChanged() const;
         public:
            /// Update the list of entries with information contained
            /// in a list of status objects.
            void update(std::vector<btg::core::Status> const& _list);

            /// Set the method used for sorting the entries.
            void setSortBy(DisplayModel::sortBy const _sortby);

            /// Get the status for the currently selected entry.
            bool getCurrent(btg::core::Status & _status) const;

            /// Get the tracker list for the currently selected entry.
            bool getCurrent(t_strList & _trackers) const;

            /// Set the tracker list for the currently selected entry.
            bool setCurrent(t_strList const& _trackers);

            /// Get the status for a context id.
            bool get(t_int const _context_id, 
                     btg::core::Status & _status) const;

            /// Remove contexts identified by context id (the list).
            void remove(std::vector<t_int> const& _id_list);
         public:
            /// Destructor.
            virtual ~DisplayModel();
         protected:
            /// Number of visible entries.
            t_int visible_;

            /// Display interface.
            DisplayModelIf* displayModelIf_;

            /// List position.
            t_int lposition_;

            /// Window position;
            t_int wposition_;

            /// Contexts.
            std::vector<statusEntry>     statusList_;

            /// Method used for sorting this list.
            sortBy                       sortby_;

            /// Indicates if new entries are present.
            bool                         newEntries_;
            /// Indicates that entries were deleted.
            bool                         deletedEntries_;

            /// Sort the entries.
            void sort();

            /// Get an iterator for the entry identified by a context
            /// id.
            std::vector<statusEntry>::iterator find(t_int const _id);
            /// Get an iterator for the entry identified by a context
            /// id.
            std::vector<statusEntry>::const_iterator find(t_int const _id) const;
            /// Clear the updated flag.
            void resetUpdatedFlag(bool _flag = false);

            /// Remove deleted contexts.
            void removeDead();

            /// Comparator, used for sorting entries.
            bool isStatusLess(statusEntry const& _l, 
                              statusEntry const& _r) const;
         };

         /** @} */

      } // namespace cli
   } // namespace UI
} // namespace btg

#endif // DISPLAY_MODEL_H


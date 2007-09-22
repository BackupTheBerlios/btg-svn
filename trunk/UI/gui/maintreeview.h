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

#ifndef MAINTREEVIEW_H
#define MAINTREEVIEW_H

#include <map>
#include <gtkmm/treeview.h>
#include "mainmodel.h"
#include <bcore/status.h>

namespace btg
{
   namespace UI
      {
         namespace gui
            {
               /**
                * \addtogroup gui
                */
               /** @{ */

               /// Iterator, used to access the data contained in this table.
               typedef Gtk::TreeModel::Children::iterator  child_iter;
               /// Type used to describe if a context id is selected.
               typedef std::map<int, bool>                 elemMap;
               /// Iterator used to access selection information.
               typedef std::map<int, bool>::const_iterator elemMap_citer;

               class mainToolbar;

               /// Implements a table which shows context and their
               /// properties.
               class mainTreeview : public Gtk::TreeView
                  {
                  public:
                     /// Constructor.
                     mainTreeview();

                     /// Update this model.
                     /// @param [in] _id             Context ID.
                     /// @param [in] _status         Status of the ID.
                     /// @param [in] _updateProgress If true, the status field will be updated.
                     void updateStatus(int const _id, btg::core::Status const& _status,
                                       bool const _updateProgress = false);

                     /// Remove the a context from the list.
                     /// @param [in] _id Context ID.
                     void removeStatus(int const _id);

                     /// Get a list of context IDs which are selected.
                     elemMap getSelectedIds() const;

                     /// Returns true if contexts are selected.
                     bool gotSelection() const;

                     /// Returns the number of selected contexts.
                     t_uint numberOfSelected() const;

                     /// Destructor.
                     virtual ~mainTreeview();
                  private:
                     /// Call back for general change of selection.
                     void on_selection_changed();

                     /// Callback used when something gets selected.
                     void selected_row_callback(const Gtk::TreeModel::iterator& iter);

                     /// Clears the selection, called by on_selection_changed.
                     void clearSelection();

                     /// Find and return an iterator to a context ID.
                     child_iter find_id(int const _id);

                     /// Add a context to the list.
                     void add(int const _id, btg::core::Status const& _status);

                     /// Update an already present context.
                     void update(child_iter & _iter, btg::core::Status const& _status, bool const _updateProgress);
                     /// Remove a context.
                     void remove(child_iter & _iter);

                     /// Data this view operates on.
                     statusRecord                      statusrecord;

                     /// The store.
                     Glib::RefPtr<Gtk::ListStore>      refListStore;

                     /// Reference to the selected elements of this list.
                     Glib::RefPtr<Gtk::TreeSelection>  refSelection;

                     /// List of selected elements.
                     elemMap                           selectedElements;
                  private:
                     /// Copy constructor.
                     mainTreeview(mainTreeview const& _mtv);
                     /// Assignment operator.
                     mainTreeview& operator=(mainTreeview const& _mtv);
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif


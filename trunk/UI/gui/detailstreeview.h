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

#ifndef DETAILSTREEVIEW_H
#define DETAILSTREEVIEW_H

#include <gtkmm/treeview.h>
#include <bcore/type_btg.h>

#include "detailsmodel.h"

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

               /// Implements a table which shows peers and their
               /// properties.
               class detailsTreeview : public Gtk::TreeView
                  {
                  public:
                     /// Constructor.
                     detailsTreeview();

		     /// Update the table using a reference to a
		     /// status object.
                     void update(btg::core::Status const& _status);

                     /// Clear the table.
                     void clear();

                     /// Destructor.
                     virtual ~detailsTreeview();
                  private:
                     /// Data this view operates on.
                     detailsRecord                     detailsrecord;

                     /// The store.
                     Glib::RefPtr<Gtk::ListStore>      refListStore;

                     /// Callback, triggered when a selection is made.
                     void on_selection_changed();

		     /// Add two lines to the table, a topic and some text.
                     void add(std::string const& _topic,
                              std::string const& _text);
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif // DETAILSTREEVIEW_H


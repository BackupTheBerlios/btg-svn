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
 * $Id: peertreeview.h,v 1.1.2.1 2006/03/23 21:15:48 wojci Exp $
 */

#ifndef PEERTREEVIEW_H
#define PEERTREEVIEW_H

#include <gtkmm/treeview.h>
#include <bcore/type_btg.h>

#include "peermodel.h"

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
               class peerTreeview : public Gtk::TreeView
                  {
                  public:
                     /// Constructor.
                     peerTreeview();

                     /// Update the table with a list of peers.
                     void update(t_peerList const& _peerlist);

                     /// Clear the table.
                     void clear();

                     /// Destructor.
                     virtual ~peerTreeview();
                  private:
                     /// Data this view operates on.
                     peerRecord                        peerrecord;

                     /// The store.
                     Glib::RefPtr<Gtk::ListStore>      refListStore;

                     /// Callback, triggered when a selection is made.
                     void on_selection_changed();
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif // PEERTREEVIEW_H


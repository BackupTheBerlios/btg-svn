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

#ifndef SELECTFILETREEVIEW_H
#define SELECTFILETREEVIEW_H

#include <map>
#include <gtkmm/treeview.h>
#include "filemodel.h"
#include <bcore/type_btg.h>
#include <bcore/file_info.h>

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

               class mainToolbar;

               /// Implements a table used for showing files.
               class selectFileTreeview : public Gtk::TreeView
                  {
                  public:
                     /// Constructor.
                     selectFileTreeview();

                     /// Update the view with information about which
                     /// files are selected.
                     void update(int const _id, 
                                 btg::core::selectedFileEntryList const& _sf);

                     /// Clear the table.
                     void clear();

                     /// Callback: executed when a file gets selected.
                     void on_toggled(const Glib::ustring& path);

                     /// Get the list of selected files.
                     bool getToogled(t_int & _context_id, 
                                     btg::core::selectedFileEntryList & _sfel);

                     /// Destructor.
                     virtual ~selectFileTreeview();
                  private:
                     /// Id of current context.
                     t_int                            current_id;

                     /// A record used to keep the data this table displays in.
                     selectRecord                     filerecord;

                     /// Reference to a store.
                     Glib::RefPtr<Gtk::ListStore>     refListStore;

                     /// Struct used to represent selected files.
                     struct selectedItems
                     {
                        /// The filename.
                        std::string                      toggle_filename;
                        /// True, if selected.
                        bool                             toggle_value;
                     };

                     /// List of selected files.
                     std::vector<selectedItems> items;
                  private:
                     /// Copy constructor.
                     selectFileTreeview(selectFileTreeview const& _sftv);

                     /// Assignment operator.
                     selectFileTreeview& operator=(selectFileTreeview const& _sftv);
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif // SELECTFILETREEVIEW_H


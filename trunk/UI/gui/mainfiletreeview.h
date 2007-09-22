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

#ifndef MAINFILETREEVIEW_H
#define MAINFILETREEVIEW_H

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
               class mainFileTreeview : public Gtk::TreeView
                  {
                  public:
                     /// Constructor.
                     mainFileTreeview();

                     /// Update the table using a list of file information objects.
                     void update(int const _id, 
                                 t_fileInfoList const& _fileinfolist);

                     /// Clear the table.
                     void clear();

                     /// Destructor.
                     virtual ~mainFileTreeview();
                  private:
                     enum
                        {
                           /// The width of the pixmap used to show pieces of a context.
                           pixmap_size_x          = 200,

                           /// The height of the pixmap used to show pieces of a context.
                           pixmap_size_y          = 100,
                           
                           /// The number of pixels the pixmap used to show pieces of a context contains.
                           pixmap_bits_per_sample = 8
                        };

                     /// Id of current context.
                     t_int                            current_id;

                     /// A record used to keep the data this table displays in.
                     fileRecord                       filerecord;

                     /// Reference to a store.
                     Glib::RefPtr<Gtk::ListStore>     refListStore;

                     /// The saved size of the list, used to detect
                     /// updates which require clearing the list.
                     t_uint                           saved_size;
                  private:
                     /// Copy constructor.
                     mainFileTreeview(mainFileTreeview const& _mftv);

                     /// Assignment operator.
                     mainFileTreeview& operator=(mainFileTreeview const& _mftv);
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif

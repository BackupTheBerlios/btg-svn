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

#ifndef DETAILSMODEL_H
#define DETAILSMODEL_H

#include <gtkmm.h>
#include <bcore/type.h>

namespace Gtk
{
   class TreeView;
}

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

               /// Record used in connection with mainTreeview
               /// (table). Represents a detailed view of a torrent.
               class detailsRecord : public Gtk::TreeModel::ColumnRecord
                  {
                  public:
                     /// Constructor.
                     detailsRecord();

                     /// Append data from an instance of this object to a view.
                     void appendToView(Gtk::TreeView *treeview);

                     /// Line of text.
                     Gtk::TreeModelColumn<Glib::ustring> text;
                     
                     /// Destructor.
                     virtual ~detailsRecord();
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif // DETAILSMODEL_H

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

#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <gtkmm.h>
#include <string>

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

               class mainFileTreeview;
               class selectFileTreeview;

               /// Model used for showing files torrent is made of.
               class fileRecord : public Gtk::TreeModel::ColumnRecord
                  {
                  public:
                     /// Constructor.
                     fileRecord();

                     /// Append an instance of this class to a TreeView, for viewing.
                     /// @param [out] _treeview Pointer to the view to which
                     /// data should be appended.
                     void appendToView(mainFileTreeview* _treeview);

                     /// Field: the filename.
                     Gtk::TreeModelColumn<Glib::ustring>              filename;

                     /// Field: Pieces of this torrent, displayed using graphics.
                     Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > filepieces;

                     /// Field: the file size.
                     Gtk::TreeModelColumn<Glib::ustring>              filesize;

                     /// Destructor.
                     virtual ~fileRecord();
                  };

               /// Model used for showing files torrent is made
               /// of. Allows selecting/deselecting files.
               class selectRecord : public Gtk::TreeModel::ColumnRecord
                  {
                  public:
                     /// Constructor.
                     selectRecord();

                     /// Append an instance of this class to a TreeView, for viewing.
                     /// @param [out] _treeview Pointer to the view to which
                     /// data should be appended.
                     void appendToView(selectFileTreeview* _treeview);

                     /// Field: the filename.
                     Gtk::TreeModelColumn<Glib::ustring>              filename;

                     /// Indicates that the file is selected.
                     Gtk::TreeModelColumn<bool>                       selected;

                     /// Destructor.
                     virtual ~selectRecord();
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif

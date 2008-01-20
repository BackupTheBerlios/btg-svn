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

#include <gtkmm/treeview.h>
#include "filemodel.h"

#include "mainfiletreeview.h"
#include "selectfiletreeview.h"

#include "gtkmm_ver.h"

#if GTKMM_2_6_OR_BETTER
#  include <gtkmm/cellrenderertoggle.h>
#endif // GTKMM_2_6_OR_BETTER

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         fileRecord::fileRecord()
            : filename(),
              filepieces(),
              filesize()
         {
            add(filename);
            add(filepieces);
            add(filesize);
         }

         void fileRecord::appendToView(mainFileTreeview* _treeview)
         {
            _treeview->append_column("Filename", filename);

            Gtk::CellRendererPixbuf* imagerenderer = Gtk::manage(new Gtk::CellRendererPixbuf);
            Gtk::TreeViewColumn* iconcolumn = Gtk::manage(new Gtk::TreeViewColumn("Pieces", *imagerenderer));
            iconcolumn->set_renderer(*imagerenderer, filepieces);
            _treeview->append_column (*iconcolumn);
            _treeview->append_column("Filesize", filesize);
         }

         fileRecord::~fileRecord()
         {

         }

         /* */
         /* */

         selectRecord::selectRecord()
            : filename(),
              selected()
         {
            add(filename);
            add(selected);
         }

         void selectRecord::appendToView(selectFileTreeview* _treeview)
         {
            _treeview->append_column("Filename", filename);

            Gtk::CellRendererToggle* toggle_renderer = Gtk::manage(new Gtk::CellRendererToggle);
            toggle_renderer->signal_toggled().connect(sigc::mem_fun(*_treeview, &selectFileTreeview::on_toggled));

            Gtk::TreeViewColumn* togglecolumn = Gtk::manage(new Gtk::TreeViewColumn("Selected", *toggle_renderer));
            togglecolumn->set_renderer(*toggle_renderer, selected);
            _treeview->append_column(*togglecolumn);
         }

         selectRecord::~selectRecord()
         {

         }

      } // namespace gui
   } // namespace UI
} // namespace btg

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

#include "mainmodel.h"

#include <UI/gui/guiutils.h>

#include <gtkmm.h>

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         statusRecord::statusRecord()
            : id(),
              filename(),
              filesize(),
              status(),
              done(),
              doneText(),
              dn_rate(),
              ul_rate(),
              peers()
         {
            add(id);
            add(filename);
            add(filesize);
            add(status);
            add(done);
            add(doneText);
            add(dn_rate);
            add(ul_rate);
            add(peers);
         }

         void statusRecord::appendToView(Gtk::TreeView* _treeview)
         {
            _treeview->append_column("Filename", filename);
            
            _treeview->append_column("Status",   status);

            // Add a progress bar, showing status.
            Gtk::CellRendererProgress* cell = Gtk::manage(new Gtk::CellRendererProgress);
            int last_count = _treeview->append_column("Progress", *cell);

            Gtk::TreeViewColumn* column = _treeview->get_column(last_count - 1);
            if (column)
               {
                  column->add_attribute(cell->property_value(), done);
                  column->add_attribute(cell->property_text(), doneText);
               }

            _treeview->append_column("Download", dn_rate);
            _treeview->append_column("Upload",   ul_rate);
            _treeview->append_column("Filesize", filesize);
            _treeview->append_column("Peers l/s", peers);
            
            headersSetResizable(*_treeview);
         }

         statusRecord::~statusRecord()
         {

         }

      } // namespace gui
   } // namespace UI
} // namespace btg

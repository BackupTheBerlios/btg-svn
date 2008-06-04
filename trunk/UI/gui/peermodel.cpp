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

#include "peermodel.h"

#include <UI/gui/guiutils.h>

namespace btg
{
   namespace UI
   {
      namespace gui
      {
         peerRecord::peerRecord()
         {
            add(ipv4);
            add(status);
            add(identification);
            
            /*
             * Extended info
             */
            add(flags);
            add(source);
            add(down_speed);
            add(up_speed);
            add(payload_down_speed);
            add(payload_up_speed);
            add(pieces);
            add(download_limit);
            add(upload_limit);
            add(country);
            add(load_balancing);
            add(download_queue_length);
            add(upload_queue_length);
            add(downloading_piece_index);
            add(downloading_block_index);
            add(downloading_progress);
            add(downloading_total);
            add(client);
            add(connection_type);
            add(last_request);
            add(last_active);
            add(num_hashfails);
            add(failcount);
            add(target_dl_queue_length);
            add(remote_dl_rate);
         }

         void peerRecord::appendToView(Gtk::TreeView* _treeview)
         {
            _treeview->append_column("Address", ipv4);
            _treeview->append_column("Status",  status);
            _treeview->append_column("Identification", identification);

            /*
             * Extended info
             */
            _treeview->append_column("Flags", flags);
            _treeview->append_column("Source", source);
            _treeview->append_column("Down speed", down_speed);
            _treeview->append_column("Up speed", up_speed);
            _treeview->append_column("Payload down speed", payload_down_speed);
            _treeview->append_column("Payload up speed", payload_up_speed);

            Gtk::CellRendererPixbuf* imagerenderer = Gtk::manage(new Gtk::CellRendererPixbuf);
            Gtk::TreeViewColumn* iconcolumn = Gtk::manage(new Gtk::TreeViewColumn("Pieces", *imagerenderer));
            iconcolumn->set_renderer(*imagerenderer, pieces);
            _treeview->append_column(*iconcolumn);
            
            _treeview->append_column("Download limit", download_limit);
            _treeview->append_column("Upload limit", upload_limit);
            _treeview->append_column("Country", country);
            _treeview->append_column("Load balancing", load_balancing);
            _treeview->append_column("Upload queue length", upload_queue_length);
            _treeview->append_column("Download queue length", download_queue_length);
            _treeview->append_column("Downloading piece index", downloading_piece_index);
            _treeview->append_column("Downloading block index", downloading_block_index);
            
            Gtk::CellRendererProgress* cell = Gtk::manage(new Gtk::CellRendererProgress);
            int last_count = _treeview->append_column("Downloading block progress", *cell);
            Gtk::TreeViewColumn* column = _treeview->get_column(last_count - 1);
            if (column)
               {
                  column->add_attribute(cell->property_value(), downloading_progress);
                  column->add_attribute(cell->property_text(), downloading_total);
               }

            _treeview->append_column("Client", client);
            _treeview->append_column("Connection type", connection_type);
            _treeview->append_column("Last request", last_request);
            _treeview->append_column("Last active", last_active);
            _treeview->append_column("Hash fails", num_hashfails);
            _treeview->append_column("Fail count", failcount);
            _treeview->append_column("Target dl queue length", target_dl_queue_length);
            _treeview->append_column("Remote dl rate", remote_dl_rate);

            headersSetResizable(*_treeview);
         }

         peerRecord::~peerRecord()
         {

         }

      } // namespace gui
   } // namespace UI
} // namespace btg

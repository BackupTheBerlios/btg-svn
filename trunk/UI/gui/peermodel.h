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

#ifndef PEERMODEL_H
#define PEERMODEL_H

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
               /// (table). Represents a peer.
               class peerRecord : public Gtk::TreeModel::ColumnRecord
                  {
                  public:
                     /// Constructor.
                     peerRecord();

                     /// Append data from an instance of this object to a view.
                     void appendToView(Gtk::TreeView *treeview);

                     Gtk::TreeModelColumn<Glib::ustring> ipv4;

                     Gtk::TreeModelColumn<Glib::ustring> status;

                     Gtk::TreeModelColumn<Glib::ustring> identification;
                     
                     /*
                      * Extended fields
                      */

                     Gtk::TreeModelColumn<Glib::ustring> flags;

                     Gtk::TreeModelColumn<Glib::ustring> source;

                     Gtk::TreeModelColumn<Glib::ustring> down_speed;

                     Gtk::TreeModelColumn<Glib::ustring> up_speed;

                     Gtk::TreeModelColumn<Glib::ustring> payload_down_speed;
                     
                     Gtk::TreeModelColumn<Glib::ustring> payload_up_speed;

                     Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > pieces;

                     Gtk::TreeModelColumn<Glib::ustring> download_limit;

                     Gtk::TreeModelColumn<Glib::ustring> upload_limit;

                     Gtk::TreeModelColumn<Glib::ustring> country;

                     Gtk::TreeModelColumn<Glib::ustring> load_balancing;

                     Gtk::TreeModelColumn<Glib::ustring> upload_queue_length;

                     Gtk::TreeModelColumn<Glib::ustring> download_queue_length;

                     Gtk::TreeModelColumn<Glib::ustring> downloading_piece_index;

                     Gtk::TreeModelColumn<Glib::ustring> downloading_block_index;

                     Gtk::TreeModelColumn<Glib::ustring> downloading_progress;

                     Gtk::TreeModelColumn<Glib::ustring> downloading_total;

                     Gtk::TreeModelColumn<Glib::ustring> client;

                     Gtk::TreeModelColumn<Glib::ustring> connection_type;

                     Gtk::TreeModelColumn<Glib::ustring> last_request;

                     Gtk::TreeModelColumn<Glib::ustring> last_active;

                     Gtk::TreeModelColumn<Glib::ustring> num_hashfails;

                     Gtk::TreeModelColumn<Glib::ustring> failcount;

                     Gtk::TreeModelColumn<Glib::ustring> target_dl_queue_length;

                     Gtk::TreeModelColumn<Glib::ustring> remote_dl_rate;

                     /// Destructor.
                     virtual ~peerRecord();
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif // PEERMODEL_H

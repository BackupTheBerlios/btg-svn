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

                     /// Address.
                     Gtk::TreeModelColumn<Glib::ustring> ipv4;

                     /// Leecher / seeder.
                     Gtk::TreeModelColumn<Glib::ustring> status;

                     /// What client.
                     Gtk::TreeModelColumn<Glib::ustring> identification;
                     
                     /*
                      * Extended fields
                      */

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> flags;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> source;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> down_speed;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> up_speed;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> payload_down_speed;
                     
                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> payload_up_speed;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > pieces;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> download_limit;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> upload_limit;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> country;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> load_balancing;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> upload_queue_length;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> download_queue_length;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> downloading_piece_index;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> downloading_block_index;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<t_uint>        downloading_progress;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> downloading_total;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> client;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> connection_type;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> last_request;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> last_active;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> num_hashfails;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> failcount;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> target_dl_queue_length;

                     /// @see btg::core::PeerEx.
                     Gtk::TreeModelColumn<Glib::ustring> remote_dl_rate;

                     /// Destructor.
                     virtual ~peerRecord();
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif // PEERMODEL_H

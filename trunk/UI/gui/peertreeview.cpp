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

#include "peertreeview.h"

#include "fipixbox.h"
#include <bcore/t_string.h> // only for debugging

namespace btg
{
   namespace UI
   {
      namespace gui
      {
         using namespace btg::core;

         peerTreeview::peerTreeview()
         {
            set_flags(Gtk::CAN_FOCUS);
            set_headers_visible(true);
            set_rules_hint(false);
            set_reorderable(false);
            set_enable_search(false);

            // Create a store which consists of status records.
            refListStore = Gtk::ListStore::create(peerrecord);
            set_model(refListStore);

            // Add the status record to this view.
            peerrecord.appendToView(this);

            Glib::RefPtr<Gtk::TreeSelection> refSelection = get_selection();

            refSelection->signal_changed().connect(sigc::mem_fun(*this, &peerTreeview::on_selection_changed));

            show();
         }

         void peerTreeview::update(t_peerList const& _peerlist)
         {
            for (t_peerListCI piter = _peerlist.begin();
                 piter != _peerlist.end();
                 piter++)
               {
                  // Add a new row.
                  Gtk::TreeModel::iterator iter = refListStore->append();
                  Gtk::TreeModel::Row       row = *iter;

                  row[peerrecord.ipv4]          = piter->address();

                  if (piter->seeder())
                     {
                        row[peerrecord.status]  = "seed";
                     }
                  else
                     {
                        row[peerrecord.status]  = "leech";
                     }

                  row[peerrecord.identification] = piter->identification();
               }
         }

         void peerTreeview::update(t_uint _offset, t_peerExList const& _peerExlist)
         {
            for(
               t_peerExListCI pi = _peerExlist.begin();
               _offset < refListStore->children().size() && pi != _peerExlist.end();
               ++_offset, ++pi)
            {
               refListStore->children()[_offset][peerrecord.flags] = pi->flags();
               refListStore->children()[_offset][peerrecord.source] = pi->source();
               refListStore->children()[_offset][peerrecord.down_speed] = pi->down_speed();
               refListStore->children()[_offset][peerrecord.up_speed] = pi->up_speed();
               refListStore->children()[_offset][peerrecord.payload_down_speed] = pi->payload_down_speed();
               refListStore->children()[_offset][peerrecord.payload_up_speed] = pi->payload_up_speed();

               fileInfoPixbox piecespixbox(pixmap_bits_per_sample,
                                           pixmap_size_x,
                                           pixmap_size_y,
                                           pi->pieces());
               refListStore->children()[_offset][peerrecord.pieces] = piecespixbox.getPixBuf();
               
               refListStore->children()[_offset][peerrecord.download_limit] = pi->download_limit();
               refListStore->children()[_offset][peerrecord.upload_limit] = pi->upload_limit();
               refListStore->children()[_offset][peerrecord.country] = pi->country();
               refListStore->children()[_offset][peerrecord.load_balancing] = pi->load_balancing();
               refListStore->children()[_offset][peerrecord.download_queue_length] = pi->download_queue_length();
               refListStore->children()[_offset][peerrecord.upload_queue_length] = pi->upload_queue_length();
               refListStore->children()[_offset][peerrecord.downloading_piece_index] = pi->downloading_piece_index();
               refListStore->children()[_offset][peerrecord.downloading_block_index] = pi->downloading_block_index();
               refListStore->children()[_offset][peerrecord.downloading_progress] = convertToString(pi->downloading_progress());
               refListStore->children()[_offset][peerrecord.downloading_total] = pi->downloading_total();
               refListStore->children()[_offset][peerrecord.client] = pi->client();
               refListStore->children()[_offset][peerrecord.connection_type] = pi->connection_type();
               refListStore->children()[_offset][peerrecord.last_request] = pi->last_request();
               refListStore->children()[_offset][peerrecord.last_active] = pi->last_active();
               refListStore->children()[_offset][peerrecord.num_hashfails] = pi->num_hashfails();
               refListStore->children()[_offset][peerrecord.failcount] = pi->failcount();
               refListStore->children()[_offset][peerrecord.target_dl_queue_length] = pi->target_dl_queue_length();
               refListStore->children()[_offset][peerrecord.remote_dl_rate] = pi->remote_dl_rate();
            }
         }
         
         void peerTreeview::clear()
         {
            refListStore->clear();
         }

         void peerTreeview::on_selection_changed()
         {
            // When something gets selected, unselect it again.
            // There is no reason to be able to select rows in this
            // table, it is only used to show peers.
            Glib::RefPtr<Gtk::TreeSelection> refSelection = get_selection();
            refSelection->unselect_all();
         }

         peerTreeview::~peerTreeview()
         {
         }

      } // namespace gui
   } // namespace UI
} // namespace btg

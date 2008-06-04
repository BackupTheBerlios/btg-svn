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
         
         const char * peerTreeview::tooltips[] = {
            // ipv4 [0]
            "Address",
            
            // status [1]
            "Status",
            
            // identification [2]
            "Identification - BT client name determined by decoding its peer-id",
            
            // flags [3]
            "Flags - the state of the peer.\n"
            "\tI   (interesting) - we are interested in pieces from this peer\n"
            "\tCh  (choked) - we have choked this peer\n"
            "\trI  (remote_interested) - the peer is interested in us\n"
            "\trCh (remote_choked) - the peer has choked us\n"
            "\tEx  (supports_extensions) - peer supports the extension protocol\n"
            "\tL   (local_connection) - The connection was initiated by us,"
               " the peer has a listen port open, and that port is the same as in the "
               "address of this peer. If this flag is not set, this peer connection "
               "was opened by this peer connecting to us.\n"
            "\tH   (handshake) - The connection is opened, and waiting for the handshake. "
               "Until the handshake is done, the peer cannot be identified.\n"
            "\tC   (connecting) - The connection is in a half-open state (i.e. it is being connected)\n"
            "\tQ   (queued) - The connection is currently queued for a connection attempt. "
               "This may happen if there is a limit set on the number of half-open TCP connections.\n"
            "\tP   (on_parole) - The peer has participated in a piece that failed the hash check, "
               "and is now \"on parole\", which means we're only requesting whole pieces from this peer "
               "until it either fails that piece or proves that it doesn't send bad data.\n"
            "\tS   (seed) - This peer is a seed (it has all the pieces)\n"
            "\tuCh (optimistic_unchoke) - This peer is subject to an optimistic unchoke. "
               "It has been unchoked for a while to see if it might unchoke us in return "
               "an earn an upload/unchoke slot. If it doesn't within some period of time, "
               "it will be choked and another peer will be optimistically unchoked.\n"
            "\tE4  (rc4_encrypted)\n"
            "\tEp  (plaintext_encrypted)",
            
            // source [4]
            "Source - combination of flags describing from which sources this peer was received\n"
            "\tT (tracker) - The peer was received from the tracker\n"
            "\tD (dht) - The peer was received from the kademlia DHT\n"
            "\tP (pex) - The peer was received from the peer exchange extension\n"
            "\tL (lsd) - The peer was received from the local service discovery "
               "(The peer is on the local network)\n"
            "\tR (resume_data) - The peer was added from the fast resume data\n"
            "\tI (incoming)",
            
            // down_speed [5]
            "Down speed - the current download speed we have from this peer "
               "(including any protocol messages)",
            
            // up_speed [6]
            "Up speed - the current upload speed we have to this peer "
               "(including any protocol messages)",
            
            // payload_down_speed [7]
            "Payload down speed - the download rate of payload data only",
            
            // payload_up_speed [8]
            "Payload up speed - the upload rate of payload data only",
            
            // pieces [9]
            "Pieces - pieces in the whole torrent. Each pixel tells you "
               "if the peer has that piece (if it's set to green) "
               "or if the peer miss that piece (set to red)",
            
            // download_limit [10]
            "Download limit - the number of bytes per second this peer "
               "is allowed to receive (empty means unlimited)",
            
            // upload_limit [11]
            "Upload limit - the number of bytes per second we are allowed "
               "to send to this peer every second. It may be empty "
               "if there's no local limit on the peer. "
               "The global limit and the torrent limit is always enforced anyway.",
            
            // country [12]
            "Country - the two letter ISO 3166 country code for the country "
               "the peer is connected from. If the country hasn't been resolved yet, "
               "both chars are set to space. If the resolution failed for some reason, "
               "the field is set to \"--\". If the resolution service returns an invalid country code, "
               "it is set to \"!!\". The countries.nerd.dk service is used to look up countries.",
            
            // load_balancing [13]
            "Load balancing - a measurement of the balancing of free download (that we get) "
               "and free upload that we give. Every peer gets a certain amount of free upload, "
               "but this parameter says how much extra free upload this peer has got. "
               "If it is a negative number it means that this was a peer from which "
               "we have got this amount of free download.",
            
            // upload_queue_length [14]
            "Upload queue length - the number of piece-requests we have received from this peer "
               "that we haven't answered with a piece yet",
            
            // download_queue_length [15]
            "Download queue length - the number of piece-requests we have sent to this peer "
               "that hasn't been answered with a piece yet",
            
            // downloading_piece_index [16]
            "Downloading piece index - the index of the piece that is currently being downloaded. "
               "This may be set to empty if there's currently no piece downloading from this peer.",
            
            // downloading_block_index [17]
            "Downloading block index - the index of the block (or sub-piece) that is being downloaded",
            
            // downloading_progress/downloading_total [18]
            "Downloading block progress - the part of this block we have received from the peer / "
               "the total number of bytes in this block",
            
            // client [19]
            "Client - a string describing the software at the other end of the connection. "
               "In some cases this information is not available, then it will contain "
               "a string that may give away something about which software is running in the other end. "
               "In the case of a web seed, the server type and version will be a part of this string.",
            
            // connection_type [20]
            "Connection type:\n"
            "\tBT (standard_bittorrent)\n"
            "\tW  (web_seed)",
            
            // last_request [21]
            "Last request - the time since we last sent a request to this peer",
            
            // last_active [22]
            "Last active - the time since any transfer occurred with this peer",
            
            // num_hashfails [23]
            "Hash fails (num_hashfails) - the number of pieces this peer has participated "
               "in sending us that turned out to fail the hash check",
            
            // failcount [24]
            "Fail count (failcount) - the number of times this peer has \"failed\". "
               "i.e. failed to connect or disconnected us. The failcount is decremented "
               "when we see this peer in a tracker response or peer exchange message.",
            
            // target_dl_queue_length [25]
            "Target dl queue length - the number of requests that is tried to be maintained "
               "(this is typically a function of download speed)",
            
            // remote_dl_rate [26]
            "Remote dl rate - approximate peer download rate"
         };

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

            get_selection()->set_mode(Gtk::SELECTION_SINGLE);
            get_selection()->signal_changed().connect(sigc::mem_fun(*this, &peerTreeview::on_selection_changed));

#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 12)
            signal_query_tooltip().connect(sigc::mem_fun(*this, &peerTreeview::on_query_tooltip));
            set_has_tooltip();
#endif

            show();
         }
         /*set_tooltip_text(
            );*/

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
                  
                  // restore selection
                  if (row[peerrecord.ipv4] == selected_peer)
                     get_selection()->select(row);
                  
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
               refListStore->children()[_offset][peerrecord.downloading_progress] = pi->downloading_progress();
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

         peerTreeview::~peerTreeview()
         {
         }

         const char * peerTreeview::get_tooltip(unsigned _n)
         {
            if (_n >= sizeof(tooltips) / sizeof(tooltips[0]))
               return 0;
            return tooltips[_n];
         }

#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 12)
         bool peerTreeview::on_query_tooltip(int _x, int _y, bool _keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip> & _tooltip)
         {
            int tx, ty;
            convert_widget_to_tree_coords(_x, _y, tx, ty);

            Gtk::TreeModel::Path path;
            Gtk::TreeViewColumn * column;
            int cell_x, cell_y;
            if (get_path_at_pos(tx, ty, path, column, cell_x, cell_y))
            {
               // FIXME: very ugly, but I still don't know how to convert _x to the column number, anybody knows?
               for(size_t i=0; i < get_columns().size(); ++i)
               {
                  if (get_column(i) == column)
                  {
                     if (get_tooltip(i))
                     {
                        _tooltip->set_text(get_tooltip(i));
                        return true;
                     }
                     else
                     {
                        return false;
                     }
                  }
               }
            }
            return false;
         }
#endif
         
         void peerTreeview::on_selection_changed()
         {
            if (!get_selection()->count_selected_rows())
               return;
            selected_peer = get_selection()->get_selected()->get_value(peerrecord.ipv4);
         }
         
      } // namespace gui
   } // namespace UI
} // namespace btg

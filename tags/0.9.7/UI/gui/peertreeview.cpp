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

#include <string>

#include "peertreeview.h"

#include <bcore/util.h>
#include <bcore/hru.h>
#include <bcore/hrr.h>
#include <bcore/t_string.h>

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
            t_peerListCI piter;

            for (piter = _peerlist.begin();
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

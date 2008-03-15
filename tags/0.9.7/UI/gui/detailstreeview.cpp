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

#include "detailstreeview.h"

#include <bcore/util.h>
#include <bcore/hru.h>
#include <bcore/hrr.h>
#include <bcore/hrr.h>
#include <bcore/hrt.h>
#include <bcore/t_string.h>
#include <bcore/client/ratio.h>

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         using namespace btg::core;

         detailsTreeview::detailsTreeview()
         {
            set_flags(Gtk::CAN_FOCUS);
            set_headers_visible(false);
            set_rules_hint(true);
            set_reorderable(false);
            set_enable_search(false);

            // Create a store which consists of status records.
            refListStore = Gtk::ListStore::create(detailsrecord);
            set_model(refListStore);

            // Add the status record to this view.
            detailsrecord.appendToView(this);

            Glib::RefPtr<Gtk::TreeSelection> refSelection = get_selection();

            refSelection->signal_changed().connect(sigc::mem_fun(*this, &detailsTreeview::on_selection_changed));

            show();
         }

         void detailsTreeview::update(btg::core::Status const& _status)
         {
            clear();

            add("Filename:", _status.filename());

            std::string st_status;
            switch (_status.status())
               {
               case btg::core::Status::ts_undefined:
                  st_status = "Undefined";
                  break;
               case btg::core::Status::ts_queued:
                  st_status = "Waiting";
                  break;
               case btg::core::Status::ts_checking:
                  st_status = "Checking";
                  break;
               case btg::core::Status::ts_connecting:
                  st_status = "Connecting";
                  break;
               case btg::core::Status::ts_downloading:
                  st_status = "Downloading";
                  break;
               case btg::core::Status::ts_seeding:
                  st_status = "Seeding (download done)";
                  break;
               case btg::core::Status::ts_finished:
                  st_status = "Finished (download done)";
                  break;
               case btg::core::Status::ts_stopped:
                  st_status = "Stopped";
                  break;
               }

            add("Status:", st_status);

            if (_status.status() == btg::core::Status::ts_downloading)
               {
                  btg::core::humanReadableTime hrt = btg::core::humanReadableTime::convert(
                                                                                           60ull * _status.activityCounter()
                                                                                           );
                  std::string download_time = hrt.toString();

                  add("Download time:", download_time);
               }
            else if (_status.status() == btg::core::Status::ts_seeding)
               {
                  btg::core::humanReadableTime hrt = btg::core::humanReadableTime::convert(
                                                                                           60ull * _status.activityCounter()
                                                                                           );
                  std::string seed_time = hrt.toString();
                  
                  add("Seed time:", seed_time);
               }

            // Ratio:
            std::string st_ratio;
            btg::core::client::CalculateUlDlRatio(_status, st_ratio);
            add("Up/down ratio:", st_ratio);

            // Progress:
            if (_status.validTimeLeft())
               {
                  std::string st_progress;
                  
                  _status.timeLeftToString(st_progress);
                  add("Time left:", st_progress);
               }
            
            
            btg::core::humanReadableUnit hru = 
               btg::core::humanReadableUnit::convert(_status.downloadTotal());
            std::string st_tdl = hru.toString();
            add("Total download:", st_tdl);
            
            hru = btg::core::humanReadableUnit::convert(_status.uploadTotal());
            std::string st_tul = hru.toString();
            add("Total upload:", st_tul);
            
            btg::core::humanReadableRate hrr =
               btg::core::humanReadableRate::convert(static_cast<t_uint>(_status.downloadRate()));
            std::string st_dlr = hrr.toString();
            add("Download rate:", st_dlr);
            
            hrr = btg::core::humanReadableRate::convert(static_cast<t_uint>(_status.uploadRate()));
            std::string st_ulr = hrr.toString();
            add("Upload rate:", st_ulr);
            
            std::string st_done = btg::core::convertToString<t_ulong>(_status.done()) + " %";
            add("Done:", st_done);
            
            std::string st_l = btg::core::convertToString<t_int>(_status.leechers());
            add("Leeches:", st_l);
            
            std::string st_s = btg::core::convertToString<t_int>(_status.seeders());
            add("Seeds:", st_s);
         }

         void detailsTreeview::clear()
         {
            refListStore->clear();
         }

         void detailsTreeview::on_selection_changed()
         {
            // When something gets selected, unselect it again.
            // There is no reason to be able to select rows in this
            // table.
            Glib::RefPtr<Gtk::TreeSelection> refSelection = get_selection();
            refSelection->unselect_all();
         }

         void detailsTreeview::add(std::string const& _topic,
                                   std::string const& _text)
         {
            Gtk::TreeModel::iterator iter = refListStore->append();
            Gtk::TreeModel::Row       row = *iter;
            row[detailsrecord.text]       = _topic;

            iter                          = refListStore->append();
            row                           = *iter;
            row[detailsrecord.text]       = _text;
         }
         detailsTreeview::~detailsTreeview()
         {
         }

      } // namespace gui
   } // namespace UI
} // namespace btg

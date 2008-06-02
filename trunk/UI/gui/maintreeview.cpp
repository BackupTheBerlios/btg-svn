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

#include "maintreeview.h"

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

         typedef std::map<int, bool>::iterator elemMap_iter;

         mainTreeview::mainTreeview()
            : statusrecord(),
              refListStore(),
              refSelection(),
              selectedElements()
         {
            set_flags(Gtk::CAN_FOCUS);
            set_headers_visible(true);
            set_rules_hint(false);
            set_reorderable(false);
            set_enable_search(false);

            // Enable selecting multiple rows.
            get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);

            refSelection = get_selection();

            // Create a store which consists of status records.
            refListStore = Gtk::ListStore::create(statusrecord);
            set_model(refListStore);

            // Add the status record to this view.
            statusrecord.appendToView(this);

            this->clearSelection();

            refSelection->signal_changed().connect(sigc::mem_fun(*this, &mainTreeview::on_selection_changed));
         }

         void mainTreeview::on_selection_changed()
         {
            this->clearSelection();
            refSelection->selected_foreach_iter(sigc::mem_fun(*this, &mainTreeview::selected_row_callback));
         }

         void mainTreeview::selected_row_callback(const Gtk::TreeModel::iterator& iter)
         {
            Gtk::TreeModel::Row row = *iter;
            selectedElements[row[statusrecord.id]] = true;
         }

         void mainTreeview::clearSelection()
         {
            for (elemMap_iter i = selectedElements.begin(); i != selectedElements.end(); i++)
               {
                  i->second = false;
               }
         }

         void mainTreeview::updateStatus(int const _id, btg::core::Status const& _status,  bool const _updateProgress)
         {
            child_iter iter = this->find_id(_id);

            if (iter != refListStore->children().end())
               {
                  // Modify.
                  this->update(iter, _status, _updateProgress);
               }
            else
               {
                  // Add.
                  this->add(_id, _status);
               }
         }

         void mainTreeview::add(int const _id, btg::core::Status const& _status)
         {
            // Add a new row.
            Gtk::TreeModel::iterator iter = refListStore->append();
            Gtk::TreeModel::Row       row = *iter;
            row[statusrecord.id]          = _id;
            // Selection:
            selectedElements[_id]         = false;
            this->update(iter, _status, false /* Do not update progress. */);
         }

         void mainTreeview::update(child_iter & _iter, btg::core::Status const& _status, bool const _updateProgress)
         {
            Gtk::TreeModel::Row row = *_iter;

            std::string filename;

            if (Util::getFileFromPath(_status.filename(), filename))
               {
                  row[statusrecord.filename] = filename;
               }
            else
               {
                  row[statusrecord.filename] = _status.filename();
               }

            switch(_status.status())
               {
               case btg::core::Status::ts_undefined:
                  row[statusrecord.status]   = "Undefined";
                  break;
               case btg::core::Status::ts_queued:
                  row[statusrecord.status]   = "Queued";
                  break;
               case btg::core::Status::ts_checking:
                  row[statusrecord.status]   = "Checking file";
                  break;
               case btg::core::Status::ts_connecting:
                  row[statusrecord.status]   = "Connecting";
                  break;
               case btg::core::Status::ts_downloading:
                  row[statusrecord.status]   = "Downloading";
                  break;
               case btg::core::Status::ts_seeding:
                  row[statusrecord.status]   = "Seeding";
                  break;
               case btg::core::Status::ts_stopped:
                  row[statusrecord.status]   = "Stopped";
                  break;
               case btg::core::Status::ts_finished:
                  row[statusrecord.status]   = "Finished";
                  break;
               default:
                  row[statusrecord.status]   = "ERROR";
                  break;
               }

            row[statusrecord.dn_rate] = humanReadableRate::convert(_status.downloadRate()).toString();
            row[statusrecord.ul_rate] = humanReadableRate::convert(_status.uploadRate()).toString();

            if (_updateProgress)
               {
                  std::string progress;

                  std::string done_str = convertToString<t_int>(_status.done()) + " %";

                  if (_status.status() == btg::core::Status::ts_downloading)
                     {
                        if (_status.validTimeLeft())
                           {
                              std::string timespec;
                              _status.timeLeftToString(timespec);
                              progress = timespec + " (" + done_str + ")";
                           }
                        else
                           {
                              // Invalid progress, show percent.
                              progress = done_str;
                           }
                     } // downloading
                  else
                     {
                        // Not downloading.
                        progress = done_str;
                     }
                  row[statusrecord.done]     = _status.done();
                  row[statusrecord.doneText] = progress;
               } // Update progress

            // Convert the filesize to human readable form.
            t_ullong bytes = _status.filesize();
            hRU hru        = hRU::convert(bytes);
            row[statusrecord.filesize] = hru.toString();

            // Update the peer count.
            row[statusrecord.peers] = convertToString<t_int>(_status.leechers()) + "/" +
               convertToString<t_int>(_status.seeders());

         }

         child_iter mainTreeview::find_id(int const _id)
         {
            Gtk::TreeModel::Children children = refListStore->children();

            for(child_iter iter = children.begin(); iter != children.end(); ++iter)
               {
                  Gtk::TreeModel::Row row = *iter;

                  if (row[statusrecord.id] == _id)
                     {
                        return iter;
                     }
               }
            return children.end();
         }

         elemMap mainTreeview::getSelectedIds() const
         {
            elemMap em;
            for (elemMap_citer i = selectedElements.begin(); i != selectedElements.end(); i++)
               {
                  if (i->second)
                     {
                        em[i->first] = i->second;
                     }
               }

            return em;
         }

         bool mainTreeview::gotSelection() const
         {
            bool result = false;
            for (elemMap_citer i = selectedElements.begin(); i != selectedElements.end(); i++)
               {
                  if (i->second)
                     {
                        result = true;
                        break;
                     }
               }
            return result;
         }

         t_uint mainTreeview::numberOfSelected() const
         {
            t_uint n = 0;

            for (elemMap_citer i = selectedElements.begin(); i != selectedElements.end(); i++)
               {
                  if (i->second)
                     {
                        n++;
                     }
               }
            return n;
         }

         void mainTreeview::removeStatus(int const _id)
         {
            child_iter i = find_id(_id);

            if (i != refListStore->children().end())
               {
                  // remove.
                  this->remove(i);
               }

            // Make sure that selection also gets updated.
            // Just clear the whole selection for now.
            selectedElements.clear();
         }

         void mainTreeview::remove(child_iter & _iter)
         {
            refListStore->erase(_iter);
         }

         mainTreeview::~mainTreeview()
         {
         }

      } // namespace gui
   } // namespace UI
} // namespace btg

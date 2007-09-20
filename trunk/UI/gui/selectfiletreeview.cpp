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
 * $Id: selectfiletreeview.cpp,v 1.1.2.2 2007/06/21 21:53:23 wojci Exp $
 */

#include <string>

#include <bcore/util.h>
#include <bcore/hru.h>
#include <bcore/t_string.h>

#include "selectfiletreeview.h"

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         using namespace std;
         using namespace btg::core;

         typedef Gtk::TreeModel::Children::iterator  child_iter;
         typedef std::map<int, bool>                 elemMap;
         typedef std::map<int, bool>::iterator       elemMap_iter;
         typedef std::map<int, bool>::const_iterator elemMap_citer;

         selectFileTreeview::selectFileTreeview()
            : current_id(-1),
              filerecord(),
              refListStore()
         {
            set_flags(Gtk::CAN_FOCUS);
            set_headers_visible(true);
            set_rules_hint(false);
            set_reorderable(false);
            set_enable_search(false);

            // Create a store which consists of status records.
            refListStore = Gtk::ListStore::create(filerecord);
            set_model(refListStore);

            // Add the status record to this view.
            filerecord.appendToView(this);

            show();
         }

         void selectFileTreeview::update(int const _id, 
                                         btg::core::selectedFileEntryList const& _sf)
         {
            bool cleared = false;

            if (_id != current_id)
               {
                  current_id = _id;
                  cleared    = true;
                  this->clear();
               }

            std::vector<selectedFileEntry> files = _sf.files();

            std::vector<selectedFileEntry>::const_iterator sf_iter;

            if (cleared)
               {
                  // Only add.
                  for (sf_iter = files.begin(); 
                       sf_iter != files.end(); 
                       sf_iter++)
                     {
                        // Add a new row.
                        Gtk::TreeModel::iterator iter = refListStore->append();
                        Gtk::TreeModel::Row row       = *iter;
                  
                        row[filerecord.filename] = sf_iter->filename();
                        row[filerecord.selected] = sf_iter->selected();
                     }
               }
            else
               {
                  // Update.
                  sf_iter = files.begin();
                  Gtk::TreeModel::Children children = refListStore->children();

                  for(child_iter iter = children.begin(); 
                      iter != children.end(); 
                      ++iter)
                     {
                        Gtk::TreeModel::Row row  = *iter;

                        row[filerecord.filename] = sf_iter->filename();
                        row[filerecord.selected] = sf_iter->selected();

                        sf_iter++;
                     }
               }
         }

         void selectFileTreeview::clear()
         {
            refListStore->clear();
         }

         void selectFileTreeview::on_toggled(const Glib::ustring& _path)
         {
            Gtk::TreeIter iter;
            Gtk::TreePath tp(_path);
            
            // Get toggled iter.
            iter = refListStore->get_iter(tp);

            selectedItems si;

            si.toggle_value = refListStore->get_iter(tp)->get_value(filerecord.selected);

            if (si.toggle_value)
               {
                  si.toggle_value = false;
               }
            else
               {
                  si.toggle_value = true;
               }

            refListStore->get_iter(tp)->set_value(filerecord.selected, si.toggle_value);

            si.toggle_filename = refListStore->get_iter(tp)->get_value(filerecord.filename);

            items.push_back(si);
         }

         bool selectFileTreeview::getToogled(t_int & _context_id, 
                                             btg::core::selectedFileEntryList & _sfel)
         {
            bool status = false;

            if (items.size() > 0)
               {
                  status = true;
               }

            std::vector<selectedItems>::iterator iter;

            for (iter = items.begin();
                 iter != items.end();
                 iter++)
               {
                  if (_sfel.selected(iter->toggle_filename) && !iter->toggle_value)
                     {
                        _sfel.deSelect(iter->toggle_filename);
                     }
                  else if (!_sfel.selected(iter->toggle_filename) && iter->toggle_value)
                     {
                        _sfel.select(iter->toggle_filename);
                     }
               }

            items.clear();

            return status;
         }

         selectFileTreeview::~selectFileTreeview()
         {
         }

      } // namespace gui
   } // namespace UI
} // namespace btg


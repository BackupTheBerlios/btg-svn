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

#include <bcore/util.h>
#include <bcore/hru.h>
#include <bcore/t_string.h>

#include "mainfiletreeview.h"
#include "fipixbox.h"

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         using namespace btg::core;

         typedef Gtk::TreeModel::Children::iterator  child_iter;
         typedef std::map<int, bool>                 elemMap;
         typedef std::map<int, bool>::iterator       elemMap_iter;
         typedef std::map<int, bool>::const_iterator elemMap_citer;

         mainFileTreeview::mainFileTreeview()
            : current_id(-1),
              filerecord(),
              refListStore(),
              saved_size(0)
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

         void mainFileTreeview::update(int const _id, 
                                       t_fileInfoList const& _fileinfolist)
         {
            bool cleared = false;

            if (_id != current_id)
               {
                  current_id = _id;
                  cleared    = true;
                  this->clear();
               }

            if (saved_size != _fileinfolist.size())
               {
                  cleared    = true;
                  saved_size = _fileinfolist.size();
                  this->clear();
               }

            t_fileInfoListCI fi_iter;

            if (cleared)
               {
                  // Add, since contents were cleared.
                  for (fi_iter = _fileinfolist.begin(); 
                       fi_iter != _fileinfolist.end(); 
                       fi_iter++)
                     {
                        // Add a new row.
                        Gtk::TreeModel::iterator iter = refListStore->append();
                        Gtk::TreeModel::Row       row = *iter;

                        row[filerecord.filename]      = fi_iter->getFilename();

                        //fileInfoPixbox(*fi_iter, row[filerecord.filepieces]);
                        
                        // Create a pixbox showing a file's bits.
                        fileInfoPixbox fileinfopixbox(pixmap_bits_per_sample, 
                                                      pixmap_size_x, 
                                                      pixmap_size_y,
                                                      *fi_iter);
                        row[filerecord.filepieces] = fileinfopixbox.getPixBuf();

                        // Convert the filesize to human readable form.
                        t_ulong bytes            = fi_iter->getFileSize();
                        hRU hru                  = hRU::convert(bytes);
                        row[filerecord.filesize] = hru.toString();
                     }
               }
            else
               {
                  // Update.
                  fi_iter = _fileinfolist.begin();
                  Gtk::TreeModel::Children children = refListStore->children();

                  for(child_iter iter = children.begin(); 
                      iter != children.end(); 
                      ++iter)
                     {
                        Gtk::TreeModel::Row row  = *iter;

                        row[filerecord.filename] = fi_iter->getFilename();

                        // Create a pixbox showing a file's bits.
                        // Use an existing pixbuf to draw on, constructing draws.
                        fileInfoPixbox(*fi_iter, row[filerecord.filepieces]);

                        // Convert the filesize to human readable form.
                        t_ulong bytes            = fi_iter->getFileSize();
                        hRU hru                  = hRU::convert(bytes);
                        row[filerecord.filesize] = hru.toString();

                        fi_iter++;
                     }
               }
         }

         void mainFileTreeview::clear()
         {
            refListStore->clear();
         }

         mainFileTreeview::~mainFileTreeview()
         {
         }

      } // namespace gui
   } // namespace UI
} // namespace btg


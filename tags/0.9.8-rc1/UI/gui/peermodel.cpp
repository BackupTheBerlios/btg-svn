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
            : ipv4(),
              status(),
              identification()
         {
            add(ipv4);
            add(status);
            add(identification);
         }

         void peerRecord::appendToView(Gtk::TreeView* _treeview)
         {
            _treeview->append_column("Address", ipv4);
            _treeview->append_column("Status",  status);
            _treeview->append_column("Identification", identification);
            
            headersSetResizable(*_treeview);
         }

         peerRecord::~peerRecord()
         {

         }

      } // namespace gui
   } // namespace UI
} // namespace btg

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

#ifndef MAINMODEL_H
#define MAINMODEL_H

#include <gtkmm.h>
#include <bcore/type.h>
#include "gtkmm_ver.h"

#if GTKMM_2_6_OR_BETTER
#include <gtkmm/progressbar.h>
#endif // GTKMM_2_6_OR_BETTER

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

               class Gtk::TreeView;

               /// Record used in connection with mainTreeview
               /// (table). Represents a context.
               class statusRecord : public Gtk::TreeModel::ColumnRecord
                  {
                  public:
                     /// Constructor.
                     statusRecord();
                     /// Append data from an instance of this object to a view.
                     void appendToView(Gtk::TreeView *treeview);

                     // private:
                     /// Hidden: context ID.
                     Gtk::TreeModelColumn<t_int> id;

                     /// The filename of the context.
                     Gtk::TreeModelColumn<Glib::ustring> filename;

                     /// The file size of the context.
                     Gtk::TreeModelColumn<Glib::ustring> filesize;

                     /// Status of a the context.
                     Gtk::TreeModelColumn<Glib::ustring> status;

                     /// Percent done.
#if GTKMM_2_6_OR_BETTER
                     Gtk::TreeModelColumn<int> done;
                     Gtk::TreeModelColumn<Glib::ustring> doneText;
#else
                     Gtk::TreeModelColumn<Glib::ustring> done;
#endif // GTKMM_2_6_OR_BETTER

                     /// Download rate.
                     Gtk::TreeModelColumn<Glib::ustring> dn_rate;
                     /// Upload rate.
                     Gtk::TreeModelColumn<Glib::ustring> ul_rate;

                     /// Peers.
                     Gtk::TreeModelColumn<Glib::ustring> peers;

                     /// Destructor.
                     virtual ~statusRecord();
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif

/*
 * btg Copyright (C) 2008 Michael Wojciechowski.
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
 * $Id: aboutdialog.h 10 2007-09-22 14:18:40Z wojci $
 */

#ifndef URLDIALOG_H
#define URLDIALOG_H

#include <gtkmm/dialog.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/table.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>

#include <string>

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

         class urlDialog : public Gtk::Dialog
            {  
            public:
               urlDialog(std::string const& _url = "",
                         std::string const& _filename = "");

               bool GetResult(std::string & _url,
                              std::string & _filename);

               virtual ~urlDialog();

            protected:
               void file_changed();
               void url_changed();
               void check_contents();

               class Gtk::Label*  label1;
               class Gtk::Entry*  file_entry;
               class Gtk::Entry*  url_entry;
               class Gtk::Label*  fn_label;
               class Gtk::Label*  url_label;
               class Gtk::Table*  table1;
               class Gtk::VBox*   vbox1;
               class Gtk::Button* cancelbutton;
               class Gtk::Button* applybutton;

               bool url_filename_filled;
            };

         /** @} */

      } // namespace gui
   } // namespace UI
} // namespace btg

#endif // URLDIALOG_H


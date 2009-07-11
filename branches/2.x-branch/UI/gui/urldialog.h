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

#include <string>

namespace Gtk
{
   class Button;
   class Label;
   class Entry;
   class Table;
   class VBox;
}

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

         /// Dialog used to input URL and file name.
         class urlDialog : public Gtk::Dialog
            {  
            public:
               /// Constructor.
               urlDialog(std::string const& _url = "",
                         std::string const& _filename = "");

               /// Get the input.
               bool GetResult(std::string & _url,
                              std::string & _filename);

               /// Destructor.
               virtual ~urlDialog();

            protected:
               /// Called when the file input was changed.
               void file_changed();

               /// Called when the url input was changed.
               void url_changed();

               /// Called to check the contents of both inputs.
               void check_contents();

               /// Label.
               class Gtk::Label*  label1;
               /// File input.
               class Gtk::Entry*  file_entry;
               /// Url input.
               class Gtk::Entry*  url_entry;
               /// Label.
               class Gtk::Label*  fn_label;
               /// Label.
               class Gtk::Label*  url_label;
               /// Table used for layout.
               class Gtk::Table*  table1;
               /// Box used for layout.
               class Gtk::VBox*   vbox1;
               /// Button.
               class Gtk::Button* cancelbutton;
               /// Button.
               class Gtk::Button* okbutton;

               /// Indicates if both inputs contain text.
               bool url_filename_filled;

               /// Indicates if the user entered a filename used to
               /// download something into.
               bool filename_filled;
            };

         /** @} */

      } // namespace gui
   } // namespace UI
} // namespace btg

#endif // URLDIALOG_H


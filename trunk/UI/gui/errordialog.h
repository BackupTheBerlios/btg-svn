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
 * $Id: errordialog.h,v 1.1.2.3 2006/08/11 20:45:25 wojci Exp $
 */

#ifndef ERROR_DIALOG_H
#define ERROR_DIALOG_H

#include <gtkmm/dialog.h>

#include <bcore/project.h>
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

               /// Dialog used to display fatal errors.
               class errorDialog : public Gtk::Dialog
                  {
                  public:
                     /// Constructor.
                     errorDialog(std::string const& _messageToShow);

                     /// Construct an instance of this class, show the
                     /// dialog. After the user presses the single
                     /// button present, destroy the instance.
                     static void showAndDie(std::string const& _messageToShow);

                     /// Destructor.
                     virtual ~errorDialog();

                  private:
                     /// Callback: called when the button on this dialog is pressed.
                     void on_closebutton_clicked();
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif // ERROR_DIALOG_H

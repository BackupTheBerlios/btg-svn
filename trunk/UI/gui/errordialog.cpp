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

#include "errordialog.h"
#include <memory>

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         errorDialog::errorDialog(std::string const& _messageToShow)
            : Gtk::MessageDialog(_messageToShow, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE, true)
         {
            set_title("Error notice");
            run();
         }

         void errorDialog::showAndDie(std::string const& _messageToShow)
         {
            std::auto_ptr<errorDialog> ed(new errorDialog(_messageToShow));
         }

         errorDialog::~errorDialog()
         {
         }

         /** @} */

      } // namespace gui
   } // namespace UI
} // namespace btg

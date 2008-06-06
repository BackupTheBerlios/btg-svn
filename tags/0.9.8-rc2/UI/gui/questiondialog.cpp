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

#include "questiondialog.h"
#include <memory>

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         questionDialog::questionDialog(std::string const& _title, std::string const& _question)
            : Gtk::MessageDialog(_question, false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO, true)
         {
            set_title(_title);
            status_ = run() == Gtk::RESPONSE_YES;
         }

         questionDialog::questionDialog(Gtk::MessageType _mt,std::string const& _title, std::string const& _question)
            : Gtk::MessageDialog(_question, false, _mt, Gtk::BUTTONS_YES_NO, true)
         {
            set_title(_title);
            status_ = run() == Gtk::RESPONSE_YES;
         }

         bool questionDialog::status() const
         {
            return status_;
         }

         bool questionDialog::showAndDie(std::string const& _title, std::string const& _question)
         {
            std::auto_ptr<questionDialog> qd(new questionDialog(_title, _question));
            return qd->status();
         }

         questionDialog::~questionDialog()
         {
         }

      } // namespace gui
   } // namespace UI
} // namespace btg

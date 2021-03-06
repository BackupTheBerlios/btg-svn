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

#ifndef QUESTION_DIALOG_H
#define QUESTION_DIALOG_H

#include <gtkmm.h>
#include <string>

namespace btg
{
   namespace UI
      {
         namespace gui
            {

               /// Dialog showing a question and two buttons, yes and
               /// no.
               class questionDialog: public Gtk::MessageDialog
                  {
                  public:
                     /// Constructor.
                     /// @param [in] _title    The title.
                     /// @param [in] _question Question to ask.
                     questionDialog(std::string const& _title, std::string const& _question);

                     /// Constructor.
                     /// @param [in] _mt       Gtk::MessageType - dialog icon. (Gtk::MESSAGE_INFO, Gtk::MESSAGE_WARNING, Gtk::MESSAGE_QUESTION, Gtk::MESSAGE_ERROR, Gtk::MESSAGE_OTHER)
                     /// @param [in] _title    The title.
                     /// @param [in] _question Question to ask.
                     questionDialog(Gtk::MessageType _mt, std::string const& _title, std::string const& _question);
                     
                     /// Construct an instance of this class, show the
                     /// dialog. Wait for the user to press one of the
                     /// buttons.
                     /// @param [in] _title    The title.
                     /// @param [in] _question Question to ask.
                     /// @return True if the Yes-button was pressed. False otherwise.
                     static bool showAndDie(std::string const& _title, std::string const& _question);

                     /// Get the status.
                     /// @return True if the Yes-button was pressed. False otherwise.
                     bool status() const;

                     /// Destructor.
                     virtual ~questionDialog();
                  private:
                     /// True if the Yes-button was pressed. False
                     /// otherwise.
                     bool status_;
                  };

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif // QUESTION_DIALOG_H

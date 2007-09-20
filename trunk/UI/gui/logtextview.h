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
 * $Id: logtextview.h,v 1.8.4.5 2006/08/11 20:45:25 wojci Exp $
 */

#ifndef LOGTEXTVIEW_H
#define LOGTEXTVIEW_H

#include <string>
#include <gtkmm/textview.h>

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

               /// A text view used for logging.
               /// Editing the text is disabled.
               /// The view auto scrolls down when text is added.
               class logTextview : public Gtk::TextView
                  {
                  public:
                     /// Constructor.
                     logTextview();
                     /// Write a message to the log.
                     void writeLog(std::string const& _s);
                     /// Destructor.
                     virtual ~logTextview();
                  private:
                     /// Event.
                     bool on_textview_clicked(GdkEventButton* _event);
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif

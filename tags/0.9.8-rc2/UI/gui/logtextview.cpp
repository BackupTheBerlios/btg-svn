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

#include "logtextview.h"

#include <boost/date_time/posix_time/posix_time.hpp>

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         logTextview::logTextview()
         {
            Gtk::TextView *lt = this;

            lt->set_flags(Gtk::CAN_FOCUS);
            lt->set_editable(false);
            lt->set_cursor_visible(false);
            lt->set_pixels_above_lines(0);
            lt->set_pixels_below_lines(0);
            lt->set_pixels_inside_wrap(0);
            lt->set_left_margin(0);
            lt->set_right_margin(0);
            lt->set_indent(5);
            lt->set_wrap_mode(Gtk::WRAP_WORD);
            lt->set_justification(Gtk::JUSTIFY_LEFT);
            lt->show();

            writeLog("Log begins ..");
         }

         void logTextview::writeLog(std::string const& _s)
         {
            // Add a timestamp.
            boost::posix_time::ptime t(boost::posix_time::second_clock::local_time());
            std::string timestamp = to_simple_string(t);

            // Add the string.
            Glib::ustring us("[" + timestamp + "] " + _s + "\n");
            Gtk::TextIter iter = get_buffer()->end();
            get_buffer()->insert(iter, us);

            // Scroll to the end of the buffer.
            scroll_to_mark(get_buffer()->create_mark("end", get_buffer()->end()), 0.0);
         }

         bool logTextview::on_textview_clicked(GdkEventButton* _event)
         {
            return true;
         }

         logTextview::~logTextview()
         {

         }

      } // namespace gui
   } // namespace UI
} // namespace btg

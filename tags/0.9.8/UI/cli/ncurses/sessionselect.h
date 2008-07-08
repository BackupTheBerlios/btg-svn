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

#ifndef SESSIONSELECT_H
#define SESSIONSELECT_H

#include <bcore/project.h>
#include <bcore/type_btg.h>

extern "C"
{
#include <ncurses.h>
}

#include "window.h"

#include <string>
#include <vector>

namespace btg
{
   namespace UI
      {
         namespace cli
            {
               /**
                * \addtogroup ncli
                */
               /** @{ */

               /// Structure used for keeping a session id/session name pair in. 
               struct sessionIdAndName
               {
                  /// Session id.
                  t_long      id;
                  /// Session name.
                  std::string name; 
               };

               /// Window used to select sessions.
               class sessionSelect: public baseWindow, public dialog
                  {
                  public:
                     /// Constructor, view sessions.
                     sessionSelect(keyMapping const& _kmap, 
                                   windowSize const& _ws,
                                   t_longList const& _sessions,
                                   t_strList  const& _names,
                                   t_long const _current_session);

                     /// Show the list.
                     dialog::RESULT run();

                     /// Get the selected session.
                     /// Return false if no session has been selected, 
                     /// for example if the user terminated the dialog or 
                     /// it was resized.
                     bool getSession(t_long & _session);

                     /// Destructor.
                     virtual ~sessionSelect();
                  private:
                     /// Size of this window.
                     windowSize size_;

                     /// Session information used by this window.
                     std::vector<sessionIdAndName> sessions_;

                     /// Number of lines this window can contain.
                     t_int      numberOfLines_;

                     /// Start of the window used to shown a part of
                     /// the sessions, in case not all can be shown at
                     /// the same time.
                     t_int      positionWindowStart_;

                     /// End of the window used to shown a part of the
                     /// sessions, in case not all can be shown at the
                     /// same time.
                     t_int      positionWindowEnd_;

                     /// The current position.
                     t_int      currentPosition_;

                     /// The current ID.
                     t_int      currentId_;

                     /// Indicates that the user selected a session.
                     bool       pressed_select_key_;

                     /// Selected session id.
                     t_long     selected_session_;

                     bool init(windowSize const& _ws);

                     void resize(windowSize const& _ws);

                     bool destroy();

                     void refresh();

                     /// Draw the contained list.
                     void drawList();

                     /// Move down the list of sessions.
                     void moveDown();

                     /// Move up the list of sessions.
                     void moveUp();

                     /// Move to the beginning of the list of sessions.
                     void toStart();

                     /// Move to the end of the list of sessions.
                     void toEnd();

                     windowSize calculateDimenstions(windowSize const& _ws) const;

                     /// Show help for this window.
                     dialog::RESULT showHelp();

                     /// Handle selecting a session.
                     void handleSelect();
                  };

               /** @} */

            } // namespace cli
      } // namespace UI
} // namespace btg

#endif // SESSIONSELECT_H


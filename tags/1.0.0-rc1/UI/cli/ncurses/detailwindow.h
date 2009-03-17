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

#ifndef DETAILWINDOW_H
#define DETAILWINDOW_H

#include <string>
#include <vector>

#include <bcore/project.h>
#include <bcore/type_btg.h>

#include <bcore/status.h>

extern "C"
{
#include <ncurses.h>
}

#include "window.h"

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

               class mainWindow;

               /// Window showing details about a certain torrent.
               class detailWindow: public baseWindow
                  {
                  public:
                     /// Constructor.
                     detailWindow(keyMapping const& _kmap, mainWindow & _mainwindow);

                     void resize(windowSize const& _ws);

                     bool destroy();

                     void refresh();

                     /// Add a topic.
                     void addTopic(std::string const& _text);

                     /// Add a value.
                     void addValue(std::string const& _text);

                     windowSize calculateDimenstions(windowSize const& _ws) const;

                     /// Destructor.
                     virtual ~detailWindow();
                  private:
                     /// Reference to the main window, containing contexts.
                     mainWindow & mainwindow_;

                     /// Counter used as the Y-position while drawing
                     /// the contents of this window.
                     t_uint counter_;

                     /// Draw the contents of this window.
                     void draw();

                     /// Set the contents of this window to show this
                     /// text.
                     void setText(std::string const& _text);
                  };

	       /** @} */

            } // namespace cli
      } // namespace UI
} // namespace btg

#endif // DETAILWINDOW_H


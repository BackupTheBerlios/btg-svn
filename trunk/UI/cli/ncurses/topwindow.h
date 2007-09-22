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

#ifndef TOPWINDOW_H
#define TOPWINDOW_H

#include "window.h"

#include <string>

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

               /// Window showing application title and dnl/upl status
               /// at the top of the screen.
               class topWindow: public baseWindow
                  {
                  public:
                     /// Constructor.
                     topWindow(keyMapping const& _keymap);

                     void resize(windowSize const& _ws);

                     bool destroy();

                     void refresh();

                     /// Set the title.
                     void setTitle(std::string const _text);

                     /// Set dl/up status.
                     void setStatus(std::string const _text);

                     windowSize calculateDimenstions(windowSize const& _ws) const;

                     /// Destructor.
                     virtual ~topWindow();
                  private:
                     /// Draw the contents of this window.
                     void draw();

                     /// The title.
                     std::string title_;

                     /// Status.
                     std::string status_;
                  };

	       /** @} */

            } // namespace cli
      } // namespace UI
} // namespace btg

#endif // TOPWINDOW_H


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
 * $Id: statuswindow.h,v 1.1.2.7 2007/01/16 20:26:36 wojci Exp $
 */

#ifndef STATUSWINDOW_H
#define STATUSWINDOW_H

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

               /// Window showing status at the bottom of the physical
               /// screen.
               class statusWindow: public baseWindow
                  {
                  public:
                     /// Constructor.
                     statusWindow(keyMapping const& _kmap);

                     void resize(windowSize const& _ws);

                     bool destroy();

                     void refresh();

                     /// Show a message using default color.
                     void setStatus(std::string const _text);

                     /// Show an error message using another color.
                     void setError(std::string const _text);

                     windowSize calculateDimenstions(windowSize const& _ws) const;

                     /// Destructor.
                     virtual ~statusWindow();
                  private:
                     /// Draw the contents of this window.
                     void draw();

                     /// The status.
                     std::string        status_;

                     /// Color used for drawing the status.
                     Colors::COLORLABEL currentColor_;
                  };

	       /** @} */

            } // namespace cli
      } // namespace UI
} // namespace btg

#endif // STATUSWINDOW_H


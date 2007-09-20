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
 * $Id: helpwindow.h,v 1.1.2.7 2007/01/16 20:26:36 wojci Exp $
 */

#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <bcore/project.h>
#include <bcore/type_btg.h>

#include <string>
#include <vector>

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

               /// Windows showing help.
               class helpWindow: private baseWindow, public dialog
                  {
                  public:
                     /// Constructor.
                     /// @param [in] _kmap Keys to be used by this window.
                     /// @param [in] _text List of lines of text to be shown.
                     helpWindow(keyMapping const& _kmap,
                                std::vector<std::string> const& _text);

                     /// Show the help.
                     dialog::RESULT run();

                     /// Generate a help string for a certain key.
                     static bool generateHelpForKey(keyMapping const& _kmap,
                                                    keyMapping::KEYLABEL const _label,
                                                    std::string const& _explanation,
                                                    std::string & _output,
                                                    bool _short = true);

                     /// Destructor.
                     virtual ~helpWindow();
                  private:
                     /// The contained text.
                     std::vector<std::string> text_;

                     /// Given a width and height, find the max line
                     /// length that can be shown.
                     void findMaxLineLength(t_uint & _width, t_uint & _height) const;

                     void resize(windowSize const& _ws);

                     bool destroy();

                     void refresh();

                     /// Draw the contents of this window.
                     void draw();

                     windowSize calculateDimenstions(windowSize const& _ws) const;
                  };

	       /** @} */

            } // namespace cli
      } // namespace UI
} // namespace btg

#endif // HELPWINDOW_H


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

#ifndef PROGRESS_H
#define PROGRESS_H

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

               /// A window showing a progress bar.
               class progressWindow : public baseWindow
                  {

                  public:
                     /// Constructor.
                     progressWindow(windowSize const& _ws,
                                    keyMapping const& _kmap);

                     /// Update the progress bar contained in this window.
                     void updateProgress(t_uint _percent, 
                                         std::string const _text);

                     /// Destructor.
                     virtual ~progressWindow();
                  protected:
                     /// The size of the window.
                     windowSize size_;

                     /// The middle x position for this window.
                     t_int middle_x;

                     /// The middle y position for this window.
                     t_int middle_y;
                  protected:
                     /// Set the text to be used.
                     void setText(std::string const& _text);

                     /// Set the percent of the initialization progress.
                     void setPercent(t_uint const _percent);

                     void resize(windowSize const& _ws);

                     windowSize calculateDimenstions(windowSize const& _ws) const;

                     void refresh();

                  private:
                     /// Copy constructor.
                     progressWindow(progressWindow const& _pw);
                     /// Assignment operator.
                     progressWindow& operator=(progressWindow const& _iw);
                  };

               /** @} */

            } // namespace cli
      } // namespace UI
} // namespace btg

#endif // PROGRESS_H


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

#ifndef COLORS_H
#define COLORS_H

#include <string>
#include <vector>
#include "nconfig.h"

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

               /// Maps colors, so that the user can choose his/hers
               /// own keys instead of the default ones.
               class Colors
                  {
                  public:
                     /// Used labels.
                     enum COLORLABEL
                        {
                           C_UNDEF = 0,  //!< Undefined color.
                           C_NORMAL,     //!< The normal foreground/background color.
                           C_BORDER,     //!< Color used for window borders.
                           C_FILE,       //!< Color used for showing files.
                           C_DIRECTORY,  //!< Color used to show directories.
                           C_MARK,       //!< Color used for marked elements in a list or menu.
                           C_WARNING,    //!< Color used for warnings.
                           C_ERROR,      //!< Color used for errors.
                        };

                     /// Constructor.
                     Colors();

                     /// Initialize colors.
                     /// 
                     /// \note If the terminal in which this
                     /// application runs in does not support changing
                     /// colors, the default ones will be used.
                     bool init();

                     /// Set user defined colors.
                     void set(std::vector<colorDef> const& _colors);

                     /// Destructor.
                     ~Colors();
                  private:
                     /// List of user defined colors.
                     std::vector<colorDef> userColors;

                     /// Indicates that a color was set.
                     bool c_normalSet;

                     /// Indicates that a color was set.
                     bool c_borderSet;

                     /// Indicates that a color was set.
                     bool c_fileSet;

                     /// Indicates that a color was set.
                     bool c_directorySet;

                     /// Indicates that a color was set.
                     bool c_markSet;

                     /// Indicates that a color was set.
                     bool c_warningSet;

                     /// Indicates that a color was set.
                     bool c_errorSet;

                     /// Set the default colors.
                     void setDefaults();
                  };

	       /** @} */

            } // namespace cli
      } // namespace UI
} // namespace btg

#endif // COLORS_H


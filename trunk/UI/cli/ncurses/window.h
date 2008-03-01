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

#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <vector>

#include <bcore/project.h>
#include <bcore/type_btg.h>

#include <bcore/status.h>

#include "keys.h"

#include "colors.h"

extern "C"
{
#include <ncurses.h>
}

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

         /// Describes a window's dimensions.
         class windowSize
         {
         public:
            /// Offset, X.
            t_uint topX;
            /// Offset, Y.
            t_uint topY;
            /// The width.
            t_uint width;
            /// The height.
            t_uint height;
         };

         /// Base class used for creating ncurses windows.
         class baseWindow
         {
         public:
            /// Constructor.
            baseWindow(keyMapping const& _kmap);

            /// Initialize the window.
            virtual bool init(windowSize const& _ws);

            /// Resize the window.
            virtual void resize(windowSize const& _ws) = 0;

            /// Given the possible width and height, 
            /// get the window's dimensions.
            virtual windowSize calculateDimenstions(windowSize const& _ws) const = 0;
            /// Destroy the window.
            virtual bool destroy();

            /// Refresh the window. Whatever is done to draw
            /// the contents, it should be called from here.
            virtual void refresh() = 0;

            /// Clear the contents of the window.
            virtual void clear();

            /// Get the physical screen resolution (in
            /// characters).
            static void getScreenSize(t_uint & _width,
                                      t_uint & _height);

            /// Init ncurses.
            static void ncursesInit();

            /// Deinit ncurses.
            static void ncursesDestroy();

            /// Get the window resolution (in characters).
            void getSize(windowSize & _ws);

            /// Read from keyboard.
            /// Uses the stored key map.
            virtual keyMapping::KEYLABEL handleKeyboard();

            /// As above, but use the argument instead of calling wgetch.
            /// @return True - found a label for the key. False otherwise.
            virtual bool handleKeyboard(t_int const _key, 
                                        keyMapping::KEYLABEL & _label) const;

            /// Return true, when the user presses any key.
            virtual t_int readAnyKey();

            /// Set the current color.
            void setColor(Colors::COLORLABEL _label);

            /// Disable the use of a certain color.
            void unSetColor(Colors::COLORLABEL _label);

            keyMapping getKeyMap() const;

            /// Destructor.
            virtual ~baseWindow();
         protected:
            /// Pointer to the ncurses window.
            WINDOW* window_;

            /// Top X position.
            t_uint  topX_;

            /// Top Y position.
            t_uint  topY_;

            /// The width of the window.
            t_uint  width_;

            /// The height of the window.
            t_uint  height_;

            /// Keys used by this window.
            keyMapping kmap_;

            /// Given a sting of certain length, generate
            /// spaces so that the string + spaces will fill
            /// width_.
            void genSpaces(std::string const& _input, std::string & _spaces) const;
         };

         /// Interface for dialogs.
         class dialog
         {
         public:
            /// Result of running a dialog.
            enum RESULT
            {
               R_NCREAT = -2, // Unable to create a dialog.
               R_RESIZE = -1, // Resized, take action.
               R_QUIT   =  0  // Normal quit.
            };
         public:
            /// Constructor.
            dialog();

            /// Function used to initialize and show the
            /// dialog.
            virtual dialog::RESULT run() = 0;

            /// Destructor.
            virtual ~dialog();
         };

         /** @} */

      } // namespace cli
   } // namespace UI
} // namespace btg

#endif // WINDOW_H


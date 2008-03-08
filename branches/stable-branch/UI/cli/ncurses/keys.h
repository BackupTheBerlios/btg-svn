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

#ifndef KEYS_H
#define KEYS_H

#include <string>
#include <map>
#include <vector>

#include <bcore/project.h>
#include <bcore/type_btg.h>
#include <bcore/t_string.h>

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

         /// Maps keys, so that the user can choose his/hers own
         /// keys instead of the default ones.
         class keyMapping
         {
         public:
            /// Used labels.
            enum KEYLABEL
            {
               K_UNDEF = 0,  //!< Other key or no keypress.
               // Acessible from main window:
               K_HELP,       //!< Get help.
               K_DETACH,     //!< Detach from the daemon.
               K_QUIT,       //!< Quit the application.
               K_QUITSCREEN, //!< Quit an entry display.
               K_LOAD,       //!< Load a torrent.
               K_MENU,       //!< Show a menu.
               K_GLIMIT,     //!< Show a dialog allowing one to set global limits.
               K_SESNAME,    //!< Name the current session.
               // Generic:
               K_DOWN,       //!< Move down the list.
               K_UP,         //!< Move up the list.
               K_LIST_START, //!< Start of the list.
               K_LIST_END,   //!< End of the list.
               K_SELECT,     //!< Select something.
               K_MARK,       //!< Mark something.
               K_MARK_ALL,   //!< Mark everything.
               K_NEXT,       //!< Next entry.
               K_PREV,       //!< Previous entry.
               K_DELETE,     //!< Delete.
               K_INPUT,      //!< Input using the keyboard.
               // Other:
               K_RESIZE      //!< Display was resized.
            };

            /// Constructor.
            keyMapping();

            /// Set the default key mapping.
            void setDefaults();

            /// Map a label to a value.
            void add(KEYLABEL const _keyl, int const _value);

            /// Get a textual representation of a key.
            bool getValue(KEYLABEL const _keyl, std::string & _value) const;

            /// Given a list of labels, remove the labels
            /// that are not contained in this list from
            /// keys.
            void setUsed(std::vector<KEYLABEL> const& _labels);

            /// Given a value, get a key.
            bool get(t_int const _value, KEYLABEL & _keyl) const;

            /// @return True - key label is defined. False - otherwise.
            bool exists(KEYLABEL const _keyl) const;

            /// Check that the contained keys are valid.
            /// Each ascii value must be used exacly once.
            /// @return True - good config, false otherwise.
            bool check(std::string & _error) const;

            /// Destructor.
            ~keyMapping();
         private:
            /// Maps label to key value.
            std::map<KEYLABEL, int> keys;
         };

      } // namespace cli
   } // namespace UI
} // namespace btg

#endif // KEYS_H


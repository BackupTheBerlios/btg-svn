/*
 * btg Copyright (C) 2009 Michael Wojciechowski.
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
 * $Id: mainwindow.h 516 2009-01-11 19:42:53Z wojci $
 */

#ifndef STATUS_ENTRY_H
#define STATUS_ENTRY_H

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

         /// Status and flag for marking torrents.
         class statusEntry
         {
         public:
            /// Constructor used for adding new torrent. 
            /// Sets the updated flag.
            statusEntry(btg::core::Status const& _s);

            /// Copy constructor.
            statusEntry(statusEntry const& _s);

            /// Default constructor.
            statusEntry();
         public:
            /// The context id.
            t_int             id;
            /// The status.
            btg::core::Status status;

            /// Indicates if the list of trackers was received.
            bool              trackers_set;

            /// List of trackers.
            t_strList         trackers;

            /// Flag: this torrent is marked.
            bool              marked;

            /// Flag: updated.
            bool              updated;
         };

         /** @} */

      } // namespace cli
   } // namespace UI
} // namespace btg

#endif // STATUS_ENTRY_H


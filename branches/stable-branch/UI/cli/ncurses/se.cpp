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
 * $Id$
 */

#include "se.h"

namespace btg
{
   namespace UI
   {
      namespace cli
      {
         statusEntry::statusEntry(btg::core::Status const& _s)
            : id(_s.contextID()),
              status(_s),
              marked(false),
              updated(true),
              trackers_set(false),
              trackers()
         {

         }

         statusEntry::statusEntry(statusEntry const& _s)
            : id(_s.id),
              status(_s.status),
              marked(_s.marked),
              updated(_s.updated),
              trackers_set(_s.trackers_set),
              trackers(_s.trackers)
         {
         }

         statusEntry::statusEntry()
            : id(-1),
              status(),
              marked(false),
              updated(false),
              trackers_set(false),
              trackers()
         {
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

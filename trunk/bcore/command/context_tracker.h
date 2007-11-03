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

#ifndef COMMAND_CONTEXT_TRACKER_H
#define COMMAND_CONTEXT_TRACKER_H

#include "context.h"
#include <bcore/dbuf.h>

#include <vector>
#include <string>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup commands
          */
         /** @{ */

         /// Get tracker for a context.
         class contextGetTrackersCommand: public contextCommand
            {
            public:
               /// Default constructor.
               contextGetTrackersCommand();

               /// Constructor.
               /// @param [in] _context_id     The context ID.
               /// @param [in] _allContexts    If true, this command is valid for all contexts.
               contextGetTrackersCommand(t_int const _context_id,
                                         bool const _allContexts = false);
               /// Destructor.
               virtual ~contextGetTrackersCommand();
            };

         /// List of trackers which belong to a certain context.
         class contextGetTrackersResponseCommand: public contextCommand
            {
            public:
               /// Default constructor.
               contextGetTrackersResponseCommand();

               /// Constructor.
               /// @param [in] _context_id   Context ID.
               /// @param [in] _trackers     List of trackers.
               contextGetTrackersResponseCommand(t_int const _context_id,
                                                 t_strList const& _trackers);

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Get the contained list of trackers.
               t_strList getTrackers() const;

               /// Destructor.
               virtual ~contextGetTrackersResponseCommand();
            private:
               /// List of trackers.
               t_strList trackers;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif // COMMAND_CONTEXT_TRACKER_H


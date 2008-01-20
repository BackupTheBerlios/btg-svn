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

#include "context_tracker.h"
#include <bcore/helpermacro.h>

namespace btg
{
   namespace core
   {
      contextGetTrackersCommand::contextGetTrackersCommand()
         : contextCommand(Command::CN_CGETTRACKERS, contextCommand::UNDEFINED_CONTEXT, false)
      {}

      contextGetTrackersCommand::contextGetTrackersCommand(t_int const _context_id,
                                                           bool const _allContexts)
         : contextCommand(Command::CN_CGETTRACKERS, _context_id, _allContexts)
      {}
      

      contextGetTrackersCommand::~contextGetTrackersCommand()
      {}

      /* */
      /* */
      /* */

      contextGetTrackersResponseCommand::contextGetTrackersResponseCommand()
         : contextCommand(Command::CN_CGETTRACKERSRSP, contextCommand::UNDEFINED_CONTEXT, false),
           trackers()
      {}

      contextGetTrackersResponseCommand::contextGetTrackersResponseCommand(t_int const _context_id,
                                                                           t_strList const& _trackers)
         : contextCommand(Command::CN_CGETTRACKERSRSP, _context_id, false),
           trackers(_trackers)
      {}

      bool contextGetTrackersResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         contextCommand::serialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("list of trackers", true);
         _e->stringListToBytes(&trackers);
         BTG_RCHECK(_e->status());

         return true;
      }

      bool contextGetTrackersResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         contextCommand::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("list of trackers", true);
         _e->bytesToStringList(&trackers);
         BTG_RCHECK(_e->status());

         return true;
      }


      t_strList contextGetTrackersResponseCommand::getTrackers() const
      {
         return trackers;
      }

      contextGetTrackersResponseCommand::~contextGetTrackersResponseCommand()
      {}

   } // namespace core
} // namespace btg

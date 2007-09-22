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

#include "context_status_rsp.h"
#include <bcore/helpermacro.h>

#include <iostream>

namespace btg
{
   namespace core
   {

      contextStatusResponseCommand::contextStatusResponseCommand()
         : contextCommand(Command::CN_CSTATUSRSP, contextCommand::UNDEFINED_CONTEXT),
           status()
      {
         // status is left undefined.
      }

      contextStatusResponseCommand::contextStatusResponseCommand(t_int const _context_id,
                                                                 Status const& _status)
         : contextCommand(Command::CN_CSTATUSRSP, _context_id), 
           status(_status)
      {
      }

      bool contextStatusResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( contextCommand::serialize(_e) );

         _e->setParamInfo("status", true);
         BTG_RCHECK( status.serialize(_e) );

         return true;
      }

      bool contextStatusResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( contextCommand::deserialize(_e) );

         _e->setParamInfo("status", true);
         BTG_RCHECK( status.deserialize(_e) );

         return true;
      }

      Status contextStatusResponseCommand::getStatus() const
      {
         return status;
      }

      contextStatusResponseCommand::~contextStatusResponseCommand()
      {

      }

      /* *************************************************************** */
      /* *************************************************************** */
      /* *************************************************************** */
      /* *************************************************************** */
      /* *************************************************************** */
      /* *************************************************************** */
      /* *************************************************************** */

      contextAllStatusResponseCommand::contextAllStatusResponseCommand()
         : contextCommand(Command::CN_CALLSTATUSRSP, contextCommand::UNDEFINED_CONTEXT),
           status(0)
      {
         // status is left undefined.
      }

      contextAllStatusResponseCommand::contextAllStatusResponseCommand(t_int const _context_id,
                                                                       t_statusList const& _status)
         : contextCommand(Command::CN_CALLSTATUSRSP, _context_id), status(_status)
      {}

      bool contextAllStatusResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( contextCommand::serialize(_e) );

         // Serialize the list.

         // First write the size of the list.
         t_int list_size = status.size();
         _e->setParamInfo("size of the list of status", true);
         BTG_RCHECK( _e->intToBytes(&list_size) );

         _e->setParamInfo("list of status", true);
         // Then serialize the contents of the list.
         t_statusListCI csci;
         for (csci=status.begin(); csci != status.end(); csci++)
            {
               BTG_RCHECK( csci->serialize(_e) );
            }

         return true;
      }

      bool contextAllStatusResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( contextCommand::deserialize(_e) );

         // The size of the list.
         t_int list_size = 0;
         _e->setParamInfo("size of the list of status", true);
         BTG_RCHECK( _e->bytesToInt(&list_size) );

         _e->setParamInfo("list of status", true);
         // The contents.
         for (t_int list_counter = 0; list_counter < list_size; list_counter++)
            {
               Status s;
               BTG_RCHECK( s.deserialize(_e) );
               status.push_back(s);
            }

         return true;
      }

      t_statusList contextAllStatusResponseCommand::getStatus() const
      {
         return status;
      }

      contextAllStatusResponseCommand::~contextAllStatusResponseCommand()
      {

      }

   } // namespace core
} // namespace btg

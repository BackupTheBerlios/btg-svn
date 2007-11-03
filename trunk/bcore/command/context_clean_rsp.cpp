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

#include "context_clean_rsp.h"
#include <bcore/helpermacro.h>
#include <iostream>

namespace btg
{
   namespace core
   {

      using namespace std;

      contextCleanResponseCommand::contextCleanResponseCommand()
         : Command(Command::CN_CCLEANRSP),
           filenames(),
           contextIDs()
      {}

      contextCleanResponseCommand::contextCleanResponseCommand(t_strList const& _filenames, 
                                                               t_intList const& _contextIDs)
         : Command(Command::CN_CCLEANRSP),
           filenames(_filenames),
           contextIDs(_contextIDs)
      {}

      bool contextCleanResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         Command::serialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("list of context IDs", true);
         _e->intListToBytes(&contextIDs);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("filename", true);
         _e->stringListToBytes(&filenames);
         BTG_RCHECK(_e->status());

         return true;
      }

      bool contextCleanResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         Command::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("list of context IDs", true);
         _e->bytesToIntList(&contextIDs);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("filename", true);
         _e->bytesToStringList(&filenames);
         BTG_RCHECK(_e->status());

         return true;
      }


      t_strList contextCleanResponseCommand::getFilenames() const
      {
         return filenames;
      }

      t_intList contextCleanResponseCommand::getContextIDs() const
      {
         return contextIDs;
      }

      contextCleanResponseCommand::~contextCleanResponseCommand()
      {}

   } // namespace core
} // namespace btg

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

#include "context_create.h"
#include <bcore/helpermacro.h>
#include <iostream>

namespace btg
{
   namespace core
   {
      contextCreateWithDataCommand::contextCreateWithDataCommand()
         : Command(Command::CN_CCREATEWITHDATA),
           filename(),
           torrent_file(),
           start(false)
      {}

      contextCreateWithDataCommand::contextCreateWithDataCommand(std::string const& _filename,
                                                                 sBuffer const& _torrent_file,
                                                                 bool const _start)
         : Command(Command::CN_CCREATEWITHDATA),
           filename(_filename),
           torrent_file(_torrent_file),
           start(_start)
      {}

      std::string contextCreateWithDataCommand::toString() const
      {
         std::string output = Command::toString() + " " + "Filename = " + filename + ".";
         if (start)
            {
               output += " Start flag set.";
            }

         return output;
      }

      bool contextCreateWithDataCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         Command::serialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("torrent filename", true);
         _e->stringToBytes(&filename);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("torrent file data", true);
         BTG_RCHECK( torrent_file.serialize(_e) );

         _e->setParamInfo("flag: start torrent", true);
         BTG_RCHECK( _e->boolToBytes(start) );

         return true;
      }

      bool contextCreateWithDataCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         Command::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("torrent filename", true);
         _e->bytesToString(&filename);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("torrent file data", true);
         BTG_RCHECK( torrent_file.deserialize(_e) );

         _e->setParamInfo("flag: start torrent", true);
         BTG_RCHECK( _e->bytesToBool(start) );

         return true;
      }

      contextCreateWithDataCommand::~contextCreateWithDataCommand()
      {}

   } // namespace core
} // namespace btg

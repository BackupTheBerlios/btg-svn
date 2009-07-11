/*
 * btg Copyright (C) 2008 Michael Wojciechowski.
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

#include "context_create_url.h"
#include <bcore/helpermacro.h>
#include <bcore/opstatus.h>
#include <iostream>

namespace btg
{
   namespace core
   {
      contextCreateFromUrlCommand::contextCreateFromUrlCommand()
         : Command(Command::CN_CCREATEFROMURL),
           filename(),
           url(),
           start(false)
      {}

      contextCreateFromUrlCommand::contextCreateFromUrlCommand(std::string const& _filename,
                                                               std::string const& _url,
                                                               bool const _start)
         : Command(Command::CN_CCREATEFROMURL),
           filename(_filename),
           url(_url),
           start(_start)
      {}

      std::string contextCreateFromUrlCommand::toString() const
      {
         std::string output = Command::toString() + " " + "Filename = " + filename + ".";
         if (start)
            {
               output += " Start flag set.";
            }

         return output;
      }

      bool contextCreateFromUrlCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         Command::serialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("torrent filename", true);
         _e->stringToBytes(&filename);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("torrent url", true);
         _e->stringToBytes(&url);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("flag: start torrent", true);
         BTG_RCHECK( _e->boolToBytes(start) );

         return true;
      }

      bool contextCreateFromUrlCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         Command::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("torrent filename", true);
         _e->bytesToString(&filename);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("torrent url", true);
         _e->bytesToString(&url);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("flag: start torrent", true);
         BTG_RCHECK( _e->bytesToBool(start) );

         return true;
      }

      contextCreateFromUrlCommand::~contextCreateFromUrlCommand()
      {}

      /* */
      
      contextCreateFromUrlResponseCommand::contextCreateFromUrlResponseCommand()
         : Command(Command::CN_CCREATEFROMURLRSP),
           id_(btg::core::OPSTAT_INVALID_ID)
      {}

      contextCreateFromUrlResponseCommand::contextCreateFromUrlResponseCommand(t_uint const _id)
         : Command(Command::CN_CCREATEFROMURLRSP),
           id_(_id)
      {}

      bool contextCreateFromUrlResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         Command::serialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("url id", true);
         _e->uintToBytes(&id_);
         BTG_RCHECK(_e->status());

         return true;
      }

      bool contextCreateFromUrlResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         Command::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("url id", true);
         _e->bytesToUint(&id_);
         BTG_RCHECK(_e->status());

         return true;
      }

      contextCreateFromUrlResponseCommand::~contextCreateFromUrlResponseCommand()
      {}

   } // namespace core
} // namespace btg

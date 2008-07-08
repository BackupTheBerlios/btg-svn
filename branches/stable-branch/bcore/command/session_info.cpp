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
 * Id    : $Id$
 */

#include "session_info.h"
#include <bcore/helpermacro.h>

namespace btg
{
   namespace core
   {
      sessionInfoCommand::sessionInfoCommand()
      : Command(Command::CN_SINFO)
      {

      }

      bool sessionInfoCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         return true;
      }

      bool sessionInfoCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         return true;
      }

      sessionInfoCommand::~sessionInfoCommand()
      {
         
      }

      sessionInfoResponseCommand::sessionInfoResponseCommand()
         : Command(Command::CN_SINFORSP),
           encryption_(false),
           dht_(false)
      {

      }

      sessionInfoResponseCommand::sessionInfoResponseCommand(bool _encryption, bool _dht)
         : Command(Command::CN_SINFORSP),
           encryption_(_encryption),
           dht_(_dht)
      {

      }

      std::string sessionInfoResponseCommand::toString() const
      {
         std::string output = Command::toString() + " ";

         if (encryption_)
            {
               output += "Encryption on";
            }
         else
            {
               output += "Encryption off";
            }

         if (dht_)
            {
               output += ", DHT on";
            }
         else
            {
               output += ", DHT off";
            }

         return output;
      }

      bool sessionInfoResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         _e->setParamInfo("Encryption.", true);
         BTG_RCHECK( _e->boolToBytes(encryption_) );

         _e->setParamInfo("DHT.", true);
         BTG_RCHECK( _e->boolToBytes(dht_) );

         return true;
      }

      bool sessionInfoResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         _e->setParamInfo("Encryption.", true);
         BTG_RCHECK( _e->bytesToBool(encryption_) );

         _e->setParamInfo("DHT.", true);
         BTG_RCHECK( _e->bytesToBool(dht_) );
         
         return true;
      }

      bool sessionInfoResponseCommand::encryption() const
      {
         return encryption_;
      }

      bool sessionInfoResponseCommand::dht() const
      {
         return dht_;
      }

      sessionInfoResponseCommand::~sessionInfoResponseCommand()
      {

      }

   } // namespace core
} // namespace btg


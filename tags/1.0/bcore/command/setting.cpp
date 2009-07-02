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

#include "setting.h"

#include <bcore/helpermacro.h>

namespace btg
{
   namespace core
   {
      t_byte SettingToByte(daemonSetting const _s);
      daemonSetting SettingToByte(t_byte const _b);

      /* */

      settingCommand::settingCommand()
         : Command(Command::CN_GETSETTING)
      {
      }

      settingCommand::settingCommand(daemonSetting const _s)
         : Command(Command::CN_GETSETTING),
           setting_(_s)
      {
      }

      bool settingCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         if (setting_ == SG_UNDEF)
            {
               BTG_RCHECK(false);
            }

         t_byte value = SettingToByte(setting_);

         _e->setParamInfo("Setting ID", true);
         _e->addByte(value);
         BTG_RCHECK(_e->status());

         return true;
      }

      bool settingCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         t_byte value = 0;

         _e->setParamInfo("Setting ID", true);
         _e->getByte(value);
         BTG_RCHECK(_e->status());

         setting_ = SettingToByte(value);

         if (setting_ == SG_UNDEF)
            {
               BTG_RCHECK(false);
            }

         return true;
      }

      daemonSetting settingCommand::getSetting() const
      {
         return setting_;
      }

      settingCommand::~settingCommand()
      {}

      /*
       */

      settingResponseCommand::settingResponseCommand()
         : Command(Command::CN_GETSETTINGRSP)
      {
      }

      settingResponseCommand::settingResponseCommand(daemonSetting const _s,
                                                     std::string const& _response)
         : Command(Command::CN_GETSETTINGRSP),
           setting_(_s),
           response_(_response)
      {
      }

      bool settingResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         if (setting_ == SG_UNDEF)
            {
               BTG_RCHECK(false);
            }

         t_byte value = SettingToByte(setting_);
         _e->setParamInfo("Setting ID", true);
         _e->addByte(value);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("Setting value", true);
         _e->stringToBytes(&response_);
         BTG_RCHECK(_e->status());

         return true;
      }

      bool settingResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         t_byte value = 0;

         _e->setParamInfo("Setting ID", true);
         _e->getByte(value);
         BTG_RCHECK(_e->status());

         setting_ = SettingToByte(value);

         if (setting_ == SG_UNDEF)
            {
               BTG_RCHECK(false);
            }

         _e->setParamInfo("Setting value", true);
         _e->bytesToString(&response_);
         BTG_RCHECK(_e->status());

         return true;
      }

      daemonSetting settingResponseCommand::getSetting() const
      {
         return setting_;
      }

      std::string settingResponseCommand::getResponse() const
      {
         return response_;
      }

      settingResponseCommand::~settingResponseCommand()
      {

      }
      
      /* */

      t_byte SettingToByte(daemonSetting const _s)
      {
         t_byte value = 0;
         switch (_s)
            {
            case SG_TRANSPORT:
               value = SG_TRANSPORT;
               break;
            case SG_PORT:
               value = SG_PORT;
               break;
            case SG_PEERID:
               value = SG_PEERID;
               break;
            default:
               value = SG_UNDEF;
               break;
            }
         return value;
      }

      daemonSetting SettingToByte(t_byte const _b)
      {
         daemonSetting s;
         switch (_b)
            {
            case SG_TRANSPORT:
               s = SG_TRANSPORT;
               break;
            case SG_PORT:
               s = SG_PORT;
               break;
            case SG_PEERID:
               s = SG_PEERID;
               break;
            default:
               s = SG_UNDEF;
               break;
            }
         return s;
      }

   } // namespace core
} // namespace btg

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

#include "version.h"

#include <bcore/helpermacro.h>
#include <bcore/command/limit_base.h>

#define set_bit(_src, _pos) { _src |= (1 << _pos);  }

namespace btg
{
   namespace core
   {
      versionCommand::versionCommand()
         : Command(Command::CN_VERSION)
      {
      }

      bool versionCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         return true;
      }

      bool versionCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         return true;
      }

      versionCommand::~versionCommand()
      {}

      /*
       */

      versionResponseCommand::versionResponseCommand()
         : Command(Command::CN_VERSIONRSP),
           ver_mayor(0),
           ver_minor(0),
           ver_revision(0)
      {
         bytes[0] = 0;
         bytes[1] = 0;
         bytes[2] = 0;
         bytes[3] = 0;
      }

      versionResponseCommand::versionResponseCommand(t_byte const _mayor, 
                                                     t_byte const _minor, 
                                                     t_byte const _revision)
         : Command(Command::CN_VERSIONRSP),
           ver_mayor(_mayor),
           ver_minor(_minor),
           ver_revision(_revision)
      {
         bytes[0] = 0;
         bytes[1] = 0;
         bytes[2] = 0;
         bytes[3] = 0;
      }

      bool versionResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         _e->setParamInfo("mayor version", true);
         _e->addByte(ver_mayor);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("minor version", true);
         _e->addByte(ver_minor);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("revision", true);
         _e->addByte(ver_revision);
         BTG_RCHECK(_e->status());

         for (t_uint i = 0; i < 4; i++)
            {
               _e->setParamInfo("option byte", true);
               _e->addByte(bytes[i]);
               BTG_RCHECK(_e->status());
            }

         return true;
      }

      bool versionResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         _e->setParamInfo("mayor version", true);
         _e->getByte(ver_mayor);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("minor version", true);
         _e->getByte(ver_minor);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("revision", true);
         _e->getByte(ver_revision);
         BTG_RCHECK(_e->status());

         for (t_uint i = 0; i < 4; i++)
            {
               _e->setParamInfo("option byte", true);
               _e->getByte(bytes[i]);
               BTG_RCHECK(_e->status());
            }

         return true;
      }

      void versionResponseCommand::getVersion(t_byte & _mayor,
                                              t_byte & _minor,
                                              t_byte & _revision) const
      {
         _mayor    = ver_mayor;
         _minor    = ver_minor;
         _revision = ver_revision;
      }

      void versionResponseCommand::setOption(versionResponseCommand::Option const _option)
      {
         switch (_option)
            {
            case SS:
            case PERIODIC_SS:
            case UPNP:
            case DHT:
            case ENCRYPTION:
            case OPTION_5:
            case OPTION_6:
            case OPTION_7:
               {
                  t_int n = _option;
                  set_bit(bytes[0], n);
                  break;
               }
            case OPTION_8:
            case OPTION_9:
            case OPTION_10:
            case OPTION_11:
            case OPTION_12:
            case OPTION_13:
            case OPTION_14:
            case OPTION_15:
               {
                  t_int n = _option-8;
                  set_bit(bytes[1], n);
                  break;
               }
            case OPTION_16:
            case OPTION_17:
            case OPTION_18:
            case OPTION_19:
            case OPTION_20:
            case OPTION_21:
            case OPTION_22:
            case OPTION_23:
               {
                  t_int n = _option-16;
                  set_bit(bytes[2], n);
                  break;
               }
            case OPTION_24:
            case OPTION_25:
            case OPTION_26:
            case OPTION_27:
            case OPTION_28:
            case OPTION_29:
            case OPTION_30:
            case OPTION_31:
               {
                  t_int n = _option-24;
                  set_bit(bytes[3], n);
                  break;
               }
            }
      }

      bool versionResponseCommand::getOption(versionResponseCommand::Option const _option) const
      {
         bool result = false;

         switch (_option)
            {
            case SS:
            case PERIODIC_SS:
            case UPNP:
            case DHT:
            case ENCRYPTION:
            case OPTION_5:
            case OPTION_6:
            case OPTION_7:
               {
                  t_int n = _option;
                  if (((bytes[0] >> n) & 1) == 1)
                     {
                        result = true;
                     }
                  break;
               }
            case OPTION_8:
            case OPTION_9:
            case OPTION_10:
            case OPTION_11:
            case OPTION_12:
            case OPTION_13:
            case OPTION_14:
            case OPTION_15:
               {
                  t_int n = _option-8;
                  if (((bytes[1] >> n) & 1) == 1)
                     {
                        result = true;
                     }
                  break;
               }
            case OPTION_16:
            case OPTION_17:
            case OPTION_18:
            case OPTION_19:
            case OPTION_20:
            case OPTION_21:
            case OPTION_22:
            case OPTION_23:
               {
                  t_int n = _option-16;
                  if (((bytes[1] >> n) & 1) == 1)
                     {
                        result = true;
                     }
                  break;
               }
            case OPTION_24:
            case OPTION_25:
            case OPTION_26:
            case OPTION_27:
            case OPTION_28:
            case OPTION_29:
            case OPTION_30:
            case OPTION_31:
               {
                  t_int n = _option-24;
                  if (((bytes[1] >> n) & 1) == 1)
                     {
                        result = true;
                     }
                  break;
               }
            }

         return result;
      }

      versionResponseCommand::~versionResponseCommand()
      {

      }

   } // namespace core
} // namespace btg

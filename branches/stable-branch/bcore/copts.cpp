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

#include "copts.h"
#include <bcore/project.h>
#include <bcore/t_string.h>

namespace btg
{
   namespace core
   {
      OptionBase::OptionBase()
         : Printable(),
           ver_mayor(0),
           ver_minor(0),
           ver_revision(0)
      {
         bytes[0] = 0;
         bytes[1] = 0;
         bytes[2] = 0;
         bytes[3] = 0;
      }

      OptionBase::OptionBase(t_byte const _mayor, 
                             t_byte const _minor, 
                             t_byte const _revision)
         : Printable(),
           ver_mayor(_mayor),
           ver_minor(_minor),
           ver_revision(_revision)
      {
         bytes[0] = 0;
         bytes[1] = 0;
         bytes[2] = 0;
         bytes[3] = 0;
      }

      void OptionBase::getVersion(t_byte & _mayor,
                                  t_byte & _minor,
                                  t_byte & _revision) const
      {
         _mayor    = ver_mayor;
         _minor    = ver_minor;
         _revision = ver_revision;
      }

      bool OptionBase::getOption(OptionBase::Option const _option) const
      {
         bool result = false;

         switch (_option)
            {
            case SS:
            case PERIODIC_SS:
            case UPNP:
            case DHT:
            case ENCRYPTION:
            case URL:
            case SELECTIVE_DL:
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

      std::string OptionBase::toString() const
      {
         std::string o;

         o += "btgdaemon version ";
         o += convertToString<t_uint>(ver_mayor);
         o += ".";
         o += convertToString<t_uint>(ver_minor);
         o += ".";
         o += convertToString<t_uint>(ver_revision);
         o += ".\n";

         o += "Options:\n";

         if (getOption(SS))
            {
               o += "Session saving enabled.";
               o += "\n";
            }

         if (getOption(PERIODIC_SS))
            {
               o += "Periodic session saving enabled.";
               o += "\n";
            }

         if (getOption(UPNP))
            {
               o += "UPNP enabled.";
               o += "\n";
            }

         if (getOption(DHT))
            {
               o += "DHT enabled.";
               o += "\n";
            }

         if (getOption(ENCRYPTION))
            {
               o += "Encryption enabled.";
               o += "\n";
            }

         if (getOption(URL))
            {
               o += "URL downloading enabled.";
               o += "\n";
            }

         if (getOption(SELECTIVE_DL))
            {
               o += "Selective download enabled.";
               o += "\n";
            }
         
         return o;
      }

      /* */

      void getCompileTimeOptions(std::string & _output)
      {
#if BTG_DEBUG
         _output += "debug";
         _output += "\n";
#endif // BTG_DEBUG
               
#if BTG_OPTION_SAVESESSIONS
         _output += "session saving";
         _output += "\n";
#endif // BTG_OPTION_SAVESESSIONS
               
#if BTG_OPTION_EVENTCALLBACK
         _output += "event callback";
         _output += "\n";
#endif // BTG_OPTION_EVENTCALLBACK
               
#if BTG_OPTION_UPNP
         _output += "UPnP";
         _output += "\n";
#endif // BTG_OPTION_UPNP
               
#if BTG_OPTION_USECYBERLINK
         _output += "UPnP CyberLink";
         _output += "\n";
#endif // BTG_OPTION_USECYBERLINK

      }

   } // namespace core
} // namespace btg

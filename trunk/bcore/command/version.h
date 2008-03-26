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
 * Id    : $Id$
 */

#ifndef COMMAND_VERSION_H
#define COMMAND_VERSION_H

#include "command.h"
#include <bcore/dbuf.h>
#include <bcore/addrport.h>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup commands
          */
         /** @{ */

         /// Request version and options.
         class versionCommand: public Command
            {
            public:
               /// Constructor.
               versionCommand();

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Destructor.
               virtual ~versionCommand();
            };

         /// Version and options.
         class versionResponseCommand: public Command
            {
            public:
               enum Option
               {
                  SS             = 0, // Session saving enabled.
                  PERIODIC_SS    = 1, // Periodic session saving enabled.
                  UPNP           = 2, // UPNP enabled.
                  DHT            = 3, // DHT enabled.
                  ENCRYPTION     = 4, // Encryption enabled.
                  // placeholders for new options.
                  OPTION_5       = 5,
                  OPTION_6,
                  OPTION_7,
                  OPTION_8,
                  OPTION_9,
                  OPTION_10,
                  OPTION_11,
                  OPTION_12,
                  OPTION_13,
                  OPTION_14,
                  OPTION_15,
                  OPTION_16,
                  OPTION_17,
                  OPTION_18,
                  OPTION_19,
                  OPTION_20,
                  OPTION_21,
                  OPTION_22,
                  OPTION_23,
                  OPTION_24,
                  OPTION_25,
                  OPTION_26,
                  OPTION_27,
                  OPTION_28,
                  OPTION_29,
                  OPTION_30,
                  OPTION_31      = 31
               };

               /// Default constructor.
               versionResponseCommand();

               versionResponseCommand(t_byte const _mayor, 
                                      t_byte const _minor, 
                                      t_byte const _revision);
               
               void getVersion(t_byte & _mayor,
                               t_byte & _minor,
                               t_byte & _revision) const;

               void setOption(versionResponseCommand::Option const _option);
               bool getOption(versionResponseCommand::Option const _option) const;

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Destructor.
               virtual ~versionResponseCommand();
            private:
               t_byte ver_mayor;
               t_byte ver_minor;
               t_byte ver_revision;

               /// Options.
               t_byte bytes[4];
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif // COMMAND_VERSION_H

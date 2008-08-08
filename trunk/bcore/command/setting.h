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

#ifndef COMMAND_SETTING_H
#define COMMAND_SETTING_H

#include "command.h"
#include <bcore/dbuf.h>
#include <bcore/addrport.h>
#include <bcore/copts.h>

#include "dsetting.h"

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup commands
          */
         /** @{ */

         /// Request the value of a setting identified by an Id.
         class settingCommand: public Command
            {
            public:
               
               /// Default constructor.
               settingCommand();

               /// Constructor.
               settingCommand(daemonSetting const _s);

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               daemonSetting getSetting() const;

               /// Destructor.
               virtual ~settingCommand();

            protected:
               daemonSetting setting_;
            };

         /// The value of a setting identified by an Id.
         class settingResponseCommand: public Command
            {
            public:
               /// Default constructor.
               settingResponseCommand();

               /// Constructor.
               settingResponseCommand(daemonSetting const _setting,
                                      std::string const& _response);
               
               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               daemonSetting getSetting() const;
               std::string getResponse() const;

               /// Destructor.
               virtual ~settingResponseCommand();
            protected:
               daemonSetting setting_;
               std::string   response_;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif // COMMAND_SETTING_H

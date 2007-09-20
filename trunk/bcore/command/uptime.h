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
 * Id    : $Id: uptime.h,v 1.1.2.6 2006/03/13 18:40:48 jstrom Exp $
 */

#ifndef UPTIME_H
#define UPTIME_H

#include "command.h"
#include <bcore/dbuf.h>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup commands
          */
         /** @{ */

         /// Requests the daemon to provide uptime.
         class uptimeCommand: public Command
            {
            public:
               /// Default constructor.
               uptimeCommand();

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);
               /// Destructor.
               virtual ~uptimeCommand();
            };

         /** @} */

         /**
          * \addtogroup commands
          */
         /** @{ */

         /// Requests the daemon to provide uptime.
         class uptimeResponseCommand: public Command
            {
            public:
               /// Default constructor.
               uptimeResponseCommand();
               /// Constructor.
               /// @param [in] _uptime         The uptime in seconds.
               uptimeResponseCommand(t_ulong const _uptime);
               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Get the contained uptime.
               t_ulong getUptime() const;

               /// Destructor.
               virtual ~uptimeResponseCommand();
            private:
               /// Uptime in seconds.
               t_ulong uptime;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

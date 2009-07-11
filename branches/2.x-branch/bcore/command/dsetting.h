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

#ifndef DSETTING_H
#define DSETTING_H

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup commands
          */
         /** @{ */

         enum daemonSetting
         {
            SG_UNDEF = 0, /// Undefined.
            SG_TRANSPORT, /// Type of transport used by the daemon.
            SG_PORT,      /// The port used by the daemon.
            SG_PEERID,    /// The session's peer ID.
            SG_4,         /// Placeholder.
            SG_5,         /// Placeholder.
            SG_6,         /// Placeholder.
            SG_7,         /// Placeholder.
            SG_8,         /// Placeholder.
            SG_9,         /// Placeholder.
            SG_10,        /// Placeholder.
            // ..
            SG_254 = 254, /// Placeholder.
         };

         /** @} */

      } // namespace core
} // namespace btg

#endif // DSETTING_H

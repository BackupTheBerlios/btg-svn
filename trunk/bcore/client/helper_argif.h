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

#ifndef HELPER_ARGIF_H
#define HELPER_ARGIF_H

#include <string>
#include <bcore/type.h>
#include <bcore/addrport.h>

namespace btg
{
   namespace core
   {
      namespace client
      {

         /**
          * \addtogroup gencli
          */
         /** @{ */

         class HelperArgIf
         {
         public:
            HelperArgIf();

            /// Returns true if the address of the daemon is set.
            virtual bool daemonSet() const = 0;

            /// Returns a session ID or -1 if its undefined.
            virtual t_long sessionId() const = 0;

            /// Indicates if torrents (uploaded to the
            /// daemon) should be started after creation.
            virtual bool automaticStart() const = 0;

            /// Get the ipv4 address and port of daemon for
            /// socketbased transports.
            virtual btg::core::addressPort getDaemon() const = 0;

            /// Returns true, if using encryption was set.
            virtual bool useEncryptionSet() const = 0;

            /// Indicates if encryption shall be used.
            virtual bool useEncryption() const = 0;

            /// Returns true, if using DHT was set.
            virtual bool useDHTSet() const = 0;
                  
            /// Indicates if DHT shall be used.
            virtual bool useDHT() const = 0;

            /// Destructor.
            virtual ~HelperArgIf();

         };

         /** @} */

      } // namespace client
   } // namespace core
} // namespace btg
#endif

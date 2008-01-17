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

#ifndef DAEMONDATA_H
#define DAEMONDATA_H

#include <bcore/transport/transport.h>
#include <bcore/transport/tcpip.h>
#include <bcore/transport/stcpip.h>
#include <bcore/transport/http.h>
#include <bcore/transport/shttp.h>
#include <bcore/os/gnutlsif.h>

#include <bcore/externalization/externalization.h>

#include "ipfilter/ipfilterif.h"

#include <daemon/auth/auth.h>

namespace btg
{
   namespace daemon
      {
         class commandLineArgumentHandler;
         class daemonConfiguration;

         class fileTrack;
         class connectionHandler;
         class callbackManager;

         /// Class containing the daemon data which must be
         /// initialized before it can start.
         class daemonData
            {
            public:

               enum
                  {
                     /// Ports used by the daemon should be above this value.
                     MIN_LISTEN_PORT = 1024
                  };

               /// Constructor. All pointers are null.
               daemonData();

               /// Destructor.
               ~daemonData();

               /// Destroy the contained data.
               void destroy();

               /// The start time of the daemon.
               time_t                                       daemonStartTime;

               /// Pointer to the command line argument handler.
               commandLineArgumentHandler*                  cla;

               /// Path of the config file.
               std::string                                  configFile;

               /// Pointer to the daemon configuration.
               daemonConfiguration*                         config;

               /// Pointer to a connection handler
               connectionHandler*                           connHandler;

               /// Port range used by libtorrent.
               std::pair<t_int, t_int>                      portRange;

               /// Pointer to the externalization i/f used.
               btg::core::externalization::Externalization* externalization;

               /// The transport used.
               btg::core::messageTransport*                 transport;

               /// Data used by the secure transport.
               btg::core::os::gtlsGlobalServerData*         secureTransportData;

               /// IP filename used by the IP filter.
               std::string                                  filterFilename;

               /// Pointer to the class keeping track of torrent files.
               fileTrack*                                   filetrack;

               /// IPv4 filter type.
               IpFilterIf::TYPE                             filterType;

               /// Pointer to the IP filter used.
               IpFilterIf*                                  filter;

               /// Filename used for logging, if file logging is enabled.
               std::string                                  logfilename;
#if BTG_OPTION_SAVESESSIONS
               /// Sesson saving: Indicates if session saving is enabled.
               bool                                         ss_enable;

               /// Sesson saving: filename used to save sessions.
               std::string                                  ss_filename;

               /// Sesson saving: Periodic timeout
	       t_int                                        ss_timeout;
#endif // BTG_OPTION_SAVESESSIONS
               /// Pointer to the auth interface to use.
               btg::daemon::auth::Auth*                     auth;
#if BTG_OPTION_EVENTCALLBACK
               /// Manager used to control access to callbacks.
               callbackManager*                             callbackmgr;
#endif // BTG_OPTION_EVENTCALLBACK
            private:
               /// Copy constructor.
               daemonData(daemonData const& _dd);
               /// Assignment operator.
               daemonData& operator=(daemonData const& _dd);
            };

      } // namespace daemon
} // namespace btg

#endif // DAEMONDATA_H


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
 * $Id: daemondata.cpp,v 1.1.2.8 2006/10/11 19:56:32 jstrom Exp $
 */

#include "daemondata.h"

#include <bcore/transport/transport.h>
#include <bcore/transport/tcpip.h>
#include <bcore/transport/http.h>

#include <bcore/externalization/simple.h>
#include <bcore/externalization/xmlrpc.h>

#include "darg.h"
#include "dconfig.h"
#include "filetrack.h"
#include "connectionhandler.h"
#include "callbackmgr.h"

#include "auth/auth.h"

namespace btg
{
   namespace daemon
   {
      daemonData::daemonData():
         daemonStartTime(0),
         cla(0),
         configFile(),
         config(0),
         connHandler(0),
         portRange(0,0),
         externalization(0),
         transport(0),
         secureTransportData(0),
         filterFilename(),
         filetrack(0),
         filterType(IpFilterIf::IPF_UNDEF),
         filter(0),
         logfilename()
#if BTG_OPTION_SAVESESSIONS
         , ss_enable(0),
         ss_filename(""),
			ss_timeout(60)
#endif // BTG_OPTION_SAVESESSIONS
       , auth(0)
#if BTG_OPTION_EVENTCALLBACK
       , callbackmgr(0)
#endif // BTG_OPTION_EVENTCALLBACK
      {}

      daemonData::~daemonData()
      {

      };

      void daemonData::destroy()
      {
         delete filter;
         filter = 0;

         delete filetrack;
         filetrack = 0;

         delete connHandler;
         connHandler = 0;

         delete transport;
         transport = 0;

         delete secureTransportData;
         secureTransportData = 0;

         delete externalization;
         externalization = 0;

         delete config;
         config = 0;

         delete cla;
         cla = 0;

         delete auth;
         auth = 0;
#if BTG_OPTION_EVENTCALLBACK
         delete callbackmgr;
         callbackmgr = 0;
#endif // BTG_OPTION_EVENTCALLBACK
      };

   } // namespace daemon
} // namespace btg



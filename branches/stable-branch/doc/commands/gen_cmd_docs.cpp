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

#include <bcore/project.h>
#include <bcore/trackerstatus.h>
#include <bcore/command_factory.h>
#include <bcore/externalization/externalization.h>

#include <bcore/command/ack.h>
#include <bcore/command/context_abort.h>
#include <bcore/command/context_clean.h>
#include <bcore/command/context_clean_rsp.h>
#include <bcore/command/context_create.h>
#include <bcore/command/context_create_url.h>
#include <bcore/command/context_create_file.h>
#include <bcore/command/context_last.h>
#include <bcore/command/context_fi.h>
#include <bcore/command/context_fi_rsp.h>
#include <bcore/command/context_li_status.h>
#include <bcore/command/context_li_status_rsp.h>
#include <bcore/command/context_limit.h>
#include <bcore/command/context_peers.h>
#include <bcore/command/context_peers_rsp.h>
#include <bcore/command/context_start.h>
#include <bcore/command/context_status.h>
#include <bcore/command/context_status_rsp.h>
#include <bcore/command/context_stop.h>
#include <bcore/command/error.h>
#include <bcore/command/initconnection.h>
#include <bcore/command/kill.h>
#include <bcore/command/list.h>
#include <bcore/command/list_rsp.h>
#include <bcore/command/session_attach.h>
#include <bcore/command/session_detach.h>
#include <bcore/command/session_error.h>
#include <bcore/command/session_list.h>
#include <bcore/command/session_list_rsp.h>
#include <bcore/command/session_quit.h>
#include <bcore/command/session_rw.h>
#include <bcore/command/setup.h>
#include <bcore/command/version.h>
#include <bcore/command/uptime.h>
#include <bcore/command/opstat.h>
#include <bcore/opstatus.h>

#include "ext_printer.h"

#include <iostream>

using namespace btg::core;
using namespace btg::core::externalization;

void printCommand(btg::core::commandFactory & cf,
                  btg::core::externalization::Externalization* _e, Command* _command);

int main(int argc, char* argv[])
{
   LogWrapperType logwrapper(new btg::core::logger::logWrapper);

   t_int context_id            = 1;
   std::string filename("filename");

   btg::core::externalization::Externalization* e = new externalizationPrinter(logwrapper);

   btg::core::commandFactory cf(logwrapper, *e);

   // Create a number of commands and serialize/print them.

   // Command::CN_GINITCONNECTION
   e->setDirection(TO_SERVER);
   printCommand(cf, e, new initConnectionCommand());

   // Command::CN_GSETUP
   requiredSetupData rsd("build id", -1, -1, false, false);
   e->setDirection(TO_SERVER);
   printCommand(cf, e, new setupCommand(rsd));

   // Command::CN_GSETUPRSP
   e->setDirection(FROM_SERVER);
   printCommand(cf, e, new setupResponseCommand(50000));

   // Command::CN_GKILL
   printCommand(cf, e, new killCommand());

   // Command::CN_GUPTIME
   printCommand(cf, e, new uptimeCommand());

   // Command::CN_GUPTIMERSP
   e->setDirection(FROM_SERVER);
   printCommand(cf, e, new uptimeResponseCommand(5));

   // Command::CN_GLIST
   e->setDirection(TO_SERVER);
   printCommand(cf, e, new listCommand());

   // Command::CN_GLISTRSP
   t_intList context_ids;
   context_ids.push_back(0);
   t_strList filenames;
   filenames.push_back(filename);
   e->setDirection(FROM_SERVER);
   printCommand(cf, e, new listCommandResponse(context_ids, filenames));

   // Command::CN_CCREATEWITHDATA.
   dBuffer db;
   for (t_uint i = 0; i<16; i++)
      {
         db.addByte(i);
      }
   sBuffer torrentbuffer(db);

   e->setDirection(TO_SERVER);
   printCommand(cf, e, new contextCreateWithDataCommand("filename", torrentbuffer, true));

   // Command::CN_CLAST
   printCommand(cf, e, new lastCIDCommand());

   // Command::CN_CLASTRSP
   printCommand(cf, e, new lastCIDResponseCommand(12345));

   // Command::CN_CSTART
   printCommand(cf, e, new contextStartCommand(context_id, false));

   // Command::CN_CSTOP
   printCommand(cf, e, new contextStopCommand(context_id, false));

   // Command::CN_CABORT
   printCommand(cf, e, new contextAbortCommand(context_id, false));

   // Command::CN_CSTATUS
   printCommand(cf, e, new contextStatusCommand(context_id, false));

   // Command::CN_CSTATUSRSP
   trackerStatus ts(-1, 0);
   Status status(context_id, filename, Status::ts_finished, 0, 0, 0, 0, 0, 100, 1024, 500, 200, 0, 0, 0, 0, ts, 0);
   e->setDirection(FROM_SERVER);
   printCommand(cf, e, new contextStatusResponseCommand(context_id, status));

   // Command::CN_CALLSTATUSRSP
   t_statusList statuslst;
   statuslst.push_back(status);
   e->setDirection(FROM_SERVER);
   printCommand(cf, e, new contextAllStatusResponseCommand(context_id, statuslst));

   // Command::CN_CFILEINFO
   e->setDirection(TO_SERVER);
   printCommand(cf, e, new contextFileInfoCommand(context_id, false));

   // Command::CN_CFILEINFORSP

   t_fileInfoList fileinfolist;
   t_bitVector pieces;
   pieces.push_back(true);
   pieces.push_back(false);
   pieces.push_back(false);
   pieces.push_back(false);
   pieces.push_back(false);
   pieces.push_back(false);
   pieces.push_back(false);
   fileInformation fi(filename, pieces, 1, 1024);
   fileinfolist.push_back(fi);

   e->setDirection(FROM_SERVER);
   printCommand(cf, e, new contextFileInfoResponseCommand(context_id, fileinfolist));

   // Command::CN_CALLFILEINFORSP
   // Above is not implemented.

   // Command::CN_CCLEAN
   e->setDirection(TO_SERVER);
   printCommand(cf, e, new contextCleanCommand());

   // Command::CN_CCLEANRSP
   t_intList contextIDs;
   contextIDs.push_back(context_id);
   e->setDirection(FROM_SERVER);
   printCommand(cf, e, new contextCleanResponseCommand(filenames, contextIDs));

   // Command::CN_CLIMIT
   e->setDirection(TO_SERVER);
   printCommand(cf, e, new contextLimitCommand(context_id, 10, 10, false));

   // Command::CN_CLIMITSTATUS
   e->setDirection(TO_SERVER);
   printCommand(cf, e, new contextLimitStatusCommand(context_id, false));

   // Command::CN_CLIMITSTATUSRSP
   e->setDirection(FROM_SERVER);
   printCommand(cf, e, new contextLimitStatusResponseCommand(context_id, 10, 5));

   // Command::CN_CPEERS
   contextPeersCommand *cpc = new contextPeersCommand(context_id, false);
   cpc->setExRange(0, 10);
   e->setDirection(TO_SERVER);
   printCommand(cf, e, cpc);

   // Command::CN_CPEERSRSP
   peerAddress peeraddress(127,0,0,1);
   Peer peer(peeraddress, true, "info");
   t_peerList peerlist;
   peerlist.push_back(peer);
   contextPeersResponseCommand *cprc = new contextPeersResponseCommand(context_id, peerlist);
   PeerEx peerEx;
   t_peerExList peerExList;
   peerExList.push_back(peerEx);
   cprc->setExList(0,peerExList);
   e->setDirection(FROM_SERVER);
   printCommand(cf, e, cprc);

   // Command::CN_ERROR
   e->setDirection(FROM_SERVER);
   printCommand(cf, e, new errorCommand(Command::CN_ERROR, "error text"));

   // Command::CN_ACK
   e->setDirection(FROM_SERVER);
   printCommand(cf, e, new ackCommand(Command::CN_GINITCONNECTION));

   // Command::CN_SATTACH
   e->setDirection(TO_SERVER);
   printCommand(cf, e, new attachSessionCommand("build id", 1000 /* session */));

   // Command::CN_SDETACH
   e->setDirection(TO_SERVER);
   printCommand(cf, e, new detachSessionCommand());

   // Command::CN_SQUIT
   e->setDirection(TO_SERVER);
   printCommand(cf, e, new quitSessionCommand());

   // Command::CN_SERROR
   e->setDirection(FROM_SERVER);
   printCommand(cf, e, new sessionErrorCommand(Command::CN_GINITCONNECTION));

   // Command::CN_SLIST
   e->setDirection(TO_SERVER);
   printCommand(cf, e, new listSessionCommand());

   // Command::CN_SLISTRSP
   e->setDirection(FROM_SERVER);
   t_longList sessions;
   sessions.push_back(50000);
   t_strList sessionNames;
   sessionNames.push_back("session 0");

   printCommand(cf, e, new listSessionResponseCommand(sessions, sessionNames));

   // Command::CN_MOREAD
   printCommand(cf, e, new sessionROCommand());

   // Command::CN_MOWRITE
   printCommand(cf, e, new sessionRWCommand());

   // Command::CN_CCREATEFROMURL
   t_uint urlId = 1;
   printCommand(cf, e, 
                new contextCreateFromUrlCommand("filename",
                                                "http://url",
                                                true)
                );

   // Command::CN_CCREATEFROMURLRSP
   printCommand(cf, e, 
                new contextCreateFromUrlResponseCommand(urlId)
                );

   // Command::CN_OPSTATUS
   printCommand(cf, e, 
                new opStatusCommand(urlId, 2 /* type */)
                );

   // Command::CN_OPSTATUSRSP
   t_uint urlstatus = btg::core::OP_WORKING;
   btg::core::opStatusResponseCommand* cosrc = new opStatusResponseCommand(urlId, 2 /* type */, urlstatus);
   cosrc->setData("test");
   printCommand(cf, e, cosrc);

   // Command::CN_OPABORT
   printCommand(cf, e,
                new opAbortCommand(urlId, 2 /* type */)
                );

   t_uint fileId = 1;
   // Command::CN_CCREATEFROMFILE
   printCommand(cf, e,
                new contextCreateFromFileCommand("filename.torrent",
                                                 50,
                                                 true)
                );

   // Command::CN_CCREATEFROMFILERSP
   printCommand(cf, e,
                new contextCreateFromFileResponseCommand(fileId)
                );

   // Command::CN_CCREATEFROMFILEPART
   sBuffer data;
   printCommand(cf, e,
                new contextCreateFromFilePartCommand(fileId,
                                                     1,
                                                     data)
                );

   // Command::CN_VERSION
   printCommand(cf, e, new versionCommand);

   // Command::CN_VERSIONRSP
   printCommand(cf, e, new versionResponseCommand(1, 2, 3));
   
   delete e;
   e = 0;

   return 0;
}

void printCommand(btg::core::commandFactory & cf,
                  btg::core::externalization::Externalization* _e, 
                  Command* _command)
{
   if (!cf.convertToBytes(_command))
      {
         std::cout << "Error" << std::endl;
      }

   _e->reset();

   delete _command;
   _command = 0;
}

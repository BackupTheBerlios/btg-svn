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

#include "statemachine.h"
#include "clientcallback.h"

#include <bcore/command/command.h>
#include <bcore/command/list_rsp.h>
#include <bcore/command/session_attach.h>
#include <bcore/command/context_clean_rsp.h>
#include <bcore/command/ack.h>
#include <bcore/command/context_fi_rsp.h>
#include <bcore/command/context_li_status_rsp.h>
#include <bcore/command/context_peers_rsp.h>
#include <bcore/command/context_status_rsp.h>
#include <bcore/command/error.h>
#include <bcore/command/kill.h>
#include <bcore/command/session_detach.h>
#include <bcore/command/session_list_rsp.h>
#include <bcore/command/session_quit.h>
#include <bcore/command/session_list.h>
#include <bcore/command/session_name.h>
#include <bcore/command/uptime.h>
#include <bcore/command/list.h>

#include <bcore/command/context_last.h>
#include <bcore/command/context_create.h>
#include <bcore/command/context_abort.h>

#include <bcore/command/context_clean.h>
#include <bcore/command/context_fi.h>
#include <bcore/command/context_li_status.h>
#include <bcore/command/context_limit.h>
#include <bcore/command/context_peers.h>
#include <bcore/command/context_start.h>
#include <bcore/command/context_status.h>
#include <bcore/command/context_stop.h>
#include <bcore/command/context_file.h>

#include <bcore/command/limit.h>

namespace btg
{
   namespace core
   {
      namespace client
      {

         using namespace btg::core;

         void stateMachine::cb_CN_GLIST(btg::core::Command* _command)
         {
            clientcallback->onList(
                                   dynamic_cast<listCommandResponse*>(_command)->getIDs(),
                                   dynamic_cast<listCommandResponse*>(_command)->getFilanames()
                                   );
         }

         void stateMachine::cb_CN_GSETUP(btg::core::Command* _command)
         {
            /* Is this needed here?? */
#if BTG_STATEMACHINE_DEBUG
            BTG_NOTICE("CN_GSETUP called in expectedReply");
#endif // BTG_STATEMACHINE_DEBUG
            clientcallback->onSetup(
                                    dynamic_cast<setupResponseCommand*>(_command)->getSession()
                                    );
         }

         void stateMachine::cb_CN_GLIMITSTAT(btg::core::Command* _command)
         {
            limitStatusResponseCommand* lsrc = dynamic_cast<limitStatusResponseCommand*>(_command);

            clientcallback->onGlobalLimitResponse(lsrc->getUploadLimit(),
                                                  lsrc->getDownloadLimit(),
                                                  lsrc->getMaxUplds(),
                                                  lsrc->getMaxConnections()
                                                  );
         }

         void stateMachine::cb_CN_CCLEAN(btg::core::Command* _command)
         {
            contextCleanResponseCommand* ccrc = 
               dynamic_cast<contextCleanResponseCommand*>(_command);

            clientcallback->onClean(ccrc->getFilenames(),
                                    ccrc->getContextIDs()
                                    );
         }

         void stateMachine::cb_CN_GUPTIME(btg::core::Command* _command)
         {
            clientcallback->onUptime(dynamic_cast<uptimeResponseCommand*>(_command)->getUptime());
         }

         void stateMachine::cb_CN_CSTATUS(btg::core::Command* _command)
         {
            // Special case, because of the fact that status request
            // can return two different responses.
            switch (_command->getType())
               {
               case Command::CN_CSTATUSRSP:
                  {
                     clientcallback->onStatus(dynamic_cast<contextStatusResponseCommand*>(_command)->getStatus());
                     break;
                  }
               case Command::CN_CALLSTATUSRSP:
                  {
                     clientcallback->onStatusAll(dynamic_cast<contextAllStatusResponseCommand*>(_command)->getStatus());
                     break;
                  }
               }
         }

         void stateMachine::cb_CN_CALLSTATUSRSP(btg::core::Command* _command)
         {
            clientcallback->onStatusAll(dynamic_cast<contextAllStatusResponseCommand*>(_command)->getStatus());
         }

         void stateMachine::cb_CN_CLASTRSP(btg::core::Command* _command)
         {
            clientcallback->onLast(dynamic_cast<lastCIDResponseCommand*>(_command)->getContextId());
         }

         void stateMachine::cb_CN_CFILEINFO(btg::core::Command* _command)
         {
            clientcallback->onFileInfo(dynamic_cast<contextFileInfoResponseCommand*>(_command)->getFileInfoList());
         }

         void stateMachine::cb_CN_CPEERS(btg::core::Command* _command)
         {
            clientcallback->onPeers(dynamic_cast<contextPeersResponseCommand*>(_command)->getList());
         }

         void stateMachine::cb_CN_CLIMITSTATUS(btg::core::Command* _command)
         {
            contextLimitStatusResponseCommand* clsrc = 
               dynamic_cast<contextLimitStatusResponseCommand*>(_command);

            clientcallback->onLimitStatus(clsrc->getUploadLimit(),
                                          clsrc->getDownloadLimit(),
                                          clsrc->getSeedLimit(),
                                          clsrc->getSeedTimeout());
         }

         void stateMachine::cb_CN_CGETFILES(btg::core::Command* _command)
         {
            clientcallback->onSelectedFiles(
                                            dynamic_cast<contextGetFilesResponseCommand*>(_command)->getFiles()
                                            );
         }

         void stateMachine::cb_CN_SNAME(btg::core::Command* _command)
         {
            sessionNameResponseCommand* snrc = dynamic_cast<sessionNameResponseCommand*>(_command);

            clientcallback->onSessionName(snrc->getName());
         }

      } // namespace client
   } // namespace core
} // namespace btg


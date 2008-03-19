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

#include "command_factory.h"

#include <bcore/command/ack.h>
#include <bcore/command/command.h>
#include <bcore/command/context.h>
#include <bcore/command/context_abort.h>
#include <bcore/command/context_clean.h>
#include <bcore/command/context_create.h>
#include <bcore/command/context_create_url.h>
#include <bcore/command/context_last.h>
#include <bcore/command/context_start.h>
#include <bcore/command/context_status.h>
#include <bcore/command/context_status_rsp.h>
#include <bcore/command/context_stop.h>
#include <bcore/command/context_clean.h>
#include <bcore/command/context_clean_rsp.h>
#include <bcore/command/context_limit.h>
#include <bcore/command/context_li_status.h>
#include <bcore/command/context_li_status_rsp.h>

#include <bcore/command/context_fi.h>
#include <bcore/command/context_fi_rsp.h>

#include <bcore/command/context_peers.h>
#include <bcore/command/context_peers_rsp.h>

#include <bcore/command/context_file.h>
#include <bcore/command/context_tracker.h>
#include <bcore/command/context_move.h>

#include <bcore/command/error.h>
#include <bcore/command/list.h>
#include <bcore/command/list_rsp.h>
#include <bcore/command/session_attach.h>
#include <bcore/command/session_detach.h>
#include <bcore/command/session_list.h>
#include <bcore/command/session_list_rsp.h>
#include <bcore/command/session_quit.h>
#include <bcore/command/session_error.h>
#include <bcore/command/session_name.h>
#include <bcore/command/session_rw.h>
#include <bcore/command/session_info.h>
#include <bcore/command/setup.h>

#include <bcore/command/initconnection.h>

#include <bcore/command/kill.h>
#include <bcore/command/limit.h>
#include <bcore/command/uptime.h>

#include "util.h"

#include <iostream>

namespace btg
{
   namespace core
   {
      commandFactory::commandFactory(LogWrapperType _logwrapper,
                                     btg::core::externalization::Externalization* _e)
         : Logable(_logwrapper),
           e(_e)
      {

      }

      commandFactory::~commandFactory()
      {
         
      }

      Command* commandFactory::createFromBytes(decodeStatus & _status)
      {
         Command *c = 0;
         _status    = commandFactory::DS_OK;

         t_int cmdid;
         e->determineCommandType(cmdid);

         // Create the command class
         switch (cmdid)
            {
            case Command::CN_ERROR:
               {
                  c = new errorCommand;
                  break;
               }

            case Command::CN_ACK:
               {
                  c = new ackCommand;
                  break;
               }

            case Command::CN_GLIST:
               {
                  c = new listCommand;
                  break;
               }

            case Command::CN_GLISTRSP:
               {
                  c = new listCommandResponse;
                  break;
               }

            case Command::CN_GSETUP:
               {
                  c = new setupCommand;
                  break;
               }

            case Command::CN_GSETUPRSP:
               {
                  c = new setupResponseCommand;
                  break;
               }

            case Command::CN_CCREATEWITHDATA:
               {
                  c = new contextCreateWithDataCommand;
                  break;
               }
            case Command::CN_CCREATEFROMURL:
               {
                  c = new contextCreateFromUrlCommand;
                  break;
               }
            case Command::CN_CCREATEFROMURLRSP:
               {
                  c = new contextCreateFromUrlResponseCommand;
                  break;
               }
            case Command::CN_CURLSTATUS:
               {
                  c = new contextUrlStatusCommand;
                  break;
               }
            case Command::CN_CURLSTATUSRSP:
               {
                  c = new contextUrlStatusResponseCommand;
                  break;
               }
            case Command::CN_CLAST:
               {
                  c = new lastCIDCommand;
                  break;
               }
            case Command::CN_CLASTRSP:
               {
                  c = new lastCIDResponseCommand;
                  break;
               }
            case Command::CN_CSTART:
               {
                  c = new contextStartCommand;
                  break;
               }

            case Command::CN_CSTOP:
               {
                  c = new contextStopCommand;
                  break;
               }

            case Command::CN_CABORT:
               {
                  c = new contextAbortCommand;
                  break;
               }

            case Command::CN_CSTATUS:
               {
                  c = new contextStatusCommand;
                  break;
               }

            case Command::CN_CSTATUSRSP:
               {
                  c = new contextStatusResponseCommand;
                  break;
               }

            case Command::CN_CALLSTATUSRSP:
               {
                  c = new contextAllStatusResponseCommand;
                  break;
               }

            case Command::CN_CMSTATUS:
               {
                  c = new contextMultipleStatusCommand;
                  break;
               }

            case Command::CN_CMSTATUSRSP:
               {
                  c = new contextMultipleStatusResponseCommand;
                  break;
               }

            case Command::CN_CFILEINFO:
               {
                  c = new contextFileInfoCommand;
                  break;
               }
            case Command::CN_CFILEINFORSP:
               {
                  c = new contextFileInfoResponseCommand;
                  break;
               }
            case Command::CN_CCLEAN:
               {
                  c = new contextCleanCommand;
                  break;
               }
            case Command::CN_CCLEANRSP:
               {
                  c = new contextCleanResponseCommand;
                  break;
               }

            case Command::CN_CLIMIT:
               {
                  c = new contextLimitCommand;
                  break;
               }

            case Command::CN_CLIMITSTATUS:
               {
                  c = new contextLimitStatusCommand;
                  break;
               }

            case Command::CN_CLIMITSTATUSRSP:
               {
                  c = new contextLimitStatusResponseCommand;
                  break;
               }

            case Command::CN_CPEERS:
               {
                  c = new contextPeersCommand;
                  break;
               }

            case Command::CN_CPEERSRSP:
               {
                  c = new contextPeersResponseCommand;
                  break;
               }

            case Command::CN_CSETFILES:
               {
                  c = new contextSetFilesCommand;
                  break;
               }

            case Command::CN_CGETFILES:
               {
                  c = new contextGetFilesCommand;
                  break;
               }

            case Command::CN_CGETFILESRSP:
               {
                  c = new contextGetFilesResponseCommand;
                  break;
               }

            case Command::CN_CGETTRACKERS:
               {
                  c = new contextGetTrackersCommand;
                  break;
               }

            case Command::CN_CGETTRACKERSRSP:
               {
                  c = new contextGetTrackersResponseCommand;
                  break;
               }

            case Command::CN_CMOVE:
               {
                  c = new contextMoveToSessionCommand;
                  break;
               }

            case Command::CN_SLIST:
               {
                  c = new listSessionCommand;
                  break;
               }

            case Command::CN_SLISTRSP:
               {
                  c = new listSessionResponseCommand;
                  break;
               }

            case Command::CN_SATTACH:
               {
                  c = new attachSessionCommand;
                  break;
               }

            case Command::CN_SQUIT:
               {
                  c = new quitSessionCommand;
                  break;
               }

            case Command::CN_SERROR:
               {
                  c = new sessionErrorCommand;
                  break;
               }

            case Command::CN_SINFO:
               {
                  c = new sessionInfoCommand;
                  break;
               }
            case Command::CN_SINFORSP:
               {
                  c = new sessionInfoResponseCommand;
                  break;
               }
            case Command::CN_SDETACH:
               {
                  c = new detachSessionCommand;
                  break;
               }

            case Command::CN_SNAME:
               {
                  c = new sessionNameCommand;
                  break;
               }

            case Command::CN_SNAMERSP:
               {
                  c = new sessionNameResponseCommand;
                  break;
               }

            case Command::CN_SSETNAME:
               {
                  c = new setSessionNameCommand;
                  break;
               }

            case Command::CN_GKILL:
               {
                  c = new killCommand;
                  break;
               }

            case Command::CN_GLIMIT:
               {
                  c = new limitCommand;
                  break;
               }

            case Command::CN_GLIMITSTAT:
               {
                  c = new limitStatusCommand;
                  break;
               }

            case Command::CN_GLIMITSTATRSP:
               {
                  c = new limitStatusResponseCommand;
                  break;
               }

            case Command::CN_GUPTIME:
               {
                  c = new uptimeCommand;
                  break;
               }

            case Command::CN_GUPTIMERSP:
               {
                  c = new uptimeResponseCommand;
                  break;
               }

            case Command::CN_GINITCONNECTION:
               {
                  c = new initConnectionCommand;
                  break;
               }

            case Command::CN_MOREAD:
               {
                  c = new sessionROCommand;
                  break;
               }

            case Command::CN_MOWRITE:
               {
                  c = new sessionRWCommand;
                  break;
               }
            default:
               BTG_ERROR_LOG(logWrapper(), "createFromBytes: wrong command type: " << cmdid << ".");
               _status = commandFactory::DS_UNKNOWN;
               c       = 0;
               break;
            }

         if (c != 0)
            {
               if (!c->deserialize(e))
                  {
                     BTG_ERROR_LOG(logWrapper(), "commandFactory, unable to deserialize " << 
                                   c->getName() << ".");
                     _status = commandFactory::DS_FAILED;

                     delete c;
                     c = 0;
                  }
            }

         return c;
      }

      bool commandFactory::convertToBytes(const Command* _command)
      {
         bool status = false;

         if ((_command->getType() == 0) || (_command->getType() == Command::CN_UNDEFINED))
            {
               BTG_ERROR_LOG(logWrapper(), "convertToBytes, undefined command.");
               return status;
            }

         e->reset();

         switch (_command->getType())
            {
            case Command::CN_ERROR:
            case Command::CN_ACK:
            case Command::CN_GLIST:
            case Command::CN_GLISTRSP:
            case Command::CN_GSETUP:
            case Command::CN_GSETUPRSP:
            case Command::CN_CCREATEWITHDATA:
            case Command::CN_CCREATEFROMURL:
            case Command::CN_CCREATEFROMURLRSP:
            case Command::CN_CURLSTATUS:
            case Command::CN_CURLSTATUSRSP:
            case Command::CN_CLAST:
            case Command::CN_CLASTRSP:
            case Command::CN_CSTART:
            case Command::CN_CSTOP:
            case Command::CN_CABORT:
            case Command::CN_CSTATUS:
            case Command::CN_CSTATUSRSP:
            case Command::CN_CALLSTATUSRSP:
            case Command::CN_CMSTATUS:
            case Command::CN_CMSTATUSRSP:
            case Command::CN_CFILEINFO:
            case Command::CN_CFILEINFORSP:
            //case Command::CN_CALLFILEINFORSP:
            case Command::CN_CCLEAN:
            case Command::CN_CCLEANRSP:
            case Command::CN_CLIMIT:
            case Command::CN_CLIMITSTATUS:
            case Command::CN_CLIMITSTATUSRSP:
            case Command::CN_CPEERS:
            case Command::CN_CPEERSRSP:
            case Command::CN_CSETFILES:
            case Command::CN_CGETFILES:
            case Command::CN_CGETFILESRSP:
            case Command::CN_CGETTRACKERS:
            case Command::CN_CGETTRACKERSRSP:
            case Command::CN_CMOVE:
            case Command::CN_SLIST:
            case Command::CN_SLISTRSP:
            case Command::CN_SATTACH:
            case Command::CN_SQUIT:
            case Command::CN_SERROR:
            case Command::CN_SINFO:
            case Command::CN_SINFORSP:
            case Command::CN_SDETACH:
            case Command::CN_SNAME:
            case Command::CN_SNAMERSP:
            case Command::CN_SSETNAME:
            case Command::CN_GKILL:
            case Command::CN_GLIMIT:
            case Command::CN_GLIMITSTAT:
            case Command::CN_GLIMITSTATRSP:
            case Command::CN_GUPTIME:
            case Command::CN_GUPTIMERSP:
            case Command::CN_GINITCONNECTION:
            case Command::CN_MOREAD:
            case Command::CN_MOWRITE:
               {
                  status = _command->serialize(e);
                  break;
               }
            default:
               BTG_ERROR_LOG(logWrapper(), "convertToBytes, wrong command type: " << 
                             static_cast<t_int>(_command->getType()) << ".");
               status = false;
               break;
            }

         return status;
      }

      void commandFactory::getBytes(btg::core::DIRECTION const _dir, dBuffer & _dbuffer)
      {
         e->setDirection(_dir);
         e->getBuffer(_dbuffer);
      }

   } // namespace core
} // namespace btg

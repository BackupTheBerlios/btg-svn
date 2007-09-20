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
 * $Id: statemachine.cpp,v 1.17.4.51 2007/06/06 22:04:31 wojci Exp $
 */

#include <iostream>


#include <bcore/dbuf.h>
#include <bcore/util.h>
#include <bcore/os/sleep.h>
#include <bcore/auth/hash.h>

#include "clientcallback.h"
#include "statemachine.h"

#include <bcore/verbose.h>

#include <bcore/sbuf.h>

#include <bcore/command_factory.h>

#include <bcore/command/initconnection.h>
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
#include <bcore/command/uptime.h>
#include <bcore/command/session_list.h>
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

         /// The default action to take when the state machine decides to
         /// change to a illegal state as according to the changeState
         /// function.
#define DEFAULT_ERROR_ACTION \
{ \
 BTG_ERROR_LOG("Dying: attempt to change to an illegal state. From=" << stateToString(currentState) << ", to=" << stateToString(_to) << "."); \
 currentState = SM_ERROR; \
}
         using namespace btg::core;
         using namespace std;

         stateMachine::stateMachine(btg::core::externalization::Externalization* _e,
                                    messageTransport *_transport,
                                    clientCallback *_clientcallback,
                                    bool const _verboseFlag)
            : externalization_(_e),
              counter_transinit(0), counter_transinit_max(15),
              counter_setup(0), counter_setup_max(15),
              counter_command(0), counter_command_max(15),
              counter_ack(0), counter_ack_max(15),
              counter_attach(0), counter_attach_max(15),
              counter_session_list(0), counter_session_list_max(15),
              transport(_transport),
              commands(0),
              currentState(SM_INIT),
              currentCommand(Command::CN_UNDEFINED),
              ackForCommand(Command::CN_UNDEFINED),
              clientcallback(_clientcallback),
              read_counter(0),
              max_read_counter(250),
              min_sleep_in_ms(50),
              verboseFlag_(_verboseFlag)
         {
            expectedReply[0] = Command::CN_UNDEFINED;
            expectedReply[1] = Command::CN_UNDEFINED;
         }

         void stateMachine::work()
         {
#if BTG_STATEMACHINE_DEBUG
            BTG_NOTICE("stateMachine::work()");
#endif // BTG_STATEMACHINE_DEBUG
            bool status = false;

            while (!status)
               {
#if BTG_STATEMACHINE_DEBUG
                  BTG_NOTICE("Current state = " << stateToString(currentState) << "(" << currentState << ").");
#endif // BTG_STATEMACHINE_DEBUG
                  switch(currentState)
                     {
                     case SM_INIT:
                        {
                           step_SM_INIT(status);
                           break;
                        }
                     case SM_TRANSINIT:
                        {
                           step_SM_TRANSINIT(status);
                           break;
                        }
                     case SM_TRANSINIT_WAIT:
                        {
                           step_SM_TRANSINIT_WAIT(status);
                           break;
                        }
                     case SM_TRANSPORT_READY:
                        {
                           step_SM_TRANSPORT_READY(status);
                           break;
                        }
                     case SM_SETUP:
                        {
                           step_SM_SETUP(status);
                           break;
                        }
                     case SM_SETUP_WAIT:
                        {
                           step_SM_SETUP_WAIT(status);
                           break;
                        }
                     case SM_COMMAND:
                        {
                           step_SM_COMMAND(status);
                           break;
                        }
                     case SM_COMMAND_WAIT:
                        {
                           step_SM_COMMAND_WAIT(status);
                           break;
                        }
                     case SM_COMMAND_ACK_WAIT:
                        {
                           step_SM_COMMAND_ACK_WAIT(status);
                           break;
                        }
                     case SM_ERROR:
                        {
                           step_SM_ERROR(status);
                           break;
                        }
                     case SM_SERROR:
                        {
                           // Do nothing, callback have been called.
                           return;
                        }
                     case SM_SESSION_LIST:
                        {
                           step_SM_SESSION_LIST(status);
                           break;
                        }
                     case SM_SESSION_LIST_WAIT:
                        {
                           step_SM_SESSION_LIST_WAIT(status);
                           break;
                        }
                     case SM_ATTACH:
                        {
                           step_SM_ATTACH(status);
                           break;
                        }
                     case SM_ATTACH_WAIT:
                        {
                           step_SM_ATTACH_WAIT(status);
                           break;
                        }
                     case SM_DETACH:
                        {
                           step_SM_DETACH(status);
                           break;
                        }
                     case SM_DETACH_WAIT:
                        {
                           step_SM_DETACH_WAIT(status);
                           break;
                        }
                     case SM_QUIT:
                        {
                           step_SM_QUIT(status);
                           break;
                        }
                     case SM_QUIT_WAIT:
                        {
                           step_SM_QUIT_WAIT(status);
                           break;
                        }
                     case SM_KILL:
                        {
                           step_SM_KILL(status);
                           break;
                        }
                     case SM_KILL_WAIT:
                        {
                           step_SM_KILL_WAIT(status);
                           break;
                        }
                     case SM_FINISHED:
                        {
                           step_SM_FINISHED(status);
                           break;
                        }

                     } // currentState
               } // while status is false
         }

         Command* stateMachine::getCommand()
         {
            Command *c = 0;
            if (commands.size() >= 1)
               {
                  if (commands.begin() != commands.end())
                     {
                        c = *commands.begin();
                        commands.erase(commands.begin());
                     }
               }
            return c;
         }

         bool stateMachine::sendCommand(btg::core::Command *_command)
         {
#if BTG_STATEMACHINE_DEBUG
            BTG_NOTICE("Sending command: " << _command->toString());
#endif // BTG_STATEMACHINE_DEBUG

            VERBOSE_LOG(verboseFlag_, "Snd: " << _command->getName() << ".");

            if (!commandFactory::convertToBytes(this->externalization_, _command))
               {
                  return false;
               }

            this->externalization_->setDirection(TO_SERVER);

            dBuffer dbuffer;
            this->externalization_->getBuffer(dbuffer);

            if (transport->write(dbuffer) == messageTransport::OPERATION_FAILED)
               {
                  return false;
               }

            return true;
         }

         bool stateMachine::receiveCommand(Command* & _target, bool const _waitforever)
         {
            bool status = false;
            Command *c  = 0;
            dBuffer dbuffer;
#if BTG_STATEMACHINE_DEBUG
            BTG_NOTICE("receiveCommand()");
#endif // BTG_STATEMACHINE_DEBUG
            bool read_status = false;

#if BTG_STATEMACHINE_DEBUG
            BTG_NOTICE("Reading from transport ..");
#endif // BTG_STATEMACHINE_DEBUG
            read_counter = 0;
            while (
                   ((read_counter < max_read_counter) && (!read_status)) ||
                   ((_waitforever) && (!read_status))
                   )
               {
                  t_int count = transport->read(dbuffer);
                  if (count == messageTransport::OPERATION_FAILED)
                     {
                        read_status = false;
                        btg::core::os::Sleep::sleepMiliSeconds(min_sleep_in_ms + read_counter);
                     }
                  else
                     {
                        read_status = true;
#if BTG_STATEMACHINE_DEBUG
                        BTG_NOTICE("Got data after " << read_counter << " read attempts");
#endif // BTG_STATEMACHINE_DEBUG
                     }

                  read_counter++;
               }

            if (!read_status)
               {
                  return status;
               }

            if (externalization_->setBuffer(dbuffer))
               {
                  commandFactory::decodeStatus dstatus;
                  c = commandFactory::createFromBytes(this->externalization_, dstatus);
               }

#if BTG_STATEMACHINE_DEBUG
            if (c != 0)
               {
                  BTG_NOTICE("actual reply is " << c->getName() << ".");
               }
#endif // BTG_STATEMACHINE_DEBUG

            if (c != 0)
               {
                  _target = c;
                  status  = true;

                  VERBOSE_LOG(verboseFlag_, "Rec: " << _target->getName() << ".");
               }
            else
               {
                  _target = 0;
               }

            return status;
         }

         bool stateMachine::checkState(State _state) const
         {
            if (currentState == _state)
               {
                  return true;
               }
            return false;
         }

         void stateMachine::changeState(State _to)
         {
#if BTG_STATEMACHINE_DEBUG
            BTG_NOTICE("Trying to change state to " << stateToString(_to) << " from " << stateToString(currentState) << ".");
#endif // BTG_STATEMACHINE_DEBUG
            switch (currentState)
               {

               case SM_INIT:
                  {
                     switch(_to)
                        {
                        case SM_TRANSINIT:
                           currentState = SM_TRANSINIT;
                           break;
                        default:
                           DEFAULT_ERROR_ACTION;
                           break;
                        }
                     break;
                  } // SM_INIT

               case SM_TRANSINIT:
                  {
                     switch(_to)
                        {
                        case SM_TRANSINIT_WAIT:
                           currentState = SM_TRANSINIT_WAIT;
                           break;
                        default:
                           DEFAULT_ERROR_ACTION;
                           break;
                        }
                     break;
                  } // SM_TRANSINIT

               case SM_TRANSINIT_WAIT:
                  {
                     switch(_to)
                        {
                        case SM_TRANSPORT_READY:
                           currentState = SM_TRANSPORT_READY;
                           break;
                        default:
                           DEFAULT_ERROR_ACTION;
                           break;
                        }
                     break;
                  } // SM_TRANSINIT_WAIT

               case SM_TRANSPORT_READY:
                  {
                     switch(_to)
                        {
                        case SM_SETUP:
                           currentState = SM_SETUP;
                           break;
                        case SM_ATTACH:
                           currentState = SM_ATTACH;
                           break;
                        case SM_SESSION_LIST:
                           currentState = SM_SESSION_LIST;
                           break;
                        default:
                           DEFAULT_ERROR_ACTION;
                           break;
                        }
                     break;
                  } // SM_TRANSPORT_READY

               case SM_SETUP:
                  {
                     switch(_to)
                        {
                        case SM_SETUP_WAIT:
                           currentState = SM_SETUP_WAIT;
                           break;
                        default:
                           DEFAULT_ERROR_ACTION;
                           break;
                        }
                     break;
                  } // SM_SETUP

               case SM_SETUP_WAIT:
                  {
                     switch(_to)
                        {
                        case SM_COMMAND:
                           currentState = SM_COMMAND;
                           break;
                        default:
                           DEFAULT_ERROR_ACTION;
                           break;
                        }
                     break;
                  } // SM_SETUP_WAIT

               case SM_COMMAND:
                  {
                     switch(_to)
                        {
                        case SM_COMMAND_WAIT:
                           currentState = SM_COMMAND_WAIT;
                           break;
                        case SM_QUIT:
                           currentState = SM_QUIT;
                           break;
                        case SM_COMMAND_ACK_WAIT:
                           currentState = SM_COMMAND_ACK_WAIT;
                           break;
                        case SM_DETACH:
                           currentState = SM_DETACH;
                           break;
                        case SM_KILL:
                           currentState = SM_KILL;
                           break;
                        default:
                           DEFAULT_ERROR_ACTION;
                           break;
                        }
                     break;
                  } // SM_COMMAND

               case SM_COMMAND_WAIT:
                  {
                     switch(_to)
                        {
                        case SM_COMMAND:
                           currentState = SM_COMMAND;
                           break;
                        case SM_SERROR:
                           // Used when the clients is forcefully
                           // detached and the daemon sends a session
                           // error message back.
                           currentState = SM_SERROR;
                           break;
                        default:
                           DEFAULT_ERROR_ACTION;
                           break;
                        }
                     break;
                  } // SM_COMMAND_WAIT

               case SM_COMMAND_ACK_WAIT:
                  {
                     switch(_to)
                        {
                        case SM_COMMAND:
                           currentState = SM_COMMAND;
                           break;
                        default:
                           currentState = SM_ERROR;
                           break;
                        }
                     break;
                  } // SM_COMMAND_ACK_WAIT

               case SM_QUIT:
                  {
                     switch (_to)
                        {
                        case SM_QUIT_WAIT:
                           currentState = SM_QUIT_WAIT;
                           break;
                        default:
                           DEFAULT_ERROR_ACTION;
                           break;
                        }
                     break;
                  } // SM_QUIT

               case SM_QUIT_WAIT:
                  {
                     switch(_to)
                        {
                        case SM_FINISHED:
                           currentState = SM_FINISHED;
                           break;
                        default:
                           DEFAULT_ERROR_ACTION;
                           break;
                        }
                     break;
                  } // SM_QUIT_WAIT

               case SM_KILL:
                  {
                     switch(_to)
                        {
                        case SM_KILL_WAIT:
                           currentState = SM_KILL_WAIT;
                           break;
                        default:
                           DEFAULT_ERROR_ACTION;
                           break;
                        }
                     break;
                  } // SM_KILL

               case SM_KILL_WAIT:
                  {
                     switch(_to)
                        {
                        case SM_FINISHED:
                           currentState = SM_FINISHED;
                           break;
                        case SM_COMMAND:
                           // If the daemon refuses to kill itself, go
                           // back to command state.
                           currentState = SM_COMMAND;
                           break;
                        default:
                           DEFAULT_ERROR_ACTION;
                           break;
                        }
                     break;
                  } // SM_KILL_WAIT

               case SM_SESSION_LIST:
                  {
                     switch(_to)
                        {
                        case SM_SESSION_LIST_WAIT:
                           currentState = SM_SESSION_LIST_WAIT;
                           break;
                        default:
                           DEFAULT_ERROR_ACTION;
                           break;
                        }
                     break;
                  }

               case SM_SESSION_LIST_WAIT:
                  {
                     switch(_to)
                        {
                        case SM_TRANSPORT_READY:
                           currentState = SM_TRANSPORT_READY;
                           break;
                           /*
                             case SM_FINISHED:
                             currentState = SM_FINISHED;
                             break;
                           */
                        default:
                           DEFAULT_ERROR_ACTION;
                           break;
                        }
                     break;
                  }

               case SM_ATTACH:
                  {
                     switch(_to)
                        {
                        case SM_ATTACH_WAIT:
                           {
                              currentState = SM_ATTACH_WAIT;
                              break;
                           }
                        default:
                           {
                              DEFAULT_ERROR_ACTION;
                              break;
                           }
                        }
                     break;
                  } // SM_ATTACH

               case SM_ATTACH_WAIT:
                  {
                     switch(_to)
                        {
                        case SM_COMMAND:
                           currentState = SM_COMMAND;
                           break;
                        case SM_ERROR:
                           currentState = SM_ERROR;
                           break;
                        default:
                           DEFAULT_ERROR_ACTION;
                           break;
                        }
                     break;
                  } // SM_ATTACH_WAIT

               case SM_DETACH:
                  {
                     switch(_to)
                        {
                        case SM_DETACH_WAIT:
                           currentState = SM_DETACH_WAIT;
                           break;
                        default:
                           DEFAULT_ERROR_ACTION;
                           break;
                        }
                     break;
                  } // SM_DETACH

               case SM_DETACH_WAIT:
                  {
                     switch(_to)
                        {
                        case SM_FINISHED:
                           currentState = SM_FINISHED;
                           break;
                        default:
                           DEFAULT_ERROR_ACTION;
                           break;
                        }
                     break;
                  }

                  // At this point, changing states is not possible.
               case SM_FINISHED:
                  {
                     switch(_to)
                        {
                        default:
                           DEFAULT_ERROR_ACTION;
                           break;
                        }
                     break;
                  } // SM_FINISHED

               default:
                  {
                     DEFAULT_ERROR_ACTION;
                     break;
                  }

               } // switch currentstate
         }

         void stateMachine::doInit(std::string const& _username,
                                   btg::core::Hash const& _hash)
         {
            if (checkState(SM_INIT))
               {
                  commands.push_back(new initConnectionCommand(_username, _hash));
                  changeState(SM_TRANSINIT);
               }
         }

         void stateMachine::doSetup(setupCommand* _command)
         {
            if (checkState(SM_TRANSPORT_READY))
               {
                  // Send the setup command.
                  // Reset the setup counter.
                  counter_setup  = 0;
                  changeState(SM_SETUP);
                  commands.push_back(_command);
               }
         }

         void stateMachine::doQuit()
         {
            if (checkState(SM_COMMAND))
               {
                  changeState(SM_QUIT);
               }
         }

         void stateMachine::doKill()
         {
            if (checkState(SM_COMMAND))
               {
                  changeState(SM_KILL);
               }
         }

         void stateMachine::doGlobalLimit(t_int const  _limitBytesUpld,
                                          t_int const  _limitBytesDwnld,
                                          t_int const  _maxUplds,
                                          t_long const _maxConnections)
         {
            if (checkState(SM_COMMAND))
               {
                  commands.push_back(new limitCommand(_limitBytesUpld,
                                                      _limitBytesDwnld,
                                                      _maxUplds,
                                                      _maxConnections));
               }
         }

         void stateMachine::doGlobalLimitStatus()
         {
            if (checkState(SM_COMMAND))
               {
                  commands.push_back(new limitStatusCommand());
               }
         }

         void stateMachine::doUptime()
         {
            if (checkState(SM_COMMAND))
               {
                  commands.push_back(new uptimeCommand());
               }
         }

         void stateMachine::doAttach(attachSessionCommand* _command)
         {
            if (checkState(SM_TRANSPORT_READY))
               {
                  // Reset the setup counter.
                  changeState(SM_ATTACH);
                  commands.push_back(_command);
#if BTG_STATEMACHINE_DEBUG
                  BTG_NOTICE("Trying to attach to session " << _command->getSession());
#endif // BTG_STATEMACHINE_DEBUG
               }
         }

         void stateMachine::doDetach()
         {
            if (checkState(SM_COMMAND))
               {
                  changeState(SM_DETACH);
               }
         }

         void stateMachine::doListSessions()
         {
            if (checkState(SM_TRANSPORT_READY))
               {
                  commands.push_back(new listSessionCommand());
                  changeState(SM_SESSION_LIST);
               }
         }

         void stateMachine::doList()
         {
            if (checkState(SM_COMMAND))
               {
                  // Send the command.
                  commands.push_back(new listCommand());
               }
         }

         void stateMachine::doCreate(std::string const& _pathToTorrent,
				     bool const _start)
         {
            if (checkState(SM_COMMAND))
               {
                  // Send the command.
                  sBuffer tf;
                  if (tf.read(_pathToTorrent))
                     {
#if BTG_STATEMACHINE_DEBUG
                        BTG_NOTICE("Creating file '" << _pathToTorrent << ", size=" << tf.size());
#endif // BTG_STATEMACHINE_DEBUG
                        std::string filename;
                        if (Util::getFileFromPath(_pathToTorrent, filename))
                           {
                              commands.push_back(
						 new contextCreateWithDataCommand(filename,
										  tf,
										  _start));
                           }
                        else
                           {
                              commands.push_back(new contextCreateWithDataCommand(_pathToTorrent,
										  tf,
										  _start));
                           }
                     }
                  else
                     {
                        BTG_ERROR_LOG("Unable to read '" << _pathToTorrent << "'");
                     }
               }
         }

         void stateMachine::doLast()
         {
            if (checkState(SM_COMMAND))
            {
               commands.push_back(new lastCIDCommand());
            }
         }

         void stateMachine::doAbort(t_int const _contextID, bool const _eraseData, bool const _allContexts)
         {
            if (checkState(SM_COMMAND))
               {
                  // Send the command.
                  commands.push_back(new contextAbortCommand(_contextID, _eraseData, _allContexts));
               }
         }

         void stateMachine::doStart(t_int const _contextID, bool const _allContexts)
         {
            if (checkState(SM_COMMAND))
               {
                  // Send the command.
                  commands.push_back(new contextStartCommand(_contextID, _allContexts));
               }
         }

         void stateMachine::doStop(t_int const _contextID, bool const _allContexts)
         {
            if (checkState(SM_COMMAND))
               {
                  // Send the command.
                  commands.push_back(new contextStopCommand(_contextID, _allContexts));
               }
         }

         void stateMachine::doStatus(t_int const _contextID, bool const _allContexts)
         {
            if (checkState(SM_COMMAND))
               {
                  // Send the command.
                  commands.push_back(new contextStatusCommand(_contextID, _allContexts));
               }
         }

         void stateMachine::doFileInfo(t_int const _contextID, bool const _allContexts)
         {
            if (checkState(SM_COMMAND))
               {
                  // Send the command.
                  commands.push_back(new contextFileInfoCommand(_contextID, _allContexts));
               }
         }

         void stateMachine::doPeers(t_int const _contextID, bool const _allContexts)
         {
            if (checkState(SM_COMMAND))
               {
                  // Send the command.
                  commands.push_back(new contextPeersCommand(_contextID, _allContexts));
               }
         }

         void stateMachine::doLimit(t_int const _contextID,
                                    t_int const _uploadRate,
                                    t_int const _downloadRate,
                                    t_int const _seedLimit,
                                    t_long const _seedTimeout,
                                    bool const _allContexts)
         {
            if (checkState(SM_COMMAND))
               {
                  // Send the command.
                  commands.push_back(new contextLimitCommand(_contextID,
                                                             _uploadRate,
                                                             _downloadRate,
                                                             _seedLimit,
                                                             _seedTimeout,
                                                             _allContexts));
               }
         }

         void stateMachine::doLimitStatus(t_int const _contextID, bool const _allContexts)
         {
            if (checkState(SM_COMMAND))
               {
                  // Send the command.
                  commands.push_back(new contextLimitStatusCommand(_contextID,
                                                                   _allContexts));
               }
         }
         
         void stateMachine::doSetFiles(t_int const _contextID, 
                                       selectedFileEntryList const& _files)
         {
            if (checkState(SM_COMMAND))
               {
                  commands.push_back(new contextSetFilesCommand(_contextID, _files));
               }
         }

         void stateMachine::doGetFiles(t_int const _contextID)
         {
            if (checkState(SM_COMMAND))
               {
                  commands.push_back(new contextGetFilesCommand(_contextID));
               }
         }

         void stateMachine::doClean(t_int const _contextID, bool const _allContexts)
         {
            if (checkState(SM_COMMAND))
               {
                  // Send the command.
                  commands.push_back(new contextCleanCommand(_contextID, _allContexts));
               }
         }

         void stateMachine::setExpectedReply(Command::commandType _type)
         {
            ackForCommand = Command::CN_UNDEFINED;

            switch (_type)
               {
               case Command::CN_GLIST:
                  {
                     expectedReply[0] = Command::CN_GLISTRSP;
                     expectedReply[1] = Command::CN_UNDEFINED;
                     break;
                  }
               case Command::CN_GSETUP:
                  {
                     expectedReply[0] = Command::CN_GSETUPRSP;
                     expectedReply[1] = Command::CN_UNDEFINED;
                     break;
                  }
               case Command::CN_CSTATUS:
                  {
                     expectedReply[0] = Command::CN_CSTATUSRSP;
                     expectedReply[1] = Command::CN_CALLSTATUSRSP;
                     break;
                  }
               case Command::CN_CFILEINFO:
                  {
                     expectedReply[0] = Command::CN_CFILEINFORSP;
                     expectedReply[1] = Command::CN_UNDEFINED;
                     break;
                  }
               case Command::CN_CPEERS:
                  {
                     expectedReply[0] = Command::CN_CPEERSRSP;
                     expectedReply[1] = Command::CN_UNDEFINED;
                     break;
                  }
               case Command::CN_CCLEAN:
                  {
                     expectedReply[0] = Command::CN_CCLEANRSP;
                     expectedReply[1] = Command::CN_UNDEFINED;
                     break;
                  }

               case Command::CN_GUPTIME:
                  {
                     expectedReply[0] = Command::CN_GUPTIMERSP;
                     expectedReply[1] = Command::CN_UNDEFINED;
                     break;
                  }
               case Command::CN_CLIMITSTATUS:
                  {
                     expectedReply[0] = Command::CN_CLIMITSTATUSRSP;
                     expectedReply[1] = Command::CN_UNDEFINED;
                     break;
                  }

               case Command::CN_CGETFILES:
                  {
                     expectedReply[0] = Command::CN_CGETFILESRSP;
                     expectedReply[1] = Command::CN_UNDEFINED;
                     break;
                  }
                     
               case Command::CN_CCREATEWITHDATA:
               case Command::CN_CSTART:
               case Command::CN_CSTOP:
               case Command::CN_CABORT:
               case Command::CN_CLIMIT:
               case Command::CN_CSETFILES:
                  {
                     ackForCommand = static_cast<Command::commandType>(_type);
                     expectedReply[0] = Command::CN_ACK;
                     expectedReply[1] = Command::CN_UNDEFINED;
                     break;
                  }

               case Command::CN_CLAST:
                  {
                     expectedReply[0] = Command::CN_CLASTRSP;
                     expectedReply[1] = Command::CN_UNDEFINED;
                     break;
                  }

               case Command::CN_GLIMIT:
                  {
                     ackForCommand = static_cast<Command::commandType>(_type);
                     expectedReply[0] = Command::CN_ACK;
                     expectedReply[1] = Command::CN_UNDEFINED;
                     break;
                  }
               case Command::CN_GLIMITSTAT:
                  {
                     expectedReply[0] = Command::CN_GLIMITSTATRSP;
                     expectedReply[1] = Command::CN_UNDEFINED;
                     break;
                  }

               default:
                  {
                     expectedReply[0] = Command::CN_UNDEFINED;
                     expectedReply[1] = Command::CN_UNDEFINED;
                  }
               } // switch
         }

         void stateMachine::setCurrentCommand(Command *_command)
         {
            currentCommand = static_cast<Command::commandType>(_command->getType());
         }

         void stateMachine::callAckCallback(t_int const _commandtype)
         {
#if BTG_STATEMACHINE_DEBUG
            BTG_NOTICE("callAckCallback()");
#endif // BTG_STATEMACHINE_DEBUG
            switch(_commandtype)
               {
               case Command::CN_CCREATEWITHDATA:
                  {
                     clientcallback->onCreateWithData();
                     break;
                  }
               case Command::CN_CSTART:
                  {
                     clientcallback->onStart();
                     break;
                  }
               case Command::CN_CSTOP:
                  {
                     clientcallback->onStop();
                     break;
                  }
               case Command::CN_CABORT:
                  {
                     clientcallback->onAbort();
                     break;
                  }
               case Command::CN_CLIMIT:
                  {
                     clientcallback->onLimit();
                     break;
                  }
               case Command::CN_GLIMIT:
                  {
                     clientcallback->onGlobalLimit();
                     break;
                  }
               case Command::CN_CSETFILES:
                  {
                     clientcallback->onSetFiles();
                     break;
                  }
               default:
                  {
                     BTG_ERROR_LOG("No callback defined.");
                  }
               }
         }

         void stateMachine::callCallback(Command *_command)
         {
#if BTG_STATEMACHINE_DEBUG
            BTG_NOTICE("callCallback()");
#endif // BTG_STATEMACHINE_DEBUG
            if ((expectedReply[0] != _command->getType()) && (expectedReply[1] != _command->getType()) )
               {
                  BTG_ERROR_LOG("callCallback: unexpected reply.");
                  // Call error callback.
                  return;
               }

            switch(currentCommand)
               {
               case Command::CN_GLIST:
                  {
                     clientcallback->onList(
                                            dynamic_cast<listCommandResponse*>(_command)->getIDs(),
                                            dynamic_cast<listCommandResponse*>(_command)->getFilanames()
                                            );
                     break;
                  }
               case Command::CN_GSETUP:
                  {
                     /* Is this needed here?? */
#if BTG_STATEMACHINE_DEBUG
                     BTG_NOTICE("CN_GSETUP called in expectedReply");
#endif // BTG_STATEMACHINE_DEBUG
                     clientcallback->onSetup(
                                             dynamic_cast<setupResponseCommand*>(_command)->getSession()
                                             );
                     break;
                  }

               case Command::CN_GLIMITSTAT:
                  {
                     limitStatusResponseCommand* lsrc = dynamic_cast<limitStatusResponseCommand*>(_command);

                     clientcallback->onGlobalLimitResponse(lsrc->getUploadLimit(),
                                                           lsrc->getDownloadLimit(),
                                                           lsrc->getMaxUplds(),
                                                           lsrc->getMaxConnections()
                                                           );
                     break;
                  }

               case Command::CN_CCLEAN:
                  {
                     clientcallback->onClean(
                                             dynamic_cast<contextCleanResponseCommand*>(_command)->getFilenames(),
                                             dynamic_cast<contextCleanResponseCommand*>(_command)->getContextIDs()
                                             );
                     break;
                  }

               case Command::CN_GUPTIME:
                  {
                     clientcallback->onUptime(dynamic_cast<uptimeResponseCommand*>(_command)->getUptime());
                     break;
                  }
                  // Special case, because of the fact that status request
                  // can return two different responses.
               case Command::CN_CSTATUS:
                  {
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
                           }
                        }
                     break;
                  }
               case Command::CN_CALLSTATUSRSP:
                  {
                     clientcallback->onStatusAll(dynamic_cast<contextAllStatusResponseCommand*>(_command)->getStatus());
                     break;
                  }

               case Command::CN_CLASTRSP:
                  {
                     clientcallback->onLast(dynamic_cast<lastCIDResponseCommand*>(_command)->getContextId());
                     break;
                  }
               case Command::CN_CFILEINFO:
                  {
                     clientcallback->onFileInfo(dynamic_cast<contextFileInfoResponseCommand*>(_command)->getFileInfoList());
                     break;
                  }
               case Command::CN_CPEERS:
                  {
                     clientcallback->onPeers(dynamic_cast<contextPeersResponseCommand*>(_command)->getList());
                     break;
                  }
               case Command::CN_CLIMITSTATUS:
                  {
                     clientcallback->onLimitStatus(
                                                   dynamic_cast<contextLimitStatusResponseCommand*>(_command)->getUploadLimit(),
                                                   dynamic_cast<contextLimitStatusResponseCommand*>(_command)->getDownloadLimit(),
                                                   dynamic_cast<contextLimitStatusResponseCommand*>(_command)->getSeedLimit(),
                                                   dynamic_cast<contextLimitStatusResponseCommand*>(_command)->getSeedTimeout()
                                                   );
                     break;
                  }
               case Command::CN_CGETFILES:
                  {
                     clientcallback->onSelectedFiles(
                                                     dynamic_cast<contextGetFilesResponseCommand*>(_command)->getFiles()
                                                     );
                     break;
                  }

               default:
                  {
                     BTG_ERROR_LOG("callCallback(), unexpected command, " << Command::getName(currentCommand) << ".");
                  }
               }
         }

         void stateMachine::callErrorCallback(Command *_command)
         {
            switch(_command->getType())
               {
               case Command::CN_ERROR:
                  {
                     errorCommand* errcmd = dynamic_cast<errorCommand*>(_command);
                     string errmessage    = errcmd->getMessage();

                     switch(currentCommand)
                        {
                           // Listing failed.
                        case Command::CN_GLIST:
                           {
                              clientcallback->onListError();
                              break;
                           }
                           // Status or status for all context failed.
                        case Command::CN_CSTATUS:
                           {
                              clientcallback->onStatusError(errmessage);
                              break;
                           }
                           // File infor request failed.
                        case Command::CN_CFILEINFO:
                           {
                              clientcallback->onFileInfoError(errmessage);
                              break;
                           }
                           // Peer request failed.
                        case Command::CN_CPEERS:
                           {
                              clientcallback->onPeersError(errmessage);
                              break;
                           }
                        case Command::CN_CLIMITSTATUS:
                           {
                              clientcallback->onLimitStatusError(errmessage);
                              break;
                           }
                        case Command::CN_CSETFILES:
                           {
                              clientcallback->onSetFilesError(errmessage);
                              break;
                           }
                        case Command::CN_CGETFILES:
                           {
                              clientcallback->onSelectedFilesError(errmessage);
                              break;
                           }
                        case Command::CN_GLIMIT:
                           {
                              clientcallback->onGlobalLimitError(errmessage);
                              break;
                           }
                        case Command::CN_GLIMITSTAT:
                           {
                              clientcallback->onGlobalLimitResponseError(errmessage);
                              break;
                           }
                           // General error callback to fallback on.
                        default:
                           {
                              clientcallback->onError(errmessage);
                              break;
                           }
                        } // switch
                     break;
                  }
               case Command::CN_SERROR:
                  {
                     clientcallback->onSessionError();
                     break;
                  }
               default:
                  // Wrong type of command for this function.
                  // This should never happend.
                  BTG_ERROR_LOG("callErrorCallback, unexpected command");
                  break;
               }
         }

         std::string stateMachine::stateToString(State _state) const
         {
            string statestring;
            switch(_state)
               {
               case SM_ERROR:
                  statestring = "SM_ERROR";
                  break;
               case SM_SERROR:
                  statestring = "SM_SERROR";
                  break;
               case SM_INIT:
                  statestring = "SM_INIT";
                  break;
               case SM_TRANSINIT:
                  statestring = "SM_TRANSINIT";
                  break;
               case SM_TRANSINIT_WAIT:
                  statestring = "SM_TRANSINIT_WAIT";
                  break;
               case SM_TRANSPORT_READY:
                  statestring = "SM_TRANSPORT_READY";
                  break;
               case SM_SETUP:
                  statestring = "SM_SETUP";
                  break;
               case SM_SETUP_WAIT:
                  statestring = "SM_SETUP_WAIT";
                  break;
               case SM_COMMAND:
                  statestring = "SM_COMMAND";
                  break;
               case SM_COMMAND_WAIT:
                  statestring = "SM_COMMAND_WAIT";
                  break;
               case SM_QUIT_WAIT:
                  statestring = "SM_QUIT_WAIT";
                  break;
               case SM_QUIT:
                  statestring = "SM_QUIT";
                  break;
               case SM_KILL:
                  statestring = "SM_KILL";
                  break;
               case SM_KILL_WAIT:
                  statestring = "SM_KILL_WAIT";
                  break;
               case SM_FINISHED:
                  statestring = "SM_FINISHED";
                  break;
               case SM_COMMAND_ACK_WAIT:
                  statestring = "SM_COMMAND_ACK_WAIT";
                  break;
               case SM_ATTACH:
                  statestring = "SM_ATTACH";
                  break;
               case SM_ATTACH_WAIT:
                  statestring = "SM_ATTACH_WAIT";
                  break;
               case SM_DETACH:
                  statestring = "SM_DETACH";
                  break;
               case SM_DETACH_WAIT:
                  statestring = "SM_DETACH_WAIT";
                  break;
               case SM_SESSION_LIST:
                  statestring = "SM_SESSION_LIST";
                  break;
               case SM_SESSION_LIST_WAIT:
                  statestring = "SM_SESSION_LIST_WAIT";
                  break;
               default:
                  statestring = "UNKNOWN";
               }
            return statestring;
         }

         void stateMachine::step_SM_INIT(bool & _status)
         {
            _status = false;
         }

         void stateMachine::step_SM_TRANSINIT(bool & _status)
         {
            // Tell the transport about the current session.
            _status = false;

            // Pop a command and send it.
            Command *c = this->getCommand();
            if (c != 0)
               {
                  sendCommand(c);
                  delete c;
                  c = 0;
                  changeState(SM_TRANSINIT_WAIT);
                  counter_transinit = 0;
               }
         }

         void stateMachine::step_SM_TRANSINIT_WAIT(bool & _status)
         {
            if (counter_transinit >= counter_transinit_max)
               {
                  // Nothing to be done. This machine is quite dead.
                  clientcallback->onSetupError("No reply.");
                  changeState(SM_ERROR);
                  _status = true;
                  return;
               }

            Command *c = 0;
            if (receiveCommand(c))
               {
                  switch (c->getType())
                     {
                     case Command::CN_ACK:
                        {
                           // Got one ack.
                           if (dynamic_cast<ackCommand*>(c)->getCommandType() == Command::CN_GINITCONNECTION)
                              {
                                 // Reset the counter.
                                 counter_transinit = 0;
                                 // Call the setup callback.
                                 clientcallback->onTransportInit();
                                 // Change state, ready to accept commands.
                                 changeState(SM_TRANSPORT_READY);
                                 _status = true;
                              }
                           break;
                        }

                     case Command::CN_ERROR:
                        {
                           if (dynamic_cast<errorCommand*>(c)->getErrorCommand() == Command::CN_GINITCONNECTION)
                              {
                                 clientcallback->onSetupError(
                                                              dynamic_cast<errorCommand*>(c)->getMessage()
                                                              );
#if BTG_STATEMACHINE_DEBUG
                                 BTG_NOTICE("Error: " << dynamic_cast<errorCommand*>(c)->getMessage());
#endif // BTG_STATEMACHINE_DEBUG
                                 changeState(SM_ERROR);
                                 _status = true;
                              }
                        }
                     default:
                        {
                           // Got something else, ignore it.
                        }
                     } // switch, command type.

                  delete c;
                  c = 0;
               }
            else
               {
                  counter_transinit++;
                  _status = false;
               }
         }

         void stateMachine::step_SM_TRANSPORT_READY(bool & _status)
         {
            _status = false;
         }

         void stateMachine::step_SM_SETUP(bool & _status)
         {
            Command *c;
            // Pop a command and send it.
            c = this->getCommand();
            if (c != 0)
               {
                  sendCommand(c);
                  delete c;
                  c = 0;
                  changeState(SM_SETUP_WAIT);
                  counter_setup = 0;
               }
            _status = false;
         }

         void stateMachine::step_SM_SETUP_WAIT(bool & _status)
         {
            if (counter_setup >= counter_setup_max)
               {
                  // Nothing to be done. This machine is quite dead.
                  clientcallback->onSetupError("No reply.");
                  changeState(SM_ERROR);
                  _status = true;
                  return;
               }
            Command *c = 0;
            if (receiveCommand(c))
               {
                  switch (c->getType())
                     {
                     case Command::CN_GSETUPRSP:
                        {
                           // Reset the counter.
                           counter_setup = 0;
                           // Call the setup callback.
                           clientcallback->onSetup(dynamic_cast<setupResponseCommand*>(c)->getSession());
                           // Change state, ready to accept commands.
                           changeState(SM_COMMAND);
                           _status = true;
                           return;
                        }
                     case Command::CN_ERROR:
                        {
                           clientcallback->onSetupError(
                                                        dynamic_cast<errorCommand*>(c)->getMessage()
                                                        );
#if BTG_STATEMACHINE_DEBUG
                           BTG_NOTICE("Error: " << dynamic_cast<errorCommand*>(c)->getMessage());
#endif // BTG_STATEMACHINE_DEBUG
                           changeState(SM_ERROR);
                           _status = true;
                           return;
                        }

                     default:
                        {
                           // Do nothing.
                        }
                     }
                  delete c;
               }
            else
               {
                  counter_setup++;
                  _status = false;
               }
         }

         void stateMachine::step_SM_COMMAND(bool & _status)
         {
            // Pop a command and send it.
            Command* c = this->getCommand();
            if (c != 0)
               {
                  // Reset the command counter.
                  counter_command = 0;
                  sendCommand(c);
                  // Set the current command and the expected reply.
                  setCurrentCommand(c);
                  // Set the expected reply and command type if ACK.
                  setExpectedReply(static_cast<Command::commandType>(c->getType()));
                  delete c;
                  c = 0;
                  // Change the state to the waiting state.
                  if (expectedReply[0] == Command::CN_ACK)
                     {
#if BTG_STATEMACHINE_DEBUG
                        BTG_NOTICE("expecting: ACK");
#endif // BTG_STATEMACHINE_DEBUG
                        counter_ack = 0;
                        changeState(SM_COMMAND_ACK_WAIT);
                     }
                  else
                     {
#if BTG_STATEMACHINE_DEBUG
                        BTG_NOTICE("expecting: " << Command::getName(this->expectedReply[0]));

                        if (this->expectedReply[1] != Command::CN_UNDEFINED)
                           {
                              BTG_NOTICE("or " << Command::getName(this->expectedReply[1]));
                           }
#endif // BTG_STATEMACHINE_DEBUG
                        changeState(SM_COMMAND_WAIT);
                     }

                  // Continue to work.
                  _status = false;
               }
            else
               {
                  // No more commands, this is ok.
                  _status = true;
               }
         }

         void stateMachine::step_SM_COMMAND_WAIT(bool & _status)
         {
            if (counter_command >= counter_command_max)
               {
                  // Nothing to be done. This machine is quite dead.
                  changeState(SM_ERROR);
                  _status = true;
                  return;
               }

            Command *c = 0;
            if (receiveCommand(c))
               {
                  // Call a function with the result:
                  if ((c->getType() == expectedReply[0]) || (c->getType() == expectedReply[1]))
                     {
                        // Got the expected reply.
                        counter_command = 0;
                        callCallback(c);
                        delete c;
                        changeState(SM_COMMAND);
                     }
                  else
                     {
                        // Not the expected result.
                        if (c->getType() == Command::CN_ERROR)
                           {
                              callErrorCallback(c);
                              delete c;
                              changeState(SM_COMMAND);
                           }
                        else if (c->getType() == Command::CN_SERROR)
                           {
                              callErrorCallback(c);
                              delete c;
                              changeState(SM_SERROR);
                           }
                     }
               }
            else
               {
                  if (c != 0)
                     {
                        BTG_ERROR_LOG("Not the expected reply: '" << c->getName() << "'");
                     }
                  counter_command++;
               }
         }

         void stateMachine::step_SM_COMMAND_ACK_WAIT(bool & _status)
         {
            if (counter_ack >= counter_ack_max)
               {
                  // Nothing to be done.
                  changeState(SM_ERROR);
                  _status = true;
                  return;
               }

            Command *c = 0;
            if (receiveCommand(c))
               {
                  // Call a function with the result:
                  if (
                      (c->getType() == Command::CN_ACK) &&
                      (dynamic_cast<ackCommand*>(c)->getCommandType() == this->ackForCommand)
                      )
                     {
                        // Got the expected reply.
                        counter_ack = 0;
                        callAckCallback(ackForCommand);
                        changeState(SM_COMMAND);
                        delete c;
                     }
                  else
                     {
                        // Not the expected result.
                        if (c->getType() == Command::CN_ERROR)
                           {
                              clientcallback->onError(dynamic_cast<errorCommand*>(c)->getMessage());
                              changeState(SM_COMMAND);
                              delete c;
                           }
                     }
               }
            else
               {
                  if (c != 0)
                     {
                        BTG_ERROR_LOG("Error: expecting an ack, got '" << c->getName() << "' instead.");
                     }
                  counter_ack++;
               }
         }

         void stateMachine::step_SM_ERROR(bool & _status)
         {
            clientcallback->onFatalError("Waited for an ack. Never got it.");
            _status = true;
         }

         void stateMachine::step_SM_SESSION_LIST(bool & _status)
         {
            // Pop a command and send it.
            Command* c = this->getCommand();
            if (c != 0)
               {
                  sendCommand(c);
                  delete c;
                  c = 0;
                  changeState(SM_SESSION_LIST_WAIT);
                  counter_session_list = 0;
               }
            _status = false;
         }

         void stateMachine::step_SM_SESSION_LIST_WAIT(bool & _status)
         {
            if (counter_session_list >= counter_session_list_max)
               {
                  // Nothing to be done. This machine is quite dead.
                  clientcallback->onSetupError("No reply.");
                  changeState(SM_ERROR);
                  _status = true;
                  return;
               }

            Command *c = 0;
            if (receiveCommand(c))
               {
                  // Got the response.
                  if ((c->getType() == Command::CN_SLISTRSP))
                     {
                        // Reset the counter.
                        counter_session_list = 0;
                        // Call the list sessions callback.
                        clientcallback->onListSessions(
                                                       dynamic_cast<listSessionResponseCommand*>(c)->getSessions());
                        // Change state, ready to accept commands.
                        changeState(SM_TRANSPORT_READY);
                        _status = true;
                     }
                  else if ((c->getType() == Command::CN_ERROR))
                     {
                        // Call the error callback.
                        clientcallback->onListSessionsError(dynamic_cast<errorCommand*>(c)->getMessage());
                        changeState(SM_TRANSPORT_READY);
                        _status = true;
                     }
                  delete c;
               }
            else
               {
                  counter_session_list++;
                  _status = false;
               }
         }

         void stateMachine::step_SM_ATTACH(bool & _status)
         {
            // Pop a command and send it.
            Command* c = this->getCommand();
            if (c != 0)
               {
                  sendCommand(c);
                  delete c;
                  c = 0;
                  changeState(SM_ATTACH_WAIT);
                  counter_setup = 0;
               }
            _status = false;
         }

         void stateMachine::step_SM_ATTACH_WAIT(bool & _status)
         {
            if (counter_attach >= counter_attach_max)
               {
                  // Nothing to be done. This machine is quite dead.
                  changeState(SM_ERROR);
                  _status = true;
                  return;
               }
            Command *c = 0;
            if (receiveCommand(c))
               {
                  // Got one ack.
                  if ((c->getType() == Command::CN_ACK))
                     {
                        // Reset the counter.
                        counter_attach = 0;
                        // Call the setup callback.
                        clientcallback->onAttach();
                        // Change state, ready to accept commands.
                        changeState(SM_COMMAND);
                        _status = true;
                     }
                  else if((c->getType() == Command::CN_ERROR))
                     {
                        // Reset the counter.
                        counter_attach = 0;
                        // Call the setup callback.
                        clientcallback->onAttachError(dynamic_cast<errorCommand*>(c)->getMessage());
                        // Change state, ready to accept commands.
                        changeState(SM_ERROR);
                        _status = true;
                     }
                  delete c;
               }
            else
               {
                  counter_attach++;
                  _status = false;
               }
         }

         void stateMachine::step_SM_DETACH(bool & _status)
         {
            detachSessionCommand* dsc = new detachSessionCommand();
            sendCommand(dsc);
            delete dsc;
            dsc = 0;
            changeState(SM_DETACH_WAIT);
            _status = false;
         }

         void stateMachine::step_SM_DETACH_WAIT(bool & _status)
         {
            Command *c = 0;
            /* wait forever */
            if (receiveCommand(c, true))
               {
                  delete c;
               }
            changeState(SM_FINISHED);
            clientcallback->onDetach();
            _status = true;
         }

         void stateMachine::step_SM_QUIT(bool & _status)
         {
            quitSessionCommand *qsc = new quitSessionCommand();
            sendCommand(qsc);
            delete qsc;
            qsc = 0;
            changeState(SM_QUIT_WAIT);
            _status = false;
         }

         void stateMachine::step_SM_QUIT_WAIT(bool & _status)
         {
            Command *c = 0;
            /* wait forever */
            if (receiveCommand(c, true))
               {
#if BTG_STATEMACHINE_DEBUG
                  BTG_NOTICE("SM_QUIT_WAIT, actual reply is " << c->getName() << ".");
#endif // BTG_STATEMACHINE_DEBUG
                  delete c;
               }
#if BTG_STATEMACHINE_DEBUG
            else
               {

                  BTG_NOTICE("SM_QUIT_WAIT, no reply");
               }
#endif // BTG_STATEMACHINE_DEBUG
            changeState(SM_FINISHED);
            _status = false;
         }

         void stateMachine::step_SM_KILL(bool & _status)
         {
            killCommand* kc = new killCommand();
            sendCommand(kc);
            delete kc;
            kc = 0;
            changeState(SM_KILL_WAIT);
            _status = false;
         }

         void stateMachine::step_SM_KILL_WAIT(bool & _status)
         {
            Command *c = 0;
            if (receiveCommand(c))
               {
                  // If the daemon did not kill itself, go back to the
                  // command state.
                  if (c->getType() == Command::CN_ERROR)
                     {
                        clientcallback->onKillError(dynamic_cast<errorCommand*>(c)->getMessage());
                        changeState(SM_COMMAND);
                        _status = true;
                        delete c;

                        return;
                     }

                  // Received something that is not an error, so
                  // continue.
                  delete c;
               }

            clientcallback->onKill();

            changeState(SM_FINISHED);
            _status = false;
         }

         void stateMachine::step_SM_FINISHED(bool & _status)
         {
            clientcallback->onQuit();
            _status = true;
         }

         stateMachine::~stateMachine()
         {
            t_commandPointerListI iter;
            for(iter = commands.begin();
                iter != commands.end();
                iter++)
               {
                  Command* c = *iter;
                  delete c;
                  c = 0;
               }
         }

      } // namespace client
   } // namespace core
} // namespace btg

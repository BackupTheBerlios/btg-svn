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

#include "eventhandler.h"

#include <bcore/util.h>
#include <bcore/logmacro.h>

#include <bcore/transport/transport.h>

#include <daemon/auth/auth.h>

#if BTG_OPTION_EVENTCALLBACK
#include "callbackmgr.h"
#endif // BTG_OPTION_EVENTCALLBACK

#include "context.h"

#include <bcore/command/context_abort.h>
#include <bcore/command/context_clean.h>
#include <bcore/command/context_create.h>
#include <bcore/command/context_last.h>
#include <bcore/command/context_fi.h>
#include <bcore/command/context_li_status.h>
#include <bcore/command/context_limit.h>
#include <bcore/command/context_peers.h>
#include <bcore/command/context_start.h>
#include <bcore/command/context_status.h>
#include <bcore/command/context_stop.h>
#include <bcore/command/context_file.h>
#include <bcore/command/error.h>

#include <bcore/command/ack.h>
#include <bcore/command/context_clean_rsp.h>
#include <bcore/command/context_fi_rsp.h>
#include <bcore/command/context_li_status_rsp.h>
#include <bcore/command/context_peers_rsp.h>
#include <bcore/command/context_status_rsp.h>
#include <bcore/command/list_rsp.h>

#include <bcore/verbose.h>

#include "connectionextrastate.h"

#include "modulelog.h"

namespace btg
{
   namespace daemon
   {

      using namespace btg::core;
      using namespace std;

      const std::string moduleName("evt");

      eventHandler::eventHandler(bool const _verboseFlag,
                                 std::string const& _username,
                                 std::string const& _tempDir,
                                 std::string const& _workDir,
                                 std::string const& _seedDir,
                                 std::string const& _outputDir,
#if BTG_OPTION_EVENTCALLBACK
                                 std::string const& _callback,
#endif // BTG_OPTION_EVENTCALLBACK
                                 portManager* _portMgr,
                                 limitManager* _limitMgr,
                                 btg::core::externalization::Externalization* _e,
                                 t_long const _session,
                                 messageTransport *_transport,
                                 fileTrack* _filetrack,
                                 IpFilterIf* _filter,
				 bool const _useTorrentName,
                                 connectionHandler* _connectionHandler
#if BTG_OPTION_EVENTCALLBACK
                                 , callbackManager* _cbm
#endif // BTG_OPTION_EVENTCALLBACK
                                 )
         : verboseFlag_(_verboseFlag),
           externalization_(_e),
           session(_session),
           username_(_username),
           seedLimit_(btg::core::limitBase::LIMIT_DISABLED),
           seedTimeout_(btg::core::limitBase::LIMIT_DISABLED),
           useDht_(false),
           transport(_transport),
           daemoncontext(0),
           connHandler(_connectionHandler),
           numClients(1),
#if BTG_OPTION_EVENTCALLBACK
           cbm_(_cbm),
#endif // BTG_OPTION_EVENTCALLBACK
           buffer()
      {
         daemoncontext = new Context(_verboseFlag,
                                     _username,
                                     _tempDir,
                                     _workDir,
                                     _seedDir,
                                     _outputDir,
                                     _portMgr,
                                     _limitMgr,
                                     _filetrack,
                                     _filter,
				     _useTorrentName
#if BTG_OPTION_EVENTCALLBACK
                                     , _cbm
#endif // BTG_OPTION_EVENTCALLBACK
                                     );

#if BTG_OPTION_EVENTCALLBACK
         // Register this user.
         if (_callback != btg::daemon::auth::Auth::callbackDisabled)
            {
               _cbm->add(username_, _callback);
            }
#endif // BTG_OPTION_EVENTCALLBACK

         BTG_MNOTICE("created");
      }

      void eventHandler::event(Command *_command, t_int _connectionID)
      {
         if ((_command == 0))
            {
               return;
            }

         switch (_command->getType())
            {
            case Command::CN_GLIST:
               {
                  handle_CN_GLIST(_connectionID);
                  break;
               }
            case Command::CN_CCREATEWITHDATA:
               {
                  handle_CN_CCREATEWITHDATA(_command, _connectionID);
                  break;
               }
            case Command::CN_CLAST:
               {
                  handle_CN_CLAST(_command, _connectionID);
                  break;
               }
            case Command::CN_CSTART:
               {
                  handle_CN_CSTART(_command, _connectionID);
                  break;
               }
            case Command::CN_CSTOP:
               {
                  handle_CN_CSTOP(_command, _connectionID);
                  break;
               }
            case Command::CN_CABORT:
               {
                  handle_CN_CABORT(_command, _connectionID);
                  break;
               }
            case Command::CN_CCLEAN:
               {
                  handle_CN_CCLEAN(_command, _connectionID);
                  break;
               }
            case Command::CN_CSTATUS:
               {
                  handle_CN_CSTATUS(_command, _connectionID);
                  break;
               }
            case Command::CN_CLIMIT:
               {
                  handle_CN_CLIMIT(_command, _connectionID);
                  break;
               }

            case Command::CN_CSETFILES:
               {
                  handle_CN_CSETFILES(_command, _connectionID);
                  break;
               }

            case Command::CN_CGETFILES:
               {
                  handle_CN_CGETFILES(_command, _connectionID);
                  break;
               }

            case Command::CN_CLIMITSTATUS:
               {
                  handle_CN_CLIMITSTATUS(_command, _connectionID);
                  break;
               }

            case Command::CN_CFILEINFO:
               {
                  handle_CN_CFILEINFO(_command, _connectionID);
                  break;
               }
            case Command::CN_CPEERS:
               {
                  handle_CN_CPEERS(_command, _connectionID);
                  break;
               }
            default:
               {
                  BTG_MERROR("event: unhandled command");
                  sendError(_connectionID, Command::CN_UNDEFINED, "Unhandled command.");
               }
            }
      }

      bool eventHandler::setup(const daemonConfiguration* _config,
                               setupCommand *_setupcommand)
      {
         BTG_MNOTICE("setup, using:" << _setupcommand->toString());

         seedLimit_   = _setupcommand->getRequiredData().getSeedLimit();
         seedTimeout_ = _setupcommand->getRequiredData().getSeedTimeout();
         useDht_      = _setupcommand->getRequiredData().useDHT();

         return daemoncontext->setup(_config, 
                                     _setupcommand->getRequiredData());
      }

      void eventHandler::sendCommand(t_int _connectionID, Command *_command)
      {
         buffer.erase();

         bool status = true;
         switch (_command->getType())
            {
            case Command::CN_GLISTRSP:
            case Command::CN_ERROR:
            case Command::CN_ACK:
            case Command::CN_CSTATUSRSP:
            case Command::CN_CALLSTATUSRSP:
            case Command::CN_CFILEINFORSP:
            case Command::CN_CCLEANRSP:
            case Command::CN_CLIMITSTATUSRSP:
            case Command::CN_CPEERSRSP:
            case Command::CN_CLASTRSP:
            case Command::CN_CGETFILESRSP:
               {
                  externalization_->reset();
                  status = _command->serialize(externalization_);
                  externalization_->setDirection(FROM_SERVER);
                  if (status)
                     {
                        externalization_->getBuffer(buffer);

                        // Verbose logging:

                        if (verboseFlag_)
                           {
                              if (_command->getType() == Command::CN_ERROR)
                                 {
                                    // If sending an error, write which command the
                                    // error was caused by.

                                    std::string causedBy = Command::getName(
                                                                            dynamic_cast<errorCommand*>(_command)->getErrorCommand()
                                                                            );

                                    VERBOSE_LOG(verboseFlag_, "daemon (" << _connectionID << "): " <<
                                                _command->getName() << " (caused by " << causedBy << ").");
                                 }
                              else
                                 {
                                    VERBOSE_LOG(verboseFlag_, "daemon (" << _connectionID << "): " <<
                                                _command->getName() << ".");
                                 }
                           }

                        BTG_MNOTICE("sending " << _command->getName() <<
                                   " (connection " << _connectionID << ", " <<
                                   buffer.size() << " bytes" << ")");
                     }
                  break;
               }
            default:
               {
                  BTG_MERROR("sendcommand: unhandled command");
               }

            } // switch

         if (status)
            {
               transport->write(buffer, _connectionID);
            }
         else
            {
               BTG_MNOTICE("attempt to serialize " << _command->getName() << " failed.");
            }

         delete _command;
      }

      void eventHandler::sendAckOrError(t_int _connectionID,
                                        bool _opStatus,
                                        btg::core::Command::commandType _type,
                                        std::string const& _errorDescription)
      {
         switch(_opStatus)
            {
            case true:
               {
                  this->sendAck(_connectionID, _type);
                  break;
               }
            case false:
               {
                  this->sendError(_connectionID, _type, _errorDescription);
                  break;
               }
            }
      }

      void eventHandler::sendAck(t_int _connectionID, Command::commandType _type)
      {
         sendCommand(_connectionID, new ackCommand(_type));
      }

      void eventHandler::sendError(t_int _connectionID, Command::commandType _type, string const& _error)
      {
         sendCommand(_connectionID, new errorCommand(_type, _error));
      }

      void eventHandler::incClients()
      {
         numClients++;
         BTG_MNOTICE("session " << session << " increased numClients to " << numClients << ".");
      }

      void eventHandler::decClients()
      {
         numClients--;
         BTG_MNOTICE("session " << session << " decreased numClients to " << numClients << ".");
      }

      t_uint eventHandler::getNumClients()
      {
         return numClients;
      }

      t_long eventHandler::getSession() const
      {
         return session;
      }

      void eventHandler::checkSeedLimits()
      {
         daemoncontext->checkSeedLimits();
      }

     void eventHandler::updateElapsedOrSeedCounter()
     {
       daemoncontext->updateElapsedOrSeedCounter();
     }

      void eventHandler::handleAlerts()
      {
         daemoncontext->handleAlerts();
      }

#if BTG_OPTION_SAVESESSIONS
      void eventHandler::serialize(btg::core::externalization::Externalization* _e, 
                                   bool const _dumpFastResume)
      {
         _e->longToBytes(&session);
         _e->stringToBytes(&username_);
         _e->intToBytes(&seedLimit_);
         _e->longToBytes(&seedTimeout_);

         daemoncontext->serialize(_e, _dumpFastResume);
      }

      bool eventHandler::deserialize(btg::core::externalization::Externalization* _e, t_uint _version)
      {
         // Parent already loaded us with session id and seed
         // limit&timeout when we were created Constructor however
         // sets numClients to 1.. Change to 0.
         numClients = 0;
         return daemoncontext->deserialize(_e, _version);
      }
#endif // BTG_OPTION_SAVESESSIONS

      void eventHandler::handle_CN_GLIST(t_int _connectionID)
      {
         vector<t_int> list = daemoncontext->getContexts();
         if (list.size() > 0)
            {
               vector<string> filenames;
               vector<t_int>::const_iterator ci;
               string s;
               for (ci = list.begin(); ci != list.end(); ci++)
                  {
                     // BROKEN: this locks libtorrent!
                     if (daemoncontext->getFilename(*ci, s))
                        {
                           filenames.push_back(s);
                        }
                     else
                        {
                           filenames.push_back("Undefined");
                        }
                  }
               sendCommand(_connectionID, new listCommandResponse(list, filenames));
            }
         else // Empty list:
            {
               sendError(_connectionID, Command::CN_GLIST, "No contexts to list.");
            }
      }

      void eventHandler::handle_CN_CCREATEWITHDATA(btg::core::Command *_command, t_int _connectionID)
      {
         contextCreateWithDataCommand* ccwdc = dynamic_cast<contextCreateWithDataCommand*>(_command);
	 t_int handle_id = -1;
         Context::addResult res = daemoncontext->add(ccwdc->getFilename(),
						     ccwdc->getFile(),
						     handle_id);

         ConnectionExtraState& extraState = this->connHandler->getConnection(_connectionID)->ExtraState();
         extraState.setLastCreatedContextId(handle_id);

	 if (!ccwdc->getStart())
	   {
	     // Not starting torrents automagically after adding.
	     if (res == Context::ERR_OK)
	       {
		 daemoncontext->stop(handle_id);
	       }
	   }

         switch(res)
            {
            case Context::ERR_OK:
               {
                  sendCommand(_connectionID, new ackCommand(Command::CN_CCREATEWITHDATA));
                  break;
               }
            case Context::ERR_EXISTS:
               {
                  sendCommand(_connectionID, new errorCommand(Command::CN_CCREATEWITHDATA,
                                                              "Adding '" + ccwdc->getFilename() +
                                                              "' failed, torrent already exists"));
                  break;
               }
            case Context::ERR_LIBTORRENT:
               {
                  sendCommand(_connectionID, new errorCommand(Command::CN_CCREATEWITHDATA,
                                                              "Adding '" + ccwdc->getFilename() +
                                                              "' failed, libtorrent didnt like the file."));
                  break;
               }
            default:
               {
                  sendCommand(_connectionID, new errorCommand(Command::CN_CCREATEWITHDATA,
                                                              "Adding '"+ccwdc->getFilename()+"' failed."));
               }
            }
      }

      void eventHandler::handle_CN_CLAST(btg::core::Command *_command, t_int _connectionID)
      {
         ConnectionExtraState& extraState = this->connHandler->getConnection(_connectionID)->ExtraState();
         t_int last_context_id = extraState.getLastCreatedContextId();
         sendCommand(_connectionID, new btg::core::lastCIDResponseCommand(last_context_id));
      }

      void eventHandler::handle_CN_CSTART(btg::core::Command *_command, t_int _connectionID)
      {
         contextStartCommand *cstac = dynamic_cast<contextStartCommand*>(_command);
         bool op_status = false;

         if (cstac->isAllContextsFlagSet())
            {
               op_status = daemoncontext->startAll();
            }
         else
            {
               op_status = daemoncontext->start(cstac->getContextId());
            }

         if (op_status)
            {
               sendCommand(_connectionID, new ackCommand(Command::CN_CSTART));
            }
         else
            {
               if (cstac->isAllContextsFlagSet())
                  {
                     sendError(_connectionID, Command::CN_CSTART, "No contexts to start.");
                  }
               else
                  {
                     sendError(_connectionID, Command::CN_CSTART, "Wrong context id.");
                  }
            }
      }

      void eventHandler::handle_CN_CSTOP(btg::core::Command *_command, t_int _connectionID)
      {
         contextStopCommand *cstoc = dynamic_cast<contextStopCommand*>(_command);
         bool op_status            = false;

         if (cstoc->isAllContextsFlagSet())
            {
               op_status = daemoncontext->stopAll();
            }
         else
            {
               op_status = daemoncontext->stop(cstoc->getContextId());
            }

         if (op_status)
            {
               sendCommand(_connectionID, new ackCommand(Command::CN_CSTOP));
            }
         else
            {
               sendError(_connectionID, Command::CN_CSTOP, "Wrong context id.");
            }
      }

      void eventHandler::handle_CN_CABORT(btg::core::Command *_command, t_int _connectionID)
      {
         contextAbortCommand *cac = dynamic_cast<contextAbortCommand*>(_command);
         bool op_status           = false;

         if (cac->isAllContextsFlagSet())
            {
               op_status = daemoncontext->removeAll(cac->eraseData());
            }
         else
            {
               op_status = daemoncontext->remove(cac->getContextId(), cac->eraseData());
            }

         if (op_status)
            {
               sendCommand(_connectionID, new ackCommand(Command::CN_CABORT));
            }
         else
            {
               sendError(_connectionID, Command::CN_CABORT, "Wrong context id.");
            }
      }

      void eventHandler::handle_CN_CCLEAN(btg::core::Command *_command, t_int _connectionID)
      {
         t_strList files;
         t_intList contextIDs;

         contextCleanCommand *ccc = dynamic_cast<contextCleanCommand*>(_command);
         bool op_status           = false;

         if (ccc->isAllContextsFlagSet())
            {
               op_status = daemoncontext->clean(files, contextIDs);
            }
         else
            {
               op_status = daemoncontext->clean(ccc->getContextId(), files, contextIDs);
            }

         if (op_status)
            {
               sendCommand(_connectionID, new contextCleanResponseCommand(files, contextIDs));
            }
         else
            {
               sendError(_connectionID, Command::CN_CCLEANRSP, "Nothing to clean.");
            }
      }

      void eventHandler::handle_CN_CSTATUS(btg::core::Command *_command, t_int _connectionID)
      {
         contextStatusCommand *csc = dynamic_cast<contextStatusCommand*>(_command);
         bool op_status            = false;
         Status status;
         std::vector<Status> v_status;

         if (csc->isAllContextsFlagSet())
            {
               // Client wants status of all contexts.
               op_status = daemoncontext->getStatusAll(v_status);
            }
         else
            {
               op_status = daemoncontext->getStatus(csc->getContextId(), status);
            }

         if (!op_status)
            {
               sendError(_connectionID, Command::CN_CSTATUS, "Wrong context id.");
            }
         else
            {
               if (csc->isAllContextsFlagSet())
                  {
                     // All contexts.
#if BTG_DEBUG
                     BTG_MNOTICE("sending list of status objects.");
                     vector<Status>::const_iterator iter;
                     for (iter = v_status.begin(); iter != v_status.end(); iter++)
                        {
                           BTG_MNOTICE("status: " << iter->toString());
                        }
#endif
                     sendCommand(_connectionID, new contextAllStatusResponseCommand(csc->getContextId(), v_status));
                  }
               else
                  {
                     // One context.
#if BTG_DEBUG
                     BTG_MNOTICE("sending status back: " << status.toString() << ".");
#endif
                     sendCommand(_connectionID, new contextStatusResponseCommand(csc->getContextId(), status));
                  }
            }
      }

      void eventHandler::handle_CN_CLIMIT(btg::core::Command *_command, t_int _connectionID)
      {
         contextLimitCommand *clc = dynamic_cast<contextLimitCommand*>(_command);
         if (clc->isAllContextsFlagSet())
            {
               // Remove limit from all contexts.
               bool removeDownloadFlag    = false;
               bool removeUploadFlag      = false;
               bool removeSeedLimitFlag   = false;
               bool removeSeedTimeoutFlag = false;

               if (clc->getUploadLimit() == btg::core::limitBase::LIMIT_DISABLED)
                  {
                     removeUploadFlag   = true;
                  }

               if (clc->getDownloadLimit() == btg::core::limitBase::LIMIT_DISABLED)
                  {
                     removeDownloadFlag = true;
                  }

               if (clc->getSeedLimit() == btg::core::limitBase::LIMIT_DISABLED)
                  {
                     removeSeedLimitFlag = true;
                  }

               if (clc->getSeedTimeout() == btg::core::limitBase::LIMIT_DISABLED)
                  {
                     removeSeedTimeoutFlag = true;
                  }

               if (removeDownloadFlag  ||
                   removeUploadFlag    ||
                   removeSeedLimitFlag ||
                   removeSeedTimeoutFlag)
                  {
                     // Clear the limit for all contexts.
                     daemoncontext->removeLimitAll(removeUploadFlag,
                                                   removeDownloadFlag,
                                                   removeSeedLimitFlag,
                                                   removeSeedTimeoutFlag);
                  }

               // Remove upload and download limit, send an ack back.
               if (removeDownloadFlag && removeUploadFlag && removeSeedLimitFlag && removeSeedTimeoutFlag)
                  {
                     sendAck(_connectionID, Command::CN_CLIMIT);
                     return;
                  }

               // Set limit for all contexts.
               sendAckOrError(_connectionID,
                              daemoncontext->limitAll(clc->getUploadLimit(),
                                                      clc->getDownloadLimit(),
                                                      clc->getSeedLimit(),
                                                      clc->getSeedTimeout()),
                              Command::CN_CLIMIT,
                              "Could not limit context.");
            }
         else
            {
               // Limit a single context.

               bool removeDownloadFlag    = false;
               bool removeUploadFlag      = false;
               bool removeSeedLimitFlag   = false;
               bool removeSeedTimeoutFlag = false;

               if (clc->getUploadLimit() == btg::core::limitBase::LIMIT_DISABLED)
                  {
                     removeUploadFlag   = true;
                  }

               if (clc->getDownloadLimit() == btg::core::limitBase::LIMIT_DISABLED)
                  {
                     removeDownloadFlag = true;
                  }

               if (clc->getSeedLimit() == btg::core::limitBase::LIMIT_DISABLED)
                  {
                     removeSeedLimitFlag = true;
                  }

               if (clc->getSeedTimeout() == btg::core::limitBase::LIMIT_DISABLED)
                  {
                     removeSeedTimeoutFlag = true;
                  }

               // Remove limit from a context.
               if (removeDownloadFlag  ||
                   removeUploadFlag    ||
                   removeSeedLimitFlag ||
                   removeSeedTimeoutFlag)
                  {
                     // Clear the limit for all contexts.
                     daemoncontext->removeLimit(clc->getContextId(),
                                                removeUploadFlag,
                                                removeDownloadFlag,
                                                removeSeedLimitFlag,
                                                removeSeedTimeoutFlag);
                  }

               // Remove all limits.
               if (removeDownloadFlag && removeUploadFlag && removeSeedLimitFlag && removeSeedTimeoutFlag)
                  {
                     sendAck(_connectionID, Command::CN_CLIMIT);
                     return;
                  }

               // Set limit for a context.
               sendAckOrError(_connectionID,
                              daemoncontext->limit(clc->getContextId(),
                                                   clc->getUploadLimit(),
                                                   clc->getDownloadLimit(),
                                                   clc->getSeedLimit(),
                                                   clc->getSeedTimeout()),
                              Command::CN_CLIMIT,
                              "Could not limit context.");
            }

      }

      void eventHandler::handle_CN_CLIMITSTATUS(btg::core::Command *_command, t_int _connectionID)
      {
         // Attempt to get the limits, set by the client, for a context.

         contextLimitStatusCommand *cls = dynamic_cast<contextLimitStatusCommand*>(_command);

         if (cls->isAllContextsFlagSet())
            {
               // Not supported, no need, it seems.
               sendError(_connectionID, Command::CN_CLIMITSTATUS, "Not implemented.");
            }
         else
            {
               t_int limitUpld    = 0;
               t_int limitDwnld   = 0;
               t_int seedLimit    = 0;
               t_long seedTimeout = 0;

               if (daemoncontext->getLimit(
                                           cls->getContextId(),
                                           limitUpld,
                                           limitDwnld,
                                           seedLimit,
                                           seedTimeout
                                           ))
                  {
                     sendCommand(_connectionID, new contextLimitStatusResponseCommand(cls->getContextId(),
                                                                                      limitUpld,
                                                                                      limitDwnld,
                                                                                      seedLimit,
                                                                                      seedTimeout));
                  }
               else
                  {
                     sendError(_connectionID, Command::CN_CLIMITSTATUS, "Could not obtain limit status.");
                  }
            }

      }

      void eventHandler::handle_CN_CFILEINFO(btg::core::Command *_command, t_int _connectionID)
      {
         bool op_status                = false;
         contextFileInfoCommand *cfic  = dynamic_cast<contextFileInfoCommand*>(_command);

         t_fileInfoList  fileinfolist;

         if (cfic->isAllContextsFlagSet())
            {
               // Client wants status file info for all contexts.
               // This will generate large amount of data, so not
               // supported for now.
               op_status = false;
               sendError(_connectionID, Command::CN_CFILEINFO, "The all context flag is not supported.");
               return;
            }
         else
            {
               op_status = daemoncontext->getFileInfo(cfic->getContextId(), fileinfolist);
               switch(op_status)
                  {
                  case true:
                     sendCommand(_connectionID, new contextFileInfoResponseCommand(cfic->getContextId(), fileinfolist));
                     break;
                  case false:
                     sendError(_connectionID, Command::CN_CFILEINFO, "Failed to obtain file information.");
                     break;
                  }
            }
      }

      void eventHandler::handle_CN_CSETFILES(btg::core::Command* _command, t_int _connectionID)
      {
         contextSetFilesCommand* csfc = dynamic_cast<contextSetFilesCommand*>(_command);

         if (csfc->isAllContextsFlagSet())
            {
               // Not supported
               sendError(_connectionID, 
                         Command::CN_CSETFILES, 
                         "The all context flag is not supported.");
               return;
            }

         sendAckOrError(_connectionID, daemoncontext->setSelectedFiles(csfc->getContextId(),
                                                                       csfc->getFiles()),
                        Command::CN_CSETFILES,
                        "Could not set files.");
      }

      void eventHandler::handle_CN_CGETFILES(btg::core::Command* _command, t_int _connectionID)
      {
         contextGetFilesCommand* cgfc = dynamic_cast<contextGetFilesCommand*>(_command);

         if (cgfc->isAllContextsFlagSet())
            {
               // Not supported
               sendError(_connectionID, 
                         Command::CN_CGETFILES, 
                         "The all context flag is not supported.");
               return;
            }

         selectedFileEntryList file_list;
         
         bool op_status = daemoncontext->getSelectedFiles(cgfc->getContextId(), 
                                                          file_list);
         switch(op_status)
            {
            case true:
               {
                  sendCommand(_connectionID, 
                                    new contextGetFilesResponseCommand(cgfc->getContextId(),
                                                                       file_list));
                  break;
               }
            case false:
               {
                  sendError(_connectionID, 
                            Command::Command::CN_CGETFILES, 
                            "Failed to obtain selected files.");
                  break;
               }
            }
      }

      void eventHandler::handle_CN_CPEERS(btg::core::Command *_command, t_int _connectionID)
      {
         bool op_status            = false;
         contextPeersCommand* cpc  = dynamic_cast<contextPeersCommand*>(_command);

         t_peerList peerlist;

         // Get a list of peers.
         if (cpc->isAllContextsFlagSet())
            {
               // Client wants peer file info for all contexts.
               // This will generate large amount of data, so not
               // supported for now.
               op_status = false;
               sendError(_connectionID, Command::CN_CPEERS, "The all context flag is not supported.");
               return;
            }
         else
            {
               op_status = daemoncontext->getPeers(cpc->getContextId(), peerlist);

               switch(op_status)
                  {
                  case true:
                     sendCommand(_connectionID, new contextPeersResponseCommand(
                                                                                cpc->getContextId(),
                                                                                peerlist));
                     break;
                  case false:
                     sendError(_connectionID, Command::CN_CPEERS, "Failed to obtain peer information.");
                     break;
                  }
            }
      }

      void eventHandler::shutdown()
      {
         daemoncontext->shutdown();
      }

      eventHandler::~eventHandler()
      {
         BTG_MENTER("destructor", "");
         delete daemoncontext;
         daemoncontext = 0;
#if BTG_OPTION_EVENTCALLBACK
         cbm_->remove(username_);
#endif // BTG_OPTION_EVENTCALLBACK
         BTG_MEXIT("destructor", "");
      }

      std::string eventHandler::getUsername() const
      { 
         return username_;
      }

   } // namespace daemon
} // namespace btg

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

#include "daemonhandler.h"
#include "darg.h"

#include <bcore/os/fileop.h>

#include <bcore/command/ack.h>
#include <bcore/command/error.h>
#include <bcore/command/initconnection.h>
#include <bcore/command/session_attach.h>
#include <bcore/command/session_list_rsp.h>
#include <bcore/command/session_name.h>
#include <bcore/command/session_rw.h>
#include <bcore/command/session_info.h>
#include <bcore/command/uptime.h>
#include <bcore/command/limit.h>
#include <bcore/command/version.h>
#include <bcore/command/setting.h>
#include <bcore/command/opstat.h>
#include <bcore/command/context_move.h>
#include <bcore/command/context_create.h>
#include <bcore/command/context_create_url.h>
#include <bcore/verbose.h>
#include <bcore/opstatus.h>
#include <bcore/t_string.h>

#include "modulelog.h"

#include <bcore/btg_assert.h>

extern int global_btg_run;

namespace btg
{
   namespace daemon
   {

      using namespace btg::core;

      const std::string moduleName("hdl");

      typedef std::map<t_long, eventHandler*> handlerMap;

      daemonHandler::daemonHandler(btg::core::LogWrapperType _logwrapper,
                                   daemonData* _dd,
                                   bool const _verboseFlag)
         : btg::core::Logable(_logwrapper),
           dd_(_dd),
           verboseFlag_(_verboseFlag),
           sessionlist_(_logwrapper, _verboseFlag, 1024*1024 /* Max number of sessions. */),
           session_(0),
           readBytes_(0),
           connectionID_(btg::core::messageTransport::NO_CONNECTION_ID),
           connection_(0),
           handlerCount_(0),
           buffer_(),
           session_timer_(10), /* 10 seconds. */
           session_timer_trigger_(false),
#if BTG_OPTION_URL
           url_timer_(5),
           url_timer_trigger_(false),
#endif
           file_timer_(5),
           file_timer_trigger_(false),
           limit_timer_(30), /* 30 seconds. */
           limit_timer_trigger_(false),
           elapsed_seed_timer_(60), /* 1 minute. */
           elapsed_timer_trigger_(false),
           periodic_ssave_timer_(_dd->config->getSSTimeout()),
           periodic_ssave_timer_trigger_(false),
#if BTG_DEBUG
           aliveCounter_(0),
           aliveCounterMax_(5),
#endif // BTG_DEBUG
           portManager_(_logwrapper, _verboseFlag, _dd->portRange),
           limitManager_(_logwrapper, _verboseFlag, limit_timer_.maxValue(),
                         _dd->config->getUploadRateLimit(),
                         _dd->config->getDownloadRateLimit(),
                         _dd->config->getMaxUploads(),
                         _dd->config->getMaxConnections()),
#if BTG_OPTION_SAVESESSIONS
           sessionsaver_(_logwrapper,
                         _verboseFlag,
                         portManager_,
                         limitManager_,
                         sessionlist_,
                         *dd_),
           sessiontimer_(dd_->ss_timeout),
#endif // BTG_OPTION_SAVESESSIONS
           sendBuffer_(),
           cf_(_logwrapper, *_dd->externalization),
#if BTG_OPTION_URL
           urlmgr(_logwrapper, _verboseFlag, _dd->filetrack, &sessionlist_, opid),
#endif
           filemgr(_logwrapper, _dd->filetrack, opid),
           opid(_logwrapper)
      {
         /// Set the initial limits.
         limitManager_.set(_dd->config->getUploadRateLimit(),
                           _dd->config->getDownloadRateLimit(),
                           _dd->config->getMaxUploads(),
                           _dd->config->getMaxConnections());

         limitManager_.update();
#if BTG_OPTION_SAVESESSIONS
         if (dd_->ss_enable)
            {
               if (!dd_->cla->doNotReloadSessions())
                  {
                     MVERBOSE_LOG(logWrapper(), verboseFlag_, "Loading sessions from stream.");
                     BTG_MNOTICE(logWrapper(), "loading sessions from stream.");
                     sessionsaver_.loadSessions(dd_->ss_file);
                  }
               else
                  {
                     BTG_MNOTICE(logWrapper(), "session reloading disabled on command line, skipping reload");
                  }
            }
         else
            {
               MVERBOSE_LOG(logWrapper(), verboseFlag_, "Sesssion saving disabled.");
               BTG_MNOTICE(logWrapper(), "sesssion saving disabled");
            }
#endif // BTG_OPTION_SAVESESSIONS
      }

      void daemonHandler::readFromTransport()
      {
         session_      = 0;
         connectionID_ = btg::core::messageTransport::NO_CONNECTION_ID;

         checkDeadConnections();

         // Read a message.
         buffer_.erase();
#if BTG_TRANSPORT_DEBUG
         BTG_MNOTICE(logWrapper(), "readFromTransport, reading from transport");
#endif // BTG_TRANSPORT_DEBUG
         readBytes_ = dd_->transport->read(buffer_, connectionID_);
#if BTG_TRANSPORT_DEBUG
         BTG_MNOTICE(logWrapper(), "readFromTransport, " << readBytes_ << " bytes read from connection " << connectionID_);
#endif // BTG_TRANSPORT_DEBUG

         if (readBytes_ > 0)
            {
               connection_ = dd_->connHandler->getConnection(connectionID_);

               if (connection_ == 0)
                  {
                     BTG_MNOTICE(logWrapper(), "new connection " << connectionID_);
                     dd_->connHandler->addConnection(connectionID_);
                     connection_ = dd_->connHandler->getConnection(connectionID_);
                  }

               BTG_MNOTICE(logWrapper(),
                           "got " << readBytes_ << " bytes from " << connection_->toString());
               BTG_MNOTICE(logWrapper(),
                           "data (in): " << buffer_.size() << " bytes");

               if (!dd_->externalization->setBuffer(buffer_))
                  {
                     // This buffer is also used for sending messages, so erase
                     // the contents.
                     buffer_.erase();

                     // Unable to set the buffer to be decoded. No
                     // point in doing anything else.
                     return;
                  }

               btg::core::commandFactory::decodeStatus dstatus;
               btg::core::Command* currentCmd = cf_.createFromBytes(dstatus);

               // This buffer is also used for sending messages, so erase
               // the contents.
               buffer_.erase();

               if ((dstatus != btg::core::commandFactory::DS_OK) || (currentCmd == 0))
                  {
                     handleDeserializationError(dstatus);
                     return;
                  }

               session_ = connection_->getSession();

               BTG_MNOTICE(logWrapper(), "attempting to handle " << currentCmd->getName() << " with " << connection_->toString());

               // Try to find an eventhandler for this session.
               // We dont have to check auth here, since the connecton
               // wont have an associated session if he havent been
               // authed (setup and attach cannot be done without
               // auth).

               eventHandler* eventhandler = 0;

               if (sessionlist_.get(session_, eventhandler))
                  {
                     switch (currentCmd->getType())
                        {
                        case Command::CN_SQUIT:
                           {
                              // Special case, where the client wishes to close
                              // the session.
                              handleQuit(eventhandler, currentCmd);
                              break;
                           }
                        case Command::CN_SLIST:
                           {
                              handleSessionList(currentCmd);
                              break;
                           }
                        case Command::CN_SDETACH:
                           {
                              handleDetach(eventhandler, currentCmd);
                              break;
                           }
                        case Command::CN_GKILL:
                        case Command::CN_GLIMIT:
                           {
                              handleControlCommand(currentCmd);
                              break;
                           }
                        case Command::CN_GLIMITSTAT:
                           {
                              handleGlobalStatus(currentCmd);
                              break;
                           }
                        case Command::CN_GUPTIME:
                           {
                              handleUptime(currentCmd);
                              break;
                           }
                        case Command::CN_SNAME:
                           {
                              handleSessionName(eventhandler, currentCmd);
                              break;
                           }
                        case Command::CN_SSETNAME:
                           {
                              handleSessionSetName(eventhandler, currentCmd);
                              break;
                           }
                        case Command::CN_SINFO:
                           {
                              handleSessionInfo(eventhandler, currentCmd);
                           }
                        case Command::CN_MOREAD:
                           {
                              break;
                           }
                        case Command::CN_MOWRITE:
                           {
                              break;
                           }
                        case Command::CN_CMOVE:
                           {
                              handleMoveContext(eventhandler, currentCmd);
                              break;
                           }

                        case Command::CN_OPABORT:
                        case Command::CN_OPSTATUS:
                           {
                              handleOpMessages(eventhandler, currentCmd);
                              break;
                           }

                        case Command::CN_CCREATEFROMURL:
                           {
                              handleUrlMessages(eventhandler, currentCmd);
                              break;
                           }

                        case Command::CN_CCREATEFROMFILE:
                        case Command::CN_CCREATEFROMFILEPART:
                           {
                              handleCreateFileMessages(eventhandler, currentCmd);
                              break;
                           }

                        case Command::CN_VERSION:
                           {
                              handleVersion(currentCmd);
                              break;
                           }
                        case Command::CN_GETSETTING:
                           {
                              handleGetSetting(currentCmd);
                              break;
                           }
                        default:
                           {
                              handleOther(eventhandler, currentCmd);
                           }
                        } // switch special commands.

                  }
               // No session is established at this point.
               else if (currentCmd->getType() == Command::CN_GINITCONNECTION)
                  {
                     handleInitConnection(currentCmd);
                  }
               else
                  {
                     // No session is established at this point.

                     // This connection is not associated with any session (yet), and we did not receive
                     // a initConnection command. This means that it has to be authed to proceed.
                     // Check for auth.
                     if (!connection_->isAuthed())
                        {
                           // User is not authorized to do attach/list/setup.

                           MVERBOSE_LOG(logWrapper(), verboseFlag_, "Client (" << connectionID_ << ") is not authorized.");

                           sendError(currentCmd->getType(), "Not authorized.");
                        }
                     else
                        {
                           // User is authed.

                           // ***************
                           // No session yet.
                           // ***************

                           switch(currentCmd->getType())
                              {
                              case Command::CN_SATTACH:
                                 {
                                    handleAttach(currentCmd);
                                    break;
                                 }
                              case Command::CN_SLIST:
                                 {
                                    handleSessionList(currentCmd);
                                    break;
                                 }
                              case Command::CN_GSETUP:
                                 {
                                    handleSetup(currentCmd);
                                    break;
                                 }
                              case Command::CN_SNAME:
                              case Command::CN_SNAMERSP:
                              case Command::CN_SSETNAME:
                                 {
                                    // These commands are not supported here.
                                    // Send an nice error back.
                                    handleSessionInInvalidState(currentCmd->getType());
                                    break;
                                 }
                              }
                        } // End if !authed...
                  }

               delete currentCmd;
               currentCmd = 0;
            }

#if BTG_DEBUG
         aliveCounter_++;
         if (aliveCounter_ > aliveCounterMax_)
            {
               aliveCounter_ = 0;
               BTG_MNOTICE(logWrapper(), "alive");
            }
#endif // BTG_DEBUG

      }

      void daemonHandler::handleQuit(eventHandler* _eventhandler, Command* _command)
      {
         // Dont allow detach if we have more clients than this one.
         if(_eventhandler->getNumClients() > 1)
            {
               sendError(_command->getType(), "Cannot quit, other clients attached.");
            }
         else
            {
               BTG_MNOTICE(logWrapper(), "terminating session " << session_);

               MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");

               // Remove the session data.
               sessionlist_.erase(session_);

               // Send an ack message back, as the session was deleted.
               sendAck(Command::CN_SQUIT);
               // sendCommand(dd_->externalization, dd_->transport, connectionID_, new ackCommand(Command::CN_SQUIT));
               connection_->setSession(0);
               dd_->transport->endConnection(connectionID_);
            }
      }

      void daemonHandler::handleDetach(eventHandler* _eventhandler, Command* _command)
      {
         BTG_MNOTICE(logWrapper(), "detaching the current session");
         _eventhandler->decClients();

         // Send an ack message back, as the session was detached.
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");
         sendAck(Command::CN_SDETACH);
         connection_->setSession(0);
         dd_->transport->endConnection(connectionID_);
      }

      void daemonHandler::handleControlCommand(btg::core::Command* _command)
      {
         t_int id         = _command->getType();
         std::string user = connection_->getUsername();

         bool control_flag = false;

         if (!dd_->auth->getControlFlag(user, control_flag))
            {
               sendError(id, "Control not allowed.");
               return;
            }

         if (!control_flag)
            {
               sendError(id, "Control not allowed.");
               return;
            }

         switch (id)
            {
            case Command::CN_GKILL:
               {
                  handleKill(_command);
                  break;
               }
            case Command::CN_GLIMIT:
               {
                  handleGlobalLimit(_command);
                  break;
               }
            default:
               {
                  sendError(id, "Control command not implemented.");
                  break;
               }
            }
      }

      void daemonHandler::handleKill(Command* _command)
      {
         BTG_MNOTICE(logWrapper(), "killing the daemon");
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");

         sendAck(Command::CN_GKILL);
         dd_->transport->endConnection(connectionID_);

         global_btg_run = 0;
      }

      void daemonHandler::validateGlobalLimits(t_int & _limitBytesUpld,
                                               t_int & _limitBytesDwnld,
                                               t_int & _maxUplds,
                                               t_long & _maxConnections) const
      {
         validateLimit<t_int>(_limitBytesUpld);
         validateLimit<t_int>(_limitBytesDwnld);
         validateLimit<t_int>(_maxUplds);
         validateLimit<t_long>(_maxConnections);
      }

      void daemonHandler::handleGlobalLimit(Command* _command)
      {
         BTG_MNOTICE(logWrapper(), "Setting global limits");
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");

         limitCommand* lc = dynamic_cast<limitCommand*>(_command);

         t_int limitBytesUpld   = lc->getUploadLimit();
         t_int limitBytesDwnld  = lc->getDownloadLimit();
         t_int maxUplds         = lc->getMaxUplds();
         t_long maxConnections  = lc->getMaxConnections();

         validateGlobalLimits(limitBytesUpld, limitBytesDwnld, maxUplds, maxConnections);

         limitManager_.set(limitBytesUpld, limitBytesDwnld, maxUplds, maxConnections);

         // Ack the limit command.
         sendAck(Command::CN_GLIMIT);
      }

      void daemonHandler::handleGlobalStatus(Command* _command)
      {
         BTG_MNOTICE(logWrapper(), "Global limits request");

         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");

         t_int limitBytesUpld  = btg::core::limitBase::LIMIT_DISABLED;
         t_int limitBytesDwnld = btg::core::limitBase::LIMIT_DISABLED;
         t_int maxUplds        = btg::core::limitBase::LIMIT_DISABLED;
         t_long maxConnections = btg::core::limitBase::LIMIT_DISABLED;

         limitManager_.get(limitBytesUpld,
                           limitBytesDwnld,
                           maxUplds,
                           maxConnections);

         sendCommand(dd_->externalization,
                     dd_->transport,
                     connectionID_,
                     new limitStatusResponseCommand(limitBytesUpld,
                                                    limitBytesDwnld,
                                                    maxUplds,
                                                    maxConnections));
      }

      void daemonHandler::handleUptime(Command* _command)
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");
         buffer_.erase();

         time_t now;
         time(&now);

         t_ulong timeDiff = now - dd_->daemonStartTime;

         sendCommand(dd_->externalization,
                     dd_->transport,
                     connectionID_,
                     new uptimeResponseCommand(timeDiff));
      }

      void daemonHandler::handleSessionName(eventHandler* _eventhandler, Command* _command)
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");

         sendCommand(dd_->externalization,
                     dd_->transport,
                     connectionID_,
                     new sessionNameResponseCommand(_eventhandler->getName()));
      }

      void daemonHandler::handleSessionSetName(eventHandler* _eventhandler,
                                               Command* _command)
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");

         setSessionNameCommand* ssnc = dynamic_cast<setSessionNameCommand*>(_command);

         std::string sname = ssnc->getName();

         t_uint const maxNameSize = 255;

         if (sname.size() > maxNameSize)
            {
               sname = sname.substr(0, maxNameSize);
            }

         if (sname.size() >= 1)
            {
               _eventhandler->setName(sname);
               sendAck(Command::CN_SSETNAME);
            }
         else
            {
               sendError(_command->getType(), "Session name too short.");
            }
      }

      void daemonHandler::handleMoveContext(eventHandler* _eventhandler,
                                            btg::core::Command* _command)
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");

         contextMoveToSessionCommand* cmtsc = dynamic_cast<contextMoveToSessionCommand*>(_command);

         if (cmtsc->isAllContextsFlagSet())
            {
               sendError(_command->getType(), "Moving all contexts is not supported.");
            }
         else
            {
               t_int context_id   = cmtsc->getContextId();
               t_long old_session = cmtsc->session();
               eventHandler* new_eventhandler = 0;
               if (!sessionlist_.get(old_session, new_eventhandler))
                  {
                     sendError(_command->getType(), "Unable to find target session.");
                     return;
                  }

               if (!_eventhandler->move(connectionID_, context_id, new_eventhandler))
                  {
                     sendError(_command->getType(), "Unable to move context.");
                     return;
                  }
               sendAck(_command->getType());
            }
      }

      void daemonHandler::handleVersion(btg::core::Command* _command)
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");

         versionResponseCommand* vrc = new versionResponseCommand(projectDefaults::iMAJORVERSION(),
                                                                  projectDefaults::iMINORVERSION(),
                                                                  projectDefaults::iREVISIONVERSION());
         // Set the differnent options, default is off.
#if BTG_OPTION_SAVESESSIONS
         vrc->setOption(versionResponseCommand::SS);
#endif
         vrc->setOption(versionResponseCommand::PERIODIC_SS);
#if BTG_OPTION_UPNP
         vrc->setOption(versionResponseCommand::UPNP);
#endif

#if (BTG_LT_0_13 || BTG_LT_0_14)
         vrc->setOption(versionResponseCommand::DHT);
         vrc->setOption(versionResponseCommand::ENCRYPTION);
         vrc->setOption(versionResponseCommand::SELECTIVE_DL);
#endif

#if BTG_OPTION_URL
         vrc->setOption(versionResponseCommand::URL);
#endif
         sendCommand(dd_->externalization,
                     dd_->transport,
                     connectionID_,
                     vrc);
      }

      void daemonHandler::handleGetSetting(btg::core::Command* _command)
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");

         settingCommand* sc = dynamic_cast<settingCommand*>(_command);

         btg::core::daemonSetting ds = sc->getSetting();

         bool sendResponse = true;
         std::string response;

         switch(ds)
            {
            case btg::core::SG_TRANSPORT:
               {
                  switch (dd_->config->getTransport())
                     {
                     case btg::core::messageTransport::TCP:
                        response = "transport=TCP;";
                        break;
                     case btg::core::messageTransport::STCP:
                        response = "transport=STCP;";
                        break;
                     case btg::core::messageTransport::XMLRPC:
                        response = "transport=XMLRPC";
                        break;
                     case btg::core::messageTransport::SXMLRPC:
                        response = "transport=SXMLRPC";
                        break;
                     default:
                        response = "transport=UNKNOWN";
                        break;
                     }
                  break;
               }
            case btg::core::SG_PORT:
               {
                  btg::core::addressPort ap = dd_->config->getListenTo();
                  t_uint port = ap.getPort();
                  response = "port=" + convertToString<t_uint>(port);
                  break;
               }
            case btg::core::SG_PEERID:
               {
                  // Peer ID contained in the config file.

                  std::string peerid = dd_->config->getPeerId();
                  if (peerid.size() == 0)
                     {
                        response = "peerid=NOT_SET";
                     }
                  else
                     {
                        response = "peerid="+peerid;
                     }
                  break;
               }
            default:
               sendResponse = false;
               break;
            }

         if (sendResponse)
            {
               sendCommand(dd_->externalization,
                           dd_->transport,
                           connectionID_,
                           new settingResponseCommand(ds, response));
            }
         else
            {
               sendError(Command::CN_GETSETTING, "Setting not supported.");
            }
      }

      void daemonHandler::handleSessionInfo(eventHandler* _eventhandler,
                                            btg::core::Command* _command)
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");

         sendCommand(dd_->externalization,
                     dd_->transport,
                     connectionID_,
                     new sessionInfoResponseCommand(_eventhandler->encryptionEnabled(),
                                                    _eventhandler->dhtEnabled())
                     );
      }

      void daemonHandler::handleSessionInInvalidState(t_int const _id)
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): Message not supported.");

         sendError(_id, "Not supported in this state.");
      }

      void daemonHandler::handleInitConnection(Command* _command)
      {
         initConnectionCommand *icc = dynamic_cast<initConnectionCommand*>(_command);
         btg::core::addressPort ap;
         // Authorize the user
         if (dd_->auth->checkUser(icc->getUsername(), icc->getPassword(), ap))
            {
               // Success
               connection_->setUsername(icc->getUsername());

               BTG_MNOTICE(logWrapper(), connection_->toString() <<
                           " is authorized with username " << icc->getUsername());

               MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");

               if (!sendAck(Command::CN_GINITCONNECTION))
                  {
                     BTG_MNOTICE(logWrapper(), "sending of ack(initConnectionCommand) failed");
                  }
            }
         else
            {
               // Failure.

               BTG_MNOTICE(logWrapper(), "connection " << connection_->toString() << " used incorrect username '" << icc->getUsername() << "', password hash = '" << icc->getPassword() << "'");
               MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << " failed.");

               MVERBOSE_LOG(logWrapper(), verboseFlag_, "Client (" << connectionID_ << "): Rejecting connection from user '" <<
                            icc->getUsername() << "'.");

               sendError(Command::CN_GINITCONNECTION, "Failed to authorize user.");
            }
      }

      void daemonHandler::handleSessionList(Command* _command)
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");

         std::vector<t_long> sessions;
         std::vector<std::string> session_names;

         bool controlFlag = false;
         if (!dd_->auth->getControlFlag(connection_->getUsername(), controlFlag))
            {
               controlFlag = false;
            }

         if (controlFlag)
            {
               // The user has the control flag set. This means that
               // the user can see other user's sessions and attach to
               // them.
               sessionlist_.getIds(sessions);
               sessionlist_.getNames(session_names);
            }
         else
            {
               sessionlist_.getIds(connection_->getUsername(), sessions);
               sessionlist_.getNames(connection_->getUsername(), session_names);
            }

         if (sessions.size() > 0)
            {
               btg_assert(sessions.size() == session_names.size(),
                          logWrapper(),
                          "Session list and Id list must have same size.");
               sendCommand(dd_->externalization,
                           dd_->transport,
                           connectionID_,
                           new listSessionResponseCommand(sessions, session_names));
            }
         else
            {
               sendError(_command->getType(), "No sessions to list.");
            }
      }

      void daemonHandler::handleAttach(Command* _command)
      {
         attachSessionCommand *asc = dynamic_cast<attachSessionCommand*>(_command);
         /*
         if (asc->getBuildID() != projectDefaults::sBUILD())
            {
               // Wrong build.
               BTG_ERROR_LOG(logWrapper(), "Attach, unexpected build id '" << asc->getBuildID() << "'");

               sendCommand(dd_->externalization, dd_->transport, connectionID_, new errorCommand(_command->getType(), "Wrong build ID"));
               return;
            }
         */
         // Build ok.

         BTG_MNOTICE(logWrapper(), "attach, client build id '" << asc->getBuildID() << "'");

         t_long attachTo    = asc->getSession();
         if (connection_->getSession() != 0)
            {
               sendError(_command->getType(),
                         "Could not attach to session, already attached to another session.");
               return;

            }

         if (attachTo == Command::INVALID_SESSION)
            {
               sendError(_command->getType(),
                         "Could not attach to session, invalid session id.");
               return;
            }

         eventHandler* eventhandler = 0;

         if (!sessionlist_.get(attachTo, eventhandler))
            {
               sendError(_command->getType(),
                         "Could not attach to session, unknown session id.");
               return;
            }

         // Allow attach if the user attaching has the control flag
         // set.

         bool controlFlag = false;
         if (!dd_->auth->getControlFlag(connection_->getUsername(), controlFlag))
            {
               controlFlag = false;
            }


         if ((!controlFlag) &&
             (eventhandler->getUsername() != connection_->getUsername()))
            {
               sendError(_command->getType(),
                         "Could not attach to session, wrong username.");
               return;
            }

         // Change the session to the new one:
         eventhandler->incClients();

         // Update the Connection
         connection_->setSession(attachTo);

         // Send an ack message back with the new session. Everything is OK.
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");
         sendAck(Command::CN_SATTACH);
      }

      void daemonHandler::handleSetup(Command* _command)
      {
         setupCommand *sc = dynamic_cast<setupCommand*>(_command);

         if (sc->getRequiredData().getBuildID() != projectDefaults::sBUILD())
            {
               BTG_ERROR_LOG(logWrapper(), "Setup, unexpected build id '" << sc->getRequiredData().getBuildID() << "', expected '" << projectDefaults::sBUILD() << "'");
               sendError(_command->getType(),
                         "Wrong build ID");
            }
         else
            {
               // Build id ok.

               // The auth interface is used to get the user's
               // temp and destination directory.

               std::string userTempDir;
               std::string userWorkDir;
               std::string userSeedDir;
               std::string userDestDir;

#if BTG_OPTION_EVENTCALLBACK
               std::string userCallback;
#endif // BTG_OPTION_EVENTCALLBACK

               std::string errorDescription;

               if (!getDirectories(userTempDir, userWorkDir, userSeedDir, userDestDir, errorDescription))
                  {
                     // Error handling.
                     sendError(_command->getType(), errorDescription);
                     return;
                  }

#if BTG_OPTION_EVENTCALLBACK
               if (!dd_->auth->getCallbackFile(connection_->getUsername(), userCallback))
                  {
                     // Error handling.
                     errorDescription = "Missing callback.";
                     sendError(_command->getType(), errorDescription);
                     return;
                  }
#endif // BTG_OPTION_EVENTCALLBACK

               // The following will not fail, as a huge number (1024*1024) of
               // sessions is allowed.
               t_long session;
               sessionlist_.new_session(session);

               BTG_MNOTICE(logWrapper(),
                           "setup, client build id '" << sc->getRequiredData().getBuildID() << "'");

               // Setup a new context.
               BTG_MNOTICE(logWrapper(),
                           "setup, attempt to establish a new session with session id " << session);

               BTG_MNOTICE(logWrapper(),
                           "using the following paths:" <<
                           "tempdir '" << userTempDir << "', " <<
                           "work dir '" << userWorkDir << "', " <<
                           "seed dir '" << userSeedDir << "', " <<
                           "output dir '" << userDestDir << "'"
#if BTG_OPTION_EVENTCALLBACK
                           << ", callback '" << userCallback << "'"
#endif // BTG_OPTION_EVENTCALLBACK
                           );

               if (!portManager_.available())
                  {
                     sendError(_command->getType(),
                               "Failed to setup session (all ports used).");

                     MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << " failed.");
                     return;
                  }

               eventHandler* eh = new eventHandler(logWrapper(),
                                                   dd_->config,
                                                   verboseFlag_,
                                                   connection_->getUsername(),
                                                   userTempDir,
                                                   userWorkDir,
                                                   userSeedDir,
                                                   userDestDir,
#if BTG_OPTION_EVENTCALLBACK
                                                   userCallback,
#endif // BTG_OPTION_EVENTCALLBACK
                                                   dd_->interface_used,
                                                   dd_->interface,
                                                   &portManager_,
                                                   &limitManager_,
                                                   dd_->externalization,
                                                   session,
                                                   dd_->transport,
                                                   dd_->filetrack,
                                                   dd_->filter,
                                                   dd_->config->getUseTorrentName(),
                                                   dd_->connHandler,
                                                   "no name"
#if BTG_OPTION_EVENTCALLBACK
                                                   , dd_->callbackmgr
#endif // BTG_OPTION_EVENTCALLBACK
                                                   );

               // Use the received setup command to setup
               // the eventhandler and context.
               if (eh->setup(sc))
                  {
                     sessionlist_.add(session, eh);
                     handlerCount_++;

                     // Tell connection about his session.
                     connection_->setSession(session);

                     // Log some of the parameters contained in the
                     // setup message.

                     std::string setupInfoMessage;
                     if (sc->getRequiredData().useDHT())
                        {
                           setupInfoMessage += "DHT: ON.";
                        }
                     else
                        {
                           setupInfoMessage += "DHT: OFF.";
                        }

                     if (sc->getRequiredData().useEncryption())
                        {
                           setupInfoMessage += " Encryption: ON.";
                        }
                     else
                        {
                           setupInfoMessage += " Encryption: OFF.";
                        }

                     MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << "(" << setupInfoMessage << ").");

                     // Write a response:
                     sendCommand(dd_->externalization,
                                 dd_->transport,
                                 connectionID_,
                                 new setupResponseCommand(session));
                  }
               else
                  {
                     delete eh;
                     sendError(_command->getType(),
                               "Failed to setup session.");
                     MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << " failed.");
                  }
            }
      }

      bool daemonHandler::getDirectories(std::string & _userTempDir,
                                         std::string & _userWorkDir,
                                         std::string & _userSeedDir,
                                         std::string & _userDestDir,
                                         std::string & _errorDescription)
      {
         bool status = true;

         std::string username = connection_->getUsername();

         if (!dd_->auth->getTempDir(username, _userTempDir) ||
             !dd_->auth->getWorkDir(username, _userWorkDir) ||
             !dd_->auth->getSeedDir(username, _userSeedDir) ||
             !dd_->auth->getDestDir(username, _userDestDir))
            {
               status = false;
               _errorDescription = "Failed to setup session: invalid directories.";
               return status;
            }

         std::string errorString;

         if (!btg::core::os::fileOperation::check(_userTempDir, errorString, true))
            {
               logDirectoryNotFound("Temp dir", _userTempDir);
            }

         if (!btg::core::os::fileOperation::check(_userWorkDir, errorString, true))
            {
               logDirectoryNotFound("Work dir", _userWorkDir);
            }

         if (!btg::core::os::fileOperation::check(_userSeedDir, errorString, true))
            {
               logDirectoryNotFound("Seed dir", _userSeedDir);
            }

         if (!btg::core::os::fileOperation::check(_userDestDir, errorString, true))
            {
               logDirectoryNotFound("Dest dir", _userDestDir);
            }

         // Check that the directories gotten from the auth interface exist.
         if (!btg::core::os::fileOperation::check(_userTempDir, errorString, true) ||
             !btg::core::os::fileOperation::check(_userWorkDir, errorString, true) ||
             !btg::core::os::fileOperation::check(_userSeedDir, errorString, true) ||
             !btg::core::os::fileOperation::check(_userDestDir, errorString, true))
            {
               status = false;
               _errorDescription = "Failed to setup session: invalid directories.";
               return status;
            }

         return status;
      }

      void daemonHandler::handleOpMessages(eventHandler* _eventhandler,
                                           btg::core::Command* _command)
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");

         // Find out which kind of op message was received.
         switch (_command->getType())
            {
            case Command::CN_OPABORT:
               {
                  btg::core::opAbortCommand* coac = dynamic_cast<btg::core::opAbortCommand*>(_command);
                  switch (coac->type())
                     {
                     case btg::core::ST_URL:
                        handleUrlMessages(_eventhandler, _command);
                        break;
                     case btg::core::ST_FILE:
                        handleCreateFileMessages(_eventhandler, _command);
                        break;
                     }
                  break;
               }
            case Command::CN_OPSTATUS:
               {
                  btg::core::opStatusCommand* cosc = dynamic_cast<btg::core::opStatusCommand*>(_command);
                  switch (cosc->type())
                     {
                     case btg::core::ST_URL:
                        handleUrlMessages(_eventhandler, _command);
                        break;
                     case btg::core::ST_FILE:
                        handleCreateFileMessages(_eventhandler, _command);
                        break;
                     }
                  break;
               }
            }
      }

      void daemonHandler::handleUrlMessages(eventHandler* _eventhandler,
                                            btg::core::Command* _command)
      {
#if BTG_OPTION_URL
         // Messages which download from URL or which get the status
         // of a download.
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");
         switch (_command->getType())
            {
            case Command::CN_CCREATEFROMURL:
               {
                  handle_CN_CCREATEFROMURL(_eventhandler, _command);
                  break;
               }
            case Command::CN_OPSTATUS:
               {
                  handle_CN_CURLSTATUS(_command);
                  break;
               }
            case Command::CN_OPABORT:
               {
                  handle_CN_CCREATEURLABORT(_command);
                  break;
               }
            }
#else
         // URL loading disabled.
         sendError(_command->getType(),
                   "URL loading not enabled.");
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "Session " << _eventhandler->getSession() <<
                      " received unsupported URL command: " << _command->getName() <<
                      " from client (" << connectionID_ << ").");

#endif // BTG_OPTION_URL
      }

      void daemonHandler::handleCreateFileMessages(eventHandler* _eventhandler,
                                                   btg::core::Command* _command)
      {
         switch (_command->getType())
            {
            case Command::CN_CCREATEFROMFILE:
               {
                  handle_CN_CCREATEFROMFILE(_eventhandler, _command);
                  break;
               }
            case Command::CN_CCREATEFROMFILEPART:
               {
                  handle_CN_CCREATEFROMFILEPART(_eventhandler, _command);
                  break;
               }
            case Command::CN_OPSTATUS:
               {
                  handle_CN_CCRFILESTATUS(_command);
                  break;
               }
            case Command::CN_OPABORT:
               {
                  handle_CN_CCREATEFFABORT(_command);
                  break;
               }
            }
      }

      void daemonHandler::handleOther(eventHandler* _eventhandler, Command* _command)
      {
         // All other events.
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");
         _eventhandler->event(_command, connectionID_);
      }

      void daemonHandler::sendError(t_int const _cmdId, std::string const& _description)
      {
         errorCommand* err = new errorCommand(_cmdId, _description);

         if (err->messagePresent())
            {
               MVERBOSE_LOG(logWrapper(),
                            verboseFlag_, "Sending error to client (" << connectionID_ <<
                            "): '" << err->getMessage() << "'.");
            }

         sendCommand(dd_->externalization,
                     dd_->transport,
                     connectionID_,
                     err);
      }

      bool daemonHandler::sendAck(t_int const _cmdId)
      {
         return sendCommand(dd_->externalization,
                            dd_->transport,
                            connectionID_,
                            new ackCommand(_cmdId));
      }

      bool daemonHandler::sendCommand(btg::core::externalization::Externalization* _e,
                                      btg::core::messageTransport* _transport,
                                      t_int _connectionID,
                                      Command* _command)
      {
         bool status          = false;
         sendBuffer_.erase();

         BTG_MNOTICE(logWrapper(), "sending '" << _command->getName() <<"'");

         if (verboseFlag_)
            {
               if (_command->getType() == Command::CN_ERROR)
                  {
                     // If sending an error, write which command the
                     // error was caused by.

                     std::string causedBy = Command::getName(
                                                             dynamic_cast<errorCommand*>(_command)->getErrorCommand()
                                                             );

                     MVERBOSE_LOG(logWrapper(), verboseFlag_, "daemon (" << _connectionID << "): " <<
                                  _command->getName() << " (caused by " << causedBy << ").");
                  }
               else
                  {
                     MVERBOSE_LOG(logWrapper(), verboseFlag_, "daemon (" << _connectionID << "): " <<
                                  _command->getName() << ".");
                  }

            }
         _e->reset();

         status = _command->serialize(_e);
         _e->setDirection(FROM_SERVER);

         if (status)
            {
               _e->getBuffer(sendBuffer_);
            }
         else
            {
               BTG_MNOTICE(logWrapper(), "failed to serialize command for sending");
            }

         delete _command;
         _command = 0;

         BTG_MNOTICE(logWrapper(), "data (out): " << sendBuffer_.size() << " bytes");

         if (_transport->write(sendBuffer_, _connectionID) != btg::core::messageTransport::OPERATION_FAILED)
            {
               status = true;
            }

         return status;
      }

      void daemonHandler::checkDeadConnections()
      {
         // Check for dead connections
         t_intList dead = dd_->transport->getDeadConnections();

         if (dead.size() > 0)
            {
               t_intListCI iter;

               for (iter = dead.begin();
                    iter != dead.end();
                    iter++)
                  {
                     // Does the connection handler know about this connection?
                     connection_ = dd_->connHandler->getConnection(*iter);
                     if (connection_ == 0)
                        {
                           BTG_MNOTICE(logWrapper(),
                                       "got a dead connection ID " <<
                                       *iter << " from transport");
                           continue;
                        }

                     // Find out if the connection had a session attached.
                     BTG_MNOTICE(logWrapper(),
                                 "connection " << connection_->toString() << " has died");
                     if (connection_->getSession() != 0)
                        {
                           eventHandler* eventhandler = 0;
                           if (sessionlist_.get(connection_->getSession(), eventhandler))
                              {
                                 BTG_MNOTICE(logWrapper(),
                                             "detaching dead connection from session " << connection_->getSession());
                                 eventhandler->decClients();
                              }
                        }

                     // Remove the connection
                     dd_->connHandler->removeConnection(*iter);
                     connection_ = 0;
                  }
            }
      }

      void daemonHandler::handleDeserializationError(btg::core::commandFactory::decodeStatus const& _decodestatus)
      {
         // Broken command, we failed to deserialize it...
         BTG_MNOTICE(logWrapper(), "unhandled data: " << buffer_.toString());

         // OK This is probably not good for some transports... A null session...
         // However, we should somehow notify the client that the command failed...
         switch(_decodestatus)
            {
            case btg::core::commandFactory::DS_UNKNOWN:
               {
                  sendError(Command::CN_UNDEFINED,
                               "Unknown command.");
                  break;
               }
            case btg::core::commandFactory::DS_FAILED:
               {
                  sendError(Command::CN_UNDEFINED,
                            "Failed to deserialize command.");
                  break;
               }
            default:
               {
                  // Do nothing, its ok.
               }
            }
      }

      void daemonHandler::logDirectoryNotFound(std::string const& _type, std::string const& _value)
      {
         BTG_MNOTICE(logWrapper(),
                     _type << ": directory '" << _value << "' not found");

         MVERBOSE_LOG(logWrapper(),
                      verboseFlag_, "daemon (" << connectionID_ << "): " <<
                      _type << ", directory '" << _value << "' not found.");
      }

      void daemonHandler::checkTimeout()
      {
         if (file_timer_trigger_)
            {
               file_timer_trigger_ = false;
               file_timer_.Reset();
               handleFileDownloads();
            }

#if BTG_OPTION_URL
         if (url_timer_trigger_)
            {
               url_timer_trigger_ = false;
               url_timer_.Reset();

               handleUrlDownloads();
            }
#endif
         if (session_timer_trigger_)
            {
               MVERBOSE_LOG(logWrapper(), verboseFlag_, "Checking limits and alerts.");
               sessionlist_.checkLimitsAndAlerts();
               session_timer_trigger_ = false;
               session_timer_.Reset();
               return;
            }

         if (periodic_ssave_timer_trigger_)
            {
#if BTG_OPTION_SAVESESSIONS
               if (dd_->ss_enable)
                  {
                     BTG_MNOTICE(logWrapper(), "Periodically saving sessions");
                     MVERBOSE_LOG(logWrapper(), verboseFlag_, "Periodically saving sessions.");
                     sessionsaver_.saveSessions(dd_->ss_file, true /* also save fast-resume data */);
                  }
#endif
               periodic_ssave_timer_trigger_ = false;
               periodic_ssave_timer_.Reset();
               return;
            }

         if (limit_timer_trigger_)
            {
               MVERBOSE_LOG(logWrapper(), verboseFlag_, "Updating limits.");
               limitManager_.update();
               limit_timer_.Reset();
               limit_timer_trigger_ = false;
               return;
            }

         if (elapsed_timer_trigger_)
            {
               MVERBOSE_LOG(logWrapper(), verboseFlag_, "Updating seed counters.");
               sessionlist_.updateElapsedOrSeedCounter();
               elapsed_timer_trigger_ = false;
               elapsed_seed_timer_.Reset();
               return;
            }
         if (file_timer_.Timeout())
            {
               file_timer_trigger_ = true;
            }
#if BTG_OPTION_URL
         if (url_timer_.Timeout())
            {
               url_timer_trigger_ = true;
            }
#endif
         if (session_timer_.Timeout())
            {
               session_timer_trigger_ = true;
            }

         if (periodic_ssave_timer_.Timeout())
            {
               periodic_ssave_timer_trigger_ = true;
            }

         if (limit_timer_.Timeout())
            {
               limit_timer_trigger_ = true;
            }

         if (elapsed_seed_timer_.Timeout())
            {
               elapsed_timer_trigger_ = true;
            }
      }

      void daemonHandler::shutdown()
      {
         BTG_MNOTICE(logWrapper(), "cleaning up.");

         limitManager_.stop();

#if BTG_OPTION_SAVESESSIONS
         if(dd_->ss_enable)
            {
               MVERBOSE_LOG(logWrapper(), verboseFlag_, "Saving sessions.");
               sessionsaver_.saveSessions(dd_->ss_file, true);
            }
         else
            {
               MVERBOSE_LOG(logWrapper(), verboseFlag_, "Sesssion saving disabled.");
            }
#endif // BTG_OPTION_SAVESESSIONS

         sessionlist_.shutdown();

         // Delete all sessions/eventhandlers.
         sessionlist_.deleteAll();
      }

      void daemonHandler::updateFilter()
      {
         sessionlist_.updateFilter(dd_->filter);
      }

      daemonHandler::~daemonHandler()
      {

      }

   } // namespace daemon
} // namespace btg

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
#include <bcore/command/context_move.h>
#include <bcore/command/context_create.h>
#include <bcore/command/context_create_url.h>

#include <bcore/verbose.h>
#include "modulelog.h"

#include <bcore/btg_assert.h>
#include "filetrack.h"

extern int global_btg_run;

namespace btg
{
   namespace daemon
   {

      using namespace btg::core;

      const std::string moduleName("hdl");
      const t_uint max_url_age = 30;
      const t_uint url_expired = 31;

      typedef std::map<t_long, eventHandler*> handlerMap;

      daemonHandler::daemonHandler(btg::core::LogWrapperType _logwrapper,
                                   daemonData* _dd, 
                                   bool const _verboseFlag)
         : btg::core::Logable(_logwrapper),
           dd_(_dd),
           verboseFlag_(_verboseFlag),
           sessionlist_(_logwrapper, _verboseFlag, 1024*1024 /* Max number of sessions. */),
           command_(0),
           session_(0),
           readBytes_(0),
           connectionID_(btg::core::messageTransport::NO_CONNECTION_ID),
           connection_(0),
           handlerCount_(0),
           buffer_(),
           session_timer_(10), /* 10 seconds. */
           session_timer_trigger_(false),
           url_timer_(5),
           url_timer_trigger_(false),
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
           cf_(_logwrapper, _dd->externalization),
           httpmgr(_logwrapper),
           UrlIdSessions()
      {
         /// Set the initial limits.
         limitManager_.set(_dd->config->getUploadRateLimit(),
                           _dd->config->getDownloadRateLimit(),
                           _dd->config->getMaxUploads(),
                           _dd->config->getMaxConnections());

         limitManager_.update();
#if BTG_OPTION_SAVESESSIONS
         if(dd_->ss_enable)
            {
               if(!dd_->cla->doNotReloadSessions())
                  {
                     if(btg::core::os::fileOperation::check(dd_->ss_filename))
                        {
                           MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "Loading sessions from " << dd_->ss_filename << ".");
                           BTG_MNOTICE(logWrapper(), "loading sessions from " << dd_->ss_filename);

                           t_int numberOfSessions = sessionsaver_.loadSessions(dd_->ss_filename);

                           MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "Loaded " << numberOfSessions << " sesssion(s).");
                        }
                     else
                        {
                           BTG_MNOTICE(logWrapper(), "session file " << dd_->ss_filename << " not found. Not loading");
                        }
                  }
               else
                  {
                     BTG_MNOTICE(logWrapper(), "session reloading disabled on command line, skipping reload");
                  }
            }
         else
            {
               MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "Sesssion saving disabled.");
               BTG_MNOTICE(logWrapper(), "sesssion saving disabled");
            }
#endif // BTG_OPTION_SAVESESSIONS
      }

      void daemonHandler::readFromTransport()
      {
         command_      = 0;
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
               command_ = cf_.createFromBytes(dstatus);

               // This buffer is also used for sending messages, so erase
               // the contents.
               buffer_.erase();

               if ((dstatus != btg::core::commandFactory::DS_OK) || (command_ == 0))
                  {
                     handleDeserializationError(dstatus);
                     return;
                  }

               session_ = connection_->getSession();

               BTG_MNOTICE(logWrapper(), "attempting to handle " << command_->getName() << " with " << connection_->toString());

               // Try to find an eventhandler for this session.
               // We dont have to check auth here, since the connecton
               // wont have an associated session if he havent been
               // authed (setup and attach cannot be done without
               // auth).

               eventHandler* eventhandler = 0;

               if (sessionlist_.get(session_, eventhandler))
                  {
                     switch (command_->getType())
                        {
                        case Command::CN_SQUIT:
                           {
                              // Special case, where the client wishes to close
                              // the session.
                              handleQuit(eventhandler);
                              break;
                           }
                        case Command::CN_SLIST:
                           {
                              handleSessionList();
                              break;
                           }
                        case Command::CN_SDETACH:
                           {
                              handleDetach(eventhandler);
                              break;
                           }
                        case Command::CN_GKILL:
                        case Command::CN_GLIMIT:
                           {
                              handleControlCommand(command_->getType());
                              break;
                           }
                        case Command::CN_GLIMITSTAT:
                           {
                              handleGlobalStatus();
                              break;
                           }
                        case Command::CN_GUPTIME:
                           {
                              handleUptime();
                              break;
                           }
                        case Command::CN_SNAME:
                           {
                              handleSessionName(eventhandler);
                              break;
                           }
                        case Command::CN_SSETNAME:
                           {
                              handleSessionSetName(eventhandler, command_);
                              break;
                           }
                        case Command::CN_SINFO:
                           {
                              handleSessionInfo(eventhandler, command_);
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
                              handleMoveContext(eventhandler, command_);
                              break;
                           }

                        case Command::CN_CCREATEFROMURL:
                        case Command::CN_CURLSTATUS:
                           {
                              handleUrlMessages(eventhandler, command_);
                              break;
                           }

                        default:
                           {
                              handleOther(eventhandler, command_);
                           }
                        } // switch special commands.

                  }
               // No session is established at this point.
               else if (command_->getType() == Command::CN_GINITCONNECTION)
                  {
                     handleInitConnection(command_);
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

                           MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "Client (" << connectionID_ << ") is not authorized.");

                           sendError(command_->getType(), "Not authorized.");
                        }
                     else
                        {
                           // User is authed.

                           // ***************
                           // No session yet.
                           // ***************

                           switch(command_->getType())
                              {
                              case Command::CN_SATTACH:
                                 {
                                    handleAttach(command_);
                                    break;
                                 }
                              case Command::CN_SLIST:
                                 {
                                    handleSessionList();
                                    break;
                                 }
                              case Command::CN_GSETUP:
                                 {
                                    handleSetup(command_);
                                    break;
                                 }
                              case Command::CN_SNAME:
                              case Command::CN_SNAMERSP:
                              case Command::CN_SSETNAME:
                                 {
                                    // These commands are not supported here.
                                    // Send an nice error back.
                                    handleSessionInInvalidState(command_->getType());
                                    break;
                                 }
                              }
                        } // End if !authed...
                  }

               delete command_;
               command_ = 0;
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

      void daemonHandler::handleQuit(eventHandler* _eventhandler)
      {
         // Dont allow detach if we have more clients than this one.
         if(_eventhandler->getNumClients() > 1)
            {
               sendError(command_->getType(), "Cannot quit, other clients attached.");
            }
         else
            {
               BTG_MNOTICE(logWrapper(), "terminating session " << session_);

               MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "client (" << connectionID_ << "): " << command_->getName() << ".");

               // Remove the session data.
               sessionlist_.erase(session_);

               // Send an ack message back, as the session was deleted.
               sendAck(Command::CN_SQUIT);
               // sendCommand(dd_->externalization, dd_->transport, connectionID_, new ackCommand(Command::CN_SQUIT));
               connection_->setSession(0);
               dd_->transport->endConnection(connectionID_);
            }
      }

      void daemonHandler::handleDetach(eventHandler* _eventhandler)
      {
         BTG_MNOTICE(logWrapper(), "detaching the current session");
         _eventhandler->decClients();

         // Send an ack message back, as the session was detached.
         MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "client (" << connectionID_ << "): " << command_->getName() << ".");
         sendAck(Command::CN_SDETACH);
         connection_->setSession(0);
         dd_->transport->endConnection(connectionID_);
      }

      void daemonHandler::handleControlCommand(t_int const _id)
      {
         std::string user = connection_->getUsername();

         bool control_flag = false;

         if (!dd_->auth->getControlFlag(user, control_flag))
            {
               sendError(_id, "Control not allowed.");
               return;
            }

         if (!control_flag)
            {
               sendError(_id, "Control not allowed.");
               return;
            }

         switch (_id)
            {
            case Command::CN_GKILL:
               {
                  handleKill();
                  break;
               }
            case Command::CN_GLIMIT:
               {
                  handleGlobalLimit();
                  break;
               }
            default:
               {
                  sendError(_id, "Control command not implemented.");
                  break;
               }
            }
      }

      void daemonHandler::handleKill()
      {
         BTG_MNOTICE(logWrapper(), "killing the daemon");
         MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "client (" << connectionID_ << "): " << command_->getName() << ".");

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

      void daemonHandler::handleGlobalLimit()
      {
         BTG_MNOTICE(logWrapper(), "Setting global limits");
         MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "client (" << connectionID_ << "): " << command_->getName() << ".");

         limitCommand* lc = dynamic_cast<limitCommand*>(command_);

         t_int limitBytesUpld   = lc->getUploadLimit();
         t_int limitBytesDwnld  = lc->getDownloadLimit();
         t_int maxUplds         = lc->getMaxUplds();
         t_long maxConnections  = lc->getMaxConnections();

         validateGlobalLimits(limitBytesUpld, limitBytesDwnld, maxUplds, maxConnections);

         limitManager_.set(limitBytesUpld, limitBytesDwnld, maxUplds, maxConnections);

         // Ack the limit command.
         sendAck(Command::CN_GLIMIT);
      }

      void daemonHandler::handleGlobalStatus()
      {
         BTG_MNOTICE(logWrapper(), "Global limits request");

         MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "client (" << connectionID_ << "): " << command_->getName() << ".");

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

      void daemonHandler::handleUptime()
      {
         MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "client (" << connectionID_ << "): " << command_->getName() << ".");
         buffer_.erase();

         time_t now;
         time(&now);

         t_ulong timeDiff = now - dd_->daemonStartTime;

         sendCommand(dd_->externalization, 
                     dd_->transport, 
                     connectionID_, 
                     new uptimeResponseCommand(timeDiff));
      }

      void daemonHandler::handleSessionName(eventHandler* _eventhandler)
      {
         MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "client (" << connectionID_ << "): " << command_->getName() << ".");

         sendCommand(dd_->externalization, 
                     dd_->transport,
                     connectionID_, 
                     new sessionNameResponseCommand(_eventhandler->getName()));
      }

      void daemonHandler::handleSessionSetName(eventHandler* _eventhandler, 
                                               Command* _command)
      {
         MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "client (" << connectionID_ << "): " << command_->getName() << ".");

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
               sendError(command_->getType(), "Session name too short.");
            }
      }

      void daemonHandler::handleMoveContext(eventHandler* _eventhandler, 
                                            btg::core::Command* _command)
      {
         MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "client (" << connectionID_ << "): " << command_->getName() << ".");

         contextMoveToSessionCommand* cmtsc = dynamic_cast<contextMoveToSessionCommand*>(_command);

         if (cmtsc->isAllContextsFlagSet())
            {
               sendError(command_->getType(), "Moving all contexts is not supported.");
            }
         else
            {
               t_int context_id   = cmtsc->getContextId();
               t_long old_session = cmtsc->session();
               eventHandler* new_eventhandler;
               if (!sessionlist_.get(old_session, new_eventhandler))
                  {
                     sendError(command_->getType(), "Unable to find target session.");
                     return;
                  }
               
               if (!_eventhandler->move(connectionID_, context_id, new_eventhandler))
                  {
                     sendError(command_->getType(), "Unable to move context.");
                     return;
                  }
               sendAck(command_->getType());
            }
      }

      void daemonHandler::handleSessionInfo(eventHandler* _eventhandler, 
                                            btg::core::Command* _command)
      {
         MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "client (" << connectionID_ << "): " << command_->getName() << ".");

         sendCommand(dd_->externalization, 
                     dd_->transport,
                     connectionID_, 
                     new sessionInfoResponseCommand(_eventhandler->encryptionEnabled(), 
                                                    _eventhandler->dhtEnabled())
                     );
      }

      void daemonHandler::handleSessionInInvalidState(t_int const _id)
      {
         MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "client (" << connectionID_ << "): Message not supported.");

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

               MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "client (" << connectionID_ << "): " << command_->getName() << ".");

               if (!sendAck(Command::CN_GINITCONNECTION))
                  {
                     BTG_MNOTICE(logWrapper(), "sending of ack(initConnectionCommand) failed");
                  }
            }
         else
            {
               // Failure.

               BTG_MNOTICE(logWrapper(), "connection " << connection_->toString() << " used incorrect username '" << icc->getUsername() << "', password hash = '" << icc->getPassword() << "'");
               MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "client (" << connectionID_ << "): " << command_->getName() << " failed.");

               MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "Client (" << connectionID_ << "): Rejecting connection from user '" <<
                            icc->getUsername() << "'.");

               sendError(Command::CN_GINITCONNECTION, "Failed to authorize user.");
            }
      }

      void daemonHandler::handleSessionList()
      {
         MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "client (" << connectionID_ << "): " << command_->getName() << ".");

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
               sendError(command_->getType(), "No sessions to list.");
            }
      }

      void daemonHandler::handleAttach(Command* _command)
      {
         attachSessionCommand *asc = dynamic_cast<attachSessionCommand*>(_command);
         /*
         if (asc->getBuildID() != GPD->sBUILD())
            {
               // Wrong build.
               BTG_ERROR_LOG(logWrapper(), "Attach, unexpected build id '" << asc->getBuildID() << "'");

               sendCommand(dd_->externalization, dd_->transport, connectionID_, new errorCommand(command_->getType(), "Wrong build ID"));
               return;
            }
         */
         // Build ok.

         BTG_MNOTICE(logWrapper(), "attach, client build id '" << asc->getBuildID() << "'");

         t_long attachTo    = asc->getSession();
         if (connection_->getSession() != 0)
            {
               sendError(command_->getType(), 
                         "Could not attach to session, already attached to another session.");
               return;

            }

         if (attachTo == Command::INVALID_SESSION)
            {
               sendError(command_->getType(), 
                         "Could not attach to session, invalid session id.");
               return;
            }

         eventHandler* eventhandler = 0;

         if (!sessionlist_.get(attachTo, eventhandler))
            {
               sendError(command_->getType(), 
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
               sendError(command_->getType(), 
                         "Could not attach to session, wrong username.");
               return;
            }

         // Change the session to the new one:
         eventhandler->incClients();

         // Update the Connection
         connection_->setSession(attachTo);

         // Send an ack message back with the new session. Everything is OK.
         MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "client (" << connectionID_ << "): " << command_->getName() << ".");
         sendAck(Command::CN_SATTACH);
      }

      void daemonHandler::handleSetup(Command* _command)
      {
         setupCommand *sc = dynamic_cast<setupCommand*>(_command);

         if (sc->getRequiredData().getBuildID() != GPD->sBUILD())
            {
               BTG_ERROR_LOG(logWrapper(), "Setup, unexpected build id '" << sc->getRequiredData().getBuildID() << "', expected '" << GPD->sBUILD() << "'");
               sendError(command_->getType(), 
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
                     sendError(command_->getType(), errorDescription);
                     return;
                  }

#if BTG_OPTION_EVENTCALLBACK
               if (!dd_->auth->getCallbackFile(connection_->getUsername(), userCallback))
                  {
                     // Error handling.
                     errorDescription = "Missing callback.";
                     sendError(command_->getType(), errorDescription); 
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
                     sendError(command_->getType(), 
                               "Failed to setup session (all ports used)."); 

                     MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "client (" << connectionID_ << "): " << command_->getName() << " failed.");
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
                     
                     MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "client (" << connectionID_ << "): " << command_->getName() << "(" << setupInfoMessage << ").");

                     // Write a response:
                     sendCommand(dd_->externalization,
                                 dd_->transport,
                                 connectionID_,
                                 new setupResponseCommand(session));
                  }
               else
                  {
                     delete eh;
                     sendError(command_->getType(), 
                               "Failed to setup session."); 
                     MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "client (" << connectionID_ << "): " << command_->getName() << " failed.");
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

      void daemonHandler::handleUrlMessages(eventHandler* _eventhandler, btg::core::Command* _command)
      {
         // Messages which download from URL or which get the status
         // of a download.
         MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");
      
         switch (_command->getType())
            {
            case Command::CN_CCREATEFROMURL:
               {
                  handle_CN_CCREATEFROMURL(_eventhandler, _command);
                  break;
               }
            case Command::CN_CURLSTATUS:
               {
                  contextUrlStatusCommand* cusc = dynamic_cast<contextUrlStatusCommand*>(_command);
                  t_uint hid = cusc->id();
                  btg::daemon::http::httpInterface::Status httpstat = httpmgr.getStatus(hid);

                  btg::core::urlStatus urlstat = URLS_UNDEF;
                  
                  switch (httpstat)
                     {
                     case btg::daemon::http::httpInterface::ERROR:
                        urlstat = URLS_ERROR;
                        break;
                     case btg::daemon::http::httpInterface::INIT:
                        urlstat = URLS_WORKING;
                        break;
                     case btg::daemon::http::httpInterface::WAIT:
                        urlstat = URLS_WORKING;
                        break;
                     case btg::daemon::http::httpInterface::FINISH:
                        {
                           // TODO: Finished downloading. Add it.
                           urlstat = URLS_FINISHED;
                           break;
                        }
                     }

                  sendCommand(dd_->externalization,
                              dd_->transport,
                              connectionID_,
                              new contextUrlStatusResponseCommand(hid, urlstat));

                  if (httpstat == btg::daemon::http::httpInterface::FINISH)
                     {
                        // The http download finished, add the torrent.
                        handleUrlDownload(hid);
                     }

                  break;
               }
            }
      }

      void daemonHandler::handleOther(eventHandler* _eventhandler, Command* _command)
      {
         // All other events.
         MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");
         _eventhandler->event(_command, connectionID_);
      }

      void daemonHandler::sendError(t_int const _cmdId, std::string const& _description)
      {
         errorCommand* err = new errorCommand(_cmdId, _description);

         if (err->messagePresent())
            {
               MVERBOSE_LOG(logWrapper(), 
                            moduleName, 
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

                     MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "daemon (" << _connectionID << "): " <<
                                  _command->getName() << " (caused by " << causedBy << ").");
                  }
               else
                  {
                     MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "daemon (" << _connectionID << "): " <<
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
                      moduleName, verboseFlag_, "daemon (" << connectionID_ << "): " <<
                      _type << ", directory '" << _value << "' not found.");
      }

      void daemonHandler::checkTimeout()
      {
         if (url_timer_trigger_)
            {
               url_timer_trigger_ = false;
               url_timer_.Reset();
               if (UrlIdSessions.size() > 0)
                  {
                     MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "Checking url downloads.");
                     handleUrlDownloads();
                  }
            }

         if (session_timer_trigger_)
            {
               MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "Checking limits and alerts.");
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
                     MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "Periodically saving sessions.");
                     sessionsaver_.saveSessions(dd_->ss_filename, false);
                  }
#endif
               periodic_ssave_timer_trigger_ = false;
               periodic_ssave_timer_.Reset();
               return;
            }

         if (limit_timer_trigger_)
            {
               MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "Updating limits.");
               limitManager_.update();
               limit_timer_.Reset();
               limit_timer_trigger_ = false;
               return;
            }

         if (elapsed_timer_trigger_)
            {
               MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "Updating seed counters.");
               sessionlist_.updateElapsedOrSeedCounter();
               elapsed_timer_trigger_ = false;
               elapsed_seed_timer_.Reset();
               return;
            }

         if (url_timer_.Timeout())
            {
               url_timer_trigger_ = true;
            }

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
               BTG_MNOTICE(logWrapper(), "saving sessions to '" << dd_->ss_filename << "'");
               MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "Saving sessions to '" << dd_->ss_filename << "'.");
               sessionsaver_.saveSessions(dd_->ss_filename, true);
            }
         else
            {
               BTG_MNOTICE(logWrapper(), "sesssion saving disabled");
               MVERBOSE_LOG(logWrapper(), moduleName, verboseFlag_, "Sesssion saving disabled.");
            }
#endif // BTG_OPTION_SAVESESSIONS

         sessionlist_.shutdown();

         // Delete all sessions/eventhandlers.
         sessionlist_.deleteAll();
      }

      void daemonHandler::handleUrlDownload(t_uint _hid)
      {
         std::vector<UrlIdSessionMapping>::iterator i;
         for (i = UrlIdSessions.begin();
              i != UrlIdSessions.end();
              i++)
            {
               if (i->hid == _hid)
                  {
                     addUrl(*i);
                     UrlIdSessions.erase(i);
                     break;
                  }
            }
      }

      void daemonHandler::handleUrlDownloads()
      {
         std::vector<UrlIdSessionMapping>::iterator i;
         for (i = UrlIdSessions.begin();
              i != UrlIdSessions.end();
              i++)
            {
               if (i->age > max_url_age)
                  {
                     continue;
                  }

               i->age++;

               btg::daemon::http::httpInterface::Status s = httpmgr.getStatus(i->hid);
               if (s == btg::daemon::http::httpInterface::FINISH)
                  {
                     // Dl finished, now create the context.
                     addUrl(*i);
                  }

               if (s == btg::daemon::http::httpInterface::ERROR)
                  {
                     // Expire this download.
                     i->age += url_expired;
                  }
            }

         // Remove expired downloads.
         i = UrlIdSessions.begin();
         while (i != UrlIdSessions.end())
            {
               if (i->age > max_url_age)
                  {
                     httpmgr.Terminate(i->hid);
                     dd_->filetrack->remove(i->userdir,
                                            i->filename);
                     i = UrlIdSessions.erase(i);
                  }
               else
                  {
                     i++;
                  }
            }
      }

      void daemonHandler::addUrl(UrlIdSessionMapping & _mapping)
      {
         // Find the required event handler.
         eventHandler* eh = 0;
         if (!sessionlist_.get(_mapping.session, eh))
            {
               // No session, expire.
               _mapping.age += url_expired;
               return;
            }

         btg::core::sBuffer sbuf;
         if (!sbuf.read(_mapping.userdir + GPD->sPATH_SEPARATOR() + _mapping.filename))
            {
               // No data, expire.
               _mapping.age += url_expired;
               return;
            }

         // Remove the file from filetrack, as it will be added when
         // creating the torrent.
         dd_->filetrack->remove(_mapping.userdir,
                                _mapping.filename);

         btg::core::contextCreateWithDataCommand* ccwdc = 
            new btg::core::contextCreateWithDataCommand(_mapping.filename, sbuf, _mapping.start);

         // Do not use a connection id.
         if (!eh->createWithData(ccwdc))
            {
               BTG_MERROR(logWrapper(), 
                          "Adding '" << _mapping.filename << "' from URL failed");
               _mapping.age += url_expired;
            }
         else
            {
               MVERBOSE_LOG(logWrapper(), 
                            moduleName, 
                            verboseFlag_, "Added '" << _mapping.filename << "' from URL");
            }
         
         delete ccwdc;
         ccwdc = 0;
      }

      void daemonHandler::handle_CN_CCREATEFROMURL(eventHandler* _eventhandler, 
                                                   btg::core::Command* _command)
      {
         contextCreateFromUrlCommand* ccful = dynamic_cast<contextCreateFromUrlCommand*>(_command);

         std::string userdir  = _eventhandler->getTempDir();
         std::string filename = ccful->getFilename();

         if (!dd_->filetrack->add(userdir,
                                 filename))
            {
               // File already exists.
               sendError(_command->getType(), "Torrent file already exists.");
               return;
            }

         bool unique = true;
         std::vector<UrlIdSessionMapping>::iterator i;
         for (i = UrlIdSessions.begin();
              i != UrlIdSessions.end();
              i++)
            {
               if ((i->filename == filename) && (i->userdir == userdir))
                  {
                     unique = false;
                     break;
                  }
            }

         if (!unique)
            {
               dd_->filetrack->remove(userdir,
                                      filename);

               sendError(_command->getType(), "Torrent file already exists.");
               return;
            }

         t_uint hid = httpmgr.Fetch(ccful->getUrl(), 
                                    userdir + GPD->sPATH_SEPARATOR() + filename);
         UrlIdSessionMapping usmap(hid, _eventhandler->getSession(), userdir, filename, ccful->getStart());
         UrlIdSessions.push_back(usmap);
         
         BTG_MNOTICE(logWrapper(), "Added mapping: HID " << usmap.hid << " -> " << usmap.session << " -> " << usmap.filename);
         
         sendCommand(dd_->externalization,
                     dd_->transport,
                     connectionID_,
                     new contextCreateFromUrlResponseCommand(hid));
      }

      daemonHandler::~daemonHandler()
      {

      }

   } // namespace daemon
} // namespace btg

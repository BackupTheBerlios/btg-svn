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

#ifndef DAEMONHANDLER_H
#define DAEMONHANDLER_H

#include "context.h"
#include "eventhandler.h"
#include "connectionhandler.h"
#include "dconfig.h"
#include <bcore/command/command.h>

#include <bcore/dbuf.h>
#include <bcore/transport/transport.h>
#include <bcore/command_factory.h>

#include "session_list.h"

#include "daemondata.h"

#if BTG_OPTION_SAVESESSIONS
#  include "sessionsaver.h"
#endif // BTG_OPTION_SAVESESSIONS

#include <bcore/os/timer.h>

#include <bcore/command/limit_base.h>

#include <map>

#include "portmgr.h"
#include "limitmgr.h"

#if BTG_OPTION_URL
#  include "urlmgr.h"
#endif

#include "filemgr.h"
#include <bcore/logable.h>
#include "opid.h"

namespace btg
{
   namespace daemon
   {
      /// Handler class used by the daemon.
      class daemonHandler: public btg::core::Logable
         {
         public:
            /// Construct the daemon handler.
            /// 
            /// Note: the pointer _must_ point at initialized daemon data.
            daemonHandler(btg::core::LogWrapperType _logwrapper,
                          daemonData* _dd, 
                          bool const _verboseFlag);

            /// Read data from the transport used.
            void readFromTransport();

            /// Check if timer used expired.
            /// If so, execute an action for each eventhandler which is present.
            void checkTimeout();

            /// Shutdown all eventhandlers.
            void shutdown();

            /// Update the IP filter used for all contained sessions.
            void updateFilter();

            /// Destructor.
            ~daemonHandler();
         private:
            /// Handle quit request.
            void handleQuit(eventHandler* _eventhandler, btg::core::Command* _command);

            /// Handle detach request.
            void handleDetach(eventHandler* _eventhandler, btg::core::Command* _command);

            /// Handle commands that require the authorized user
            /// with a control flag set.
            ///
            /// The commands are kill and set global limits.
            /// @param [in] _command Pointer to the command to handle.
            void handleControlCommand(btg::core::Command* _command);

            /// Handle daemon kill requested by the client.
            void handleKill(btg::core::Command* _command);

            /// Handle setting of global limits.
            void handleGlobalLimit(btg::core::Command* _command);

            /// Validate global limits.
            void validateGlobalLimits(t_int & _limitBytesUpld,
                                      t_int & _limitBytesDwnld,
                                      t_int & _maxUplds,
                                      t_long & _maxConnections) const;

            /// Template used to validate limits.
            template<typename T> void validateLimit(T& _limit) const
               {
                  if ((_limit == 0) || (_limit < btg::core::limitBase::LIMIT_DISABLED))
                     {
                        _limit = btg::core::limitBase::LIMIT_DISABLED;
                     }
               }

            /// Handle sending the current global limit to client.
            void handleGlobalStatus(btg::core::Command* _command);

            /// Handle uptime request.
            void handleUptime(btg::core::Command* _command);

            /// Handle session name request.
            void handleSessionName(eventHandler* _eventhandler, btg::core::Command* _command);

            /// Handle setting session name.
            void handleSessionSetName(eventHandler* _eventhandler, 
                                      btg::core::Command* _command);

            /// Handle getting information about a session.
            void handleSessionInfo(eventHandler* _eventhandler, 
                                   btg::core::Command* _command);

            /// Handle session messages in invalid state.
            void handleSessionInInvalidState(t_int const _id);

            /// Handle initialization of a connection from a client.
            void handleInitConnection(btg::core::Command* _command);

            /// Handle attach request.
            void handleAttach(btg::core::Command* _command);

            /// Handle list session request.
            void handleSessionList(btg::core::Command* _command);

            /// Handle setup request.
            void handleSetup(btg::core::Command* _command);
               
            /// Move a context from one session to another.
            void handleMoveContext(eventHandler* _eventhandler, 
                                   btg::core::Command* _command);

            /// Handle operation status or operation abort messages.
            /// These can be both about URL downloads and file
            /// uploads.
            void handleOpMessages(eventHandler* _eventhandler,
                                  btg::core::Command* _command);

            /// Handle URL messages - download and status.
            void handleUrlMessages(eventHandler* _eventhandler, 
                                   btg::core::Command* _command);

            /// Handle messages used to upload files.
            void handleCreateFileMessages(eventHandler* _eventhandler, 
                                          btg::core::Command* _command);
#if BTG_OPTION_URL
            /// Handle URL status mesasge.
            void handle_CN_CURLSTATUS(btg::core::Command* _command);

            /// Handle URL create message.
            void handle_CN_CCREATEFROMURL(eventHandler* _eventhandler, 
                                          btg::core::Command* _command);

            /// Handle URL abort message.
            void handle_CN_CCREATEURLABORT(btg::core::Command* _command);

            /// Once every few seconds, see if any of the URLs
            /// downloading have changed state.
            void handleUrlDownloads();
#endif // BTG_OPTION_URL

            /// File upload.
            void handle_CN_CCREATEFROMFILE(eventHandler* _eventhandler, 
                                           btg::core::Command* _command);

            /// File uploade, part x of y.
            void handle_CN_CCREATEFROMFILEPART(eventHandler* _eventhandler, 
                                               btg::core::Command* _command);

            /// Status of file upload.
            void handle_CN_CCRFILESTATUS(btg::core::Command* _command);

            /// File upload abort.
            void handle_CN_CCREATEFFABORT(btg::core::Command* _command);

            /// Handle Version request.
            void handleVersion(btg::core::Command* _command);

            /// Handle setting request.
            void handleGetSetting(btg::core::Command* _command);

            /// Handle any other request.
            void handleOther(eventHandler* _eventhandler, btg::core::Command* _command);

            /// Send a command to a connection.
            bool sendCommand(btg::core::externalization::Externalization* _e,
                             btg::core::messageTransport* _transport,
                             t_int _connectionID,
                             btg::core::Command* _command);

            /// Send an error back to the client.
            /// @param [in] _cmdId       The ID of the command this error was caused by.
            /// @param [in] _description Textual error description.
            void sendError(t_int const _cmdId, std::string const& _description);

            /// Send an ACK back to the client.
            /// @param [in] _cmdId The ID of the command this the ACK is a response to.
            bool sendAck(t_int const _cmdId);

            /// Check for dead clients/connections.
            void checkDeadConnections();

            /// Get all the user directories from the auth interface.
            bool getDirectories(std::string & _userTempDir,
                                std::string & _userWorkDir,
                                std::string & _userSeedDir,
                                std::string & _userDestDir,
                                std::string & _errorDescription);

            /// Called when a deserialization fails.
            void handleDeserializationError(btg::core::commandFactory::decodeStatus const& _decodestatus);

            /// Called to log a directory that was not found. Like
            /// during setup.
            void logDirectoryNotFound(std::string const& _type, std::string const& _value);

            /*       */
            /* Data. */
            /*       */

            /// Pointer to daemon data.
            daemonData*                     dd_;

            /// Indicates if the daemon should be verbose.
            bool const                      verboseFlag_;

            /// List of contained sessions.
            sessionList                     sessionlist_;

            /// Current Session.
            long                            session_;

            /// The number of bytes read from the transport.
            t_int                           readBytes_;

            /// The current connection ID.
            t_int                           connectionID_;

            /// Pointer to the current connection.
            Connection*                     connection_;

            /// The number of event handlers.
            t_int                           handlerCount_;

            /// Buffer used by the externalization i/f.
            btg::core::dBuffer              buffer_;

            /// The timer used for checking limits and alerts for
            /// the contained sessions.
            btg::core::os::Timer            session_timer_;

            /// Indicates that the session timer has expired.
            bool                            session_timer_trigger_;
#if BTG_OPTION_URL
            /// Timer used to check for completed URL downloads.
            btg::core::os::Timer            url_timer_;
            /// Indicates that the URL timer timed out.
            bool                            url_timer_trigger_;
#endif // BTG_OPTION_URL
            /// Timer used to check for completed URL downloads.
            btg::core::os::Timer            file_timer_;
            /// Indicates that the URL timer timed out.
            bool                            file_timer_trigger_;
            /// Timer used for setting global limits.
            btg::core::os::Timer            limit_timer_;

            /// Indicates that the limit timer has expired.
            bool                            limit_timer_trigger_;

            /// Timer used for increasing the elapsed or seed time
            /// for torrents.
            btg::core::os::Timer            elapsed_seed_timer_;

            /// Indicates that the elapsed/seed timer has expired.
            bool                            elapsed_timer_trigger_;

            /// Timer used for timed session saving.
            btg::core::os::Timer            periodic_ssave_timer_;

            /// Indicates that the periodic session save timer has
            /// expired.
            bool                            periodic_ssave_timer_trigger_;
#if BTG_DEBUG
            /// Debug: alive counter used for logging.
            t_int                           aliveCounter_;
            /// Debug: alive counter max used for logging.
            t_int const                     aliveCounterMax_;
#endif // BTG_DEBUG

            /// Entity used for accessing/monitoring tcp/ip ports.
            portManager                     portManager_;

            /// Entity used for setting global limits.
            limitManager                    limitManager_;

#if BTG_OPTION_SAVESESSIONS
            /// Instance of the class used for saving sessions to a file.
            SessionSaver                    sessionsaver_;

            /// The timer used for periodicly save sessions
            btg::core::os::Timer            sessiontimer_;
#endif // BTG_OPTION_SAVESESSIONS

            /// The buffer used to send commands back to the
            /// clients.
            btg::core::dBuffer              sendBuffer_;

            /// Command factory used by this instance.
            btg::core::commandFactory       cf_;

            /// Every few seconds, check if any of the file uploads
            /// changed state.
            void handleFileDownloads();
#if BTG_OPTION_URL
            /// URL manager.
            btg::daemon::urlManager  urlmgr;
#endif // BTG_OPTION_URL
            /// File manager.
            btg::daemon::fileManager filemgr;
            /// Used to create unique ids used for tracking status of
            /// commands or aborting the same commands.
            btg::daemon::opId        opid;
         private:
            /// Copy constructor.
            daemonHandler(daemonHandler const& _dh);
            /// Assignment operator.
            daemonHandler& operator=(daemonHandler const& _dh);
         };

   } // namespace daemon
} // namespace btg

#endif // DAEMONHANDLER_H


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

#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <string>

#include <bcore/project.h>

#include <bcore/command_factory.h>
#include <bcore/transport/transport.h>

#include <bcore/command/setup.h>
#include <bcore/addrport.h>
#include <bcore/logable.h>

#include "connectionhandler.h"

namespace btg
{
   namespace daemon
      {

         /**
          * \addtogroup daemon Daemon
          */
         /** @{ */

         class Context;
         class fileTrack;
         class IpFilterIf;
#if BTG_OPTION_EVENTCALLBACK
         class callbackManager;
#endif // BTG_OPTION_EVENTCALLBACK
         class portManager;
         class limitManager;
         class daemonConfiguration;

         /// Daemon handles events using this class.
         class eventHandler: public btg::core::Logable
            {
            public:
               /// Constructor.
               /// @param [in] _logwrapper     Pointer used to send logs to.
               /// @param [in] _verboseFlag    Decides if this eventhandler should log verbose messages.
               /// @param [in] _username       User creating the session.
               /// @param [in] _tempDir        Temporary directory.
               /// @param [in] _workDir        Working directory.
               /// @param [in] _seedDir        Seeding directory.
               /// @param [in] _outputDir      Output directory.
               /// @param [in] _callback       Callback to use, if enabled.
               /// @param [in] _portMgr        Pointer to the port manager to use to obtain ports.
               /// @param [in] _limitMgr       Pointer to the global limit manager.
               /// @param [in] _e              The externalization to use.
               /// @param [in] _session        The session to use.
               /// @param [in] _transport      Pointer to the transport to use to communicate with clients.
               /// @param [in] _filetrack      Pointer to the class keeping track of added/removed torrents.
               /// @param [in] _filter         Pointer to a filter, used to ban peers.
               /// @param [in] _useTorrentName Enable/disable using the torrent name from the tracker.
               /// @param [in] _connectionHandler Connection handler used by the eventhandler to store per-connection data.
               /// @param [in] _sessionName    Name of the session.
               /// @param [in] _cbm            Pointer to the callback manager used.
               eventHandler(btg::core::LogWrapperType _logwrapper,
                            bool const _verboseFlag,
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
                            btg::core::messageTransport* _transport,
                            fileTrack* _filetrack,
                            IpFilterIf* _filter,
                            bool const _useTorrentName,
                            connectionHandler* _connectionHandler,
                            std::string const& _sessionName
#if BTG_OPTION_EVENTCALLBACK
                            , callbackManager* _cbm
#endif // BTG_OPTION_EVENTCALLBACK
                            );

               /// Setup this instance and the context.
               bool setup(const daemonConfiguration* _config,
                          btg::core::setupCommand* _setupcommand);

               /// Handle an event. Send a response if needed.
               /// @param [in] _command Pointer to an instance of command,
               /// @param [in] _connectionID Connection ID of the client
               /// which causes this handler to act.
               void event(btg::core::Command* _command, t_int _connectionID);

               /// Increase the number of connected clients
               void incClients();

               /// Decrease the number of connected clients
               void decClients();

               /// Get the number of connected clients
               t_uint getNumClients();

               /// Get the session id.
               t_long getSession() const;

               /// Stop seeding if the torrent has meet the seeding
               /// limits.
               void checkSeedLimits();

               /// Update elapsed or seed counter.
               void updateElapsedOrSeedCounter();

               /// Log alerts from libtorrent.
               void handleAlerts();

#if BTG_OPTION_SAVESESSIONS
               /// Dump the session as XML based data to allow for
               /// later reloading.
               void serialize(btg::core::externalization::Externalization* _e, 
                              bool const _dumpFastResume);
               /// Read the session back from dumped data.
               /// @param [in] _e       Pointer to the externalization interface used.
               /// @param [in] _version Version of the file
               bool deserialize(btg::core::externalization::Externalization* _e, t_uint _version);
#endif // BTG_OPTION_SAVESESSIONS

               /// Shutdown this eventhandler.
               /// This closes the contained libtorrent session.
               void shutdown();
               
               /// Get the username of the user this eventhandler
               /// belongs to.
               std::string getUsername() const;

               /// Set the name of the session.
               void setName(std::string const& _name);

               /// Get the name of the session.
               std::string getName() const;

               /// Destructor.
               ~eventHandler();
            private:
               /// Indicates if this class should log verbose
               /// messages.
               bool const verboseFlag_;

               /// The externalization used.
               btg::core::externalization::Externalization* externalization_;

               /// The session ID.
               t_long                       session;

               /// User whom this session belongs to.
               std::string                  username_;

               /// Seed limit in percent.
               t_int                        seedLimit_;

               /// Seed timeout in minutes.
               t_long                       seedTimeout_;

               /// Indicates that DHT is being used.
               bool                         useDht_;

               /// The message que used.
               btg::core::messageTransport* transport;

               /// The daemon context.
               btg::daemon::Context*        daemoncontext;

               /// The connection handler.
               connectionHandler*           connHandler;

               /// Number of clients currently attached.
               t_uint                       numClients;

               /// The name of the session this event handler belongs
               /// to.
               std::string                  name_;
#if BTG_OPTION_EVENTCALLBACK
               /// Pointer to the callback manager used.
               callbackManager* cbm_;
#endif // BTG_OPTION_EVENTCALLBACK

               /// Buffer used to send responses back to client which
               /// uses this event handler.
               btg::core::dBuffer           buffer;
               /// Send a command using the messageTransport. Delete the memory used
               /// by the command afterwards.
               /// @param [in] _connectionID The ID of the connection used.
               /// @param [in] _command      The command to send.
               /// \note Warning: This _deletes_ the argument after sending the command.
               void sendCommand(t_int _connectionID, btg::core::Command* _command);

               /// Send either an ack or an error message, based on
               /// the value of _opStatus.
               /// @param [in] _connectionID     The ID of the connection used.
               /// @param [in] _opStatus         True: send an ack, false: send an error with _errorDescription.
               /// @param [in] _type             The command for which to send an message.
               /// @param [in] _errorDescription The error message text.
               void sendAckOrError(t_int _connectionID,
                                   bool _opStatus,
                                   btg::core::Command::commandType _type,
                                   std::string const& _errorDescription);

               /// Send an ack command.
               /// @param [in] _connectionID The ID of the connection used.
               /// @param [in] _type The command for which to send an ACK.
               void sendAck(t_int _connectionID, btg::core::Command::commandType _type);

               /// Send an error command.
               /// @param [in] _connectionID The ID of the connection used.
               /// @param [in] _type         The command for which to send an error message.
               /// @param [in] _error        The error message text.
               void sendError(t_int _connectionID, 
                              btg::core::Command::commandType _type, 
                              std::string const& _error);

               /// Handle list contexts.
               void handle_CN_GLIST(t_int _connectionID);

               /// Create a context.
               void handle_CN_CCREATEWITHDATA(btg::core::Command* _command, t_int _connectionID);

               /// Get the last created context Id.
               void handle_CN_CLAST(btg::core::Command* _command, t_int _connectionID);

               /// Start a context.
               void handle_CN_CSTART(btg::core::Command* _command, t_int _connectionID);

               /// Stop a context.
               void handle_CN_CSTOP(btg::core::Command* _command, t_int _connectionID);

               /// Abort a context.
               void handle_CN_CABORT(btg::core::Command* _command, t_int _connectionID);

               /// Clean a context.
               void handle_CN_CCLEAN(btg::core::Command* _command, t_int _connectionID);

               /// Status of a context.
               void handle_CN_CSTATUS(btg::core::Command* _command, t_int _connectionID);

               /// Status of a number of contexts.
               void handle_CN_CMSTATUS(btg::core::Command* _command, t_int _connectionID);

               /// Limit a context.
               void handle_CN_CLIMIT(btg::core::Command* _command, t_int _connectionID);

               /// Get limit status of a context.
               void handle_CN_CLIMITSTATUS(btg::core::Command* _command, t_int _connectionID);

               /// Get file information about a context.
               void handle_CN_CFILEINFO(btg::core::Command* _command, t_int _connectionID);

               /// Set the files in a torrent which shall be downloaded.
               void handle_CN_CSETFILES(btg::core::Command* _command, t_int _connectionID);

               /// Get the file names in a torrent, which are being downloaded.
               void handle_CN_CGETFILES(btg::core::Command* _command, t_int _connectionID);

               /// Get a list of peers for a context.
               void handle_CN_CPEERS(btg::core::Command* _command, t_int _connectionID);

               /// Get a list of trackers which are used.
               void handle_CN_CGETTRACKERS(btg::core::Command* _command, t_int _connectionID);

            private:
               /// Copy constructor.
               eventHandler(eventHandler const& _eh);
               /// Assignment operator.
               eventHandler& operator=(eventHandler const& _eh);
            };

         /** @} */

      } // namespace daemon
} // namespace btg
#endif

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

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <bcore/externalization/externalization.h>
#include <bcore/transport/transport.h>
#include <bcore/type_btg.h>

#include <bcore/command/command.h>
#include <bcore/command/setup.h>
#include <bcore/command/session_attach.h>

#include <bcore/auth/hash.h>
#include <bcore/logable.h>
#include <bcore/command_factory.h>

namespace btg
{
   namespace core
   {
      namespace client
      {
         /**
          * \addtogroup gencli
          */
         /** @{ */

         class clientCallback;

         /// A statemachine for the generic client.
         class stateMachine: public Logable
         {
         public:
            /// Constructor.
            /// @param [in] _logwrapper     Pointer used to send logs to.
            /// @param [in] _e              Pointer to the externalization which is used.
            /// @param [in] _transport      Pointer to the message que used to communicate with the daemon.
            /// @param [in] _clientcallback Pointer to an instance which are used for callbacks.
            /// @param [in] _verboseFlag    Instructs the statemachine to do verbose logging.
            stateMachine(LogWrapperType _logwrapper,
                         btg::core::externalization::Externalization* _e,
                         btg::core::messageTransport *_transport,
                         clientCallback *_clientcallback,
                         bool const _verboseFlag);

            /// Make the machine work: change states, call callbacks.
            void work();

            // Actions.

            /// Initialize connection.
            /// @param [in] _username The username to authorize with.
            /// @param [in] _hash     Hash of the password to authorize with.
            void doInit(std::string const& _username,
                        btg::core::Hash const& _hash);

            /// Action.
            void doSetup(btg::core::setupCommand* _command);
            /// Action.
            void doQuit();
            /// Action.
            void doKill();
            /// Action.
            void doGlobalLimit(t_int const  _limitBytesUpld,
                               t_int const  _limitBytesDwnld,
                               t_int const  _maxUplds,
                               t_long const _maxConnections);
            /// Action.
            void doGlobalLimitStatus();
            /// Action.
            void doUptime();
            /// Action.
            void doSessionName();
            /// Action.
            void doSetSessionName(std::string const& _name);

            /// Action.
            void doAttach(attachSessionCommand* _command);
            /// Action.
            void doDetach();
            /// Action.
            void doListSessions();
            /// Action.
            void doList();
            /// Action.
            void doCreate(std::string const& _pathToTorrent,
                          bool const _start = true);
            /// Action.
            void doLast();
            /// Action.
            void doAbort(t_int const _contextID, bool const _eraseData, bool const _allContexts = false);
            /// Action.
            void doStart(t_int const _contextID, bool const _allContexts = false);
            /// Action.
            void doStop(t_int const _contextID, bool const _allContexts = false);
            /// Action.
            void doStatus(t_int const _contextID, bool const _allContexts = false);
            /// Action.
            void doFileInfo(t_int const _contextID, bool const _allContexts = false);
            /// Action.
            void doPeers(t_int const _contextID, bool const _allContexts = false);

            /// Action.
            void doLimit(t_int const _contextID,
                         t_int const _uploadRate,
                         t_int const _downloadRate,
                         t_int const _seedLimit,
                         t_long const _seedTimeout,
                         bool const _allContexts = false);

            /// Action.
            void doLimitStatus(t_int const _contextID, bool const _allContexts = false);

            /// Action.
            void doSetFiles(t_int const _id, 
                            selectedFileEntryList const& _files);

            /// Action.
            void doGetFiles(t_int const _id);

            /// Action.
            void doClean(t_int const _contextID, bool const _allContexts = false);

            /// Destructor.
            virtual ~stateMachine();

         private:

            /// Pointer to the externalization which is used.
            btg::core::externalization::Externalization* externalization_;

            /// The command factory used by this class.
            btg::core::commandFactory cf;

            /// The states this machine can be in.
            enum State
            {
               SM_ERROR = -2,        //!< An error occured.
               SM_SERROR = -1,       //!< A session error occured.
               SM_INIT  = 0,         //!< The initial state.
               SM_TRANSINIT,         //!< Initialize the transport.
               SM_TRANSINIT_WAIT,    //!< Wait for the other end of the transport to report back.
               SM_TRANSPORT_READY,   //!< Transport is ready.
               SM_SETUP,             //!< The daemon received a setup command and acknowledged it.
               SM_SETUP_WAIT,        //!< Wait for the daemon to reply.
               SM_COMMAND,           //!< The client is ready for sending commands.
               SM_COMMAND_WAIT,      //!< The client is waiting for an response.
               SM_COMMAND_ACK_WAIT,  //!< The client is waiting for an ACK.
               SM_QUIT,              //!< The client is ready to quit.
               SM_QUIT_WAIT,         //!< The client told the daemon that it wishes to quit.
               SM_KILL,              //!< The client wishes to kill the daemon.
               SM_KILL_WAIT,         //!< The client waits for the confirmation that the daemon was killed.
               SM_ATTACH,            //!< The client wishes to attach to a session.
               SM_ATTACH_WAIT,       //!< The client waits for an ACK.
               SM_DETACH,            //!< The client requested a detach of the current session.
               SM_DETACH_WAIT,       //!< The client waits for the ACK.
               SM_SESSION_LIST,      //!< The client wants to list sessions.
               SM_SESSION_LIST_WAIT, //!< The client waits for the session list.
               SM_FINISHED           //!< The client finished execution, nothing to be done.

            };

            // Counters:

            /// Transport init message counter.
            t_int                                 counter_transinit;
            /// Maximum value of thet ransport init message counter.
            t_int const                           counter_transinit_max;
            /// Setup message counter.
            t_int                                 counter_setup;
            /// Maximum value of the setup message counter.
            t_int const                           counter_setup_max;
            /// Command message counter.
            t_int                                 counter_command;
            /// Maximum value of the command message counter.
            t_int const                           counter_command_max;
            /// Ack message counter.
            t_int                                 counter_ack;
            /// Maximum value of the ack message counter.
            t_int const                           counter_ack_max;
            /// Attach message counter.
            t_int                                 counter_attach;
            /// Maximum value of the attach message counter.
            t_int const                           counter_attach_max;
            /// Session list message counter.
            t_int                                 counter_session_list;
            /// Maximum value of the session list message counter.
            t_int const                           counter_session_list_max;
            /// The message que used to communicate with the daemon.
            btg::core::messageTransport*        transport;
            /// Do* functions add commands to this vector.
            t_commandPointerList                commands;
            // std::vector<btg::core::Command*> commands;
            /// The state this machine is in.
            State                               currentState;
            /// The current command.
            btg::core::Command::commandType     currentCommand;
            /// Array containing the expected replies to the current command.
            btg::core::Command::commandType     expectedReply[2];
            /// The type of command the expected Ack is for.
            btg::core::Command::commandType     ackForCommand;
            /// Pointer to the instance of the client callback class.
            clientCallback*                     clientcallback;

            /// Used to keep the number of calls to read.
            t_int                                 read_counter;

            /// The max number of reads before this machine gives up.
            t_int const                           max_read_counter;

            /// The minimum amount of sleeping betweeen reads.
            t_int const                           min_sleep_in_ms;

            /// Indicates if the statemachine should do verbose logging.
            bool const                            verboseFlag_;

            /// Convert a state to a std::string.
            /// @param [in] _state The state.
            /// @return String representing the state.
            std::string stateToString(State _state) const;

            /// Used to change the state.
            void changeState(State _to);

            /// Check the state of the machine.
            bool checkState(State _state) const;

            /// After sending a command, set the reply that is expected.
            void setExpectedReply(btg::core::Command::commandType _type);

            /// Tell this machine which command its handling now.
            void setCurrentCommand(btg::core::Command *_command);

            /// Call a callback for certain response.
            void callCallback(btg::core::Command *_command);

            /// Call a callback for certain errors or a default error handler.
            void callErrorCallback(btg::core::Command *_command);

            /// Call a callback, for use with received ACKs.
            void callAckCallback(t_int const _commandtype);

            /// Get a pointer to a command. Do* functions use this.
            /// @return Pointer to a command or NULL(0) on failture.
            btg::core::Command* getCommand();

            /// Send a command to the daemon.
            bool sendCommand(btg::core::Command *_command);

            /// Receive a command from the daemon.
            bool receiveCommand(btg::core::Command* & _target,
                                bool const _waitforever = false);

            //
            // The following functions are called when
            // entering a certain state - all called from the
            // work function.
            //

            /// Action taked when in certain state.
            void step_SM_INIT(bool & _status);

            /// Action taked when in certain state.
            void step_SM_TRANSINIT(bool & _status);

            /// Action taked when in certain state.
            void step_SM_TRANSINIT_WAIT(bool & _status);

            /// Action taked when in certain state.
            void step_SM_TRANSPORT_READY(bool & _status);

            /// Action taked when in certain state.
            void step_SM_SETUP(bool & _status);

            /// Action taked when in certain state.
            void step_SM_SETUP_WAIT(bool & _status);

            /// Action taked when in certain state.
            void step_SM_COMMAND(bool & _status);

            /// Action taked when in certain state.
            void step_SM_COMMAND_WAIT(bool & _status);

            /// Action taked when in certain state.
            void step_SM_COMMAND_ACK_WAIT(bool & _status);

            /// Action taked when in certain state.
            void step_SM_ERROR(bool & _status);

            /// Action taked when in certain state.
            void step_SM_SESSION_LIST(bool & _status);

            /// Action taked when in certain state.
            void step_SM_SESSION_LIST_WAIT(bool & _status);

            /// Action taked when in certain state.
            void step_SM_ATTACH(bool & _status);

            /// Action taked when in certain state.
            void step_SM_ATTACH_WAIT(bool & _status);

            /// Action taked when in certain state.
            void step_SM_DETACH(bool & _status);

            /// Action taked when in certain state.
            void step_SM_DETACH_WAIT(bool & _status);

            /// Action taked when in certain state.
            void step_SM_QUIT(bool & _status);

            /// Action taked when in certain state.
            void step_SM_QUIT_WAIT(bool & _status);

            /// Action taked when in certain state.
            void step_SM_KILL(bool & _status);

            /// Action taked when in certain state.
            void step_SM_KILL_WAIT(bool & _status);

            /// Action taked when in certain state.
            void step_SM_FINISHED(bool & _status);

            // Callback functions, used in a switch statement.

            /// Call a callback. Received response to request to list
            /// contexts.
            void cb_CN_GLIST(btg::core::Command* _command);
            /// Call a callback. Received a response to a setup.
            void cb_CN_GSETUP(btg::core::Command* _command);
            /// Call a callback. Received global limit status
            /// response.
            void cb_CN_GLIMITSTAT(btg::core::Command* _command);
            /// Call a callback. Received clean response.
            void cb_CN_CCLEAN(btg::core::Command* _command);
            /// Call a callback. Received uptime response.
            void cb_CN_GUPTIME(btg::core::Command* _command);
            /// Call a callback. Received status response.
            void cb_CN_CSTATUS(btg::core::Command* _command);
            /// Call a callback. Received status response for all
            /// contexts.
            void cb_CN_CALLSTATUSRSP(btg::core::Command* _command);
            /// Call a callback. Received the context ID of the last
            /// added torrent.
            void cb_CN_CLASTRSP(btg::core::Command* _command);
            /// Call a callback. Received a file info for a context.
            void cb_CN_CFILEINFO(btg::core::Command* _command);
            /// Call a callback. Received a list of peers for a
            /// context.
            void cb_CN_CPEERS(btg::core::Command* _command);
            /// Call a callback. Received the limit status for a
            /// context.
            void cb_CN_CLIMITSTATUS(btg::core::Command* _command);
            /// Call a callback. Received the response to setting
            /// files to download for a context.
            void cb_CN_CGETFILES(btg::core::Command* _command);
            /// Call a callback. Received the name of the current
            /// session.
            void cb_CN_SNAME(btg::core::Command* _command);

         private:
            /// Copy constructor.
            stateMachine(stateMachine const& _sm);
            /// Assignment operator.
            stateMachine& operator=(stateMachine const& _sm);
         };

      } // namespace client
   } // namespace core
} // namespace btg

#endif

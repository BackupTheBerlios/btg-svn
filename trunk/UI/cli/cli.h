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

#ifndef CLI_H
#define CLI_H

#include <string>
#include <vector>

#include <bcore/project.h>
#include <bcore/type_btg.h>
#include <bcore/command/command.h>
#include <bcore/transport/transport.h>
#include <bcore/dbuf.h>
#include <bcore/client/statemachine.h>
#include <bcore/client/clientcallback.h>
#include <bcore/client/clienthandler.h>
#include <bcore/client/configuration.h>

#include <bcore/client/helper.h>

namespace btg
{
   namespace UI
      {
         namespace cli
            {

               /// Represents a CLI command.
               class CLICommand
                  {
                  public:
                     /// CLI Command IDs.
                     enum ID
                        {
                           cmd_help = 0,      //!< Help.
                           cmd_quit,          //!< Quit.
                           cmd_kill,          //!< Kill the daemon.
                           cmd_uptime,        //!< Request daemon uptime.
                           cmd_list,          //!< List.
                           cmd_create,        //!< Create.
                           cmd_start,         //!< Start.
                           cmd_stop,          //!< Stop.
                           cmd_abort,         //!< Abort.
                           cmd_erase,         //!< Erase.
                           cmd_status,        //!< Status.
                           cmd_fileinfo,      //!< File info.
                           cmd_peers,         //!< List of peers.
                           cmd_limit,         //!< Per torrent Limit.
                           cmd_glimit,        //!< Global limit.
                           cmd_glimitstatus,  //!< Display global limits.
                           cmd_clean,         //!< Clean.
                           cmd_last,          //!< Last.
                           cmd_syntax,        //!< Syntax.
                           cmd_detach,        //!< Detach.
                           cmd_undefined = -1 //!< Undefined.
                        };
                  public:
                     /// Default constructor.
                     CLICommand(ID const           _id,
                                std::string const& _name,
                                std::string const& _shortName,
                                std::string const& _description,
                                std::string const& _syntax,
                                t_uint      const  _numberOfParams
                                );
                     /// Copy constructor.
                     CLICommand(CLICommand const& _clicommand);

                     /// Get the id of a command.
                     CLICommand::ID getId() const;

                     /// Get the name.
                     std::string getName() const;

                     /// Get the short name.
                     std::string getShortName() const;

                     /// Get the description.
                     std::string getDescription() const;

                     /// Get the syntax.
                     std::string getSyntax() const;

                     /// Get the number of params.
                     t_uint      getNumberOfParams() const;

                     /// The destructor.
                     ~CLICommand();
                  private:
                     /// Const members only, assignment is not possible.
                     CLICommand& operator=(CLICommand const & _rhs);

                     /// The id of a command.
                     ID          const id;
                     /// The name.
                     std::string const name;
                     /// The short name.
                     std::string const shortName;
                     /// The description.
                     std::string const description;
                     /// The syntax.
                     std::string const syntax;
                     /// The number of parameters.
                     t_uint      const numberOfParams;
                  };

               /// Represents a list of commands with methods to resolve strings into commands.
               class CLICommandList
                  {
                  public:
                     CLICommandList();
                     /// Add a command to this list.
                     void addCommand(CLICommand* _clicommand);

                     /// Resolve a string into a command
                     CLICommand::ID resolve(std::string const& _s) const;

                     /// Returns a string representing a command name.
                     /// @param [in] _id A command ID.
                     std::string getName(CLICommand::ID const _id) const;

                     /// Generate help, which is a list of all commands with a short description.
                     std::string genHelp() const;

                     /// Generate a string which describes the syntax used for a command.
                     /// @param [in] _id A command ID.
                     std::string genSyntax(CLICommand::ID const _id) const;

                     /// Get the number of params required by a command.
                     t_uint getNumberOfParams(CLICommand::ID const _id) const;

                     /// Destructor.
                     ~CLICommandList();
                  private:
                     /// List of commands.
                     std::vector<CLICommand*>              list;
                     /// Maps command ID to command pointer.
                     std::map<CLICommand::ID, CLICommand*> cmap;
                  };

               /**
                * \addtogroup cli
                */
               /** @{ */

               class CLICommandList;

               /// Client handler class.
               class cliHandler : 
                  public btg::core::client::clientCallback, 
                  public btg::core::client::clientHandler
                  {
                  public:
                     /// Used to react to the different kinds of inputs from the user.
                     enum cliResponse
                        {
                           INPUT_OK              = 1,  //!< Remote command produced output, present it to the user.
                           INPUT_IGNORE          = 2,  //!< Ignore the output of the executed command.
                           INPUT_LOCAL           = 3,  //!< Local command, present output or error to user.
                           INPUT_SURE_QUERY      = 4,  //!< Present a "Are you sure y/n" dialog.
                           INPUT_ERROR           = -1, //!< Wrong number of arguments, could not convert a string to a number etc.
                           INPUT_NOT_A_COMMAND   = -2, //!< The user entered something that was not recognized as a command.
                           UNDEFINED_RESPONSE    = 255 //!< An undefined response, something is really wrong.
                        };

                     /// Values returned by cliHandler::getContextId.
                     enum contextResponse
                        {
                           ALL_CONTEXT_FLAG      = -10, //!< A remote command wishes to do something to all contexts.
                           WRONG_CONTEXT_ID      = -11, //!< A remote command failed because of a wrong context id.
                           WRONG_PARAMETER_COUNT = -12, //!< A command was not sent because it contained too few arguments.
                        };

                     /// Constructor.
                     cliHandler(btg::core::externalization::Externalization* _e,
                                btg::core::messageTransport*            _transport,
                                btg::core::client::clientConfiguration* _config,
                                btg::core::client::lastFiles*           _lastfiles,
                                bool const _verboseFlag,
                                bool const _autoStartFlag
                                );

                     /// Handle input from the user.
                     /// If _useSavedId is set to true, handle a saved
                     /// command instead of parsing the input.
                     cliHandler::cliResponse handleInput(std::string const& _input,
                                                         bool const _useSavedId = false);

                     /// Returns true if there is output.
                     bool outputAvailable() const;

                     /// Get output.
                     std::string getOutput();

                     /// Returns true if there is an error message available.
                     bool errorAvailable() const;

                     /// Get an error message.
                     std::string getError();

                     /// Destructor.
                     virtual ~cliHandler();
                  private:
                     /// Read, deserialize and act on a response for a specific command.
                     /// Responses: specific, ack, error.
                     void handleResponse(CLICommand::ID _commandid);

                     /// Take the default action on error, which is to print it for
                     /// the user to see.
                     void defaultErrorHandler(CLICommand::ID _commandid, btg::core::Command *_command);

                     /// Take the default action on ack, which is to do nothing.
                     void defaultAckHandler(CLICommand::ID _commandid,
                                            btg::core::Command *_command,
                                            std::string const& _message);

                     /// Set the text which the user should see.
                     void setOutput(std::string const& _output);

                     /// Clear the output.
                     void clearOutput();

                     /// Append to the output.
                     void addOutput(std::string const& _output);

                     /// Set the error string.
                     void setError(std::string const& _error);

                     /// Get the context ID.
                     /// @param [in] _s         Input string.
                     /// @param [in] _commandID The command ID, used to determine the number of parameters.
                     /// @return Context ID,
                     /// cliHandler::ALL_CONTEXT_FLAG or
                     /// cliHandler::WRONG_CONTEXT_ID.
                     t_int getContextId(std::string const& _s, CLICommand::ID const _commandID);

                     /// Make sure that a rate is either btg::core::limitBase::LIMIT_DISABLED or greater or equal to 0.
                     /// If a valid limit is given, it is multiplied with the multiplier argument.
                     /// @param [in] _rate       Value to validate.
                     /// @param [in] _multiplier The multiplier.
                     /// @return True on succes.
                     bool validateLimitSetting(t_int & _rate,
                                               t_int const _multiplier);

                     /// See the other validateLimitSetting function.
                     /// @param [in] _rate       Value to validate.
                     /// @param [in] _multiplier The multiplier.
                     /// @return True on succes.
                     bool validateLimitSetting(t_long & _rate,
                                               t_int const _multiplier);

                     /// Global limit handling.
                     void handleGlobalLimit(std::string const& _input, cliResponse & _result);

                     /// Limit handling, a certain context.
                     void handleLimit(t_int const _context_id,
                                      std::string const& _input,
                                      cliResponse & _result);

                     /// Limit handling, all contexts.
                     void handleLimitOnAll(std::string const& _input,
                                           cliResponse & _result);

                     /// Global limit handling.
                     void handleGlobalLimitStatus(cliResponse & _result);

                     void onTransportInit();
                     void onSetup(t_long const _session);
                     void onSetupError(std::string const& _message);

                     void onAttach();
                     void onAttachError(std::string const& _message);

                     void onDetach();
                     //void onDetachError();

                     void onList(t_intList const& _contextIDs, t_strList const& _filenames);
                     void onListError();

                     void onCreateWithData();
                     void onAbort();
                     void onStart();
                     void onStop();

                     /// Helper function. Convert a status object into a string.
                     /// @param [in] _status      Status to convert into a string.
                     /// @param [in] _output      Write output to this reference.
                     /// @param [in] _shortFormat If false, long output, else short output.
                     void StatusToString(btg::core::Status const& _status,
                                         std::string & _output,
                                         bool const _shortFormat = false) const;
		     
                     void onStatus(btg::core::Status const& _status);
                     void onStatusAll(t_statusList const& _vstatus);
                     void onStatusError(std::string const& _errorDescription);

                     void onFileInfo(t_fileInfoList const& _fileinfolist);
                     void onFileInfoError(std::string const& _errorDescription);

                     void onPeers(t_peerList const& _peerlist);
                     void onPeersError(std::string const& _errorDescription);

                     void onLimit();

                     void onListSessions(t_longList const& _sessions);
                     void onListSessionsError(std::string const& _errorDescription);

                     void onSessionError();

                     void onClean(t_strList const& _filenames, t_intList const& _contextIDs);

                     void onError(std::string const& _errorDescription);
                     void onFatalError(std::string const& _errorDescription);

                     void onLimitStatus(t_int const _uploadRate, 
                                        t_int const _downloadRate, 
                                        t_int const _seedLimit, 
                                        t_long const _seedTimeout);

                     void onLimitStatusError(std::string const& _errorDescription);

                     void onSetFiles();

                     void onSetFilesError(std::string const& _errorDescription);

                     void onSelectedFiles(btg::core::selectedFileEntryList const& _files);

                     void onSelectedFilesError(std::string const& _errorDescription);

                     void onQuit();
                     void onKill();
                     void onKillError(std::string _ErrorDescription);

                     void onUptime(t_ulong const _uptime);

                     void onGlobalLimit();
                     void onGlobalLimitError(std::string _ErrorDescription);
                     void onGlobalLimitResponse(t_int const  _limitBytesUpld,
                                                t_int const  _limitBytesDwnld,
                                                t_int const  _maxUplds,
                                                t_long const _maxConnections);
                     void onGlobalLimitResponseError(std::string _ErrorDescription);

                     /// Set the current context ID.
                     void setCurrentID(t_int const _currentID);

                     /// Get the current context ID.
                     t_int getCurrentID() const;

                     /// Attempt to find out which command the user
                     /// wrote by examining the _input parameter.
                     /// @param [in] _input The user's input.
                     CLICommand::ID findCommand(std::string const& _input) const;

                  private:
                     /// Command history.
                     t_strList      history;

                     /// Set if there is output that should be written to the user.
                     bool           isOutputAvailable;

                     /// The string to be written to the user.
                     std::string    output;

                     /// Indicates that an error occured and that an
                     /// error-description er available.
                     bool           isErrorAvailable;

                     /// String which contains the description of an error.
                     std::string    error;

                     /// List of commands.
                     CLICommandList commandlist;

                     /// The current ID.
                     t_int          currentID;

                     /// Flag.
                     bool           useCurrentID;

                     /// When the user is queried about if he is sure
                     /// about executing a command, this member
                     /// contains the ID of the command to execute.
                     CLICommand::ID savedId;
                  };

               /** @} */

               /// Helper: executes a number of tasks when this client starts.
               class cliStartupHelper: public btg::core::client::startupHelper
                  {
                  public:
                     /// Constructor.
                     cliStartupHelper(btg::core::client::clientConfiguration*        _config,
                                      btg::core::client::commandLineArgumentHandler* _clah,
                                      btg::core::messageTransport*                   _messageTransport,
                                      btg::core::client::clientHandler*              _handler);

                     /// Query the user about which session to attach to.
                     virtual t_long queryUserAboutSession(t_longList const& _sessions) const;
                     virtual bool authUserQuery();
                     virtual void showSessions(t_longList const& _sessions) const;
                     /// Destructor.
                     virtual ~cliStartupHelper();
                  };

            } // namespace cli
      } // namespace UI
} // namespace btg

#endif

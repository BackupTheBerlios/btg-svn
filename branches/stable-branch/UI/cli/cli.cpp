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

#include "cli.h"

extern t_int global_btg_run;

#include <iostream>

#include <bcore/command_factory.h>
#include <bcore/util.h>
#include <bcore/t_string.h>

#include <bcore/hru.h>
#include <bcore/hrr.h>

#include <bcore/os/stdin.h>

#include <bcore/command/context.h>
#include <bcore/command/context_abort.h>
#include <bcore/command/context_clean.h>
#include <bcore/command/context_fi.h>
#include <bcore/command/context_fi_rsp.h>
#include <bcore/command/context_li_status.h>
#include <bcore/command/context_li_status_rsp.h>
#include <bcore/command/context_limit.h>
#include <bcore/command/context_peers.h>
#include <bcore/command/context_peers_rsp.h>
#include <bcore/command/context_start.h>
#include <bcore/command/context_status.h>
#include <bcore/command/context_status_rsp.h>
#include <bcore/command/context_stop.h>
#include <bcore/command/error.h>

#include <bcore/client/ratio.h>

#include "runstate.h"

namespace btg
{
   namespace UI
   {
      namespace cli
      {

         using namespace btg::core;
         using namespace btg::core::client;

         CLICommand::CLICommand(ID const           _id,
                                std::string const& _name,
                                std::string const& _shortName,
                                std::string const& _description,
                                std::string const& _syntax,
                                t_uint      const  _numberOfParams
                                )
            : id(_id),
              name(_name),
              shortName(_shortName),
              description(_description),
              syntax(_syntax),
              numberOfParams(_numberOfParams)
         {
         }

         CLICommand::CLICommand(CLICommand const& _clicommand)
            : id(_clicommand.id),
              name(_clicommand.name),
              shortName(_clicommand.shortName),
              description(_clicommand.description),
              syntax(_clicommand.syntax),
              numberOfParams(_clicommand.numberOfParams)
         {
         }

         CLICommand::ID CLICommand::getId() const
         {
            return id;
         }

         std::string CLICommand::getName() const
         {
            return name;
         }

         std::string CLICommand::getShortName() const
         {
            return shortName;
         }

         std::string CLICommand::getDescription() const
         {
            return description;
         }

         std::string CLICommand::getSyntax() const
         {
            return syntax;
         }

         t_uint CLICommand::getNumberOfParams() const
         {
            return numberOfParams;
         }

         CLICommand::~CLICommand()
         {
         }

         /* */
         /* */
         /* */

         CLICommandList::CLICommandList()
            : list(0),
              cmap()
         {

         }

         void CLICommandList::addCommand(CLICommand* _clicommand)
         {
            list.push_back(_clicommand);
            cmap[_clicommand->getId()] = _clicommand;
         }

         CLICommand::ID CLICommandList::resolve(std::string const& _s) const
         {
            CLICommand::ID                      found = CLICommand::cmd_undefined;
            std::vector<CLICommand*>::const_iterator iter;
            CLICommand*                         cmd;
            for (iter = list.begin(); iter != list.end(); iter++)
               {
                  cmd = *iter;
                  if ((cmd->getName() == _s) || (cmd->getShortName() == _s))
                     {
                        found = cmd->getId();
                        break;
                     }
               }
            return found;
         }

         std::string CLICommandList::getName(CLICommand::ID const _id) const
         {
            std::map<CLICommand::ID, CLICommand*>::const_iterator iter = cmap.find(_id);
            if (iter != cmap.end())
               {
                  return iter->second->getName();
               }

            return "Unknown command";
         }

         std::string CLICommandList::genHelp() const
         {
            std::string output;

            std::vector<CLICommand*>::const_iterator iter;
            CLICommand*                         cmd;
            for (iter = list.begin(); iter != list.end(); iter++)
               {
                  cmd = *iter;
                  output += cmd->getName() + " (" + cmd->getShortName() + ")\t" + cmd->getDescription() + GPD->sNEWLINE();
               }

            output += GPD->sNEWLINE();
            output += "The IDs used with for example the start command are numbers which are greater or equal to zero. The client remembers the last used ID, so the following commands can be used without the ID.";
            output += GPD->sNEWLINE();
            output += "The insert key toggles between insert/overwrite mode. Overwrite mode is the default.";
            output += GPD->sNEWLINE();
            return output;
         }

         std::string CLICommandList::genSyntax(CLICommand::ID const _id) const
         {
            std::string output = "Unknown command.";

            std::map<CLICommand::ID, CLICommand*>::const_iterator iter = cmap.find(_id);
            if (iter != cmap.end())
               {
                  CLICommand* cmd = iter->second;
                  if (cmd->getSyntax() != "")
                     {
                        output = cmd->getName() + " (" + cmd->getShortName() + ") " + cmd->getSyntax() + GPD->sNEWLINE();
                     }
                  else
                     {
                        output = cmd->getName() + " (" + cmd->getShortName() + "): no parameters." + GPD->sNEWLINE();
                     }
               }

            return output;
         }

         t_uint CLICommandList::getNumberOfParams(CLICommand::ID const _id) const
         {
            std::map<CLICommand::ID, CLICommand*>::const_iterator iter = cmap.find(_id);
            if (iter != cmap.end())
               {
                  return iter->second->getNumberOfParams();
               }
            return 0;
         }

         CLICommandList::~CLICommandList()
         {
            std::vector<CLICommand*>::iterator iter;
            for (iter = list.begin(); iter != list.end(); iter++)
               {
                  CLICommand* clic = *iter;
                  delete clic;
                  clic = 0;
               }

            list.clear();
            cmap.clear();
         }

         /* */
         /* */
         /* */

         cliHandler::cliHandler(btg::core::LogWrapperType _logwrapper,
                                btg::core::externalization::Externalization* _e,
                                messageTransport* _transport,
                                clientConfiguration* _config,
                                btg::core::client::lastFiles* _lastfiles,
                                bool const _verboseFlag,
                                bool const _autoStartFlag)
            : clientHandler(_logwrapper,
                            _e,
                            this,
                            _transport,
                            _config,
                            _lastfiles,
                            _verboseFlag,
                            _autoStartFlag),
              history(0),
              isOutputAvailable(false),
              output(""),
              isErrorAvailable(false),
              error(""),
              commandlist(),
              currentID(cliHandler::WRONG_CONTEXT_ID),
              useCurrentID(false),
              savedId(CLICommand::cmd_undefined)
         {
            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_help,
                                                  "help",
                                                  "he",
                                                  "Shows help.",
                                                  "",
                                                  0)
                                   );
            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_quit,
                                                  "quit",
                                                  "qu",
                                                  "Quit this application, close the session running on the daemon.",
                                                  "",
                                                  0)
                                   );
            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_kill,
                                                  "kill",
                                                  "ki",
                                                  "Kill the daemon.",
                                                  "",
                                                  0)
                                   );
            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_glimit,
                                                  "glimit",
                                                  "gl",
                                                  "Set global limits.",
                                                  "<upload> <download> <# uplds> <# conns>: <upload> - bandwidth limit in KiB/sec, <download> - bandwitdth limit in KiB/sec, <# uplds> - Number of concurrent uploads, <# conns> - number of concurrent connections. Use '-1' to disable any limit.",
                                                  4)
                                   );
            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_glimitstatus,
                                                  "gstatus",
                                                  "gs",
                                                  "Display global limits.",
                                                  "",
                                                  0)
                                   );
            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_uptime,
                                                  "uptime",
                                                  "ut",
                                                  "Request daemon uptime.",
                                                  "",
                                                  0)
                                   );

            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_list,
                                                  "list",
                                                  "ls",
                                                  "Display a list of IDs/filenames which the daemon knows about.",
                                                  "",
                                                  0)
                                   );
            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_create,
                                                  "create",
                                                  "cr",
                                                  "Create a new download.",
                                                  "<filename> or last: <filename> - path to a torrent file, last - create all last torrents (see last).",
                                                  1)
                                   );
            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_start,
                                                  "start",
                                                  "st",
                                                  "Start downloading.",
                                                  "<id>: <id> - an ID or \"all\" to start all downloads.",
                                                  1)
                                   );
            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_stop,
                                                  "stop",
                                                  "sp",
                                                  "Stop downloading.",
                                                  "<id>: <id> - an ID or \"all\" to stop all downloads.",
                                                  1)
                                   );
            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_abort,
                                                  "abort",
                                                  "ab",
                                                  "Abort downloading.",
                                                  "<id>: <id> - an ID or \"all\" to abort all downloads.",
                                                  1)
                                   );

            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_erase,
                                                  "erase",
                                                  "er",
                                                  "Abort and erase downloaded data.",
                                                  "<id>: <id> - an ID or \"all\" to erase all downloads.",
                                                  1)
                                   );

            commandlist.addCommand(new CLICommand(CLICommand::cmd_status,
                                                  "status",
                                                  "ss",
                                                  "Show status information about a download.",
                                                  "<id>: <id> - an ID or \"all\" to show status about all downloads.",
                                                  1)
                                   );
            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_fileinfo,
                                                  "info",
                                                  "in",
                                                  "Show info about the files of a download.",
                                                  "<id>: <id> - an ID.",
                                                  1)
                                   );
            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_peers,
                                                  "peers",
                                                  "pl",
                                                  "Show list of peers.",
                                                  "<id>: <id> - an ID.",
                                                  1)
                                   );
            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_limit,
                                                  "limit",
                                                  "li",
                                                  "Set limits on a download.",
                                                  "<id> <upload> <download> <seed %> <seed timeout>: <id> - an ID, <upload> - bandwidth limit in KiB/sec, <download> - bandwitdth limit in KiB/sec, <seed %> - How many percent a torrent is seeded before its stopped, <seed timeout> - How many minutes a torrent is seeded before its stopped. Use '-1' to disable any limit.",
                                                  5)
                                   );
            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_clean,
                                                  "clean",
                                                  "cl",
                                                  "Clean finished torrent.",
                                                  "<id>: <id> - an ID or \"all\" to clean all downloads.",
                                                  1)
                                   );
            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_last,
                                                  "last",
                                                  "la",
                                                  "Show the ten last opened files.",
                                                  "",
                                                  0)
                                   );
            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_syntax,
                                                  "syntax",
                                                  "sx",
                                                  "Shows syntax for a command",
                                                  "<command>: <command> - a command name.",
                                                  1)
                                   );
            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_detach,
                                                  "detach",
                                                  "de",
                                                  "Detach the client from the running session.",
                                                  "",
                                                  0)
                                   );

            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_sname,
                                                  "sessionname",
                                                  "sn",
                                                  "Print session name.",
                                                  "",
                                                  0)
                                   );

            commandlist.addCommand(
                                   new CLICommand(CLICommand::cmd_ssname,
                                                  "setsessionname",
                                                  "sa",
                                                  "Set session name.",
                                                  "<name> - session name.",
                                                  1)
                                   );
         }

         cliHandler::cliResponse cliHandler::handleInput(std::string const& _input, bool const _useSavedId)
         {
            this->clearOutput();

            // Recognize the commands from a user:
            cliResponse result = cliHandler::UNDEFINED_RESPONSE;

            CLICommand::ID id;
            if (!_useSavedId)
               {
                  id = this->findCommand(_input);
               }
            else
               {
                  // Handle saved command.
                  // This is done in order to first query the user
                  // about if he is really sure.
                  // This is important for commands like quit and
                  // kill.
                  id = savedId;
               }

            switch (id)
               {
               case CLICommand::cmd_quit:
                  {
                     if (_useSavedId)
                        {
                           result = INPUT_LOCAL;
                           this->reqQuit();
                        }
                     else
                        {
                           result  = INPUT_SURE_QUERY;
                           savedId = CLICommand::cmd_quit;
                        }
                     break;
                  }
               case CLICommand::cmd_detach:
                  {
                     result = INPUT_LOCAL;
                     this->reqDetach();
                     break;
                  }
               case CLICommand::cmd_kill:
                  {
                     if (_useSavedId)
                        {
                           result = INPUT_LOCAL;
                           this->reqKill();
                        }
                     else
                        {
                           result  = INPUT_SURE_QUERY;
                           savedId = CLICommand::cmd_kill;
                        }
                     break;
                  }
               case CLICommand::cmd_glimit:
                  {
                     // Set global limit.
                     this->handleGlobalLimit(_input, result);
                     break;
                  }
               case CLICommand::cmd_glimitstatus:
                  {
                     this->handleGlobalLimitStatus(result);
                     break;
                  }
               case CLICommand::cmd_uptime:
                  {
                     result = INPUT_OK;
                     this->reqUptime();
                     break;
                  }
               case CLICommand::cmd_sname:
                  {
                     result = INPUT_OK;
                     this->reqSessionName();
                     break;
                  }
               case CLICommand::cmd_ssname:
                  {
                     std::string sname;

                     t_strList parts = Util::splitLine(_input, " ");
                     if (parts.size() > 1)
                        {
                           result = INPUT_OK;
                           t_strListCI iter = parts.begin();
                           iter++;
                           
                           t_uint size    = parts.size()-1;
                           t_uint counter = 1;
                           for (; iter != parts.end(); iter++)
                              {
                                 sname += *iter;
                                 if (counter < size)
                                    {
                                       sname += " ";
                                    }
                                 counter++;
                              }

                           this->reqSetSessionName(sname);
                        }
                     else
                        {
                           setError("Missing parameter.");
                           result = INPUT_ERROR;
                        }
                     break;
                  }
               case CLICommand::cmd_help:
                  {
                     result = INPUT_LOCAL;
                     // List all the available commands:
                     setOutput(commandlist.genHelp());
                     break;
                  }
               case CLICommand::cmd_list:
                  {
                     result = INPUT_OK;
                     this->reqList();
                     break;
                  }
               case CLICommand::cmd_create:
                  {
                     result = INPUT_OK;
                     // create /path/to/file
                     t_strList parts = Util::splitLine(_input, " ");
                     if (parts.size() > 1)
                        {
                           if (parts.at(1) == "last")
                              {
                                 t_strList filelist = lastfiles->getLastFiles();
                                 this->reqCreate(filelist);
                              }
                           else
                              {
                                 this->reqCreate(parts.at(1));
                              }
                        }
                     else
                        {
                           setError("Missing parameter.");
                           result = INPUT_ERROR;
                        }
                     break;
                  }
               case CLICommand::cmd_last:
                  {
                     result = INPUT_LOCAL;
                     t_strList filelist = lastfiles->getLastFiles();
                     if (filelist.size() > 0)
                        {
                           std::string temp_output;
                           t_strListCI iter;
                           for (iter  = filelist.begin();
                                iter != filelist.end();
                                iter++)
                              {
                                 temp_output += *iter + GPD->sNEWLINE();
                              }
                           setOutput(temp_output);
                           // List the files.
                        }
                     else
                        {
                           result = INPUT_ERROR;
                           setError("No files to list.");
                        }
                     break;
                  }
               case CLICommand::cmd_start:
               case CLICommand::cmd_stop:
               case CLICommand::cmd_abort:
               case CLICommand::cmd_erase:
               case CLICommand::cmd_status:
               case CLICommand::cmd_fileinfo:
               case CLICommand::cmd_peers:
               case CLICommand::cmd_limit:
               case CLICommand::cmd_clean:
                  {
                     result = INPUT_OK;
                     t_int context_id = getContextId(_input, id);

                     if (context_id != cliHandler::WRONG_CONTEXT_ID)
                        {
                           // Save the current ID.
                           this->setCurrentID(context_id);
                        }

                     switch (context_id)
                        {
                        case cliHandler::WRONG_CONTEXT_ID:
                           {
                              this->setCurrentID(cliHandler::WRONG_CONTEXT_ID);
                              setError("Wrong context id.");
                              result = INPUT_ERROR;
                              break;
                           }
                        case cliHandler::WRONG_PARAMETER_COUNT:
                           {
                              this->setCurrentID(cliHandler::WRONG_CONTEXT_ID);
                              setError("Missing parameter(s).");
                              result = INPUT_ERROR;
                              break;
                           }
                        case cliHandler::ALL_CONTEXT_FLAG:
                           {
                              switch (id)
                                 {
                                 case CLICommand::cmd_start:
                                    {
                                       this->reqStart(contextCommand::UNDEFINED_CONTEXT, true);
                                       break;
                                    }
                                 case CLICommand::cmd_stop:
                                    {
                                       this->reqStop(contextCommand::UNDEFINED_CONTEXT, true);
                                       break;
                                    }
                                 case CLICommand::cmd_abort:
                                    {
                                       this->reqAbort(contextCommand::UNDEFINED_CONTEXT, false /* do not erase* */, true);
                                       break;
                                    }
                                 case CLICommand::cmd_erase:
                                    {
                                       this->reqAbort(contextCommand::UNDEFINED_CONTEXT, true /* erase */, true);
                                       break;
                                    }
                                 case CLICommand::cmd_status:
                                    {
                                       this->reqStatus(contextCommand::UNDEFINED_CONTEXT, true);
                                       break;
                                    }
                                 case CLICommand::cmd_fileinfo:
                                    {
                                       this->reqFileInfo(contextCommand::UNDEFINED_CONTEXT, true);
                                       break;
                                    }
                                 case CLICommand::cmd_peers:
                                    {
                                       this->reqPeers(contextCommand::UNDEFINED_CONTEXT, true);
                                       break;
                                    }
                                 case CLICommand::cmd_clean:
                                    {
                                       this->reqClean(contextCommand::UNDEFINED_CONTEXT, true);
                                       break;
                                    }
                                 case CLICommand::cmd_limit:
                                    {
                                       this->handleLimitOnAll(_input, result);
                                    }

                                 default:
                                    {
                                       // Do nothing, since the other commands are not handled here.
                                    }
                                 } // switch id
                              break;
                           } // ALL_CONTEXT_FLAG

                        default: // Valid context id.
                           {
                              switch (id)
                                 {
                                 case CLICommand::cmd_start:
                                    {
                                       this->reqStart(context_id);
                                       break;
                                    }
                                 case CLICommand::cmd_stop:
                                    {
                                       this->reqStop(context_id);
                                       break;
                                    }
                                 case CLICommand::cmd_abort:
                                    {
                                       this->reqAbort(context_id, false);
                                       break;
                                    }
                                 case CLICommand::cmd_erase:
                                    {
                                       this->reqAbort(context_id, true /* erase */);
                                       break;
                                    }
                                 case CLICommand::cmd_status:
                                    {
                                       this->reqStatus(context_id);
                                       break;
                                    }
                                 case CLICommand::cmd_fileinfo:
                                    {
                                       this->reqFileInfo(context_id);
                                       break;
                                    }
                                 case CLICommand::cmd_peers:
                                    {
                                       this->reqPeers(context_id);
                                       break;
                                    }
                                 case CLICommand::cmd_clean:
                                    {
                                       this->reqClean(context_id);
                                       break;
                                    }
                                 case CLICommand::cmd_limit:
                                    {
                                       this->handleLimit(context_id, _input, result);
                                       break;
                                    }
                                 default:
                                    {
                                       // Do nothing, since the other commands are not handled here.
                                    }
                                 } // switch id
                           } // valid context
                        } // switch context_id
                     break;
                  } // cmd_status

               case CLICommand::cmd_syntax:
                  {
                     result = INPUT_LOCAL;

                     this->setCurrentID(cliHandler::WRONG_CONTEXT_ID);

                     t_strList parts = Util::splitLine(_input, " ");
                     if (parts.size() > 1)
                        {
                           std::string commandname = parts.at(1);
                           CLICommand::ID id  = this->findCommand(commandname);
                           setOutput(commandlist.genSyntax(id));
                        }
                     else
                        {
                           setError("Missing parameter.");
                           result = INPUT_ERROR;
                        }

                     break;
                  }
               case CLICommand::cmd_undefined:
                  {
                     result = INPUT_NOT_A_COMMAND;
                     break;
                  }
               } // switch

            return result;
         }

         bool cliHandler::validateLimitSetting(t_int & _rate, t_int const _multiplier)
         {
            bool status = true;

            if (_rate < 0)
               {
                  if (_rate != btg::core::limitBase::LIMIT_DISABLED)
                     {
                        status = false;
                     }
               }
            else
               {
                  _rate *= _multiplier;
               }
            return status;
         }

         bool cliHandler::validateLimitSetting(t_long & _rate, t_int const _multiplier)
         {
            bool status = true;

            if (_rate < 0)
               {
                  if (_rate != btg::core::limitBase::LIMIT_DISABLED)
                     {
                        status = false;
                     }
               }
            else
               {
                  _rate *= _multiplier;
               }
            return status;
         }

         void cliHandler::defaultErrorHandler(CLICommand::ID _commandid, Command *_command)
         {
            this->cmd_failture++;

            switch(_command->getType())
               {
               case Command::CN_ERROR:
                  {
                     if (dynamic_cast<errorCommand*>(_command)->messagePresent())
                        {
                           setError(dynamic_cast<errorCommand*>(_command)->getMessage());
                        }
                     else
                        {
                           setError(commandlist.getName(_commandid) + ": Unhandled Error.");
                        }
                     break;
                  }
               default:
                  {
                     setError(commandlist.getName(_commandid) + ": Unhandled Error.");
                  }
               }
         }

         void cliHandler::defaultAckHandler(CLICommand::ID _commandid,
                                            Command *_command,
                                            std::string const& _message)
         {
            switch(_command->getType())
               {
               case Command::CN_ACK:
                  {
                     setOutput(_message + ".");
                     break;
                  }

               default:
                  setError(commandlist.getName(_commandid) + ": unhandled response.");
               }

         }

         bool cliHandler::outputAvailable() const
         {
            return isOutputAvailable;
         }

         void cliHandler::setOutput(std::string const& _output)
         {
            isOutputAvailable = true;
            isErrorAvailable  = false;
            output           += GPD->sNEWLINE();
            output           += _output;
         }

         void cliHandler::clearOutput()
         {
            // Clear the old output first.
            output.clear();
         }

         void cliHandler::addOutput(std::string const& _output)
         {
            isOutputAvailable = true;
            output           += _output;
         }

         std::string cliHandler::getOutput()
         {
            isOutputAvailable = false;
            return output;
         }

         bool cliHandler::errorAvailable() const
         {
            return isErrorAvailable;
         }

         void cliHandler::setError(std::string const& _error)
         {
            isOutputAvailable = false;
            isErrorAvailable  = true;
            error             = _error;
         }

         std::string cliHandler::getError()
         {
            isErrorAvailable = false;
            return error;
         }

         t_int cliHandler::getContextId(std::string const& _s, CLICommand::ID const _commandID)
         {
            t_int context_id            = cliHandler::WRONG_CONTEXT_ID;
            t_uint const numberOfParams = commandlist.getNumberOfParams(_commandID);

            // Reset the flag which makes the limit command use the
            // current context ID.
            this->useCurrentID = false;

            t_strList parts = Util::splitLine(_s, " ");

            if (parts.size() <= 1)
               {
                  context_id = cliHandler::WRONG_PARAMETER_COUNT;
               }

            if ((context_id != cliHandler::WRONG_PARAMETER_COUNT) && (parts.at(1) == "all"))
               {
                  context_id = cliHandler::ALL_CONTEXT_FLAG;
               }

            if ((parts.size() - 1) == numberOfParams)
               {
                  // The input string is missing one parameter,
                  // possible because the user wants to use the last used id.

                  if ((context_id != cliHandler::WRONG_PARAMETER_COUNT) &&
                      (context_id != cliHandler::ALL_CONTEXT_FLAG))
                     {
                        if (!Util::stringToInt(parts.at(1), context_id))
                           {
                              context_id = cliHandler::WRONG_CONTEXT_ID;
                           }
                     }
               }
            else
               {
                  context_id = cliHandler::WRONG_PARAMETER_COUNT;
               }

            if (context_id == cliHandler::WRONG_PARAMETER_COUNT)
               {
                  if (this->getCurrentID() != cliHandler::WRONG_CONTEXT_ID)
                     {
                        context_id   = this->getCurrentID();
                        useCurrentID = true;
                     }
               }

            return context_id;
         }

         CLICommand::ID cliHandler::findCommand(std::string const& _input) const
         {
            t_strList parts = Util::splitLine(_input, " ");

            if (parts.size() < 1)
               {
                  return CLICommand::cmd_undefined;
               }

            std::string first = parts.at(0);

            return commandlist.resolve(first);
         }

         void cliHandler::onTransportInit()
         {

         }

         void cliHandler::onSetup(t_long const _session)
         {
            setSession(_session);
            setOutput("New session successfully created.");
            this->setupDone = true;
         }

         void cliHandler::onSetupError(std::string const& _message)
         {
            setSession(ILLEGAL_ID);
            setupFailtureMessage = _message;
            setupDone            = false;
         }

         void cliHandler::onAttach()
         {
            this->attachDone = true;
         }

         void cliHandler::onAttachError(std::string const& _message)
         {
            std::cout << _message << std::endl;
            setSession(ILLEGAL_ID);
            attachFailtureMessage = _message;
            attachDone            = false;
         }

         void cliHandler::onDetach()
         {
            setOutput("Detaching current session");
            global_btg_run = GR_QUIT;
         }

         void cliHandler::onList(t_intList const& _contextIDs, t_strList const& _filenames)
         {
            std::string output("");
            t_strListCI ci = _filenames.begin();
            for (t_intListCI i=_contextIDs.begin(); i != _contextIDs.end(); i++)
               {
                  output += convertToString<t_int>(*i);
                  output += ": ";
                  output += *ci;
                  output += "\n";
                  ci++;
               }

            setOutput(GPD->sNEWLINE() + output);
         }

         void cliHandler::onListError()
         {
            this->cmd_failture++;

            setError("Listing failed.");
            BTG_NOTICE(logWrapper(), "Listing failed.");

            // Listing failed, so there are no context IDs.
            setCurrentID(cliHandler::WRONG_CONTEXT_ID);
         }

         void cliHandler::onError(std::string const& _errorDescription)
         {
            this->cmd_failture++;

            setError(_errorDescription);
            BTG_NOTICE(logWrapper(), "On error: " << _errorDescription);

            setCurrentID(cliHandler::WRONG_CONTEXT_ID);
         }

         void cliHandler::onFatalError(std::string const& _errorDescription)
         {
            setOutput("Quitting: " + _errorDescription);

            global_btg_run = GR_FATAL;
         }

         void cliHandler::onQuit()
         {
            setOutput("Quitting current session");
            global_btg_run = GR_QUIT;
         }

         void cliHandler::onKill()
         {
            setOutput("Daemon killed.");
         }

         void cliHandler::onKillError(std::string _ErrorDescription)
         {
            setOutput("Unable to kill daemon: " + _ErrorDescription);
         }

         void cliHandler::onUptime(t_ulong const _uptime)
         {
            setOutput("Daemon uptime: " + convertToString<t_ulong>(_uptime) + " sec..");
         }

         void cliHandler::onSessionName(std::string const& _name)
         {
            setOutput("Session name: '" + _name + "'.");
         }

         void cliHandler::onSetSessionName()
         {
            setOutput("Session name set.");
         }

         void cliHandler::onGlobalLimit()
         {
            setOutput("Global limits set.");
         }

         void cliHandler::onGlobalLimitError(std::string _ErrorDescription)
         {
            setError("Unable to set global limits.");
         }
         
         void cliHandler::onGlobalLimitResponse(t_int const  _limitBytesUpld,
                                                t_int const  _limitBytesDwnld,
                                                t_int const  _maxUplds,
                                                t_long const _maxConnections)
         {
            std::string limitdesc = "Global limits:";
            limitdesc += GPD->sNEWLINE();

            limitdesc += " ";
            if (_limitBytesUpld == limitBase::LIMIT_DISABLED)
               {
                  limitdesc += "unlimited";
               }
            else
               {
                  limitdesc += convertToString<t_int>(_limitBytesUpld);
                  limitdesc += " bytes/sec";
               }
            limitdesc += " upload,";
            limitdesc += GPD->sNEWLINE();

            limitdesc += " ";
            if (_limitBytesDwnld == limitBase::LIMIT_DISABLED)
               {
                  limitdesc += "unlimited";
               }
            else
               {
                  limitdesc += convertToString<t_int>(_limitBytesDwnld);
                  limitdesc += " bytes/sec";
               }
            limitdesc += " download,";
            limitdesc += GPD->sNEWLINE();

            limitdesc += " ";
            if (_maxUplds == limitBase::LIMIT_DISABLED)
               {
                  limitdesc += "unlimited";
               }
            else
               {
                  limitdesc += convertToString<t_int>(_maxUplds);
               }
            limitdesc += " # uploads,";
            limitdesc += GPD->sNEWLINE();

            limitdesc += " ";
            if (_maxConnections == limitBase::LIMIT_DISABLED)
               {
                  limitdesc += "unlimited";
               }
            else
               {
                  limitdesc += convertToString<t_long>(_maxConnections);
               }

            limitdesc += " # connections.";
            limitdesc += GPD->sNEWLINE();

            setOutput(limitdesc);
         }
         
         void cliHandler::onGlobalLimitResponseError(std::string _ErrorDescription)
         {
            setError("Unable to get global limits.");
         }


         void cliHandler::onCreateWithData()
         {
            lastfiles->addLastFile(last_filename);
            setOutput("Created " + last_filename + ".");
            last_filename.clear();
         }

         void cliHandler::onAbort()
         {
            if (eraseFlag)
               {
                  setOutput("Erased.");
               }
            else
               {
                  setOutput("Aborted.");
               }
         }

         void cliHandler::onStart()
         {
            setOutput("Started.");
         }

         void cliHandler::onStop()
         {
            setOutput("Stopped.");
         }

         void cliHandler::onStatus(btg::core::Status const& _status)
         {
            std::string output(GPD->sNEWLINE());
            StatusToString(_status, output);
            setOutput(output);
         }

         void cliHandler::onStatusAll(t_statusList const& _vstatus)
         {
            std::string output(GPD->sNEWLINE() + "Status of all contexts:" + GPD->sNEWLINE());

            t_long down_rate = 0;
            t_long up_rate   = 0;
            for (t_statusListCI cvsi = _vstatus.begin();
                 cvsi != _vstatus.end();
                 cvsi++)
               {
                  StatusToString(*cvsi, output, true /* short status */);

                  down_rate += cvsi->downloadRate();
                  up_rate   += cvsi->uploadRate();
               }

            if (_vstatus.size() > 1)
               {
                  humanReadableRate hrr = humanReadableRate::convert(down_rate);

                  output += "--" + GPD->sNEWLINE();
                  output += "Totals: " + hrr.toString() + " dl, ";

                  hrr = humanReadableRate::convert(up_rate);
                  output += hrr.toString() + " ul." + GPD->sNEWLINE();
               }

            setOutput(output);
         }

         void cliHandler::StatusToString(btg::core::Status const& _status,
                                         std::string & _output,
                                         bool const _shortFormat) const
         {
            if (!_shortFormat)
               {
                  // The long format.

                  _output += "Id: " + convertToString<t_int>(_status.contextID());
                  _output += GPD->sNEWLINE();
                  _output += "Filename: " + _status.filename();
                  _output += GPD->sNEWLINE();
                  _output += "Status: ";

                  switch (_status.status())
                     {
                     case Status::ts_undefined:
                        _output += "Undefined";
                        break;
                     case Status::ts_queued:
                        _output += "Waiting";
                        break;
                     case Status::ts_checking:
                        _output += "Checking";
                        break;
                     case Status::ts_connecting:
                        _output += "Connecting";
                        break;
                     case Status::ts_downloading:
                        _output += "Downloading";
                        break;
                     case Status::ts_seeding:
                        _output += "Seeding (download done)";
                        break;
                     case Status::ts_finished:
                        _output += "Finished (download done)";
                        break;
                     case Status::ts_stopped:
                        _output += "Stopped";
                        break;
                     }

                  _output += GPD->sNEWLINE();
                  _output += "Up/down ratio: ";
                  std::string st_ratio;
                  btg::core::client::CalculateUlDlRatio(_status, st_ratio);
                  _output += st_ratio;
                  _output += GPD->sNEWLINE();

                  // Progress:
                  std::string timespec("Time left: ");

                  if (_status.validTimeLeft())
                     {
                        _status.timeLeftToString(timespec, _shortFormat);
                        _output += timespec;
                        _output += GPD->sNEWLINE();
                     }

                  humanReadableUnit hru = humanReadableUnit::convert(_status.downloadTotal());
                  _output += "Total download: " + hru.toString() + GPD->sNEWLINE();

                  hru = humanReadableUnit::convert(_status.uploadTotal());
                  _output += "Total upload: " + hru.toString() + GPD->sNEWLINE();

                  humanReadableRate hrr = humanReadableRate::convert(static_cast<t_uint>(_status.downloadRate()));

                  _output += "Download rate: " + hrr.toString() + GPD->sNEWLINE();

                  hrr = humanReadableRate::convert(static_cast<t_uint>(_status.uploadRate()));
                  _output += "Upload rate: " + hrr.toString() + GPD->sNEWLINE();

                  _output += "Done: " + convertToString<t_ulong>(_status.done()) + " %." + GPD->sNEWLINE();

                  _output += "Leeches/seeders: ";
                  _output += convertToString<t_int>(_status.leechers());
                  _output += "/";
                  _output += convertToString<t_int>(_status.seeders());

                  _output += GPD->sNEWLINE();

               }
            else
               {
                  // The short format.

                  std::string short_filename;

                  _output += convertToString<t_int>(_status.contextID()) + ": ";
                  if (Util::getFileFromPath(_status.filename(), short_filename))
                     {
                        _output += short_filename;
                     }
                  else
                     {
                        _output += _status.filename();
                     }

                  _output += " ";

                  switch (_status.status())
                     {
                     case Status::ts_undefined:
                        _output += "unde";
                        break;
                     case Status::ts_queued:
                        _output += "wait";
                        break;
                     case Status::ts_checking:
                        _output += "chek";
                        break;
                     case Status::ts_connecting:
                        _output += "conn";
                        break;
                     case Status::ts_downloading:
                        _output += "down";
                        break;
                     case Status::ts_seeding:
                        _output += "seed";
                        break;
                     case Status::ts_finished:
                        _output += "fini";
                        break;
                     case Status::ts_stopped:
                        _output += "stop";
                        break;
                     }

                  _output += ", ";
                  if (_status.done() <= 9)
                     {
                        _output += "0";
                     }
                  _output += convertToString<t_ulong>(_status.done()) + " %, ";

                  // Progress:
                  if (_status.validTimeLeft())
                     {
                        std::string timespec;
                        _status.timeLeftToString(timespec, _shortFormat);

                        timespec += ", ";
                        _output  += timespec;
                     }

                  humanReadableRate hrr = humanReadableRate::convert(_status.downloadRate());
                  _output += hrr.toString(true) + " dl, ";

                  hrr = humanReadableRate::convert(_status.uploadRate());
                  _output += hrr.toString(true) + " ul.";
                  _output += GPD->sNEWLINE();
               }
         }
	
         void cliHandler::onFileInfo(t_fileInfoList const& _fileinfolist)
         {
            std::string output;

            t_uint const width = 79;
            t_uint charcounter = 0;

            t_fileInfoListCI iter;
            for (iter = _fileinfolist.begin(); iter != _fileinfolist.end(); iter++)
               {
                  charcounter = 0;

                  output     += "Filename: " + iter->getFilename() + GPD->sNEWLINE();
                  output     += "Bits: " + GPD->sNEWLINE();

                  if (iter->isFull())
                     {
                        for (t_uint counter=0; counter<iter->size(); counter++)
                           {
                              output += "X";

                              if (charcounter >= width)
                                 {
                                    output     += GPD->sNEWLINE();
                                    charcounter = 0;
                                 }
                              charcounter++;
                           }
                     }
                  else if(iter->isEmpty())
                     {
                        for (t_uint counter=0; counter<iter->size(); counter++)
                           {
                              output += "_";

                              if (charcounter >= width)
                                 {
                                    output     += GPD->sNEWLINE();
                                    charcounter = 0;
                                 }
                              charcounter++;
                           }
                     }
                  else
                     {
                        t_bitList bitlist = iter->getBits();
                        t_bitListCI piece_iter;
                        for (piece_iter = bitlist.begin(); piece_iter != bitlist.end(); piece_iter++)
                           {
                              if (*piece_iter)
                                 {
                                    output += "X";
                                 }
                              else
                                 {
                                    output += "_";
                                 }

                              if (charcounter >= width)
                                 {
                                    output     += GPD->sNEWLINE();
                                    charcounter = 0;
                                 }
                              charcounter++;
                           } // for piece iter
                     }
                  output += GPD->sNEWLINE();
               } // for file iter
            setOutput(output);
         }

         void cliHandler::onFileInfoError(std::string const& _errorDescription)
         {
            this->cmd_failture++;

            setError(_errorDescription);
         }

         void cliHandler::onPeers(t_peerList const& _peerlist)
         {
            t_int const maxPeers  = 40;
            t_int       peerCount = 0;

            std::string output = "Peers";
            output += GPD->sNEWLINE();

            t_peerListCI iter;

            t_int leeches = 0;
            t_int seeders = 0;

            std::string peer_str;

            for (iter=_peerlist.begin(); iter!= _peerlist.end(); iter++)
               {
                  peer_str += iter->toString();
                  peer_str += GPD->sNEWLINE();

                  if (iter->seeder())
                     {
                        seeders++;
                     }
                  else
                     {
                        leeches++;
                     }

                  if (peerCount >= maxPeers)
                     {
                        break;
                     }
                  peerCount++;
               }

            output += "(";
            output += convertToString<t_int>(leeches);
            output += " leeches, ";
            output += convertToString<t_int>(seeders);
            output += " seeds):";
            output += GPD->sNEWLINE();
            output += peer_str;

            setOutput(output);
         }

         void cliHandler::onPeersError(std::string const& _errorDescription)
         {
            this->cmd_failture++;

            setError(_errorDescription);
            setCurrentID(cliHandler::WRONG_CONTEXT_ID);
         }

         void cliHandler::onLimit()
         {
            setOutput("Limited.");
         }

         void cliHandler::onStatusError(std::string const& _errorDescription)
         {
            this->cmd_failture++;

            setError(_errorDescription);
            setCurrentID(cliHandler::WRONG_CONTEXT_ID);
         }

         void cliHandler::onListSessions(t_longList const& _sessions,
                                         t_strList const& _sessionNames)
         {
            sessionList  = _sessions;
            sessionNames = _sessionNames;
         }

         void cliHandler::onSessionError()
         {
            this->cmd_failture++;

            global_btg_run = GR_QUIT;
            setError("Invalid session. Quitting.");
         }

         void cliHandler::onClean(t_strList const& _filenames, t_intList const& _contextIDs)
         {
            std::string output(GPD->sNEWLINE() + "Following files finished downloading:" + GPD->sNEWLINE());

            for (t_strListCI vsci = _filenames.begin();
                 vsci != _filenames.end();
                 vsci++)
               {
                  output += *vsci + GPD->sNEWLINE();
                  // Since the torrent finished downloading, remove it
                  // from the list of last opened files.
                  lastfiles->removeLastFile(*vsci);
               }
            setOutput(output);
         }

         void cliHandler::onListSessionsError(std::string const& _errorDescription)
         {
            this->cmd_failture++;

            std::cout << _errorDescription << std::endl;
         }

         void cliHandler::onLimitStatus(t_int const _uploadRate, t_int const _downloadRate, t_int const _seedLimit, t_long const _seedTimeout)
         {
            BTG_FATAL_ERROR(logWrapper(),
                            GPD->sCLI_CLIENT(), "Unused function, onLimitStatus, called with parameters: " << _uploadRate << ", " << _downloadRate << ", " << _seedLimit << ", " << _seedTimeout << ".");
         }

         void cliHandler::onLimitStatusError(std::string const& _errorDescription)
         {
            this->cmd_failture++;

            BTG_FATAL_ERROR(logWrapper(),
                            GPD->sCLI_CLIENT(), "Unused function, onLimitStatusError, called with parameter: " << _errorDescription << ".");
         }

         void cliHandler::setCurrentID(t_int const _currentID)
         {
            currentID = _currentID;
         }

         t_int cliHandler::getCurrentID() const
         {
            return currentID;
         }

         void cliHandler::handleLimitOnAll(std::string const& _input, cliResponse & _result)
         {
            // Get the arguments.
            t_strList parts = Util::splitLine(_input, " ");

            t_uint partSize;
            t_uint initialIterStep;

            if (!useCurrentID)
               {
                  // User wrote a context ID.
                  partSize        = 5;
                  initialIterStep = 2;
               }
            else
               {
                  // No context ID - using the saved
                  // context ID from the last executed command.
                  partSize        = 4;
                  initialIterStep = 1;
               }

            if (parts.size() > partSize)
               {
                  t_strListCI iter = parts.begin();
                  iter            += initialIterStep;

                  t_int uploadrate = convertStringTo<t_int>(*iter);

                  if (!this->validateLimitSetting(uploadrate, btg::core::limitBase::KiB_to_B))
                     {
                        this->setCurrentID(cliHandler::WRONG_CONTEXT_ID);
                        if (!useCurrentID)
                           {
                              setError("Wrong parameter, #2");
                           }
                        else
                           {
                              setError("Wrong parameter, #1");
                           }

                        _result = INPUT_ERROR;
                        return;
                     }

                  iter++;

                  t_int downloadrate = convertStringTo<t_int>(*iter);

                  if (!this->validateLimitSetting(downloadrate, btg::core::limitBase::KiB_to_B))
                     {
                        this->setCurrentID(cliHandler::WRONG_CONTEXT_ID);
                        if (!useCurrentID)
                           {
                              setError("Wrong parameter, #3");
                           }
                        else
                           {
                              setError("Wrong parameter, #2");
                           }
                        _result = INPUT_ERROR;
                        return;
                     }

                  iter++;

                  t_int seedLimit = convertStringTo<t_int>(*iter);

                  if (!this->validateLimitSetting(seedLimit, 1))
                     {
                        this->setCurrentID(cliHandler::WRONG_CONTEXT_ID);
                        if (!useCurrentID)
                           {
                              setError("Wrong parameter, #4");
                           }
                        else
                           {
                              setError("Wrong parameter, #3");
                           }
                        _result = INPUT_ERROR;
                        return;
                     }

                  iter++;

                  t_long seedTimeout = convertStringTo<t_long>(*iter);

                  if (!this->validateLimitSetting(seedTimeout, 1))
                     {
                        this->setCurrentID(cliHandler::WRONG_CONTEXT_ID);
                        if (!useCurrentID)
                           {
                              setError("Wrong parameter, #5");
                           }
                        else
                           {
                              setError("Wrong parameter, #4");
                           }
                        _result = INPUT_ERROR;
                        return;
                     }

                  _result = INPUT_OK;
                  this->reqLimit(contextCommand::UNDEFINED_CONTEXT, uploadrate, downloadrate, seedLimit, seedTimeout, true);
               }
            else
               {
                  this->setCurrentID(cliHandler::WRONG_CONTEXT_ID);
                  setError("Missing parameter(s).");
                  _result = INPUT_ERROR;
               }
         }

         void cliHandler::handleLimit(t_int const _context_id, std::string const& _input, cliResponse & _result)
         {
            // Get the arguments.
            t_strList parts = Util::splitLine(_input, " ");

            t_uint partSize;
            t_uint initialIterStep;

            if (!useCurrentID)
               {
                  // User wrote a context ID.
                  partSize        = 5;
                  initialIterStep = 2;
               }
            else
               {
                  // No context ID - using the saved
                  // context ID from the last executed command.
                  partSize        = 4;
                  initialIterStep = 1;
               }

            if (parts.size() > partSize)
               {
                  t_strListCI iter = parts.begin();
                  iter += initialIterStep;
                  
                  t_int uploadrate = convertStringTo<t_int>(*iter);
                  
                  if (!this->validateLimitSetting(uploadrate, btg::core::limitBase::KiB_to_B))
                     {
                        this->setCurrentID(cliHandler::WRONG_CONTEXT_ID);
                        if (!useCurrentID)
                           {
                              setError("Wrong parameter, #2");
                           }
                        else
                           {
                              setError("Wrong parameter, #1");
                           }
                        
                        _result = INPUT_ERROR;
                        return;
                     }
                  
                  iter++;
                  
                  t_int downloadrate = convertStringTo<t_int>(*iter);

                  if (!this->validateLimitSetting(downloadrate, btg::core::limitBase::KiB_to_B))
                     {
                        this->setCurrentID(cliHandler::WRONG_CONTEXT_ID);
                        if (!useCurrentID)
                           {
                              setError("Wrong parameter, #3");
                           }
                        else
                           {
                              setError("Wrong parameter, #2");
                           }
                        _result = INPUT_ERROR;
                        return;
                     }
                  
                  iter++;
                  
                  t_int seedLimit = convertStringTo<t_int>(*iter);
                  
                  if (!this->validateLimitSetting(seedLimit, 1))
                     {
                        this->setCurrentID(cliHandler::WRONG_CONTEXT_ID);
                        if (!useCurrentID)
                           {
                              setError("Wrong parameter, #4");
                           }
                        else
                           {
                              setError("Wrong parameter, #3");
                           }
                        _result = INPUT_ERROR;
                        return;
                     }
                  
                  iter++;
                  
                  t_long seedTimeout = convertStringTo<t_long>(*iter);
                  
                  if (!this->validateLimitSetting(seedTimeout, 1))
                     {
                        this->setCurrentID(cliHandler::WRONG_CONTEXT_ID);
                        if (!useCurrentID)
                           {
                              setError("Wrong parameter, #5");
                           }
                        else
                           {
                              setError("Wrong parameter, #4");
                           }
                        _result = INPUT_ERROR;
                        return;
                     }
                  
                  _result = INPUT_OK;
                  this->reqLimit(_context_id, uploadrate, downloadrate, seedLimit, seedTimeout);
               }
            else
               {
                  this->setCurrentID(cliHandler::WRONG_CONTEXT_ID);
                  setError("Missing parameter(s).");
                  _result = INPUT_ERROR;
               }
         }

         void cliHandler::handleGlobalLimit(std::string const& _input, cliResponse & _result)
         {
            t_strList parts = Util::splitLine(_input, " ");

            // Number of parameters.
            t_uint partSize = 4;

            // The first position is the command.
            t_uint initialIterStep = 1;

            if (parts.size() > partSize)
               {
                  t_strListCI iter = parts.begin();
                  iter            += initialIterStep;

                  t_int uploadrate = convertStringTo<t_int>(*iter);
                  
                  if (!this->validateLimitSetting(uploadrate, btg::core::limitBase::KiB_to_B))
                     {
                        setError("Wrong parameter, #1");

                        _result = INPUT_ERROR;
                        return;
                     }

                  iter++;

                  t_int downloadrate = convertStringTo<t_int>(*iter);
                  
                  if (!this->validateLimitSetting(downloadrate, btg::core::limitBase::KiB_to_B))
                     {
                        setError("Wrong parameter, #2");

                        _result = INPUT_ERROR;
                        return;
                     }

                  iter++;

                  t_int maxUploads = convertStringTo<t_int>(*iter);

                  if (!this->validateLimitSetting(maxUploads, 1))
                     {
                        setError("Wrong parameter, #3");

                        _result = INPUT_ERROR;
                        return;
                     }

                  iter++;

                  t_long maxConnections = convertStringTo<t_long>(*iter);
                  
                  if (!this->validateLimitSetting(maxConnections, 1))
                     {
                        setError("Wrong parameter, #4");

                        _result = INPUT_ERROR;
                        return;
                     }
                  
                  _result = INPUT_OK;
                  this->reqGlobalLimit(uploadrate,
                                       downloadrate,
                                       maxUploads,
                                       maxConnections);
               }
            else
               {
                  this->setCurrentID(cliHandler::WRONG_CONTEXT_ID);
                  setError("Missing parameter(s).");
                  _result = INPUT_ERROR;
                  return;
               }
         }

         void cliHandler::handleGlobalLimitStatus(cliResponse & _result)
         {
            _result = INPUT_OK;
            this->reqGlobalLimitStatus();
         }

         void cliHandler::onSetFiles()
         {

         }

         void cliHandler::onMove()
         {
            
         }

         void cliHandler::onSetFilesError(std::string const& _errorDescription)
         {

         }
         
         void cliHandler::onSelectedFiles(btg::core::selectedFileEntryList const& _files)
         {

         }
         
         void cliHandler::onSelectedFilesError(std::string const& _errorDescription)
         {

         }

         void cliHandler::onSessionInfo(bool const _encryption, bool const _dht)
         {
            dht_enabled_        = _encryption;
            encryption_enabled_ = _dht;
         }

         cliHandler::~cliHandler()
         {
         }

         cliStartupHelper::cliStartupHelper(btg::core::LogWrapperType _logwrapper,
                                            btg::core::client::clientConfiguration*        _config,
                                            btg::core::client::commandLineArgumentHandler* _clah,
                                            btg::core::messageTransport*                   _messageTransport,
                                            btg::core::client::clientHandler*              _handler)
            : btg::core::client::startupHelper(_logwrapper,
                                               GPD->sCLI_CLIENT(),
                                               _config,
                                               _clah,
                                               _messageTransport,
                                               _handler)
         {
         }

         t_long cliStartupHelper::queryUserAboutSession(t_longList const& _sessions,
                                                        t_strList const& _sessionsIDs) const
         {
            std::cout << "Session(s):" << std::endl;

            t_strListCI sessionIdIter = _sessionsIDs.begin();
            t_int session_no          = 0;
            for (t_longListCI vlci = _sessions.begin();
                 vlci != _sessions.end();
                 vlci++)
               {
                  std::cout << session_no << ": " << *vlci << " (" << *sessionIdIter << ")" << std::endl;
                  sessionIdIter++;
                  session_no++;
               }

            // Get user input.
            bool done = false;
            t_int input = -1;
            while (done != true)
               {
                  if ((session_no-1) > 0)
                     {
                        std::cout << "Enter a number (0-" << (session_no-1) << "): " << std::endl;
                     }
                  else
                     {
                        std::cout << "Enter a number: " << std::endl;
                     }

                  if(!(std::cin >> input))
                     {
                        std::cin.clear();
                        std::cin.ignore(254, '\n');
                     }
                  else
                     {
                        if (input >= 0 && input < session_no)
                           {
                              done = true;
                           }
                     }
               }

            return _sessions.at(input);
         }

         bool cliStartupHelper::authUserQuery()
         {
            bool status = true;

            std::cout << "Username:" << std::endl;

            std::string username;

            std::cin >> username;

            std::cout << "Password:" << std::endl;

            std::string password;

            if (!btg::core::os::stdIn::getPassword(password))
               {
                  std::cout << "Unable to get password." << std::endl;

                  status = false;
                  return status;
               }

            setAuth(username, password);

            return status;
         }

         void cliStartupHelper::showSessions(t_longList const& _sessions,
                                             t_strList const& _sessionNames) const
         {
            t_strListCI sessionNameIter = _sessionNames.begin();

            if (_sessions.size() > 0)
               {
                  std::cout << "Session(s):" << std::endl;

                  for (t_longListCI vlci = _sessions.begin();
                       vlci != _sessions.end();
                       vlci++)
                     {
                        std::cout << *vlci << " (" << *sessionNameIter << ")" << std::endl;
                        sessionNameIter++;
                     }
               }
            else
               {
                  std::cout << "No sessions to list." << std::endl;
               }
         }

         cliStartupHelper::~cliStartupHelper()
         {

         }

      } // namespace cli
   } // namespace UI
} // namespace btg

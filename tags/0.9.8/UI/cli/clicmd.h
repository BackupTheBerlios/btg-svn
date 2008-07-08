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

#ifndef CLICMD_H
#define CLICMD_H

#include <string>
#include <vector>

#include <bcore/project.h>
#include <bcore/type_btg.h>

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
                           cmd_url,           //!< Create from URL.
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
                           cmd_sname,         //!< Session name.
                           cmd_ssname,        //!< Set session name.
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

            } // namespace cli
      } // namespace UI
} // namespace btg

#endif // CLICMD_H

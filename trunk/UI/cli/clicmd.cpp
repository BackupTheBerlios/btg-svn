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

#include "clicmd.h"

namespace btg
{
   namespace UI
   {
      namespace cli
      {

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
            CLICommand* cmd;
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

      } // namespace cli
   } // namespace UI
} // namespace btg


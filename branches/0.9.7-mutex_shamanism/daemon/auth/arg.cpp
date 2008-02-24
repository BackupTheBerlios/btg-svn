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

#include "arg.h"

#include <bcore/project.h>
#include <bcore/copts.h>

#include <boost/program_options.hpp>

#include <iostream>

namespace btg
{
   namespace daemon
   {
      namespace auth
      {

         using namespace btg::core;

#define configLabel      "config,f"
#define configLabelKey   "config"

#define createLabel      "create,c"
#define createLabelKey   "create"

#define addLabel         "add,a"
#define addLabelKey      "add"

#define modifyLabel      "modify,m"
#define modifyLabelKey   "modify"

#define deleteLabel      "remove,r"
#define deleteLabelKey   "remove"

#define listLabel        "list,l"
#define listLabelKey     "list"

#define usernameLabel    "username,u"
#define usernameLabelKey "username"

#define passwordLabel    "password,p"
#define passwordLabelKey "password"

#define tempDirLabel     "temp,t"
#define tempDirLabelKey  "temp"

#define workDirLabel     "work,w"
#define workDirLabelKey  "work"

#define seedDirLabel     "seed,s"
#define seedDirLabelKey  "seed"

#define destDirLabel     "dest,d"
#define destDirLabelKey  "dest"

#define controlLabel     "control"
#define controlLabelKey  "control"

#define callbackLabel    "callback,b"
#define callbackLabelKey "callback"

         const std::string parsingFailedMessage("Parsing command line arguments failed: ");

         commandLineArgumentHandler::commandLineArgumentHandler(std::string const& _config_file)
            : btg::core::argumentInterface("BTG passwd", false),
              createNewSwitch(false),
              addUserSwitch(false),
              modifyUserSwitch(false),
              deleteUserSwitch(false),
              listUsersSwitch(false),
              usernameArg(),
              passwordSwitch(false),
              tempDirArg(),
              workDirArg(),
              seedDirArg(),
              destDirArg(),
              callbackArg(),
              controlFlagArg(false),
              controlFlagArgSet(false),
              config_file(_config_file),
              config_file_present(false)
         {
         }

         void commandLineArgumentHandler::setup()
         {
            btg::core::argumentInterface::setup();

            desc.add_options()
               (configLabel, boost::program_options::value<std::string>(), "Configuration file.")
               (createLabel, "Create new passwd file if it does not already exist.")
               (addLabel, "Add user.")
               (modifyLabel, "Modify user.")
               (deleteLabel, "Delete user.")
               (listLabel, "List users.")
               (usernameLabel, boost::program_options::value<std::string>(), "User name.")
               (passwordLabel, "Query for password.")
               (tempDirLabel, boost::program_options::value<std::string>(), "Temp directory.")
               (workDirLabel, boost::program_options::value<std::string>(), "Work directory.")
               (seedDirLabel, boost::program_options::value<std::string>(), "Seed directory.")
               (destDirLabel, boost::program_options::value<std::string>(), "Destination directory.")
               (controlLabel, boost::program_options::value<bool>(), "Set control flag (1 or 0).")
               (callbackLabel, boost::program_options::value<std::string>(), "Callback.")
               ;

         }

         bool commandLineArgumentHandler::parse(int argc, char **argv)
         {
            bool result = btg::core::argumentInterface::parse(argc, argv);

            if (printHelpMessage())
               {
                  return false;
               }

            if (!result)
               {
                  return result;
               }

            bool parsed = true;
            std::string parse_error;
            try
               {
                  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
               }
            catch (boost::program_options::error _e)
               {
                  parse_error = std::string(_e.what());
                  parsed      = false;
               }

            if (!parsed)
               {
                  std::cerr << parsingFailedMessage << parse_error << std::endl;
                  std::cout << desc << std::endl;
                  return false;
               }

            boost::program_options::notify(vm);

            t_int switchCount = 0;

            if (vm.count(configLabelKey))
               {
                  config_file         = vm[configLabelKey].as<std::string>();
                  config_file_present = true;
               }

            if (vm.count(createLabelKey))
               {
                  createNewSwitch = true;
               }

            if (vm.count(addLabelKey))
               {
                  addUserSwitch = true;
                  switchCount++;
               }

            if (vm.count(modifyLabelKey))
               {
                  modifyUserSwitch = true;
                  switchCount++;
               }

            if (vm.count(deleteLabelKey))
               {
                  deleteUserSwitch = true;
                  switchCount++;
               }

            if (vm.count(listLabelKey))
               {
                  listUsersSwitch = true;
                  switchCount++;
               }

            if (switchCount > 1)
               {
                  std::cerr << parsingFailedMessage << "Use only one of "
                            << addLabelKey << ", " << modifyLabelKey << ", "
                            << deleteLabelKey << ", " << listLabelKey
                            << std::endl;
                  std::cout << desc << std::endl;
                  return false;
               }

            if (vm.count(usernameLabelKey))
               {
                  usernameArg = vm[usernameLabelKey].as<std::string>();
               }

            if (vm.count(passwordLabelKey))
               {
                  passwordSwitch = true;
               }

            if (vm.count(tempDirLabelKey))
               {
                  tempDirArg = vm[tempDirLabelKey].as<std::string>();
               }

            if (vm.count(workDirLabelKey))
               {
                  workDirArg = vm[workDirLabelKey].as<std::string>();
               }

            if (vm.count(seedDirLabelKey))
               {
                  seedDirArg = vm[seedDirLabelKey].as<std::string>();
               }

            if (vm.count(destDirLabelKey))
               {
                  destDirArg = vm[destDirLabelKey].as<std::string>();
               }

            if (vm.count(controlLabelKey))
               {
                  controlFlagArg    = vm[controlLabelKey].as<bool>();
                  controlFlagArgSet = true;
               }

            if (vm.count(callbackLabelKey))
               {
                  callbackArg = vm[callbackLabelKey].as<std::string>();
               }
            return true;
         }

         bool commandLineArgumentHandler::configFileSet() const
         {
            return config_file_present;
         }

         std::string commandLineArgumentHandler::configFile() const
         {
            return config_file;
         }

         commandLineArgumentHandler::OPERATION commandLineArgumentHandler::getOperation() const
         {
            commandLineArgumentHandler::OPERATION o = OP_UNDEF;

            if (addUserSwitch)
               {
                  o = OP_ADD;
               }

            if (modifyUserSwitch)
               {
                  o = OP_MOD;
               }

            if (deleteUserSwitch)
               {
                  o = OP_DEL;
               }

            if (listUsersSwitch)
               {
                  o = OP_LST;
               }
            return o;
         }

         bool commandLineArgumentHandler::getUsername(std::string & _username) const
         {
            bool status = false;

            if (usernameArg != "")
               {
                  status    = true;
                  _username = usernameArg;
               }

            return status;
         }

         bool commandLineArgumentHandler::readPassword() const
         {
            return passwordSwitch;
         }

         bool commandLineArgumentHandler::getTempDir(std::string & _temp_dir) const
         {
            bool status = false;

            if (tempDirArg != "")
               {
                  status    = true;
                  _temp_dir = tempDirArg;
               }

            return status;
         }

         bool commandLineArgumentHandler::getWorkDir(std::string & _work_dir) const
         {
            bool status = false;

            if (workDirArg != "")
               {
                  status    = true;
                  _work_dir = workDirArg;
               }

            return status;
         }

         bool commandLineArgumentHandler::getSeedDir(std::string & _seed_dir) const
         {
            bool status = false;

            if (seedDirArg != "")
               {
                  status    = true;
                  _seed_dir = seedDirArg;
               }

            return status;
         }

         bool commandLineArgumentHandler::getDestDir(std::string & _dest_dir) const
         {
            bool status = false;

            if (destDirArg != "")
               {
                  status    = true;
                  _dest_dir = destDirArg;
               }

            return status;
         }

         bool commandLineArgumentHandler::getControlFlag(bool & _controlFlag) const
         {
            if (controlFlagArgSet)
               {
                  _controlFlag = controlFlagArg;
               }

            return controlFlagArgSet;
         }

         bool commandLineArgumentHandler::getCallback(std::string & _callback) const
         {
            bool status = false;

            if (callbackArg != "")
               {
                  status    = true;
                  _callback = callbackArg;
               }

            return status;
         }

         bool commandLineArgumentHandler::createNewFile() const
         {
            return createNewSwitch;
         }

         std::string commandLineArgumentHandler::getCompileTimeOptions() const
         {
            std::string output = argumentInterface::getCompileTimeOptions();
            output            += GPD->sNEWLINE();

            btg::core::getCompileTimeOptions(output);

            return output;
         }

         commandLineArgumentHandler::~commandLineArgumentHandler()
         {

         }

      } // namespace auth
   } // namespace daemon
} // namespace btg


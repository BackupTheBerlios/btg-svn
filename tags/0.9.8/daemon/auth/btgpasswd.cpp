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

#include <bcore/project.h>
#include <bcore/logmacro.h>

#include "arg.h"
#include <daemon/dconfig.h>

#include <bcore/os/fileop.h>

#include "passwd_auth.h"

#include <bcore/auth/hash.h>

#include <bcore/os/stdin.h>

#include <bcore/verbose.h>

#include <algorithm>

using namespace btg::core;
using namespace btg::core::logger;
using namespace btg::daemon::auth;

bool createDirectory(LogWrapperType _logwrapper, bool _verboseFlag, std::string const& _label, std::string const& _path);

bool uniqueDirectories(LogWrapperType _logwrapper,
                       std::string const& _temp_dir,
                       std::string const& _work_dir,
                       std::string const& _seed_dir,
                       std::string const& _dest_dir);

int main(int argc, char* argv[])
{
   LogWrapperType logwrapper(new btg::core::logger::logWrapper);

   commandLineArgumentHandler* cla = new commandLineArgumentHandler(projectDefaults::sDAEMON_CONFIG());
   cla->setup();
   if (!cla->parse(argc, argv))
      {
         delete cla;
         cla = 0;

         return BTG_ERROR_EXIT;
      }

   bool verboseFlag = cla->beVerbose();

   std::string configFile = projectDefaults::sDAEMON_CONFIG();
   if (cla->configFileSet())
      {
         configFile = cla->configFile();
      }

   VERBOSE_LOG(logwrapper, verboseFlag, "config: '" << configFile << "'.");

   std::string errorString;
   if (!btg::core::os::fileOperation::check(configFile, errorString, false))
      {
         BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Unable to open config file '" << configFile << "': " << errorString);

         delete cla;
         cla = 0;

         return BTG_ERROR_EXIT;
      }

   btg::daemon::daemonConfiguration* config  = new btg::daemon::daemonConfiguration(logwrapper,
                                                                                    configFile);

   if (!config->read())
      {
         BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Could not open configuration file '" << configFile << "'");

         delete config;
         config = 0;
         delete cla;
         cla = 0;

         return BTG_ERROR_EXIT;
      }

#if BTG_AUTH_DEBUG
   BTG_NOTICE(logwrapper, "Config read from '" << configFile << "'.");
#endif // BTG_AUTH_DEBUG

   // Open the passwd file, filename from the config file.

   std::string auth_file = config->getAuthFile();

   bool newfile = cla->createNewFile();

   if (newfile)
      {
         if (cla->getOperation() != commandLineArgumentHandler::OP_ADD)
            {
               // Only allow creating new file when an user is being added.
               newfile = false;
            }
      }

   // resolve relative paths etc.
   if (!btg::core::os::fileOperation::resolvePath(auth_file))
      {
         BTG_NOTICE(logwrapper, "Unable to resolve path '" << auth_file << "'");
      }

   if (!newfile)
      {
         if (!btg::core::os::fileOperation::check(auth_file, errorString, false))
            {
               BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Could not open passwd file '" << auth_file << "': " << errorString);

               delete config;
               config = 0;
               delete cla;
               cla = 0;

               return BTG_ERROR_EXIT;
            }
      }

   passwordAuth* auth = new passwordAuth(logwrapper, auth_file, newfile);

   if (!auth->initialized())
      {
         BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Unable to load passwd file '" << auth_file << "'.");

         delete auth;
         auth = 0;
         delete config;
         config = 0;
         delete cla;
         cla = 0;

         return BTG_ERROR_EXIT;
      }

   commandLineArgumentHandler::OPERATION operation = cla->getOperation();

   switch (operation)
      {
      case commandLineArgumentHandler::OP_ADD:
         {
            VERBOSE_LOG(logwrapper, verboseFlag, "Adding user.");

            std::string user;
            std::string password;
            std::string password_confirm;
            std::string temp_dir;
            std::string work_dir;
            std::string seed_dir;
            std::string dest_dir;
            bool        control_flag = false;
            std::string callback;

            if (!cla->getUsername(user))
               {
                  BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Missing user name.");
                  break;
               }

            if (!cla->getTempDir(temp_dir))
               {
                  BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Missing temporary directory.");
                  break;
               }

            if (!cla->getWorkDir(work_dir))
               {
                  BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Missing working directory.");
                  break;
               }

            if (!cla->getSeedDir(seed_dir))
               {
                  BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Missing seed directory.");
                  break;
               }

            if (!cla->getDestDir(dest_dir))
               {
                  BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Missing destination directory.");
                  break;
               }

            // Make sure that the directories are unique.
            if (!uniqueDirectories(logwrapper,
                                   temp_dir, work_dir,
                                   seed_dir, dest_dir))
               {
                  BTG_FATAL_ERROR(logwrapper,
                                  "btgpasswd", "Provided directories are not unique. " << 
                                  "Make sure temporary/working/seed/destination directory is unique.");
                  break;
               }

            if (!cla->getControlFlag(control_flag))
               {
                  control_flag = false;
               }

            if (!cla->getCallback(callback))
               {
                  callback = Auth::callbackDisabled;
               }

            std::cout << "Enter password:" << std::endl;

            // Ask for a password.
            if (!btg::core::os::stdIn::getPassword(password))
               {
                  BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Reading password failed.");
                  break;
               }

            std::cout << "Confirm password:" << std::endl;

            // Ask for a password.
            if (!btg::core::os::stdIn::getPassword(password_confirm))
               {
                  BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Reading password failed.");
                  break;
               }

            // Make sure passwords are equal
            if (password != password_confirm)
               {
                  BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Password missmatch.");
                  break;
               }


            Hash h;
            h.generate(password);
            std::string hashed;
            h.get(hashed);

            VERBOSE_LOG(logwrapper,
                        verboseFlag, "Adding user '" << user << "' with:" << "\n" <<
                        "hash '" << hashed << "'" << "\n" <<
                        "temp directory '" << temp_dir << "'" << "\n" <<
                        "work directory '" << work_dir << "'" << "\n" <<
                        "seed directory '" << seed_dir << "'" << "\n" <<
                        "destination directory '" << dest_dir << "'" << "\n" << 
                        "control flag '" << control_flag << "'" << "\n" <<
                        "callback '" << callback << "'" << "\n"
                        );

            // Create the directories used.

            if ((!createDirectory(logwrapper, verboseFlag, "temp", temp_dir)) ||
                (!createDirectory(logwrapper, verboseFlag, "work", work_dir)) ||
                (!createDirectory(logwrapper, verboseFlag, "seed", seed_dir)) ||
                (!createDirectory(logwrapper, verboseFlag, "dest", dest_dir))
                )
               {
                  // Abort, as some directories failed.
                  BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Failed to create user directorys.");
                  break;
               }

            if (!auth->addUser(user, hashed, temp_dir, work_dir, seed_dir, dest_dir, control_flag, callback))
               {
                  BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Unable to add user.");
                  break;
               }

            if (!auth->write())
               {
                  BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Unable to save password file.");
                  break;
               }

            std::cout << "User '" << user << "' added." << std::endl;
            break;
         }
      case commandLineArgumentHandler::OP_MOD:
         {
            VERBOSE_LOG(logwrapper, verboseFlag, "Modifying user.");

            std::string user;
            std::string password;
            std::string password_confirm;
            std::string temp_dir;
            std::string work_dir;
            std::string seed_dir;
            std::string dest_dir;

            bool control_flag = false;

            std::string callback;

            if (!cla->getUsername(user))
               {
                  BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Missing user name.");
                  break;
               }

            if (!auth->getControlFlag(user, control_flag))
               {
                  BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Missing control flag.");
                  break;
               }

            bool any_argument = false;

            if (cla->readPassword())
               {
                  any_argument = true;
               }

            if (cla->getTempDir(temp_dir))
               {
                  any_argument = true;
               }

            if (cla->getWorkDir(work_dir))
               {
                  any_argument = true;
               }

            if (cla->getSeedDir(seed_dir))
               {
                  any_argument = true;
               }
            if (cla->getDestDir(dest_dir))
               {
                  any_argument = true;
               }

            if (cla->getControlFlag(control_flag))
               {
                  any_argument = true;
               }

            if (cla->getCallback(callback))
               {
                  any_argument = true;
               }

            if (!any_argument)
               {
                  BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Nothing changed.");
                  break;
               }

            BTG_NOTICE(logwrapper,
                       "Mod user '" << user << "', using directories:" << "\n" <<
                       "temp directory '" << temp_dir << "'" << "\n" <<
                       "work directory '" << work_dir << "'" << "\n" <<
                       "seed directory '" << seed_dir << "'" << "\n" <<
                       "destination directory '" << dest_dir << "'" << "\n"
                       );
            std::string hashedPassword;
            if (cla->readPassword())
               {
                  std::cout << "Enter password:" << std::endl;

                  // Ask for a password.
                  if (!btg::core::os::stdIn::getPassword(password))
                     {
                        BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Reading password failed.");
                        break;
                     }

                  std::cout << "Confirm password:" << std::endl;

                  // Ask for a password.
                  if (!btg::core::os::stdIn::getPassword(password_confirm))
                     {
                        BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Reading password failed.");
                        break;
                     }

                  // Make sure passwords are equal
                  if (password != password_confirm)
                     {
                        BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Password missmatch.");
                        break;
                     }

                  Hash h;
                  h.generate(password);
                  h.get(hashedPassword);
               }

            if (verboseFlag)
               {
                  std::string output;

                  output += "Modyfying user '" + user + "', setting:" + "\n";

                  if (password.size() > 0)
                     {
                        output += "hash '" + hashedPassword + "'" + "\n";
                     }

                  if (temp_dir.size() > 0)
                     {
                        output += "temp directory '" + temp_dir + "'" + "\n";
                     }

                  if (work_dir.size() > 0)
                     {
                        output += "work directory '" + work_dir + "'" + "\n";
                     }

                  if (seed_dir.size() > 0)
                     {
                        output += "seed directory '" + seed_dir + "'" + "\n";
                     }

                  if (dest_dir.size() > 0)
                     {
                        output += "destination directory '" + dest_dir + "'" + "\n";
                     }

                  if (control_flag)
                     {
                        output += "control flag '1'\n";
                     }
                  else
                     {
                        output += "control flag '0'\n";
                     }

                  if (callback.size() > 0)
                     {
                        output += "callback '" + callback + "'\n";
                     }

                  VERBOSE_LOG(logwrapper, verboseFlag, output);
               } // verbose

            if (!auth->modUser(user, hashedPassword, temp_dir, work_dir, seed_dir, dest_dir, control_flag, callback))
               {
                  BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Unable to modify user.");
                  break;
               }

            if (!auth->write())
               {
                  BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Unable to save password file.");
                  break;
               }

            std::cout << "User '" << user << "' modified." << std::endl;
            break;
         }
      case commandLineArgumentHandler::OP_DEL:
         {
            VERBOSE_LOG(logwrapper, verboseFlag, "Deleting user.");

            std::string user;

            if (!cla->getUsername(user))
               {
                  BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Missing user name.");
                  break;
               }

            if (!auth->delUser(user))
               {
                  BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Unable to delete user.");
                  break;
               }

            if (!auth->write())
               {
                  BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Unable to save password file.");
                  break;
               }

            std::cout << "Deleted user '" << user << "'." << std::endl;
            break;
         }
      case commandLineArgumentHandler::OP_LST:
         {
            std::cout << "Listing users." << std::endl;
            std::cout << auth->toString() << std::endl;
            break;
         }
      default:
         {
            BTG_FATAL_ERROR(logwrapper, "btgpasswd", "Unknown operation.");
         }
      }

   delete auth;
   auth = 0;

   delete config;
   config = 0;

   delete cla;
   cla = 0;

   return 0;
}

bool createDirectory(LogWrapperType _logwrapper,
                     bool _verboseFlag, std::string const& _label, std::string const& _path)
{
   bool result = btg::core::os::fileOperation::createDirectory(_path);

   switch(result)
      {
      case true:
         VERBOSE_LOG(_logwrapper, _verboseFlag, "Created " << _label << " directory.");
         break;
      case false:
         VERBOSE_LOG(_logwrapper, _verboseFlag, "Unable to create " << _label << " directory, path .");
         break;
      }

   return result;
}

bool uniqueDirectories(LogWrapperType,
                       std::string const& _temp_dir,
                       std::string const& _work_dir,
                       std::string const& _seed_dir,
                       std::string const& _dest_dir)
{
   std::vector<std::string> check_directories;
   check_directories.push_back(_temp_dir);
   check_directories.push_back(_work_dir);
   check_directories.push_back(_seed_dir);
   check_directories.push_back(_dest_dir);

   const t_uint orgSize = check_directories.size();

   std::sort(check_directories.begin(), check_directories.end());
   std::unique(check_directories.begin(), check_directories.end());

   if (check_directories.size() != orgSize)
      {
         return false;
      }

   return true;
}

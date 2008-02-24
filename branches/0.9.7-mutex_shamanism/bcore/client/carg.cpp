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

#include "carg.h"

#include <bcore/project.h>
#include <bcore/copts.h>
#include <bcore/command/command.h>
#include <bcore/util.h>

#include <boost/program_options.hpp>

namespace btg
{
   namespace core
   {
      namespace client
      {
         using namespace btg::core;

#define sessionLabel        "session,s"
#define sessionLabelKey     "session"
#define daemonLabel         "daemon,d"
#define daemonLabelKey      "daemon"
#define openLabel           "open,o"
#define openLabelKey        "open"
#define nostartLabel        "nostart"
#define nostartLabelKey     "nostart"
#define listLabel           "list,l"
#define listLabelKey        "list"
#define attachLabel         "attach,a"
#define attachLabelKey      "attach"
#define attachFirstLabel    "Attach,A"
#define attachFirstLabelKey "Attach"
#define commandLabel        "command,c"
#define commandLabelKey     "command"
#define nooutputLabel       "noutput,n"
#define nooutputLabelKey    "noutput"
#define configLabel         "config"
#define configLabelKey      "config"

#define enableDHTLabel          "dht"
#define enableDHTLabelKey       "dht"
#define disableDHTLabel          "no-dht"
#define disableDHTLabelKey       "no-dht"

#define enableEncryptionLabel   "encryption"
#define enableEncryptionabelKey "encryption"
#define disableEncryptionLabel   "no-encryption"
#define disableEncryptionabelKey "no-encryption"

         commandLineArgumentHandler::commandLineArgumentHandler(std::string const& _config_file,
                                                                bool const _enableCliOptions)
            : btg::core::argumentInterface("BTG client", true),
              enableCliOptions(_enableCliOptions),
              session(Command::INVALID_SESSION),
              list_present(false),
              attach_present(false),
              attachFirst_present(false),
              input_filenames(0),
              input_present(false),
              automaticStart_(true),
              commandToExecute_present(false),
              noOutput_present(false),
              commands(0),
              daemon_set(false),
              daemon(0,0,0,0,0),
              config_file(_config_file),
              config_file_present(false),
              use_encryption_present(false),
              use_encryption(false),
              use_DHT_present(false),
              use_DHT(false)
         {

         }

         void commandLineArgumentHandler::setup()
         {
            btg::core::argumentInterface::setup();

            desc.add_options()
               (sessionLabel, boost::program_options::value<t_long>(), "Session ID.")
               (daemonLabel, boost::program_options::value<std::string>(), "IPv4 address and port to remote daemon (for socket transports).")
               (openLabel, boost::program_options::value<std::vector<std::string> >(), "Filename(s) to open.")
               (nostartLabel, "Do not automatically start openend torrents.")
               (listLabel, "List already running sessions.")
               (attachLabel, "Let user select which session to attach to.")
               (attachFirstLabel, "Attach to the first session available.")
               (enableDHTLabel, "Enable DHT.")
               (disableDHTLabel, "Disable DHT.")
               (enableEncryptionLabel, "Enable encryption.")
               (disableEncryptionLabel, "Disable encryption.")
               ;
            if (enableCliOptions)
               {
                  desc.add_options()
                     (commandLabel, boost::program_options::value<std::string>(), "One or more commands to execute, separated by ';'.")
                     ;
                  desc.add_options()
                     (nooutputLabel, "Do not output anything, useful for automated scripts.")
                     ;
               }
            desc.add_options()
               (configLabel, boost::program_options::value<std::string>(), "Configuration file.");
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
            catch(boost::program_options::error _e)
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

            t_int numberOfSwitches = 0;

            if (vm.count(sessionLabelKey))
               {
                  session = vm[sessionLabelKey].as<t_long>();
               }

            if (vm.count(daemonLabelKey))
               {
                  std::string hostport = vm[daemonLabelKey].as<std::string>();

                  if (!daemon.fromString(hostport))
                     {
                        std::cerr << "Error: invalid value for arg daemon." << std::endl;
                     }
                  else
                     {
                        daemon_set = true;
                     }
               }

            if (vm.count(openLabelKey))
               {
                  input_filenames = vm[openLabelKey].as<std::vector<std::string> >();

                  if (input_filenames.size() > 0)
                     {
                        input_present = true;
                     }
               }

            if (vm.count(nostartLabelKey))
               {
                  automaticStart_ = false;
               }

            if (vm.count(listLabelKey))
               {
                  list_present = true;
                  numberOfSwitches++;
               }

            if (vm.count(attachLabelKey))
               {
                  numberOfSwitches++;
                  attach_present = true;
               }

            if (vm.count(attachFirstLabelKey))
               {
                  numberOfSwitches++;
                  attachFirst_present = true;
               }

            if (vm.count(commandLabelKey))
               {
                  std::string command_str = vm[commandLabelKey].as<std::string>();

                  if (command_str.size() > 0)
                     {
                        commands = btg::core::Util::splitLine(command_str, ";");
                        if (commands.size() > 0)
                           {
                              commandToExecute_present = true;
                           }
                     }
               }

            if (vm.count(nooutputLabelKey))
               {
                  noOutput_present = true;
               }

            if (numberOfSwitches > 1)
               {
                  std::cerr << parsingFailedMessage << "use only one of "
                            << listLabelKey << ", " << attachLabelKey << ", " << attachFirstLabelKey
                            << std::endl;
                  std::cout << desc << std::endl;
                  return false;
               }

            if ((!attach_present) && (session != Command::INVALID_SESSION))
               {
                  std::cerr << parsingFailedMessage << sessionLabelKey << " without " << attachLabelKey
                            << std::endl;
                  std::cout << desc << std::endl;
                  return false;
               }

            if (vm.count(configLabelKey))
               {
                  config_file         = vm[configLabelKey].as<std::string>();
                  config_file_present = true;
               }

            if (vm.count(enableDHTLabelKey))
               {
                  use_DHT_present = true;
                  use_DHT         = true;
               }

            if (vm.count(disableDHTLabelKey))
               {
                  use_DHT_present = true;
                  use_DHT         = false;
               }

            if (vm.count(enableEncryptionabelKey))
               {
                  use_encryption_present = true;
                  use_encryption         = true;
               }

            if (vm.count(disableEncryptionabelKey))
               {
                  use_encryption_present = true;
                  use_encryption         = false;
               }

            return true;
         }

         bool commandLineArgumentHandler::doList() const
         {
            return list_present;
         }

         bool commandLineArgumentHandler::doAttach() const
         {
            return attach_present;
         }

         bool commandLineArgumentHandler::doAttachFirst() const
         {
            return attachFirst_present;
         }

         t_long commandLineArgumentHandler::sessionId() const
         {
            return session;
         }

         bool commandLineArgumentHandler::inputFilenamesPresent() const
         {
            return input_present;
         }

         t_strList commandLineArgumentHandler::getInputFilenames() const
         {
            return input_filenames;
         }

         bool commandLineArgumentHandler::automaticStart() const
         {
            return automaticStart_;
         }

         bool commandLineArgumentHandler::CommandPresent() const
         {
            return commandToExecute_present;
         }

         t_strList commandLineArgumentHandler::getCommand() const
         {
            return commands;
         }

         bool commandLineArgumentHandler::NoOutputPresent() const
         {
            return noOutput_present;
         }

         btg::core::addressPort commandLineArgumentHandler::getDaemon() const
         {
            return daemon;
         }

         bool commandLineArgumentHandler::daemonSet() const
         {
            return daemon_set;
         }

         std::string commandLineArgumentHandler::toString() const
         {
            std::string output("Command line arguments:");

            if (list_present)
               {
                  output += " List.";
               }

            if (attach_present)
               {
                  output += " Attach.";
               }

            if (attachFirst_present)
               {
                  output += " Attach first.";
               }

            if (input_present)
               {
                  output += " Input.";
               }

            if (commandToExecute_present)
               {
                  output += " Execute.";
               }

            if (noOutput_present)
               {
                  output += " No output.";
               }

            if (daemon_set)
               {
                  output += " Daemon host:port.";
               }

            return output;
         }

         std::string commandLineArgumentHandler::getCompileTimeOptions() const
         {
            std::string output = argumentInterface::getCompileTimeOptions();
            output            += GPD->sNEWLINE();

            btg::core::getCompileTimeOptions(output);

            return output;
         }

         bool commandLineArgumentHandler::configFileSet() const
         {
            return config_file_present;
         }
         
         std::string commandLineArgumentHandler::configFile() const
         {
            return config_file;
         }

         bool commandLineArgumentHandler::useEncryptionSet() const
         {
            return use_encryption_present;
         }

         bool commandLineArgumentHandler::useEncryption() const
         {
            return use_encryption;
         }

         bool commandLineArgumentHandler::useDHTSet() const
         {
            return use_DHT_present;
         }
         
         bool commandLineArgumentHandler::useDHT() const
         {
            return use_DHT;
         }

         commandLineArgumentHandler::~commandLineArgumentHandler()
         {

         }

      } // namespace client
   } // namespace core
} // namespace btg


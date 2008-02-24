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
#include <bcore/command/command.h>
#include <bcore/util.h>

#include <boost/program_options.hpp>

namespace btg
{
   namespace UI
   {
      namespace gui
      {
         namespace viewer
         {
            using namespace std;
            using namespace btg::core;

#define sessionLabel        "session,s"
#define sessionLabelKey     "session"
#define daemonLabel         "daemon,d"
#define daemonLabelKey      "daemon"
#define attachLabel         "attach,a"
#define attachLabelKey      "attach"
#define attachFirstLabel    "Attach,A"
#define attachFirstLabelKey "Attach"
#define configLabel         "config"
#define configLabelKey      "config"

#define fullscreenLabel           "fullscreen,f"
#define fullscreenLabelKey        "fullscreen"

#define res1440x900Label           "xtralarge,x"
#define res1440x900LabelKey        "x"

#define res1024x768Label           "large,l"
#define res1024x768LabelKey        "l"

#define updateFreqLabel            "freq,u"
#define updateFreqLabelKey         "freq"

            vsCommandLineArgumentHandler::vsCommandLineArgumentHandler(std::string const& _config_file)
               : btg::core::argumentInterface("BTG client", true),
                 session(Command::INVALID_SESSION),
                 attach_present(false),
                 attachFirst_present(false),
                 daemon_set(false),
                 daemon(0,0,0,0,0),
                 config_file(_config_file),
                 config_file_present(false),
                 fullscreen_set(false),
                 res1440x900_set(false),
                 res1024x768_set(false),
                 updateFreq(10)
            {

            }

            void vsCommandLineArgumentHandler::setup()
            {
               btg::core::argumentInterface::setup();

               desc.add_options()
                  (sessionLabel, boost::program_options::value<t_long>(), "Session ID.")
                  (daemonLabel, boost::program_options::value<std::string>(), "IPv4 address and port to remote daemon (for socket transports).")
                  (attachLabel, "Let user select which session to attach to.")
                  (attachFirstLabel, "Attach to the first session available.")
                  (fullscreenLabel, "Fullscreen.")
                  (res1440x900Label, "Use 1440x900 as the resolution.")
                  (res1024x768Label, "Use 1024x768 as the resolution.")
                  (updateFreqLabel, boost::program_options::value<t_uint>(), "Update frequency in seconds.")
                  ;
               desc.add_options()
                  (configLabel, boost::program_options::value<std::string>(), "Configuration file.");
            }

            bool vsCommandLineArgumentHandler::parse(int argc, char **argv)
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
                           cerr << "Error: invalid value for arg daemon." << endl;
                        }
                     else
                        {
                           daemon_set = true;
                        }
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

               if (numberOfSwitches > 1)
                  {
                     std::cerr << parsingFailedMessage << "use only one of "
                               << attachLabelKey << ", " << attachFirstLabelKey
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

               if (vm.count(fullscreenLabelKey))
                  {
                     fullscreen_set = true;
                  }

               if (vm.count(res1440x900LabelKey))
                  {
                     res1440x900_set = true;
                  }

               if (vm.count(res1024x768LabelKey))
                  {
                     res1024x768_set = true;
                  }

               if (vm.count(updateFreqLabelKey))
                  {
                     updateFreq = vm[updateFreqLabelKey].as<t_uint>();
                  }

               return true;
            }

            bool vsCommandLineArgumentHandler::doAttach() const
            {
               return attach_present;
            }

            bool vsCommandLineArgumentHandler::doAttachFirst() const
            {
               return attachFirst_present;
            }

            t_long vsCommandLineArgumentHandler::sessionId() const
            {
               return session;
            }

            btg::core::addressPort vsCommandLineArgumentHandler::getDaemon() const
            {
               return daemon;
            }

            bool vsCommandLineArgumentHandler::daemonSet() const
            {
               return daemon_set;
            }

            std::string vsCommandLineArgumentHandler::toString() const
            {
               std::string output("Command line arguments:");

               if (attach_present)
                  {
                     output += " Attach.";
                  }

               if (attachFirst_present)
                  {
                     output += " Attach first.";
                  }

               if (daemon_set)
                  {
                     output += " Daemon host:port.";
                  }

               return output;
            }

            std::string vsCommandLineArgumentHandler::getCompileTimeOptions() const
            {
               std::string output = argumentInterface::getCompileTimeOptions();
               output            += GPD->sNEWLINE();

               btg::core::getCompileTimeOptions(output);

               return output;
            }

            bool vsCommandLineArgumentHandler::configFileSet() const
            {
               return config_file_present;
            }
         
            std::string vsCommandLineArgumentHandler::configFile() const
            {
               return config_file;
            }

            bool vsCommandLineArgumentHandler::automaticStart() const
            {
               return false;
            }

            bool vsCommandLineArgumentHandler::useEncryptionSet() const
            {
               return false;
            }

            bool vsCommandLineArgumentHandler::useEncryption() const
            {
               return false;
            }

            bool vsCommandLineArgumentHandler::useDHTSet() const
            {
               return false;
            }

            bool vsCommandLineArgumentHandler::useDHT() const
            {
               return false;
            }

            bool vsCommandLineArgumentHandler::fullscreen() const
            {
               return fullscreen_set;
            }

            bool vsCommandLineArgumentHandler::res1440x900() const
            {
               return res1440x900_set;
            }

            bool vsCommandLineArgumentHandler::res1024x768() const
            {
               return res1024x768_set;               
            }

            t_uint vsCommandLineArgumentHandler::getUpdateFreq() const
            {
               return updateFreq;
            }
            
            vsCommandLineArgumentHandler::~vsCommandLineArgumentHandler()
            {

            }

         } // namespace viewer
      } // namespace gui
   } // namespace UI
} // namespace btg

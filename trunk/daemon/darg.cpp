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

#include "darg.h"

#include <bcore/project.h>
#include <bcore/copts.h>

#include <boost/program_options.hpp>

namespace btg
{
   namespace daemon
   {
      using namespace btg::core;
      using namespace std;

#define configLabel           "config,c"
#define configLabelKey        "config"

#define nodetachLabel         "nodetach,n"
#define nodetachLabelKey      "nodetach"
#if BTG_OPTION_SAVESESSIONS
#  define sessionFileLabel    "session-file,s"
#  define sessionFileLabelKey "session-file"

#  define noreloadLabel       "no-reload,N"
#  define noreloadLabelKey    "no-reload"
#endif // BTG_OPTION_SAVESESSIONS

      commandLineArgumentHandler::commandLineArgumentHandler(string const& _config_file)
         : btg::core::argumentInterface("BTG daemon", true),
           config_file(_config_file),
           config_file_present(false),
#if BTG_OPTION_SAVESESSIONS
           saveSessionsFile_(""),
           saveSessionsFile_present_(false),
           noReloadSessions_present_(false),
#endif // BTG_OPTION_SAVESESSIONS
           doNotDetach_present(false)
      {
      }

      void commandLineArgumentHandler::setup()
      {
         btg::core::argumentInterface::setup();

         desc.add_options()
            (configLabel, boost::program_options::value<std::string>(), "Configuration file.")
            (nodetachLabel, "Do not detach from TTY.")
#if BTG_OPTION_SAVESESSIONS
            (sessionFileLabel, boost::program_options::value<std::string>(), "File with saved sessions.")
            (noreloadLabel, "Do not reload saved sessions.")
#endif // BTG_OPTION_SAVESESSIONS
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

         if (vm.count(configLabelKey))
            {
               config_file         = vm[configLabelKey].as<std::string>();
               config_file_present = true;
            }

         if (vm.count(nodetachLabelKey))
            {
               this->doNotDetach_present = true;
            }

#if BTG_OPTION_SAVESESSIONS
         if (vm.count(sessionFileLabelKey))
            {
               saveSessionsFile_present_ = true;
               saveSessionsFile_         = vm[sessionFileLabelKey].as<std::string>();
            }

         if (vm.count(noreloadLabelKey))
            {
               this->noReloadSessions_present_ = true;
            }

#endif // BTG_OPTION_SAVESESSIONS

         return true;
      }

      bool commandLineArgumentHandler::doNotDetach() const
      {
         return doNotDetach_present;
      }

      bool commandLineArgumentHandler::configFileSet() const
      {
         return config_file_present;
      }

      std::string commandLineArgumentHandler::configFile() const
      {
         return config_file;
      }

#if BTG_OPTION_SAVESESSIONS
      bool commandLineArgumentHandler::doNotReloadSessions() const
      {
         return noReloadSessions_present_;
      }

      bool commandLineArgumentHandler::saveSessionsFileSet() const
      {
         return saveSessionsFile_present_;
      }

      std::string commandLineArgumentHandler::saveSessionsFile() const
      {
         return saveSessionsFile_;
      }
#endif // BTG_OPTION_SAVESESSIONS

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

   } // namespace daemon
} // namespace btg


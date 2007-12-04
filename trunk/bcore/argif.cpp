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

#include "argif.h"
#include <iostream>
#include <bcore/project.h>

namespace btg
{
   namespace core
   {

#define helpLabel             "help,h"
#define helpLabelKey          "help"

#if BTG_DEBUG
#  define debugLabel          "debug,D"
#  define debugLabelKey       "debug"
#endif // BTG_DEBUG
#define verboseLabel          "verbose,v"
#define verbpseLabelKey       "verbose"

#define syntaxLabel           "syntax"
#define syntaxLabelKey        "syntax"

      argumentInterface::argumentInterface(std::string const& _applicationName,
                                           bool const _syntaxEnabled)
         : parsingFailedMessage("Parsing command line arguments failed: "),
           applicationName(_applicationName),
           printHelp(false),
           syntaxEnabled(_syntaxEnabled),
#if BTG_DEBUG
           debugMode(false),
#endif // BTG_DEBUG
           verboseMode(false),
           listSyntax(false),
           desc(_applicationName + " (version " + GPD->sVERSION() + ") options"),
           vm()
      {

      }

      void argumentInterface::setup()
      {
         desc.add_options()
            (helpLabel, "Produce help message.")
#if BTG_DEBUG
            (debugLabel, "Write extensive debug info.")
#endif // BTG_DEBUG
            (verboseLabel, "Write verbose messages about what the application is doing.")
            ;

         if (syntaxEnabled)
            {
               desc.add_options()
                  (syntaxLabel, "List the syntax of the configuration file used.")
                  ;
            }
      }

      bool argumentInterface::parse(int argc, char **argv)
      {
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

         if (vm.count(helpLabelKey))
            {
               printHelp = true;
               return false;
            }
#if BTG_DEBUG
         if (vm.count(debugLabelKey))
            {
               debugMode = true;
            }
#endif // BTG_DEBUG

         if (vm.count(verbpseLabelKey))
            {
               verboseMode = true;
            }

         if (syntaxEnabled)
            {
               if (vm.count(syntaxLabelKey))
                  {
                     listSyntax = true;
                  }
            }

         return true;
      }

#if BTG_DEBUG
      bool argumentInterface::doDebug() const
      {
         return debugMode;
      }
#endif // BTG_DEBUG

      bool argumentInterface::beVerbose() const
      {
         return verboseMode;
      }

      bool argumentInterface::listConfigFileSyntax() const
      {
         if (syntaxEnabled)
            {
               return listSyntax;
            }
         else
            {
               return false;
            }
      }

      bool argumentInterface::printHelpMessage() const
      {
         if (printHelp)
            {
               std::cout << desc << std::endl;
               std::cout << getCompileTimeOptions() << std::endl;
            }
         return printHelp;
      }
      
      std::string argumentInterface::getCompileTimeOptions() const
      {
         std::string output("Compile time options enabled:");

         return output;
      }

      argumentInterface::~argumentInterface()
      {

      }

   } // namespace core
} // namespace btg



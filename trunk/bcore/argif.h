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

#ifndef ARGIF_H
#define ARGIF_H

#include <string>

#include <boost/program_options.hpp>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup core
          */
         /** @{ */

         /// Interface describing which arguments every BTG
         /// application should support.
         class argumentInterface
            {
            public:
               /// Constructor.
               /// @param [in] _applicationName Name of the application using this interface.
               /// @param [in] _syntaxEnabled   Indicates if showing syntax is enabled.
               argumentInterface(std::string const& _applicationName, bool const _syntaxEnabled = true);

               /// Setup the interface. Must be called before parsing.
               virtual void setup();

               /// Parse command line options.
               virtual bool parse(int argc, char **argv);

#if BTG_DEBUG
               /// Returns true if the daemon is to write debug
               /// information to the log.
               virtual bool doDebug() const;
#endif // BTG_DEBUG

               /// Returns true, if the daemon is to be verbose.
               virtual bool beVerbose() const;

               /// Returns true, if the daemon should list its config file syntax.
               virtual bool listConfigFileSyntax() const;

               /// Get a textual description of the compiled in options used.
               virtual std::string getCompileTimeOptions() const;

               /// Destructor.
               virtual ~argumentInterface();

            protected:
               /// Message to display when parsing arguments fails.
               std::string const parsingFailedMessage;
               /// The name of the application using this interface.
               std::string       applicationName;
               /// Indicates if the user if this interface should
               /// print a help message and quit.
               bool              printHelp;
               /// Indicates that listConfigFileSyntax should be supported.
               bool              syntaxEnabled;
#if BTG_DEBUG
               /// Switch that indicates that the daemon should write
               /// extensive debug info to the log.
               bool              debugMode;
#endif // BTG_DEBUG

               /// Verbose mode - write verbose messages about what
               /// the daemon is doing.
               bool              verboseMode;

               /// List syntax of the configuration file.
               bool              listSyntax;

               /// Description.
               boost::program_options::options_description desc;

               /// Variable map.
               boost::program_options::variables_map       vm;

               /// Print a help message if the printHelp is set to true.
               /// @return The value of the printHelp member.
               virtual bool printHelpMessage() const;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif // ARGIF_H

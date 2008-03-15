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

#ifndef DARG_H
#define DARG_H

#include <sstream>
#include <string>
#include <bcore/transport/transport.h>
#include <bcore/argif.h>

namespace btg
{
   namespace daemon
      {
         /**
          * \addtogroup daemon
          */
         /** @{ */

         /// This class implements parsing of arguments that can be passed to
         /// the btg daemon application.
         class commandLineArgumentHandler: public btg::core::argumentInterface
            {
            public:
               /// Constructor. Parameters are used for default values of
               /// command line arguments.
               commandLineArgumentHandler(std::string const& _config_file);

               void setup();

               /// Parse command line options.
               bool parse(int argc, char **argv);

               /// Returns true if the daemon is not to detach.
               bool doNotDetach() const;

               /// Returns true if a config file was set.
               bool configFileSet() const;

               /// Get the name of the configuration file used by the daemon.
               std::string configFile() const;

#if BTG_OPTION_SAVESESSIONS
               /// Returns true if the saved sessions are not to reload.
               bool doNotReloadSessions() const;

               /// Returns true if a saved sessions file was set.
               bool saveSessionsFileSet() const;

               /// Returns the name of the file used to save sessions in.
               std::string saveSessionsFile() const;
#endif // BTG_OPTION_SAVESESSIONS

               std::string getCompileTimeOptions() const;

               /// Destructor.
               virtual ~commandLineArgumentHandler();

            private:
               /// The config file used.
               std::string             config_file;
               /// Flag: true if the config file argument was set.
               bool                    config_file_present;

#if BTG_OPTION_SAVESESSIONS
               /// The file where to sessions are saved.
               std::string             saveSessionsFile_;
               /// Flag: true if the session file was set.
               bool                    saveSessionsFile_present_;
               /// Flag: true, if no sessions are to be loaded.
               bool                    noReloadSessions_present_;
#endif // BTG_OPTION_SAVESESSIONS

               /// Switch which indicates that the daemon should detach and deamonize.
               bool                    doNotDetach_present;
            };

         /** @} */

      } // namespace daemon
} // namespace btg

#endif

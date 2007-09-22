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

#ifndef CARG_H
#define CARG_H

#include <string>
#include <bcore/type.h>
#include <bcore/addrport.h>
#include <bcore/transport/transport.h>

#include <bcore/printable.h>

#include <bcore/argif.h>

namespace btg
{
   namespace core
      {
         namespace client
            {

               /**
                * \addtogroup gencli
                */
               /** @{ */

               /// This class implements parsing of arguments that can
               /// be passed to the btg client applications.
               class commandLineArgumentHandler: 
                  public btg::core::argumentInterface, public Printable
                  {
                  public:

                     /// Constructor. Parameters are used for default values of
                     /// command line arguments.
                     /// @param [in] _config_file      The config file to use.
                     /// @param [in] _enableCliOptions Enable cli specific options.
                     commandLineArgumentHandler(std::string const& _config_file,
                                                bool const _enableCliOptions = false);

                     void setup();

                     bool parse(int argc, char **argv);

                     /// Returns true if the client should list active sessions.
                     bool doList() const;

                     /// Returns true if the client should attach to a session.
                     bool doAttach() const;

                     /// Returns true if the client should attach to
                     /// the first session.
                     bool doAttachFirst() const;

                     /// Returns true if the address of the daemon is set.
                     bool daemonSet() const;

                     /// Returns a session ID or -1 if its undefined.
                     t_long sessionId() const;

                     /// Returns true if input filenames are present.
                     bool inputFilenamesPresent() const;

                     /// Returns a list of filenames or an empty list.
                     t_strList getInputFilenames() const;

                     /// Indicates if torrents (uploaded to the
                     /// daemon) should be started after creation.
                     bool automaticStart() const;

                     /// Returns true, if the user wishes to execute
                     /// one or more commands.
                     bool CommandPresent() const;

                     /// Get the string containing one or more
                     /// commands.
                     t_strList getCommand() const;

                     /// Returns true if the user wishes no output.
                     bool NoOutputPresent() const;

                     /// Get the ipv4 address and port of daemon for
                     /// socketbased transports.
                     btg::core::addressPort getDaemon() const;

                     std::string toString() const;

                     std::string getCompileTimeOptions() const;

                     /// Returns true if a config file was set.
                     bool configFileSet() const;

                     /// Get the name of the configuration file used
                     /// by the daemon.
                     std::string configFile() const;

                     /// Returns true, if using encryption was set.
                     bool useEncryptionSet() const;

                     /// Indicates if encryption shall be used.
                     bool useEncryption() const;

                     /// Returns true, if using DHT was set.
                     bool useDHTSet() const;

                     /// Indicates if DHT shall be used.
                     bool useDHT() const;

                     /// Destructor.
                     virtual ~commandLineArgumentHandler();

                  private:
                     /// Indicates if passing commands to the
                     /// application is possible. Used with the
                     /// textual client.
                     bool const                   enableCliOptions;
                     /// The current session.
                     t_long                       session;

                     /// Indicates that the user wishes to list
                     /// sessions.
                     bool                         list_present;

                     /// Indicates that the user wishes to attach to a
                     /// session.
                     bool                         attach_present;

                     /// Indicates that the user wishes to attach to
                     /// the first available session.
                     bool                         attachFirst_present;

                     /// List of filenames to open.
                     t_strList                    input_filenames;

                     /// True, if any filenames given as arguments.
                     bool                         input_present;

                     /// Indicates if opened torrents should be
                     /// started automatically.
                     bool                         automaticStart_;

                     /// True, if any commands are present.
                     bool                         commandToExecute_present;

                     /// True, if the user wishes nothing displayed.
                     bool                         noOutput_present;

                     /// List of commands.
                     t_strList                    commands;

                     /// Indicates if the address of the daemon is set.
                     bool                         daemon_set;

                     /// The ipv4 address and port of daemon for
                     /// socketbased transports
                     btg::core::addressPort       daemon;

                     /// The config file used.
                     std::string                  config_file;

                     /// Flag: true if the config file argument was
                     /// set.
                     bool                         config_file_present;

                     /// Flag: true if the encryption argument was
                     /// set.
                     bool                         use_encryption_present;
                     
                     /// Indicates if encryption should be used.
                     bool                         use_encryption;

                     /// Flag: true if the DHT argument was
                     /// set.
                     bool                         use_DHT_present;

                     /// Indicates if DHT should be used.
                     bool                         use_DHT;

                     /// Convert a string containing one or more
                     /// commands into a list.
                     t_strList splitCommands(std::string const& _commands);
                  };

               /** @} */

            } // namespace client
      } // namespace core
} // namespace btg
#endif

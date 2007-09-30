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

#ifndef VARG_H
#define VARG_H

#include <string>
#include <bcore/type.h>
#include <bcore/addrport.h>
#include <bcore/transport/transport.h>

#include <bcore/printable.h>

#include <bcore/argif.h>
#include <bcore/client/helper_argif.h>

namespace btg
{
   namespace UI
   {
      namespace gui
      {
         namespace viewer
         {

            /**
             * \addtogroup viewer
             */
            /** @{ */

            /// This class implements parsing of arguments that can
            /// be passed to the btg client applications.
            class vsCommandLineArgumentHandler: 
               public btg::core::argumentInterface, 
               public btg::core::client::HelperArgIf,
               public btg::core::Printable
            {
            public:

               /// Constructor. Parameters are used for default values of
               /// command line arguments.
               /// @param [in] _config_file      The config file to use.
               vsCommandLineArgumentHandler(std::string const& _config_file);

               void setup();

               bool parse(int argc, char **argv);

               /// Returns true if the client should attach to a session.
               bool doAttach() const;

               /// Returns true if the client should attach to
               /// the first session.
               bool doAttachFirst() const;

               /// Returns true if the address of the daemon is set.
               bool daemonSet() const;

               /// Returns a session ID or -1 if its undefined.
               t_long sessionId() const;

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

               bool automaticStart() const;
               bool useEncryptionSet() const;
               bool useEncryption() const;
               bool useDHTSet() const;
               bool useDHT() const;

               bool fullscreen() const;

               bool res1440x900() const;
               bool res1024x768() const;

               bool autoChangeTabs() const;
               t_uint getAutoDelay() const;
               t_uint getUpdateFreq() const;

               /// Destructor.
               virtual ~vsCommandLineArgumentHandler();

            private:
               /// The current session.
               t_long                       session;

               /// Indicates that the user wishes to attach to a
               /// session.
               bool                         attach_present;

               /// Indicates that the user wishes to attach to
               /// the first available session.
               bool                         attachFirst_present;

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

               bool                         fullscreen_set;

               bool                         res1440x900_set;
               bool                         res1024x768_set;

               bool                         autoTab_set;
               t_uint                       autoDelay;

               t_uint                       updateFreq;
            };

            /** @} */

         } // namespace viewer
      } // namespace gui
   } // namespace UI
} // namespace btg


#endif

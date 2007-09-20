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
 * $Id: arg.h,v 1.1.4.9 2006/10/28 16:00:11 wojci Exp $
 */

#ifndef AARG_H
#define AARG_H

#include <string>
#include <bcore/transport/transport.h>
#include <bcore/argif.h>

namespace btg
{
   namespace daemon
      {
         namespace auth
            {
               /**
                * \addtogroup daemon
                */
               /** @{ */

               /// This class implements parsing of arguments that can be passed to
               /// the btg passwd application.
               class commandLineArgumentHandler: public btg::core::argumentInterface
                  {
                  public:
                     /// Constructor.
                     /// @param [in] _config_file The configuration file to use.
                     commandLineArgumentHandler(std::string const& _config_file);

                     void setup();

                     bool parse(int argc, char **argv);

                     /// Returns true if a config file was set.
                     bool configFileSet() const;

                     /// Get the name of the configuration file used by the daemon.
                     std::string configFile() const;

                     /// True, if a new file should be created.
                     bool createNewFile() const;

                     /// Possible operations.
                     enum OPERATION
                        {
                           /// Undefined.
                           OP_UNDEF = 0,
                           /// Add an user.
                           OP_ADD,
                           /// Modifiy an user.
                           OP_MOD,
                           /// Delete an user.
                           OP_DEL,
                           /// List users.
                           OP_LST
                        };

                     /// Get the operation to perform.
                     OPERATION getOperation() const;

                     /// Get the username to use.
                     bool getUsername(std::string & _username) const;

                     /// True - read password from stdin,
                     /// false - do not read password.
                     bool readPassword() const;

                     /// Get the temp directory.
                     bool getTempDir(std::string & _temp_dir) const;

                     /// Get the work directory.
                     bool getWorkDir(std::string & _work_dir) const;

                     /// Get the seed directory.
                     bool getSeedDir(std::string & _seed_dir) const;

                     /// Get the destination directory.
                     bool getDestDir(std::string & _dest_dir) const;

                     /// Get the control flag.
                     bool getControlFlag(bool & _controlFlag) const;

                     /// Get the callback.
                     bool getCallback(std::string & _callback) const;

                     std::string getCompileTimeOptions() const;

                     /// Destructor.
                     virtual ~commandLineArgumentHandler();

                  private:
                     /// if a new file should be created.
                     bool                    createNewSwitch;

                     /// Add user.
                     bool                    addUserSwitch;

                     /// Modify user.
                     bool                    modifyUserSwitch;

                     /// Delete user.
                     bool                    deleteUserSwitch;

                     /// List users.
                     bool                    listUsersSwitch;

                     /// Username.
                     std::string             usernameArg;

                     /// Password.
                     bool                    passwordSwitch;

                     /// Temporary directory.
                     std::string             tempDirArg;

                     /// Working directory.
                     std::string             workDirArg;

                     /// Seeding directory.
                     std::string             seedDirArg;

                     /// Destination directory.
                     std::string             destDirArg;

                     /// Callback file.
                     std::string             callbackArg;

                     /// Control flag.
                     bool                    controlFlagArg;

                     /// Indicates that the control flag was set.
                     bool                    controlFlagArgSet;

                     /// The config file used.
                     std::string             config_file;

                     /// Flag: true if the config file argument was set.
                     bool                    config_file_present;
                  };

               /** @} */
            } // namespace auth
      } // namespace daemon
} // namespace btg

#endif

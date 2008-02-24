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

#ifndef PASSWORD_AUTH_H
#define PASSWORD_AUTH_H

#include "auth.h"
#include <bcore/printable.h>

#include <string>

#include <bcore/type.h>
#include <bcore/type_btg.h>

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

               class userData;

               /// Authorization of users using a passwd like file.
               class passwordAuth: public Auth, public btg::core::Printable
                  {
                  public:
                     /// Constructor.
                     /// @param [in] _filename     The filename from which read/write.
                     /// @param [in] _ignoreErrors When _ignoreErrors is set to true, allow reading from file to fail. Used when creating a new file.
                     passwordAuth(std::string const& _filename, bool const _ignoreErrors = false);

                     bool checkUser(std::string const& _username,
                                    std::string const& _passwordHash,
                                    btg::core::addressPort const& _addressport);

                     bool getTempDir(std::string const& _username, std::string & _tempDir) const;

                     bool getWorkDir(std::string const& _username, std::string & _workDir) const;

                     bool getSeedDir(std::string const& _username, std::string & _seedDir) const;

                     bool getDestDir(std::string const& _username, std::string & _destDir) const;

                     bool getControlFlag(std::string const& _username, bool & _controlFlag) const;

                     /// Get the filename for use reporting events.
                     bool getCallbackFile(std::string const& _username, std::string & _callbackFile) const;

                     bool initialized() const;

                     bool reInit();

                     /// Add an user.
                     bool addUser(std::string const& _username, std::string const& _password,
                                  std::string const& _temp_dir, std::string const& _work_dir,
                                  std::string const& _seed_dir, std::string const& _dest_dir,
                                  bool const _controlFlag, std::string const& _callback
                                  );

                     /// Modify an user.
                     bool modUser(std::string const& _username, std::string const& _password,
                                  std::string const& _temp_dir, std::string const& _work_dir,
                                  std::string const& _seed_dir, std::string const& _dest_dir,
                                  bool const _controlFlag, std::string const& _callback
                                  );

                     /// Delete an user.
                     bool delUser(std::string const& _username);

                     /// Write to file.
                     bool write();

                     std::string toString() const;

                     /// Destructor.
                     virtual ~passwordAuth();

                  private:
                     /// Read the file containing users and user data.
                     bool read(std::map<std::string, userData> & _dest);

                     /// Get an iterator or end(), using an username.
                     /// Internal function.
                     std::map<std::string, userData>::iterator GetUser(std::string const& _username);

                     /// The filename used.
                     std::string                     filename_;

                     /// Indicates if an instance of this class is
                     /// initialized.
                     bool                            initialized_;

                     /// Maps username to user data.
                     std::map<std::string, userData> users;
                  };

               /// User data.
               class userData
                  {
                  public:
                     /// Constructor.
                     userData(std::string const& _passwordHash,
                              std::string const& _tempDir,
                              std::string const& _workDir,
                              std::string const& _seedDir,
                              std::string const& _destDir,
                              bool const         _controlFlag,
                              std::string const& _callback
                              );

                     /// Copy constructor.
                     userData(userData const& _userdata);

                     /// The equality operator.
                     bool operator== (userData const& _userdata) const;

                     /// The not equal operator.
                     bool operator!= (userData const& _userdata) const;

                     /// The assignment operator.
                     userData& operator= (userData const& _userdata);

                     /// Set the hash used.
                     void setHash(std::string const& _hash) { passwordHash_ = _hash; };

                     /// Get the hash used.
                     std::string getHash() const { return passwordHash_; };

                     /// Set the temp dir.
                     void setTempDir(std::string const& _temp_dir) { tempDir_ = _temp_dir; };

                     /// Get the temp dir.
                     std::string getTempDir() const { return tempDir_; };

                     /// Set the work dir.
                     void setWorkDir(std::string const& _work_dir) { workDir_ = _work_dir; };

                     /// Get the work dir.
                     std::string getWorkDir() const { return workDir_; };

                     /// Set the seed dir.
                     void setSeedDir(std::string const& _seed_dir) { seedDir_ = _seed_dir; };

                     /// Get the seed dir.
                     std::string getSeedDir() const { return seedDir_; };

                     /// Set the destination dir.
                     void setDestDir(std::string const& _dest_dir) { destDir_ = _dest_dir; };

                     /// Get the destination dir.
                     std::string getDestDir() const { return destDir_; };

                     /// Set a flag that indicates that the user can
                     /// control the daemon.
                     void setControlFlag(bool const _controlFlag) { controlFlag_ = _controlFlag; };

                     /// Get a flag that indicates that the user can
                     /// control the daemon.
                     bool getControlFlag() const { return controlFlag_; };

                     /// Set the callback to be used.
                     void setCallback(std::string const& _callback) { callback_ = _callback; };

                     /// Get the callback to be used.
                     std::string getCallback() const { return callback_; };

                     /// Destructor.
                     ~userData();
                  private:
                     /// Password hash.
                     std::string passwordHash_;
                     /// Temp dir.
                     std::string tempDir_;
                     /// Work dir.
                     std::string workDir_;
                     /// Seed dir.
                     std::string seedDir_;
                     /// Destination dir.
                     std::string destDir_;
                     /// Indicates that the user can control the
                     /// daemon.
                     bool        controlFlag_;
                     /// Callback.
                     std::string callback_;
                  };

               /** @} */

            } // namespace auth
      } // namespace daemon
} // namespace btg

#endif // PASSWORD_AUTH_H


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

#ifndef AUTH_H
#define AUTH_H

#include <bcore/type.h>
#include <bcore/type_btg.h>
#include <bcore/addrport.h>
#include <bcore/logable.h>

#include <string>

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

               /// Authorization of users.
               /// Base class.
               class Auth: public btg::core::Logable
                  {
                  public:
                     /// Constructor.
                     Auth(btg::core::LogWrapperType _logwrapper);

                     /// Return true, if an user identified by an username,
                     /// hash of password and (address:port) is authorized
                     /// to connect to the daemon.
                     /// @param [in] _username     The username to use.
                     /// @param [in] _passwordHash The hash to use.
                     /// @param [in] _addressport  The address/port from which the users attempts to gain access.
                     /// @returns True - success, false - failture.
                     virtual bool checkUser(std::string const& _username,
                                            std::string const& _passwordHash,
                                            btg::core::addressPort const& _addressport) = 0;

                     /// Get the temporary directory for an user.
                     virtual bool getTempDir(std::string const& _username, std::string & _tempDir) const = 0;

                     /// Get the work directory for an user.
                     virtual bool getWorkDir(std::string const& _username, std::string & _workDir) const = 0;

                     /// Get the seed directory for an user.
                     virtual bool getSeedDir(std::string const& _username, std::string & _seedDir) const = 0;

                     /// Get the destination directory for an user.
                     virtual bool getDestDir(std::string const& _username, std::string & _destDir) const = 0;

                     /// Get a flag that indicates that the user can
                     /// control the daemon.
                     virtual bool getControlFlag(std::string const& _username, bool & _controlFlag) const = 0;

                     /// Get the filename for use reporting events.
                     virtual bool getCallbackFile(std::string const& _username, std::string & _callbackFile) const = 0;

                     /// String used to indicate that use of callbacks
                     /// is disabled for an user.
                     static std::string const callbackDisabled;

                     /// Return true, if an instance of this class is
                     /// initialized.
                     virtual bool initialized() const = 0;

                     /// Re-initialize and instance of this class.
                     virtual bool reInit() = 0;

                     /// Destructor.
                     virtual ~Auth();
                  };

               /** @} */

            } // namespace auth
      } // namespace daemon
} // namespace btg

#endif // AUTH

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

#ifndef HELPER_H
#define HELPER_H

#include <bcore/transport/transport.h>
#include <bcore/logmacro.h>

#include <bcore/client/helper_argif.h>
#include <bcore/client/configuration.h>
#include <bcore/client/clienthandler.h>
#include <bcore/client/carg.h>

#include <string>

/* Generic helpers, used by both gui and cli clients. */

namespace btg
{
   namespace core
      {
         namespace client
            {

               /// Base class for creating helpers used by the
               /// clients. Stores a number of pointers and is able
               /// to log messages than can be extracted later.
               class Helper
                  {
                  public:
                     /// Constructor.
                     /// @param [in] _clientName       The name of the client using this helper.
                     /// @param [in] _config           The client configuration used.
                     /// @param [in] _clah             The client command line arguments.
                     Helper(std::string const&          _clientName,
                            clientConfiguration*        _config,
                            HelperArgIf* _clah);
                     /// Get all the saved messages as one string.
                     virtual std::string getMessages();
                     /// Destructor.
                     virtual ~Helper();
                  protected:
                     /// Save a message.
                     virtual void addMessage(std::string const& _message);

                     /// The name of the client.
                     std::string                                  clientName;
                     /// The client configuration used.
                     clientConfiguration*                         config;
                     /// The client command line arguments.
                     HelperArgIf*                                 clah;
                     /// The session.
                     t_long                                       session;
                     /// A list of saved messages.
                     t_strList                                    messages;
                  private:
                     /// Copy constructor.
                     Helper(Helper const& _h);
                     /// Assignment operator.
                     Helper& operator=(Helper const& _h);
                  };

               /// Handles client startup, configuring transports,
               /// listing and attaching to already running sessions.
               ///
               /// \note Do _not_ delete any of the pointers contained
               /// in this class. Deletion should happend elsewhere.
               class startupHelper: public Helper
                  {
                  public:
                     /// Type of operation this class handles.
                     enum Operation
                        {
                           op_log = 1,      //!< Initialize logging.
                           op_auth,         //!< Get auth information from the user.
                           op_init,         //!< Init.
                           op_attach,       //!< Attach to a session, presents a list of sessions.
                           op_attach_first, //!< Attach to the first available session.
                           op_list,         //!< List sessions.
                           op_setup         //!< Execute setup.
                        };

                     /// Indicates status of an operation.
                     enum operationResult
                        {
                           or_undefined             =  0, //!< The result of an operation is undefined.

                           or_log_success           =  1, //!< Log initialized.
                           or_log_failture          = -1, //!< Log init failed.

                           or_auth_success          =  2, //!< Auth was set.
                           or_auth_failture         = -2, //!< Auth was not set.

                           // This operation cannot fail here.
                           // An attempt to setup or attach will fail
                           // if the transport is not ready.
                           or_init_success          =  3, //!< Initialize the transport.

                           or_attach_success        =  4, //!< Client attached.
                           or_attach_cancelled      = -4, //!< Client cancelled attach operation.
                           or_attach_failture       = -5, //!< Client attach failed.

                           or_attach_first_success  =  6, //!< Client attached.
                           or_attach_first_failture = -6, //!< Client attach failed.

                           or_list_success          =  7, //!< Session list success.
                           or_list_failture         = -7, //!< Session list failture.

                           or_setup_success         =  8, //!< Setup success.
                           or_setup_failture        = -8  //!< Setup failture.
                        };

                     /// Constructor.
                     /// \note None of the pointers are deleted by the destructor of this class.
                     /// @param [in] _clientName       The name of the client using this helper.
                     /// @param [in] _config           The client configuration used.
                     /// @param [in] _clah             The client command line arguments.
                     /// @param [in] _messageTransport The message transport used.
                     /// @param [in] _handler          The client handler used.

                     startupHelper(std::string const&          _clientName,
                                   clientConfiguration*        _config,
                                   HelperArgIf*                _clah,
                                   messageTransport*           _messageTransport,
                                   clientHandler*              _handler
                                   );

                     /// Init this helper.
                     virtual bool init();

                     /// Execute an operation.
                     /// @param [in] _operation The operation to execute.
                     /// @return Operation status.
                     virtual startupHelper::operationResult execute(startupHelper::Operation const _operation);

                     /// Implement this function, so it presents a
                     /// list of sessions to the user to choose from.
                     /// Should return Command::INVALID_SESSION if the
                     /// user cancels this operation.
                     /// @param [in] _sessions List of sessions to choose from.
                     /// @return Valid session ID or Command::INVALID_SESSION.
                     virtual t_long queryUserAboutSession(t_longList const& _sessions) const = 0;

                     /// Implement his function, so it asks the user
                     /// about which username and password he wishes
                     /// to use.
                     /// Make it call setAuth to set the username and the password.
                     /// @return True, if the auth information was set, false otherwise.
                     virtual bool authUserQuery() = 0;

                     /// Set the auth parameters which are used for
                     /// creating a transport to the daemon.
                     /// @param [in] _username The username to use.
                     /// @param [in] _password The password to use.
                     virtual void setAuth(std::string const& _username, std::string const& _password);

                     /// Return true, if authorization information was set.
                     virtual bool authSet() const;

                     /// Implement this function, so it shows a list of sessions.
                     /// @param [in] _sessions List of sessions.
                     virtual void showSessions(t_longList const& _sessions) const = 0;

                     /// Get the username, set by setAuth.
                     virtual std::string user() const;

                     /// Set the username.
                     virtual void setUser(std::string const& _user);

                     /// Get the password hash, set by setAuth.
                     virtual std::string passwordHash() const;

                     /// Set the password hash.
                     /// Used when password is stored in the client's
                     /// config to avoid a prompt each time one
                     /// connects to the daemon.
                     /// Also set authSet_ to true.
                     virtual void setPasswordHash(std::string const& _passwordHash);

                     /// Destructor.
                     /// \note Does not delete the stored pointers.
                     virtual ~startupHelper();

                  protected:
                     /// The message transport used.
                     btg::core::messageTransport*                   mtrans;

                     /// The client handler used.
                     btg::core::client::clientHandler*              handler;

                     /// The username used for login.
                     std::string                                    username_;
                     /// The password used for login.
                     std::string                                    passwordHash_;
                  private:
                     /// Indicates that username and password was set.
                     bool                                           authSet_;
                  private:
                     /// Copy constructor.
                     startupHelper(startupHelper const& _h);
                     /// Assignment operator.
                     startupHelper& operator=(startupHelper const& _h);
                  };

               /// Helper class which initializes the transports used in btg.
               class transportHelper: public Helper
                  {
                  public:
                     /// Constructor.
                     /// \note None of the pointers are deleted by the destructor of this class.
                     /// @param [in] _clientName       The name of the client using this helper.
                     /// @param [in] _config           The client configuration used.
                     /// @param [in] _clah             The client command line arguments.
                     transportHelper(std::string const&          _clientName,
                                     clientConfiguration*        _config,
                                     HelperArgIf*                _clah);

                     /// Init the message transport used and the
                     //externalization use to encode/decode commands
                     //sent trough the transport.
                     /// @param [out] _transport The message transport to init.
                     /// @param [out] _e         The externalization to init.
                     /// @returns True - success. False - failture.
                     bool initTransport(btg::core::externalization::Externalization* & _e,
                                        messageTransport* & _transport);

                     /// Destructor.
                     /// \note Does not delete the stored pointers.
                     virtual ~transportHelper();
                  protected:
                     /// Common setup used for secure transports.
                     bool checkRequiredFiles(std::string & _ca_cert,
                                             std::string & _cert,
                                             std::string & _privkey);
                  };

            } // namespace client
      } // namespace core
} // namespace btg

#endif

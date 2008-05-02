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
#include <bcore/logable.h>

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
               class Helper: public Logable
                  {
                  public:
                     /// Constructor.
                     /// @param [in] _logwrapper Pointer used to send logs to.
                     /// @param [in] _clientName The name of the client using this helper.
                     /// @param [in] _config     The client configuration used.
                     /// @param [in] _clah       The client command line arguments.
                     Helper(LogWrapperType _logwrapper,
                            std::string const&          _clientName,
                            clientConfiguration&        _config,
                            HelperArgIf& _clah);
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
                     clientConfiguration&                         config;
                     /// The client command line arguments.
                     HelperArgIf&                                 clah;
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

               /// Implement this interface to be able to use the
               /// startupHelper class.
               class startupHelperIf
               {
               public:
                  /// Constructor.
                  startupHelperIf();
                  
                  /// Show a query where the user can input an
                  /// username and password.
                  /// \note This function _will_ be called at some
                  ///       point during the initalization.
                  virtual bool AuthQuery() = 0;

                  virtual bool AttachSessionQuery(t_longList const& _sessionsIDs,
                                                  t_strList const& _sessionNames,
                                                  t_long & _session) = 0;

                  virtual void ListSessions(t_longList const& _sessions,
                                            t_strList const& _sessionNames) = 0;

                  virtual bool DefaultAction(t_longList const& _sessions,
                                             t_strList const& _sessionNames,
                                             bool & _attach,
                                             t_long & _sessionId) = 0;

                  /// Return true, if authorization information was set.
                  virtual bool authSet() const;

                  /// Get the password hash, set by setAuth.
                  virtual std::string passwordHash() const;

                  /// Get the username, set by setAuth.
                  virtual std::string user() const;

                  
                  /// Set the password hash.
                  /// Used when password is stored in the client's
                  /// config to avoid a prompt each time one
                  /// connects to the daemon.
                  /// Also set authSet_ to true.
                  virtual void setPasswordHash(std::string const& _passwordHash);

                  /// Set the username.
                  virtual void setUser(std::string const& _user);

                  /// Destructor.
                  virtual ~startupHelperIf();
               protected:
                  /// Set the auth parameters which are used for
                  /// creating a transport to the daemon.
                  /// @param [in] _username The username to use.
                  /// @param [in] _password The password to use.
                  virtual void setAuth(std::string const& _username, 
                                       std::string const& _password);

                  /// The username used for login.
                  std::string username_;
                  
                  /// The password used for login.
                  std::string passwordHash_;
                  
                  /// Indicates that username and password was set.
                  bool        authSet_;
               };

               /// Handles client startup, configuring transports,
               /// listing and attaching to already running sessions.
               ///
               /// \note Do _not_ delete any of the pointers contained
               /// in this class. Deletion should happend elsewhere.
               class startupHelper: public Helper
                  {
                  public:
                     startupHelper(LogWrapperType _logwrapper,
                                   std::string const&          _clientName,
                                   clientConfiguration&        _config,
                                   HelperArgIf&                _clah,
                                   messageTransport&           _messageTransport,
                                   clientHandler&              _handler,
                                   startupHelperIf &           _helperIf
                                   );

                     /// Setup logging.
                     bool SetupLogging();

                     /// Initialize connection to the daemon.
                     bool Init();

                     /// Attach to a session.
                     /// Queries the user about which session id to use, if
                     /// its not been given using command line arguments.
                     bool AttachSession(t_long & _sessionId);

                     /// Attach to a session. No questions are asked.
                     bool AttachToSession(t_long const _sessionId);

                     /// Attach to first available session.
                     bool AttachFirstSession(t_long & _sessionId);

                     /// List sessions present.
                     bool ListSessions();

                     /// Create a new session.
                     bool NewSession(t_long & _sessionId);
                     
                     /// No command line parameters were given, so
                     /// show a list of sessions which can be attached to or
                     /// allow the user to create a new one.
                     bool DefaultAction(bool & _attach, t_long & _sessionId);

                     startupHelperIf & getIf() const;

                     /// Destructor.
                     /// \note Does not delete the stored pointers.
                     virtual ~startupHelper();

                  protected:
                     /// The message transport used.
                     btg::core::messageTransport&                   mtrans;

                     /// The client handler used.
                     btg::core::client::clientHandler&              handler;

                     clientConfiguration&                           config;
                     startupHelperIf&                               helperif;
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
                     /// @param [in] _logwrapper Pointer used to send logs to.
                     /// @param [in] _clientName The name of the client using this helper.
                     /// @param [in] _config     The client configuration used.
                     /// @param [in] _clah       The client command line arguments.
                     transportHelper(LogWrapperType _logwrapper,
                                     std::string const&          _clientName,
                                     clientConfiguration&        _config,
                                     HelperArgIf&                _clah);

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

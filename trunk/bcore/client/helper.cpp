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

#include "helper.h"
#include <bcore/util.h>

#include <bcore/os/fileop.h>
#include <bcore/os/id.h>
#include <bcore/os/gnutlsif.h>

#include <bcore/transport/tcpip.h>
#include <bcore/transport/stcpip.h>
#include <bcore/transport/http.h>
#include <bcore/transport/shttp.h>
#include <bcore/addrport.h>

#include <bcore/externalization/xmlrpc.h>

#include <bcore/logger/logger.h>
// Loggers:
#include <bcore/logger/console_log.h>
#include <bcore/logger/file_log.h>
#include <bcore/logger/syslog_log.h>

#include <bcore/auth/hash.h>

namespace btg
{
   namespace core
   {
      namespace client
      {
         Helper::Helper(std::string const&          _clientName,
                        clientConfiguration*        _config,
                        HelperArgIf* _clah)
            : clientName(_clientName),
              config(_config),
              clah(_clah),
              session(Command::INVALID_SESSION),
              messages(0)
         {}

         std::string Helper::getMessages()
         {
            std::string output;

            t_strListCI iter;

            for (iter = messages.begin();
                 iter != messages.end();
                 iter++)
               {
                  output += *iter;
                  output += GPD->sNEWLINE();
               }

            messages.clear();

            return output;
         }

         Helper::~Helper()
         {

         }

         void Helper::addMessage(std::string const& _message)
         {
            messages.push_back(_message);
         }

         startupHelper::startupHelper(std::string const&          _clientName,
                                      clientConfiguration*        _config,
                                      HelperArgIf*                _clah,
                                      messageTransport*           _mtrans,
                                      clientHandler*              _handler
                                      )
            : Helper(
                     _clientName,
                     _config,
                     _clah),
              mtrans(_mtrans),
              handler(_handler),
              username_(),
              passwordHash_(),
              authSet_(false)
         {
         }

         bool startupHelper::init()
         {
            using namespace btg::core;

            if (!mtrans->isInitialized())
               {
                  addMessage("Message transport not initialized.");
                  return false;
               }

            return true;
         }

         startupHelper::operationResult startupHelper::execute(startupHelper::Operation const _operation)
         {
            startupHelper::operationResult result = startupHelper::or_undefined;

            switch(_operation)
               {
               case startupHelper::op_log:
                  {
                     using namespace btg::core::logger;

                     result = or_log_success;

                     switch(config->getLog())
                        {
                        case logBuffer::STDOUT:
                           {
                              logWrapper* log = logWrapper::getInstance();
                              log->setLogStream(new logStream(new consoleLogger()));
                              break;
                           }
                        case logBuffer::LOGFILE:
                           {
                              std::string logfilename = config->getLogFile();
                              btg::core::os::fileOperation::resolvePath(logfilename);

                              logWrapper* log     = logWrapper::getInstance();
                              fileLogger* fl      = new fileLogger(logfilename);

                              if (fl->open())
                                 {
                                    log->setLogStream(new logStream(fl));
                                    BTG_NOTICE("Logfile opened");
                                 }
                              else
                                 {
                                    result = or_log_failture;
                                    BTG_NOTICE("Failed to open '" << logfilename << "' for writing.");
                                 }

                              break;
                           }
                        case logBuffer::SYSLOG:
                           {
                              // Log to syslog.
                              logWrapper* log = logWrapper::getInstance();
                              log->setLogStream(new logStream(new syslogLogger()));
                              break;
                           }
                        case logBuffer::UNDEF:
                           {
                              // No logging.
                              break;
                           }
                        }
                     break;
                  }
               case startupHelper::op_init:
                  {
                     btg::core::Hash h;
                     h.set(passwordHash());
                     handler->reqInit(user(), h);
                     result = startupHelper::or_init_success;
                     break;
                  }
               case startupHelper::op_auth:
                  {
                     result = startupHelper::or_auth_failture;
                     if (authUserQuery())
                        {
                           // The user entered the auth information.
                           result = startupHelper::or_auth_success;
                        }

                     // Make sure that the username and hash was
                     // actually set.
                     if (!authSet())
                        {
                           BTG_NOTICE("No auth information set.");
                           result = startupHelper::or_auth_failture;
                        }

                     break;
                  }
               case startupHelper::op_attach:
                  {
                     // Attach to a certain session, either specified on the
                     // command line or chosen by the user from a list.

                     t_long temp_session = clah->sessionId();
                     if (temp_session != Command::INVALID_SESSION)
                        {
                           std::string errorMessage;
                           handler->reqSetupAttach(temp_session);

                           if (!handler->isAttachDone())
                              {
                                 addMessage("Unable to attach to session.");
                                 addMessage("The daemon returned: " +
                                            handler->getAttachFailtureMessage());
                                 result = startupHelper::or_attach_failture;
                                 break;
                              }
                           else
                              {
                                 // Was able to attach to session.
                                 result = startupHelper::or_attach_success;
                                 session = temp_session;
                              }

                           result = startupHelper::or_attach_success;
                           break;
                        }
                     else
                        {
                           // Ask user which session he wants to attach to.
                           t_longList sessions;
                           handler->reqGetActiveSessions();

                           sessions = handler->getSessionList();

                           // Make sure that sessions are available.
                           if (sessions.size() <= 0)
                              {
                                 result = startupHelper::or_attach_failture;
                                 addMessage("No sessions present.");

                                 break;
                              }

                           t_long temp_session =  this->queryUserAboutSession(sessions);

                           if (temp_session != Command::INVALID_SESSION)
                              {
                                 // Attempt to attach to session.

                                 std::string errorMessage;

                                 handler->reqSetupAttach(temp_session);

                                 if (!handler->isAttachDone())
                                    {
                                       addMessage("Unable to attach to session.");
                                       addMessage("The daemon returned: " +
                                                  handler->getAttachFailtureMessage());

                                       result = startupHelper::or_attach_failture;
                                       break;
                                    }
                                 else
                                    {
                                       // Was able to attach to session.
                                       result = startupHelper::or_attach_success;
                                       session = temp_session;
                                    }
                              }
                           else
                              {
                                 // Invalid session ID.
                                 // User cancelled.
                                 result = startupHelper::or_attach_cancelled;
                              }

                        } // Attach to as session.

                     break;
                  }

               case startupHelper::op_attach_first:
                  {
                     // Attach to the first available session.

                     t_longList sessions;
                     handler->reqGetActiveSessions();

                     sessions = handler->getSessionList();

                     t_longListCI vlci = sessions.begin();

                     if (vlci != sessions.end())
                        {
                           // Got user input, attach to session.
                           handler->reqSetupAttach(*vlci);

                           if (!handler->isAttachDone())
                              {
                                 addMessage("Unable to attach to session.");

                                 result = startupHelper::or_attach_first_failture;
                                 break;
                              }
                           else
                              {
                                 session = *vlci;
                              }
                        }
                     else
                        {
                           addMessage("No session to attach to.");

                           result = startupHelper::or_attach_first_failture;
                           break;
                        }
                     result = startupHelper::or_attach_first_success;
                     break;
                  }
               case startupHelper::op_list:
                  {
                     t_longList sessions;
                     handler->reqGetActiveSessions();

                     sessions = handler->getSessionList();

                     if (sessions.size() > 0)
                        {
                           result = startupHelper::or_list_success;
                           showSessions(sessions);
                        }
                     else
                        {
                           result = startupHelper::or_list_failture;
                           addMessage("No sessions to list.");
                        }
                     break;
                  }
               case startupHelper::op_setup:
                  {
                     // If DHT was enabled/disabled using a command
                     // line argument, use that instead of the value
                     // from the config file.
                     bool useDHT = config->getUseDHT();
                     if (clah->useDHTSet())
                        {
                           if (clah->useDHT() != useDHT)
                              {
                                 useDHT = clah->useDHT();
                              }
                        }

                     // If encryption was enabled/disabled using a
                     // command line argument, use that instead of the
                     // value from the config file.
                     bool useEncryption = config->getUseEncryption();
                     if (clah->useEncryptionSet())
                        {
                           if (clah->useEncryption() != useEncryption)
                              {
                                 useEncryption = clah->useEncryption();
                              }
                        }

                     handler->reqSetup(clientHandler::NO_SEEDLIMIT, 
                                       clientHandler::NO_SEEDTIMEOUT, 
                                       useDHT,
                                       useEncryption);

                     if (!handler->isSetupDone())
                        {
                           addMessage(handler->getSetupFailtureMessage());
                           addMessage("Could not establish a session.");
                           result = startupHelper::or_setup_failture;
                           break;
                        }

                     result = startupHelper::or_setup_success;
                     break;
                  }
               }

            return result;
         }

         void startupHelper::setAuth(std::string const& _username, std::string const& _password)
         {
            username_ = _username;
            Hash hash;
            hash.generate(_password);
            hash.get(passwordHash_);

            authSet_ = true;
         }

         bool startupHelper::authSet() const
         {
            return authSet_;
         }

         std::string startupHelper::user() const
         {
            return username_;
         }

         void startupHelper::setUser(std::string const& _user)
         {
            username_ = _user;
         }

         std::string startupHelper::passwordHash() const
         {
            return passwordHash_;
         }

         void startupHelper::setPasswordHash(std::string const& _passwordHash)
         {
            passwordHash_ = _passwordHash;
            authSet_      = true;
         }

         startupHelper::~startupHelper()
         {
            // The contained pointers are deleted elsewhere.
         }

         /* -- */

         transportHelper::transportHelper(std::string const&   _clientName,
                                          clientConfiguration* _config,
                                          HelperArgIf*         _clah)
            : Helper(
                     _clientName,
                     _config,
                     _clah)
         {

         }

         bool transportHelper::checkRequiredFiles(std::string & _ca_cert,
                                                  std::string & _cert,
                                                  std::string & _privkey)
         {
            std::string errorString;

            _ca_cert = config->getCACert();
            btg::core::os::fileOperation::resolvePath(_ca_cert);

            if (!btg::core::os::fileOperation::check(_ca_cert, errorString, false))
               {
                  addMessage("Could not read CA certificate from file '" + _ca_cert + "'.");
                  return false;
               }


            _cert = config->getCert();
            btg::core::os::fileOperation::resolvePath(_cert);

            if (!btg::core::os::fileOperation::check(_cert, errorString, false))
               {
                  addMessage("Could not read client certificate from file '" + _cert + "'.");
                  return false;
               }

            _privkey = config->getPrivKey();
            btg::core::os::fileOperation::resolvePath(_privkey);

            if (!btg::core::os::fileOperation::check(_privkey, errorString, false))
               {
                  addMessage("Could not read private key from file '" + _privkey + "'.");
                  return false;
               }

            return true;
         }

         bool transportHelper::initTransport(btg::core::externalization::Externalization* & _e,
                                             messageTransport* & _transport)
         {
            bool status = false;

            switch(config->getTransport())
               {
               case messageTransport::TCP:
                  {
                     btg::core::addressPort ap = config->getDaemonAddressPort();

                     if (clah->daemonSet())
                        {
                           ap = clah->getDaemon();
                        }

                     _e = new btg::core::externalization::XMLRPC;

                     _transport = new tcpTransport(_e,
                                                   100 * 1024,
                                                   TO_SERVER,
                                                   ap,
                                                   1 // 1ms timeout
                                                   );

                     if (!_transport->isInitialized())
                        {
                           addMessage("Attempt to connect to " + ap.toString() + " failed.");
                           addMessage("Could not create a TCP/IP message transport.");
                           return status;
                        }
                     status = true;
                     break;
                  }
               case messageTransport::STCP:
                  {
                     btg::core::addressPort ap = config->getDaemonAddressPort();

                     if (clah->daemonSet())
                        {
                           ap = clah->getDaemon();
                        }

                     std::string errorString;

                     std::string ca_cert;
                     std::string cert;
                     std::string privkey;

                     if (!checkRequiredFiles(ca_cert, cert, privkey))
                        {
                           return status;
                        }

                     _e = new btg::core::externalization::XMLRPC;
                     btg::core::os::gtlsGeneric::init();
                     _transport = new secureTcpTransport(_e,
                                                         new btg::core::os::gtlsClientData(ca_cert, cert, privkey),
                                                         100 * 1024,
                                                         TO_SERVER,
                                                         ap,
                                                         1 // 1ms timeout
                                                         );

                     if (!_transport->isInitialized())
                        {
                           addMessage("Attempt to connect to " + ap.toString() + " failed.");
                           addMessage("Could not create a secure TCP/IP message transport.");
                           return status;
                        }
                     status = true;
                     break;
                  }
               case messageTransport::XMLRPC:
                  {
                     btg::core::addressPort ap = config->getDaemonAddressPort();

                     if (clah->daemonSet())
                        {
                           ap = clah->getDaemon();
                        }

                     _e = new btg::core::externalization::XMLRPC;

                     _transport = new httpTransport(_e,
                                                    100 * 1024,
                                                    TO_SERVER,
                                                    ap,
                                                    1 // 1ms timeout
                                                    );

                     if (!_transport->isInitialized())
                        {
                           addMessage("Attempt to connect to " + ap.toString() + " failed.");
                           addMessage("Could not create a XMLRPC message transport.");
                           return status;
                        }
                     status = true;
                     break;
                  }
               case messageTransport::SXMLRPC:
                  {
                     btg::core::addressPort ap = config->getDaemonAddressPort();

                     if (clah->daemonSet())
                        {
                           ap = clah->getDaemon();
                        }

                     std::string ca_cert;
                     std::string cert;
                     std::string privkey;

                     if (!checkRequiredFiles(ca_cert, cert, privkey))
                        {
                           return status;
                        }

                     _e = new btg::core::externalization::XMLRPC;
                     btg::core::os::gtlsGeneric::init();
                     _transport = new secureHttpTransport(_e,
                                                          new btg::core::os::gtlsClientData(ca_cert, cert, privkey),
                                                          100 * 1024,
                                                          TO_SERVER,
                                                          ap,
                                                          1 // 1ms timeout
                                                          );

                     if (!_transport->isInitialized())
                        {
                           addMessage("Attempt to connect to " + ap.toString() + " failed.");
                           addMessage("Could not create a secure XMLRPC message transport.");
                           return status;
                        }
                     status = true;
                     break;
                  }
               case messageTransport::UNDEFINED:
                  {
                     status = false;
                     addMessage("Transport type not set.");
                     break;
                  }
               } // switch

            return status;
         }

         transportHelper::~transportHelper()
         {
            // The contained pointers are deleted elsewhere.
         }

      } // namespace client
   } // namespace core
} // namespace btg



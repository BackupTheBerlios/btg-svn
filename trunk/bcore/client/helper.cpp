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
#include <bcore/logmacro.h>

namespace btg
{
   namespace core
   {
      namespace client
      {
         Helper::Helper(LogWrapperType _logwrapper,
                        std::string const&          _clientName,
                        clientConfiguration&        _config,
                        HelperArgIf& _clah)
            : Logable(_logwrapper),
              clientName(_clientName),
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

         /* */

         startupHelperIf::startupHelperIf()
            : username_(),
              passwordHash_(),
              authSet_(false)
         {
         }

         void startupHelperIf::setAuth(std::string const& _username, std::string const& _password)
         {
            username_ = _username;
            Hash hash;
            hash.generate(_password);
            hash.get(passwordHash_);

            authSet_ = true;
         }

         bool startupHelperIf::authSet() const
         {
            return authSet_;
         }

         std::string startupHelperIf::user() const
         {
            return username_;
         }

         void startupHelperIf::setUser(std::string const& _user)
         {
            username_ = _user;
         }

         std::string startupHelperIf::passwordHash() const
         {
            return passwordHash_;
         }

         void startupHelperIf::setPasswordHash(std::string const& _passwordHash)
         {
            passwordHash_ = _passwordHash;
            authSet_      = true;
         }

         startupHelperIf::~startupHelperIf()
         {

         }

         /* */

         startupHelper::startupHelper(LogWrapperType _logwrapper,
                                      std::string const&          _clientName,
                                      clientConfiguration&        _config,
                                      HelperArgIf&                _clah,
                                      messageTransport&           _mtrans,
                                      clientHandler&              _handler,
                                      startupHelperIf&            _helperif
                                      )
            : Helper(_logwrapper,
                     _clientName,
                     _config,
                     _clah),
              mtrans(_mtrans),
              handler(_handler),
              config(_config),
              helperif(_helperif)
         {
         }

         bool startupHelper::SetupLogging()
         {
            bool status = true;
            using namespace btg::core::logger;

            switch(config.getLog())
               {
               case logBuffer::STDOUT:
                  {
                     boost::shared_ptr<btg::core::logger::logStream> l(new btg::core::logger::logStream(new btg::core::logger::consoleLogger()));
                     logWrapper()->setLogStream(l);
                     break;
                  }
               case logBuffer::LOGFILE:
                  {
                     std::string logfilename = config.getLogFile();
                     btg::core::os::fileOperation::resolvePath(logfilename);
                     
                     fileLogger* fl = new fileLogger(logfilename);
                     
                     if (fl->open())
                        {
                           boost::shared_ptr<btg::core::logger::logStream> l(new btg::core::logger::logStream(fl));
                           logWrapper()->setLogStream(l);
                           BTG_NOTICE(logWrapper(), "Logfile opened");
                        }
                     else
                        {
                           delete fl;
                           fl = 0;
                           BTG_NOTICE(logWrapper(), "Failed to open '" << logfilename << "' for writing.");
                           status = false;
                        }
                     
                     break;
                  }
               case logBuffer::SYSLOG:
                  {
                     // Log to syslog.
                     boost::shared_ptr<btg::core::logger::logStream> l(new logStream(new syslogLogger()));
                     logWrapper()->setLogStream(l);
                     break;
                  }
               case logBuffer::UNDEF:
                  {
                     // No logging.
                     break;
                  }
               }

            return status;
         }

         bool startupHelper::Init()
         {
            using namespace btg::core;

            if (config.authSet())
               {
                  // Auth info is in the config.
                  helperif.setUser(config.getUserName());
                  helperif.setPasswordHash(config.getPasswordHash());
               }

            if (!mtrans.isInitialized())
               {
                  addMessage("Message transport not initialized.");
                  return false;
               }

            if (!helperif.authSet())
               {
                  if (!helperif.AuthQuery())
                     {
                        return false;
                     }
               }

            btg::core::Hash h;
            h.set(helperif.passwordHash());
            handler.reqInit(helperif.user(), h);

            if (handler.transinitwaitError())
               {
                  addMessage("Auth error.");
                  return false;
               }
            
            return true;
         }

         bool startupHelper::AttachToSession(t_long const _sessionId)
         {
            handler.reqSetupAttach(_sessionId);

            if (!handler.isAttachDone())
               {
                  addMessage("Unable to attach to session.");
                  addMessage("The daemon returned: " +
                             handler.getAttachFailtureMessage());

                  return false;
               }

            return true;
         }

         bool startupHelper::AttachSession(t_long & _sessionId)
         {
            t_long temp_session = clah.sessionId();
            if (temp_session != Command::INVALID_SESSION)
               {
                  std::string errorMessage;
                  handler.reqSetupAttach(temp_session);

                  if (!handler.isAttachDone())
                     {
                        addMessage("Unable to attach to session.");
                        addMessage("The daemon returned: " +
                                   handler.getAttachFailtureMessage());

                        return false;
                     }
                  else
                     {
                        // Was able to attach to session.
                        _sessionId = temp_session;
                     }
               }
            else
               {
                  // Ask user which session he wants to attach to.
                  handler.reqGetActiveSessions();

                  t_longList sessions   = handler.getSessionList();
                  t_strList sessionsIDs = handler.getSessionNames();

                  // Make sure that sessions are available.
                  if (sessions.size() <= 0)
                     {
                        addMessage("No sessions present.");
                        return false;
                     }

                  t_long temp_session = Command::INVALID_SESSION;
                  helperif.AttachSessionQuery(sessions, sessionsIDs, temp_session);

                  if (temp_session != Command::INVALID_SESSION)
                     {
                        // Attempt to attach to session.
                        std::string errorMessage;

                        handler.reqSetupAttach(temp_session);

                        if (!handler.isAttachDone())
                           {
                              addMessage("Unable to attach to session.");
                              addMessage("The daemon returned: " +
                                         handler.getAttachFailtureMessage());

                              return false;
                           }
                        else
                           {
                              // Was able to attach to session.
                              _sessionId = temp_session;
                           }
                     }
                  else
                     {
                        addMessage("Attach cancelled.");
                        return false;
                     }
               }
            return true;
         }

         bool startupHelper::AttachFirstSession(t_long & _sessionId)
         {
            // Attach to the first available session.
            handler.reqGetActiveSessions();

            t_longList sessions = handler.getSessionList();
            t_longListCI vlci   = sessions.begin();

            if (vlci != sessions.end())
               {
                  // Got user input, attach to session.
                  handler.reqSetupAttach(*vlci);

                  if (!handler.isAttachDone())
                     {
                        addMessage("Unable to attach to session.");
                     }
                  else
                     {
                        _sessionId = *vlci;
                        return true;
                     }
               }
            else
               {
                  addMessage("No session to attach to.");
               }
            return false;
         }

         bool startupHelper::ListSessions()
         {
            handler.reqGetActiveSessions();

            t_longList sessions   = handler.getSessionList();
            t_strList sessionsIDs = handler.getSessionNames();

            if (sessions.size() > 0)
               {
                  helperif.ListSessions(sessions, sessionsIDs);
               }
            else
               {
                  addMessage("No sessions to list.");
                  return false;
               }
            
            return true;
         }

         bool startupHelper::NewSession(t_long & _sessionId)
         {
            // If DHT was enabled/disabled using a command
            // line argument, use that instead of the value
            // from the config file.
            bool useDHT = config.getUseDHT();
            if (clah.useDHTSet())
               {
                  if (clah.useDHT() != useDHT)
                     {
                        useDHT = clah.useDHT();
                     }
               }
            
            // If encryption was enabled/disabled using a
            // command line argument, use that instead of the
            // value from the config file.
            bool useEncryption = config.getUseEncryption();
            if (clah.useEncryptionSet())
               {
                  if (clah.useEncryption() != useEncryption)
                     {
                        useEncryption = clah.useEncryption();
                     }
               }
            
            handler.reqSetup(clientHandler::NO_SEEDLIMIT, 
                             clientHandler::NO_SEEDTIMEOUT, 
                             useDHT,
                             useEncryption);
            
            if (!handler.isSetupDone())
               {
                  addMessage(handler.getSetupFailtureMessage());
                  addMessage("Could not establish a session.");
                  return false;
               }
            else
               {
                  _sessionId = handler.session();
               }

            return true;
         }

         bool startupHelper::DefaultAction(bool & _attach, t_long & _sessionId)
         {
            handler.reqGetActiveSessions();

            t_longList sessions   = handler.getSessionList();
            t_strList sessionsIDs = handler.getSessionNames();

            if (!helperif.DefaultAction(sessions, 
                                        sessionsIDs,
                                        _attach,
                                        _sessionId))
               {
                  // Aborted.
                  return false;
               }

            return true;
         }

         startupHelperIf & startupHelper::getIf() const
         {
            return helperif;
         }

         startupHelper::~startupHelper()
         {
         }

         /* -- */

         transportHelper::transportHelper(LogWrapperType _logwrapper,
                                          std::string const&   _clientName,
                                          clientConfiguration& _config,
                                          HelperArgIf&         _clah)
            : Helper(_logwrapper,
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

            _ca_cert = config.getCACert();
            btg::core::os::fileOperation::resolvePath(_ca_cert);

            if (!btg::core::os::fileOperation::check(_ca_cert, errorString, false))
               {
                  addMessage("Could not read CA certificate from file '" + _ca_cert + "'.");
                  return false;
               }


            _cert = config.getCert();
            btg::core::os::fileOperation::resolvePath(_cert);

            if (!btg::core::os::fileOperation::check(_cert, errorString, false))
               {
                  addMessage("Could not read client certificate from file '" + _cert + "'.");
                  return false;
               }

            _privkey = config.getPrivKey();
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

            switch(config.getTransport())
               {
               case messageTransport::TCP:
                  {
                     btg::core::addressPort ap = config.getDaemonAddressPort();

                     if (clah.daemonSet())
                        {
                           ap = clah.getDaemon();
                        }

                     _e = new btg::core::externalization::XMLRPC(logWrapper());

                     _transport = new tcpTransport(logWrapper(),
                                                   _e,
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
                     btg::core::addressPort ap = config.getDaemonAddressPort();

                     if (clah.daemonSet())
                        {
                           ap = clah.getDaemon();
                        }

                     std::string errorString;

                     std::string ca_cert;
                     std::string cert;
                     std::string privkey;

                     if (!checkRequiredFiles(ca_cert, cert, privkey))
                        {
                           return status;
                        }

                     _e = new btg::core::externalization::XMLRPC(logWrapper());
                     btg::core::os::gtlsGeneric::init();
                     _transport = new secureTcpTransport(logWrapper(),
                                                         _e,
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
                     btg::core::addressPort ap = config.getDaemonAddressPort();

                     if (clah.daemonSet())
                        {
                           ap = clah.getDaemon();
                        }

                     _e = new btg::core::externalization::XMLRPC(logWrapper());

                     _transport = new httpTransport(logWrapper(),
                                                    _e,
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
                     btg::core::addressPort ap = config.getDaemonAddressPort();

                     if (clah.daemonSet())
                        {
                           ap = clah.getDaemon();
                        }

                     std::string ca_cert;
                     std::string cert;
                     std::string privkey;

                     if (!checkRequiredFiles(ca_cert, cert, privkey))
                        {
                           return status;
                        }

                     _e = new btg::core::externalization::XMLRPC(logWrapper());
                     btg::core::os::gtlsGeneric::init();
                     _transport = new secureHttpTransport(logWrapper(),
                                                          _e,
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
         }

      } // namespace client
   } // namespace core
} // namespace btg



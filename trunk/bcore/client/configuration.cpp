#include "configuration.h"

#include <bcore/t_string.h>
#include <bcore/util.h>
#include <bcore/os/fileop.h>

namespace btg
{
   namespace core
   {
      namespace client
      {
         using namespace btg::core;

         std::string const SECTION_TRANSPORT("transport");
         std::string const KEY_TRANSPORT_DEFAULT("default");
         std::string const COMMENT_TRANSPORT_DEFAULT("default transport");
          
         std::string const VALUE_TRANSPORT_TCPIP("tcpip");
         std::string const VALUE_TRANSPORT_STCPIP("stcpip");
         std::string const VALUE_TRANSPORT_XMLRPC("xml-rpc");
         std::string const VALUE_TRANSPORT_SXMLRPC("sxml-rpc");

         std::string const SECTION_NETWORK("network");
         std::string const KEY_DAEMON_ADDRESS("daemon-address");
         std::string const COMMENT_DAEMON_ADDRESS("IPv4:port pair the daemon listens to");
          
         std::string const SECTION_TLS("tls");
         std::string const KEY_TLS_CACERT("ca-cert");
         std::string const COMMENT_TLS_CACERT("TLS CA certificate");
         std::string const KEY_TLS_CERT("cert");
         std::string const COMMENT_TLS_CERT("TLS client certificate");
         std::string const KEY_TLS_PRIVKEY("cert-privkey");
         std::string const COMMENT_TLS_PRIVKEY("TLS client private key");
          
         std::string const SECTION_LOGGING("logging");
         std::string const KEY_LOGGING_TYPE("type");
         std::string const COMMENT_LOGGING_TYPE("Type of logging mechanism used");
          
         std::string const VALUE_LOGGING_STDOUT("stdout");
         std::string const VALUE_LOGGING_FILE("file");
         std::string const VALUE_LOGGING_SYSLOG("syslog");

         std::string const KEY_LOGGING_FILE("filename");
         std::string const COMMENT_LOGGING_FILE("File used for logging, if file based logging is used");
          
         std::string const SECTION_MISC("misc");
         std::string const KEY_MISC_LEECHMODE("leech-mode");
         std::string const COMMENT_MISC_LEECHMODE("Leech mode switch");
         std::string const KEY_MISC_NEVERASK("never-ask");
         std::string const COMMENT_MISC_NEVERASK("Never ask the user any questions");
         std::string const VALUE_MISC_TRUE("true");
         std::string const VALUE_MISC_FALSE("false");
         std::string const KEY_MISC_USE_DHT("use-dht");
         std::string const COMMENT_MISC_USE_DHT("Switch used to enable/disable DHT");
         std::string const KEY_MISC_USE_ENCRYPTION("use-encryption");
         std::string const COMMENT_MISC_USE_ENCRYPTION("Switch used to enable/disable encryption");
         std::string const SECTION_AUTH("auth");
         std::string const KEY_AUTH_USER("username");
         std::string const COMMENT_AUTH_USER("Connect to the daemon as this user");
         std::string const KEY_AUTH_PASSWORDHASH("password-hash");
         std::string const COMMENT_AUTH_PASSWORDHASH("Hash of password used to connect to the daemon");
          
         std::string const writeOperation("write");

         using namespace btg::core;
         using namespace btg::core::logger;

         clientConfiguration::clientConfiguration(LogWrapperType _logwrapper,
                                                  std::string const& _filename)
            : Configuration(_logwrapper,
                            _filename),
              def_transport(messageTransport::UNDEFINED),
              def_daemonaddress(0,0,0,0,0),
              def_TLS_CA_cert(),
              def_TLS_cert(),
              def_TLS_privkey(),
              def_logType(logBuffer::UNDEF),
              def_logFilename("/non/existing"),
              def_leechmode(false),
              def_lastFiles(0),
              def_neverAskQuestions(false),
              def_username(),
              def_passwordHash(),
              def_auth_set(false),
              def_useDHT(false),
              def_useEncryption(false)
         {
         }

         bool clientConfiguration::read(bool const _force)
         {
            bool status = Configuration::read(_force);

            if (status)
               {
                  /* Default transport */

                  std::string transportStr = inifile->GetValue(KEY_TRANSPORT_DEFAULT, SECTION_TRANSPORT);

                  if (transportStr.size() > 0)
                     {
                        if (transportStr == VALUE_TRANSPORT_TCPIP)
                           {
                              def_transport = messageTransport::TCP;
                           }
                        else if (transportStr == VALUE_TRANSPORT_STCPIP)
                           {
                              def_transport = messageTransport::STCP;
                           }
                        else if (transportStr == VALUE_TRANSPORT_XMLRPC)
                           {
                              def_transport = messageTransport::XMLRPC;
                           }
                        else if (transportStr == VALUE_TRANSPORT_SXMLRPC)
                           {
                              def_transport = messageTransport::SXMLRPC;
                           }
                        else
                           {
                              def_transport = messageTransport::UNDEFINED;
                           }
                     }

                  /* Network */
                  std::string daemonaddressStr = inifile->GetValue(KEY_DAEMON_ADDRESS, SECTION_NETWORK);
                  if (daemonaddressStr.size() > 0)
                     {
                        def_daemonaddress.fromString(daemonaddressStr);
                     }

                  /* TLS */
                  std::string TLS_temp;

                  TLS_temp = inifile->GetValue(KEY_TLS_CACERT, SECTION_TLS);
                  if (TLS_temp.size() > 0)
                     {
                        def_TLS_CA_cert = TLS_temp;
                     }
                  TLS_temp.clear();

                  TLS_temp = inifile->GetValue(KEY_TLS_CERT, SECTION_TLS);
                  if (TLS_temp.size() > 0)
                     {
                        def_TLS_cert = TLS_temp;
                     }
                  TLS_temp.clear();

                  TLS_temp = inifile->GetValue(KEY_TLS_PRIVKEY, SECTION_TLS);
                  if (TLS_temp.size() > 0)
                     {
                        def_TLS_privkey = TLS_temp;
                     }
                  TLS_temp.clear();

                  /* Log */
                  std::string loggerStr = inifile->GetValue(KEY_LOGGING_TYPE, SECTION_LOGGING);

                  if (loggerStr.size() > 0)
                     {
                        if (loggerStr == VALUE_LOGGING_STDOUT)
                           {
                              def_logType = logBuffer::STDOUT;
                           }
                        else if (loggerStr == VALUE_LOGGING_FILE)
                           {
                              def_logType = logBuffer::LOGFILE;
                           }
                        else if (loggerStr == VALUE_LOGGING_SYSLOG)
                           {
                              def_logType = logBuffer::SYSLOG;
                           }
                        else
                           {
                              def_logType = logBuffer::UNDEF;
                           }
                     }

                  if (def_logType == logBuffer::LOGFILE)
                     {
                        std::string loggerStr = inifile->GetValue(KEY_LOGGING_FILE, SECTION_LOGGING);

                        if (loggerStr.size() > 0)
                           {
                              def_logFilename = loggerStr;
                           }
                     }

                  /* Leech mode. */
                  readSwitch(KEY_MISC_LEECHMODE,
                             SECTION_MISC,
                             VALUE_MISC_TRUE,
                             VALUE_MISC_FALSE,
                             def_leechmode);
                  
                  /* Never ask questions. */
                  readSwitch(KEY_MISC_NEVERASK,
                             SECTION_MISC,
                             VALUE_MISC_TRUE,
                             VALUE_MISC_FALSE,
                             def_neverAskQuestions);
                  
                  readSwitch(KEY_MISC_USE_DHT,
                             SECTION_MISC,
                             VALUE_MISC_TRUE,
                             VALUE_MISC_FALSE,
                             def_useDHT);
                  
                  readSwitch(KEY_MISC_USE_ENCRYPTION,
                             SECTION_MISC,
                             VALUE_MISC_TRUE,
                             VALUE_MISC_FALSE,
                             def_useEncryption);
                  
                  /* Auth */

                  def_auth_set         = false;

                  bool gotUser         = false;
                  bool gotPasswordHash = false;

                  std::string userStr = inifile->GetValue(KEY_AUTH_USER, SECTION_AUTH);
                  if (userStr.size() > 0)
                     {
                        def_username = userStr;
                        gotUser      = true;
                     }

                  std::string passwordHashStr = inifile->GetValue(KEY_AUTH_PASSWORDHASH, SECTION_AUTH);
                  if (passwordHashStr.size() > 0)
                     {
                        def_passwordHash = passwordHashStr;
                        gotPasswordHash  = true;
                     }

                  if (gotUser && gotPasswordHash)
                     {
#if BTG_AUTH_DEBUG
		       BTG_NOTICE(logWrapper(), "Got auth info from configuration file.");
#endif // BTG_AUTH_DEBUG
                        def_auth_set = true;
                     }
#if BTG_AUTH_DEBUG
                  else
                     {
                        BTG_NOTICE(logWrapper(), "No auth info in configuration file, gotUser = " << gotUser << ", gotPasswordHash = " << gotPasswordHash);
                     }
#endif // BTG_AUTH_DEBUG
               } // status ok

            return status;
         }

         bool clientConfiguration::write(bool const _force)
         {
            bool status = true;

            /* Default transport */
            std::string transportStr = "";

            switch (def_transport)
               {
               case messageTransport::TCP:
                  transportStr = VALUE_TRANSPORT_TCPIP;
                  break;
               case messageTransport::STCP:
                  transportStr == VALUE_TRANSPORT_STCPIP;
                  break;
               case messageTransport::XMLRPC:
                  transportStr = VALUE_TRANSPORT_XMLRPC;
                  break;
               case messageTransport::SXMLRPC:
                  transportStr = VALUE_TRANSPORT_SXMLRPC;
                  break;
               default:
                  BTG_NOTICE(logWrapper(), "No default transport set");
               }

            if (transportStr.size() > 0)
               {
                  if (!inifile->SetValue(KEY_TRANSPORT_DEFAULT,
                                         transportStr,
                                         COMMENT_TRANSPORT_DEFAULT,
                                         SECTION_TRANSPORT))
                     {
                        setErrorDescription(writeOperation, SECTION_TRANSPORT, KEY_TRANSPORT_DEFAULT);
                        status = false;
                     }
               }

            /* Network */
            /* Listen to */
            if (!inifile->SetValue(KEY_DAEMON_ADDRESS,
                                   def_daemonaddress.toString(),
                                   COMMENT_DAEMON_ADDRESS,
                                   SECTION_NETWORK))
               {
                  setErrorDescription(writeOperation, SECTION_NETWORK, KEY_DAEMON_ADDRESS);
                  status = false;
               }

            /* TLS */

            if (def_TLS_CA_cert.size() > 0)
               {
                  if (!inifile->SetValue(KEY_TLS_CACERT,
                                         def_TLS_CA_cert,
                                         COMMENT_TLS_CACERT,
                                         SECTION_TLS))
                     {
                        setErrorDescription(writeOperation, SECTION_TLS, KEY_TLS_CACERT);
                        status = false;
                     }
               }

            if (def_TLS_cert.size() > 0)
               {
                  if (!inifile->SetValue(KEY_TLS_CERT,
                                         def_TLS_cert,
                                         COMMENT_TLS_CERT,
                                         SECTION_TLS))
                     {
                        setErrorDescription(writeOperation, SECTION_TLS, KEY_TLS_CERT);
                        status = false;
                     }
               }

            if (def_TLS_privkey.size() > 0)
               {
                  if (!inifile->SetValue(KEY_TLS_PRIVKEY,
                                         def_TLS_privkey,
                                         COMMENT_TLS_PRIVKEY,
                                         SECTION_TLS))
                     {
                        setErrorDescription(writeOperation, SECTION_TLS, KEY_TLS_PRIVKEY);
                        status = false;
                     }
               }

            /* Log */
            std::string loggerStr = "";
            bool loggerFileSet    = false;

            switch(def_logType)
               {
               case logBuffer::STDOUT:
                  loggerStr = VALUE_LOGGING_STDOUT;
                  break;
               case logBuffer::LOGFILE:
                  loggerStr     = VALUE_LOGGING_FILE;
                  loggerFileSet = true;
                  break;
               case logBuffer::SYSLOG:
                  loggerStr     = VALUE_LOGGING_SYSLOG;
                  break;
               default:
                  BTG_NOTICE(logWrapper(), "No default log method set");
               }

            if (def_logFilename.size() > 0)
               {
                  if (!inifile->SetValue(KEY_LOGGING_TYPE,
                                         loggerStr,
                                         COMMENT_LOGGING_TYPE,
                                         SECTION_LOGGING))
                     {
                        setErrorDescription(writeOperation, SECTION_LOGGING, KEY_LOGGING_TYPE);
                        status = false;
                     }
               }

            if (loggerFileSet && status)
               {
                  if (!inifile->SetValue(KEY_LOGGING_FILE,
                                         def_logFilename,
                                         COMMENT_LOGGING_FILE,
                                         SECTION_LOGGING))
                     {
                        setErrorDescription(writeOperation, SECTION_LOGGING, KEY_LOGGING_FILE);
                        status = false;
                     }

               }

            /* Misc */

            /* Leech mode. */

            if (!writeSwitch(KEY_MISC_LEECHMODE,
                             SECTION_MISC,
                             COMMENT_MISC_LEECHMODE,
                             VALUE_MISC_TRUE,
                             VALUE_MISC_FALSE,
                             def_leechmode))
            {
                status = false;
            }

            /* Never ask questions. */
            if (!writeSwitch(KEY_MISC_NEVERASK,
                             SECTION_MISC,
                             COMMENT_MISC_NEVERASK,
                             VALUE_MISC_TRUE,
                             VALUE_MISC_FALSE,
                             def_neverAskQuestions))
            {
                status = false;
            }

            /* Use DHT. */
            if (!writeSwitch(KEY_MISC_USE_DHT,
                             SECTION_MISC,
                             COMMENT_MISC_USE_DHT,
                             VALUE_MISC_TRUE,
                             VALUE_MISC_FALSE,
                             def_useDHT))
            {
                status = false;
            }

            /* Encryption */
            if (!writeSwitch(KEY_MISC_USE_ENCRYPTION,
                             SECTION_MISC,
                             COMMENT_MISC_USE_ENCRYPTION,
                             VALUE_MISC_TRUE,
                             VALUE_MISC_FALSE,
                             def_useEncryption))
            {
                status = false;
            }
            
            /* Auth */
            if (def_username.size() > 0)
               {
                  if (!inifile->SetValue(KEY_AUTH_USER,
                                         def_username,
                                         COMMENT_AUTH_USER,
                                         SECTION_AUTH))
                     {
                        setErrorDescription(writeOperation, SECTION_AUTH, KEY_AUTH_USER);
                        status = false;
                     }
               }

            if (def_passwordHash.size() > 0)
               {
                  if (!inifile->SetValue(KEY_AUTH_PASSWORDHASH,
                                         def_passwordHash,
                                         COMMENT_AUTH_PASSWORDHASH,
                                         SECTION_AUTH))
                     {
                        setErrorDescription(writeOperation, SECTION_AUTH, KEY_AUTH_PASSWORDHASH);
                        status = false;
                     }
               }

            if (status)
               {
                  status = Configuration::write(_force);
               }

            return status;
         }

         std::string clientConfiguration::getSyntax() const
         {
            std::string output;

            formatHeader(output);

            std::string const SECTION_AUTH("auth");

            formatSection(SECTION_TRANSPORT, output);

            std::vector<std::string> temp;

            temp.push_back(std::string(VALUE_TRANSPORT_TCPIP));
            temp.push_back(std::string(VALUE_TRANSPORT_STCPIP));
            temp.push_back(std::string(VALUE_TRANSPORT_XMLRPC));
            temp.push_back(std::string(VALUE_TRANSPORT_SXMLRPC));

            formatKey(KEY_TRANSPORT_DEFAULT,
                      COMMENT_TRANSPORT_DEFAULT,
                      temp,
                      output);

            /* */

            temp.clear();

            formatSection(SECTION_NETWORK, output);

            temp.push_back(std::string("IPv4 address:port"));

            formatKey(KEY_DAEMON_ADDRESS,
                      COMMENT_DAEMON_ADDRESS,
                      temp,
                      output);

            /* */

            formatSection(SECTION_TLS, output);

            temp.clear();
            temp.push_back(std::string("filename"));

            formatKey(KEY_TLS_CACERT,
                      COMMENT_TLS_CACERT,
                      temp,
                      output);

            temp.clear();
            temp.push_back(std::string("filename"));

            formatKey(KEY_TLS_CERT,
                      COMMENT_TLS_CERT,
                      temp,
                      output);

            temp.clear();
            temp.push_back(std::string("filename"));

            formatKey(KEY_TLS_PRIVKEY,
                      COMMENT_TLS_PRIVKEY,
                      temp,
                      output);

            /* */

            formatSection(SECTION_LOGGING, output);

            temp.clear();
            temp.push_back(std::string(VALUE_LOGGING_STDOUT));
            temp.push_back(std::string(VALUE_LOGGING_FILE));
            temp.push_back(std::string(VALUE_LOGGING_SYSLOG));
            temp.push_back(std::string());
            temp.push_back(std::string());

            formatKey(KEY_LOGGING_TYPE,
                      COMMENT_LOGGING_TYPE,
                      temp,
                      output);

            temp.clear();
            temp.push_back(std::string("filename"));

            formatKey(KEY_LOGGING_FILE,
                      COMMENT_LOGGING_FILE,
                      temp,
                      output);

            /* */

            formatSection(SECTION_MISC, output);

            temp.clear();

            temp.push_back(std::string(VALUE_MISC_TRUE));
            temp.push_back(std::string(VALUE_MISC_FALSE));

            formatKey(KEY_MISC_LEECHMODE,
                      COMMENT_MISC_LEECHMODE,
                      temp,
                      output);

            temp.clear();

            temp.push_back(std::string(VALUE_MISC_TRUE));
            temp.push_back(std::string(VALUE_MISC_FALSE));

            formatKey(KEY_MISC_NEVERASK,
                      COMMENT_MISC_NEVERASK,
                      temp,
                      output);

            temp.clear();

            temp.push_back(std::string(VALUE_MISC_TRUE));
            temp.push_back(std::string(VALUE_MISC_FALSE));

            formatKey(KEY_MISC_USE_DHT,
                      COMMENT_MISC_USE_DHT,
                      temp,
                      output);

            temp.clear();

            temp.push_back(std::string(VALUE_MISC_TRUE));
            temp.push_back(std::string(VALUE_MISC_FALSE));

            formatKey(KEY_MISC_USE_ENCRYPTION,
                      COMMENT_MISC_USE_ENCRYPTION,
                      temp,
                      output);
            
            /* */

            formatSection(SECTION_AUTH, output);

            temp.clear();
            temp.push_back("user name");

            formatKey(KEY_AUTH_USER,
                      COMMENT_AUTH_USER,
                      temp,
                      output);

            temp.clear();
            temp.push_back("MD5 hash");

            formatKey(KEY_AUTH_PASSWORDHASH,
                      COMMENT_AUTH_PASSWORDHASH,
                      temp,
                      output);

            return output;
         }

         void clientConfiguration::setTransport(btg::core::messageTransport::TRANSPORT const _transport)
         {
            def_transport = _transport;
            data_modified = true;
         }

         void clientConfiguration::setDaemonAddressPort(btg::core::addressPort const& _addrport)
         {
            def_daemonaddress = _addrport;
            data_modified = true;
         }

         void clientConfiguration::setLog(btg::core::logger::logBuffer::LOGGER_TYPE const _logType)
         {
            def_logType   = _logType;
            data_modified = true;
         }

         void clientConfiguration::setLogFile(std::string const& _filename)
         {
            def_logFilename = _filename;
            data_modified   = true;
         }

         void clientConfiguration::setLeechMode(bool const _mode)
         {
            def_leechmode = _mode;
         }

         btg::core::messageTransport::TRANSPORT clientConfiguration::getTransport() const
         {
            return def_transport;
         }

         btg::core::addressPort clientConfiguration::getDaemonAddressPort() const
         {
            return def_daemonaddress;
         }

         btg::core::logger::logBuffer::LOGGER_TYPE clientConfiguration::getLog() const
         {
            return def_logType;
         }

         void clientConfiguration::setCACert(std::string const& _caCert)
         {
            def_TLS_CA_cert = _caCert;
         }

         std::string clientConfiguration::getCACert() const
         {
            return def_TLS_CA_cert;
         }

         void clientConfiguration::setCert(std::string const& _cert)
         {
            def_TLS_cert = _cert;
         }

         std::string clientConfiguration::getCert() const
         {
            return def_TLS_cert;
         }

         void clientConfiguration::setPrivKey(std::string const& _key)
         {
            def_TLS_privkey = _key;
         }

         std::string clientConfiguration::getPrivKey() const
         {
            return def_TLS_privkey;
         }

         std::string clientConfiguration::getLogFile() const
         {
            return def_logFilename;
         }

         bool clientConfiguration::getLeechMode() const
         {
            return def_leechmode;
         }

         std::string clientConfiguration::getUserName() const
         {
            return def_username;
         }

         void clientConfiguration::setUserName(std::string const& _username)
         {
            def_username = _username;
         }

         std::string clientConfiguration::getPasswordHash() const
         {
            return def_passwordHash;
         }

         void clientConfiguration::setPasswordHash(std::string const& _passwordHash)
         {
            def_passwordHash = _passwordHash;
         }

         bool clientConfiguration::authSet() const
         {
            return def_auth_set;
         }

         void clientConfiguration::setUseDHT(bool const _useDHT)
         {
            def_useDHT = _useDHT;
         }

         bool clientConfiguration::getUseDHT() const
         {
            return def_useDHT;
         }

          void clientConfiguration::setUseEncryption(bool const _useEncryption)
          {
              def_useEncryption = _useEncryption;
          }

          bool clientConfiguration::getUseEncryption() const
          {
              return def_useEncryption;
          }

          void clientConfiguration::setNeverAskQuestions(bool const _mode)
         {
            def_neverAskQuestions = _mode;
         }

         bool clientConfiguration::getNeverAskQuestions() const
         {
            return def_neverAskQuestions;
         }

         bool clientConfiguration::getErrorDescription(std::string & _errordescription) const
         {
            return Configuration::getErrorDescription(_errordescription);
         }

         clientConfiguration::~clientConfiguration()
         {

         }

      } // namespace client
   } // namespace core
} // namespace btg


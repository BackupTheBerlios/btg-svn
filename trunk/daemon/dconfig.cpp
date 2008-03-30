#include "dconfig.h"

#include <bcore/t_string.h>
#include <bcore/util.h>

#include <bcore/command/limit_base.h>

namespace btg
{
   namespace daemon
   {
      using namespace btg::core;

      std::string const SECTION_TRANSPORT("transport");
      std::string const KEY_TRANSPORT_DEFAULT("default");
      std::string const DESCR_TRANSPORT_DEFAULT("default transport");

      std::string const VALUE_TRANSPORT_TCPIP("tcpip");
      std::string const VALUE_TRANSPORT_STCPIP("stcpip");
      std::string const VALUE_TRANSPORT_XMLRPC("xml-rpc");
      std::string const VALUE_TRANSPORT_SXMLRPC("sxml-rpc");

      std::string const SECTION_NETWORK("network");
      std::string const KEY_LISTEN_TO("listen-to");
      std::string const DESCR_LISTEN_TO("listen to IPv4:port");

      std::string const SECTION_ENC("encryption");

      std::string const KEY_POLICY_IN("policy-in");
      std::string const DESCR_POLICY_IN("input policy");

      std::string const KEY_POLICY_OUT("policy-out");
      std::string const DESCR_POLICY_OUT("output policy");

      std::string const KEY_LEVEL("level");
      std::string const DESCR_LEVEL("encryption level");

      std::string const KEY_PREFER_RC4("prefer-rc4");
      std::string const DESCR_PREFER_RC4("flag, indicates if rc4 is prefered");

      std::string const VALUE_POLICY_PLAIN("plaintext");
      std::string const VALUE_POLICY_RC4("rc4");
      std::string const VALUE_POLICY_BOTH("both");

      std::string const VALUE_LEVEL_FORCE("force");
      std::string const VALUE_LEVEL_ENABLE("enable");
      std::string const VALUE_LEVEL_DISABLE("disable");

      std::string const KEY_LT_PORTRANGE("torrent-range");
      std::string const DESCR_LT_PORTRANGE("libtorrent port range used");

      std::string const KEY_LT_IP("torrent-ip");
      std::string const DESCR_LT_IP("libtorrent listens to IPv4");

      std::string const KEY_USE_UPNP("use-upnp");
      std::string const DESCR_USE_UPNP("enable/disable UPnP");

      std::string const SECTION_TLS("tls");

      std::string const KEY_TLS_CACERT("ca-cert");
      std::string const DESCR_TLS_CACERT("certificate authority certificate");

      std::string const KEY_TLS_CERT("cert");
      std::string const DESCR_TLS_CERT("daemon certificate");
      std::string const KEY_TLS_PRIVKEY("cert-privkey");

      std::string const SECTION_LOGGING("logging");
      std::string const DESCR_LOGGING_TYPE("log type");
      std::string const KEY_LOGGING_TYPE("type");

      std::string const VALUE_LOGGING_STDOUT("stdout");
      std::string const VALUE_LOGGING_FILE("file");
      std::string const VALUE_LOGGING_SYSLOG("syslog");

      std::string const KEY_LOGGING_FILE("filename");
      std::string const DESCR_LOGGING_FILE("log filename");

      std::string const SECTION_MISC("misc");
      std::string const KEY_MISC_PROXY("proxy");
      std::string const DESCR_MISC_PROXY("proxy IPv4:port");

      std::string const KEY_MISC_TORRENTNAME("use-torrent-name");
      std::string const DESCR_MISC_TORRENTNAME("enable using torrent name instead of file name");

      std::string const KEY_MISC_FILTERFILE("filter");
      std::string const DESCR_MISC_FILTERFILE("file used as filter");

      std::string const KEY_MISC_FILTERTYPE("filter_type");
      std::string const DESCR_MISC_FILTERTYPE("Type of filter");
      std::string const VALUE_FILTERTYPE_LEVEL1("level1");
      std::string const VALUE_FILTERTYPE_EMULE("emule");

      std::string const KEY_MISC_PEERID("peer-id");
      std::string const DESCR_MISC_PEERID("Peer ID");

      std::string const KEY_MISC_USERAGENT("user-agent");
      std::string const DESCR_MISC_USERAGENT("user agent string");

      std::string const KEY_MISC_PIDFILE("pidfile");
      std::string const DESCR_MISC_PIDFILE("PID-file");
      
      std::string const SECTION_LIMIT("limit");
      std::string const KEY_LIMIT_UPLOAD_RATE_LIMIT("upload_rate_limit");
      std::string const KEY_LIMIT_DOWNLOAD_RATE_LIMIT("download_rate_limit");
      std::string const KEY_LIMIT_MAX_UPLOADS("max_uploads");
      std::string const KEY_LIMIT_MAX_CONNECTIONS("max_connections");

      std::string const SECTION_SS("savesessions");
      std::string const KEY_SS_FILENAME("filename");
      std::string const KEY_SS_ENABLE("enable");
      std::string const KEY_SS_TIMEOUT("timeout");

      std::string const VALUE_ENABLE_TRUE("true");
      std::string const VALUE_ENABLE_YES("yes");
      std::string const VALUE_ENABLE_ONE("1");

      std::string const VALUE_ENABLE_FALSE("false");
      std::string const VALUE_ENABLE_NO("no");
      std::string const VALUE_ENABLE_ZERO("0");

      std::string const SECTION_AUTH("auth");
      std::string const KEY_AUTH_PASSWDFILE("passwd-file");

      std::string const SECTION_RUNAS("runas");
      std::string const KEY_RUNAS_USER("user");
      std::string const KEY_RUNAS_GROUP("group");

      std::string const writeOperation("write");

      using namespace btg::core;
      using namespace btg::core::logger;

      daemonConfiguration::daemonConfiguration(btg::core::LogWrapperType _logwrapper,
                                               std::string const& _filename)
         : Configuration(_logwrapper, _filename),
           def_transport(messageTransport::UNDEFINED),
           def_listenTo(0,0,0,0,0),
           def_TLS_CA_cert(),
           def_TLS_cert(),
           def_TLS_privkey(),
           def_logType(logBuffer::UNDEF),
           def_logFilename("/var/log/btgdaemon.log"),
           def_proxy(0,0,0,0,0),
           def_filterFilename("/non/existing"),
           def_filterType(IpFilterIf::IPF_UNDEF),
           def_port_range(0,0),
           def_lt_ip(0,0,0,0),
           def_use_upnp(false),
           def_upload_rate_limit(limitBase::LIMIT_DISABLED),
           def_download_rate_limit(limitBase::LIMIT_DISABLED),
           def_max_uploads(limitBase::LIMIT_DISABLED),
           def_max_connections(limitBase::LIMIT_DISABLED),
           def_ss_enable(false),
           def_ss_filename("/ver/lib/btg/sessions"),
           def_ss_timeout(60),
           def_authFile("/var/lib/btg/auth"),
           def_runAsUser(),
           def_runAsGroup(),
           def_use_torrent_name(false),
           def_out_enc_policy(daemonConfiguration::disabled),
           def_in_enc_policy(daemonConfiguration::disabled),
           def_enc_level(daemonConfiguration::plaintext),
           def_prefer_rc4(false),
           def_peerId(""),
           def_userAgent("")
      {
      }

      bool daemonConfiguration::read()
      {
         bool status = Configuration::read();

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
               std::string listenToStr= inifile->GetValue(KEY_LISTEN_TO, SECTION_NETWORK);
               if (listenToStr.size() > 0)
                  {
                     def_listenTo.fromString(listenToStr);
                  }

               std::string portRangeStr = inifile->GetValue(KEY_LT_PORTRANGE, SECTION_NETWORK);
               t_strList lst = Util::splitLine(portRangeStr, ":");
               if (lst.size() == 2)
                  {
                     t_int port0 = 0;
                     t_int port1 = 0;

                     btg::core::stringToNumber<t_int>(lst[0], port0);
                     btg::core::stringToNumber<t_int>(lst[1], port1);

                     def_port_range.first  = port0;
                     def_port_range.second = port1;
                  }

               std::string lt_ip = inifile->GetValue(KEY_LT_IP, SECTION_NETWORK);
               if (lt_ip.size() > 0)
                  {
                     def_lt_ip.fromString(lt_ip);
                  }

               /* Encryption */
               readEncryption();

               readFlag(KEY_USE_UPNP, SECTION_NETWORK, def_use_upnp);

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
                           btg::core::os::fileOperation::resolvePath(def_logFilename);
                        }
                  }

               /* Misc */
               /* Proxy */
               std::string miscStr;
               miscStr = inifile->GetValue(KEY_MISC_PROXY, SECTION_MISC);
               if (miscStr.size() > 0)
                  {
                     def_proxy.fromString(miscStr);
                  }
               /* PID-file */
               miscStr = inifile->GetValue(KEY_MISC_PIDFILE, SECTION_MISC);
               if (miscStr.size() > 0)
                  {
                     def_pidfname = miscStr;
                     btg::core::os::fileOperation::resolvePath(def_pidfname);
                  }

               /* Use torrent name instead of file names. */
               readFlag(KEY_MISC_TORRENTNAME, SECTION_MISC, def_use_torrent_name);

               /* Filter file */
               def_filterFilename = inifile->GetValue(KEY_MISC_FILTERFILE, SECTION_MISC);

               /* Filter type */
               std::string filterStr = inifile->GetValue(KEY_MISC_FILTERTYPE, SECTION_MISC);

               if (filterStr.size() > 0)
                  {
                     if (filterStr == VALUE_FILTERTYPE_LEVEL1)
                        {
                           def_filterType = IpFilterIf::IPF_LEVELONE;
                        }
                     else if (filterStr == VALUE_FILTERTYPE_EMULE)
                        {
                           def_filterType = IpFilterIf::IPF_EMULE;
                        }
                  }

               readPeerIdAndUserAgent();

               /* Limit */
               std::string s_limit;
               t_int i_limit;

               /* Upload Rate Limit */
               s_limit = inifile->GetValue(KEY_LIMIT_UPLOAD_RATE_LIMIT, SECTION_LIMIT);
               if (s_limit.size() > 0)
                  {
                     if (btg::core::stringToNumber<t_int>(s_limit, i_limit))
                        {
                           if (i_limit < 0)
                              {
                                 def_upload_rate_limit = limitBase::LIMIT_DISABLED;
                              }
                           else
                              {
                                 def_upload_rate_limit = i_limit;
                              }
                        }
                     else
                        {
                           BTG_NOTICE(logWrapper(), "Bad configuration value for " << KEY_LIMIT_UPLOAD_RATE_LIMIT);
                        }
                  }

               /* Download Rate Limit */
               s_limit = inifile->GetValue(KEY_LIMIT_DOWNLOAD_RATE_LIMIT, SECTION_LIMIT);
               if (s_limit.size() > 0)
                  {
                     if (btg::core::stringToNumber<t_int>(s_limit, i_limit))
                        {
                           if (i_limit < 0)
                              {
                                 def_download_rate_limit = limitBase::LIMIT_DISABLED;
                              }
                           else
                              {
                                 def_download_rate_limit = i_limit;
                              }
                        }
                     else
                        {
                           BTG_NOTICE(logWrapper(), "Bad configuration value for " << KEY_LIMIT_DOWNLOAD_RATE_LIMIT);
                        }
                  }

               /* Max Uploads */
               s_limit = inifile->GetValue(KEY_LIMIT_MAX_UPLOADS, SECTION_LIMIT);
               if (s_limit.size() > 0)
                  {
                     if (btg::core::stringToNumber<t_int>(s_limit, i_limit))
                        {
                           if(i_limit < 0)
                              {
                                 def_max_uploads = limitBase::LIMIT_DISABLED;
                              }
                           else
                              {
                                 def_max_uploads = i_limit;
                              }
                        }
                     else
                        {
                           BTG_NOTICE(logWrapper(), "Bad configuration value for " << KEY_LIMIT_MAX_UPLOADS);
                        }
                  }

               /* Max Connections */
               s_limit = inifile->GetValue(KEY_LIMIT_MAX_CONNECTIONS, SECTION_LIMIT);
               if (s_limit.size() > 0)
                  {
                     if (btg::core::stringToNumber<t_int>(s_limit, i_limit))
                        {
                           if(i_limit < 0)
                              {
                                 def_max_connections = limitBase::LIMIT_DISABLED;
                              }
                           else
                              {
                                 def_max_connections = i_limit;
                              }
                        }
                     else
                        {
                           BTG_NOTICE(logWrapper(), "Bad configuration value for " << KEY_LIMIT_MAX_CONNECTIONS);
                        }
                  }
               /* Session Saving */

               readFlag(KEY_SS_ENABLE, SECTION_SS, def_ss_enable);

               if (def_ss_enable)
                  {
                     std::string ss_filename = inifile->GetValue(KEY_SS_FILENAME, SECTION_SS);
                     if (ss_filename.size() > 0)
                        {
                           def_ss_filename = ss_filename;
                        }

                     // reuse ss_filename
                     ss_filename = inifile->GetValue(KEY_SS_TIMEOUT, SECTION_SS);
                     t_int ss_timeout;
                     if (ss_filename.size() > 0)
                        {
                           if(btg::core::stringToNumber<t_int>(ss_filename, ss_timeout))
                              {
                                 def_ss_timeout = ss_timeout;
                              }
                        }
                  }

               /* Auth */
               def_authFile = inifile->GetValue(KEY_AUTH_PASSWDFILE, SECTION_AUTH);

               /* Run as */
               def_runAsUser  = inifile->GetValue(KEY_RUNAS_USER, SECTION_RUNAS);
               def_runAsGroup = inifile->GetValue(KEY_RUNAS_GROUP, SECTION_RUNAS);

            } // status ok

         return status;
      }

      bool daemonConfiguration::write(bool const _force)
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
               transportStr = VALUE_TRANSPORT_STCPIP;
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
                                      DESCR_TRANSPORT_DEFAULT,
                                      SECTION_TRANSPORT))
                  {
                     setErrorDescription(writeOperation, SECTION_TRANSPORT, KEY_TRANSPORT_DEFAULT);
                     status = false;
                  }
            }

         /* Network */
         /* Listen to */
         if (!inifile->SetValue(KEY_LISTEN_TO,
                                def_listenTo.toString(),
                                DESCR_LISTEN_TO,
                                SECTION_NETWORK))
            {
               setErrorDescription(writeOperation, SECTION_NETWORK, KEY_LISTEN_TO);
               status = false;
            }

         /* Network */
         /* Port range. */
         std::string portRangeStr;
         portRangeStr += convertToString<t_int>(def_port_range.first);
         portRangeStr += ":";
         portRangeStr += convertToString<t_int>(def_port_range.second);

         if (!inifile->SetValue(KEY_LT_PORTRANGE,
                                portRangeStr,
                                DESCR_LT_PORTRANGE,
                                SECTION_NETWORK))
            {
               setErrorDescription(writeOperation, SECTION_NETWORK, KEY_LT_PORTRANGE);
               status = false;
            }

         /* Address. */

         if (!inifile->SetValue(KEY_LT_IP,
                                def_lt_ip.toString(),
                                DESCR_LT_IP,
                                SECTION_NETWORK))
            {
               setErrorDescription(writeOperation, SECTION_NETWORK, KEY_LT_IP);
               status = false;
            }

         if (!writeEncryption())
            {
               status = false;
            }

         if (!writeFlag(KEY_USE_UPNP, SECTION_NETWORK, DESCR_USE_UPNP, def_use_upnp))
            {
               status = false;
            }

         /* TLS */
         if (def_TLS_CA_cert.size() > 0)
            {
               if (!inifile->SetValue(KEY_TLS_CACERT,
                                      def_TLS_CA_cert,
                                      DESCR_TLS_CACERT,
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
                                      DESCR_TLS_CERT,
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
                                      "daemon private key",
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
                                      DESCR_LOGGING_TYPE,
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
                                      DESCR_LOGGING_FILE,
                                      SECTION_LOGGING))
                  {
                     setErrorDescription(writeOperation, SECTION_LOGGING, KEY_LOGGING_FILE);
                     status = false;
                  }

            }

         /* Misc */
         /* Proxy */

         if (!inifile->SetValue(KEY_MISC_PROXY,
                                def_proxy.toString(),
                                DESCR_MISC_PROXY,
                                SECTION_MISC))
            {
               setErrorDescription(writeOperation, SECTION_MISC, KEY_MISC_PROXY);
               status = false;
            }

         /* Use torrent name instead of file names. */
         if (!writeFlag(KEY_MISC_TORRENTNAME,
                        SECTION_MISC,
                        DESCR_MISC_TORRENTNAME,
                        def_use_torrent_name))
            {
               status = false;
            }

         /* Filter filename */
         if (def_filterFilename.size() > 0)
            {
               if (!inifile->SetValue(KEY_MISC_FILTERFILE,
                                      def_filterFilename,
                                      DESCR_MISC_FILTERFILE,
                                      SECTION_MISC))
                  {
                     setErrorDescription(writeOperation, SECTION_MISC, KEY_MISC_FILTERFILE);
                     status = false;
                  }
            }

         /* Filter type. */

         std::string filterStr = "";

         switch (def_filterType)
            {
            case IpFilterIf::IPF_LEVELONE:
               filterStr = VALUE_FILTERTYPE_LEVEL1;
               break;
            case IpFilterIf::IPF_EMULE:
               filterStr = VALUE_FILTERTYPE_EMULE;
               break;
            case IpFilterIf::IPF_UNDEF:
               // Do not write anything.
               break;
            }

         if (filterStr.size() > 0)
            {
               if (!inifile->SetValue(KEY_MISC_FILTERTYPE,
                                      filterStr,
                                      DESCR_MISC_FILTERTYPE,
                                      SECTION_MISC))
                  {
                     setErrorDescription(writeOperation, SECTION_MISC, KEY_MISC_FILTERTYPE);
                     status = false;
                  }
            }

         if (!writePeerIdAndUserAgent())
            {
               status = false;
            }

         /* Limit */
         std::string limit;
         /* Upload rate limit. */
         if (def_upload_rate_limit > 0)
            {
               limit = convertToString<t_int>(def_upload_rate_limit);

               if (!inifile->SetValue(KEY_LIMIT_UPLOAD_RATE_LIMIT,
                                      limit,
                                      "max upload rate (in bytes/s) for a session",
                                      SECTION_LIMIT))
                  {
                     setErrorDescription(writeOperation, SECTION_LIMIT, SECTION_LIMIT);
                     status = false;
                  }
            }

         if (def_download_rate_limit > 0)
            {
               /* Download rate limit. */
               limit = convertToString<t_int>(def_download_rate_limit);

               if (!inifile->SetValue(KEY_LIMIT_DOWNLOAD_RATE_LIMIT,
                                      limit,
                                      "max download rate (in bytes/s) for a session",
                                      SECTION_LIMIT))
                  {
                     setErrorDescription(writeOperation, SECTION_LIMIT, KEY_LIMIT_DOWNLOAD_RATE_LIMIT);
                     status = false;
                  }
            }

         if (def_max_uploads > 0)
            {
               /* Max uploads . */
               limit = convertToString<t_int>(def_max_uploads);

               if (!inifile->SetValue(KEY_LIMIT_MAX_UPLOADS,
                                      limit,
                                      "max number of uploads for a session",
                                      SECTION_LIMIT))
                  {
                     setErrorDescription(writeOperation, SECTION_LIMIT, KEY_LIMIT_MAX_UPLOADS);
                     status = false;
                  }
            }

         if (def_max_connections > 0)
            {
               /* Max connections . */
               limit = convertToString<t_int>(def_max_connections);

               if (!inifile->SetValue(KEY_LIMIT_MAX_CONNECTIONS,
                                      limit,
                                      "max connections for a session (minimum 2 per torrent will be used)",
                                      SECTION_LIMIT))
                  {
                     setErrorDescription(writeOperation, SECTION_LIMIT, KEY_LIMIT_MAX_CONNECTIONS);
                     status = false;
                  }
            }

         /* Session Saving */

         if (!writeFlag(KEY_SS_ENABLE,
                        SECTION_SS,
                        "file where saved session will be written to/retreived from",
                        def_ss_enable))
            {
               status = false;
            }

         if (def_ss_filename.size() > 0)
            {
               if (!inifile->SetValue(KEY_SS_FILENAME,
                                      def_ss_filename,
                                      "file where saved session will be written to/retreived from",
                                      SECTION_SS))
                  {
                     setErrorDescription(writeOperation, SECTION_SS, KEY_SS_FILENAME);
                     status = false;
                  }
               BTG_NOTICE(logWrapper(), "conf " << def_ss_timeout<<" to string");
               limit = convertToString<t_int>(def_ss_timeout);

               if (!inifile->SetValue(KEY_SS_TIMEOUT,
                                      limit,
                                      "how often sessions will be periodicly saved, in seconds",
                                      SECTION_SS))
                  {
                     setErrorDescription(writeOperation, SECTION_SS, KEY_SS_TIMEOUT);
                     status = false;
                  }
            }

         /* Auth */
         if (def_authFile.size() > 0)
            {
               if (!inifile->SetValue(KEY_AUTH_PASSWDFILE,
                                      def_authFile,
                                      "passwd file",
                                      SECTION_AUTH))
                  {
                     setErrorDescription(writeOperation, SECTION_AUTH, KEY_AUTH_PASSWDFILE);
                     status = false;
                  }
            }

         /* Run as */
         if (def_runAsUser.size() > 0)
            {
               if (!inifile->SetValue(KEY_RUNAS_USER,
                                      def_runAsUser,
                                      "user name",
                                      SECTION_RUNAS))
                  {
                     setErrorDescription(writeOperation, SECTION_RUNAS, SECTION_RUNAS);
                     status = false;
                  }
            }

         if (def_runAsGroup.size() > 0)
            {
               if (!inifile->SetValue(KEY_RUNAS_GROUP,
                                      def_runAsUser,
                                      "group name",
                                      SECTION_RUNAS))
                  {
                     setErrorDescription(writeOperation, SECTION_RUNAS, KEY_RUNAS_GROUP);
                     status = false;
                  }
            }

         if (status)
            {
               status = Configuration::write(_force);
            }

         return status;
      }

      std::string daemonConfiguration::getSyntax() const
      {
         std::string output;

         formatHeader(output);

         formatSection(SECTION_TRANSPORT, output);

         std::vector<std::string> temp;

         temp.push_back(std::string(VALUE_TRANSPORT_TCPIP));
         temp.push_back(std::string(VALUE_TRANSPORT_STCPIP));
         temp.push_back(std::string(VALUE_TRANSPORT_XMLRPC));
         temp.push_back(std::string(VALUE_TRANSPORT_SXMLRPC));

         formatKey(KEY_TRANSPORT_DEFAULT,
                   "Default transport",
                   temp,
                   output);

         /* */

         temp.clear();

         formatSection(SECTION_NETWORK, output);

         temp.push_back(std::string("IPv4 address:port"));

         formatKey(KEY_LISTEN_TO,
                   "Daemon listens to this address and port for incomming connections",
                   temp,
                   output);

         temp.clear();
         temp.push_back(std::string("port range, separated by :"));

         formatKey(KEY_LT_PORTRANGE,
                   "Libtorrent uses this port range",
                   temp,
                   output);

         temp.clear();
         temp.push_back(std::string("IPv4 address"));

         formatKey(KEY_LT_IP,
                   "Libtorrent listens to this IPv4 address",
                   temp,
                   output);

         temp.clear();
         temp.push_back(std::string(VALUE_ENABLE_TRUE));
         temp.push_back(std::string(VALUE_ENABLE_YES));
         temp.push_back(std::string(VALUE_ENABLE_ONE));
         temp.push_back(std::string(VALUE_ENABLE_FALSE));
         temp.push_back(std::string(VALUE_ENABLE_NO));
         temp.push_back(std::string(VALUE_ENABLE_ZERO));

         formatKey(KEY_USE_UPNP,
                   "Enable/disable UPnP",
                   temp,
                   output);

         /* */

         formatSection(SECTION_ENC, output);

         temp.clear();
         temp.push_back(std::string(VALUE_LEVEL_FORCE));
         temp.push_back(std::string(VALUE_LEVEL_ENABLE));
         temp.push_back(std::string(VALUE_LEVEL_DISABLE));

         formatKey(KEY_POLICY_IN,
                   DESCR_POLICY_IN,
                   temp,
                   output);

         formatKey(KEY_POLICY_OUT,
                   DESCR_POLICY_OUT,
                   temp,
                   output);

         temp.clear();
         temp.push_back(std::string(VALUE_POLICY_PLAIN));
         temp.push_back(std::string(VALUE_POLICY_RC4));
         temp.push_back(std::string(VALUE_POLICY_BOTH));

   
         formatKey(KEY_LEVEL,
                   DESCR_LEVEL,
                   temp,
                   output);
           

         temp.clear();
         temp.push_back(std::string(VALUE_ENABLE_TRUE));
         temp.push_back(std::string(VALUE_ENABLE_FALSE));

         formatKey(KEY_PREFER_RC4,
                   DESCR_PREFER_RC4,
                   temp,
                   output);

         /* */

         formatSection(SECTION_TLS, output);

         temp.clear();
         temp.push_back(std::string("filename"));

         formatKey(KEY_TLS_CACERT,
                   "TLS CA certificate",
                   temp,
                   output);

         temp.clear();
         temp.push_back(std::string("filename"));

         formatKey(KEY_TLS_CERT,
                   "TLS daemon certificate",
                   temp,
                   output);

         temp.clear();
         temp.push_back(std::string("filename"));

         formatKey(KEY_TLS_PRIVKEY,
                   "TLS daemon private key",
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
                   "Type of logging mechanism used",
                   temp,
                   output);

         temp.clear();
         temp.push_back(std::string("filename"));

         formatKey(KEY_LOGGING_FILE,
                   "File used for logging, if file based logging is used",
                   temp,
                   output);

         /* */

         formatSection(SECTION_MISC, output);

         temp.clear();
         temp.push_back(std::string("IPv4:port"));

         formatKey(KEY_MISC_PROXY,
                   "HTTP Proxy",
                   temp,
                   output);

         temp.clear();
         temp.push_back(std::string(VALUE_ENABLE_TRUE));
         temp.push_back(std::string(VALUE_ENABLE_FALSE));

         formatKey(KEY_MISC_TORRENTNAME,
                   "Enable - use torrent names; disable - use torrent file names",
                   temp,
                   output);

         /* Filter file. */

         temp.clear();
         temp.push_back(std::string("filename"));

         formatKey(KEY_MISC_FILTERFILE,
                   "File containing IPv4 filter",
                   temp,
                   output);

         /* Filter type. */

         temp.clear();
         temp.push_back(std::string(VALUE_FILTERTYPE_LEVEL1));
         temp.push_back(std::string(VALUE_FILTERTYPE_EMULE));

         formatKey(KEY_MISC_FILTERTYPE,
                   "File containing IPv4 filter",
                   temp,
                   output);

         /* Client ID. */
         temp.clear();
         temp.push_back(std::string("string"));

         formatKey(KEY_MISC_PEERID,
                   "String for use as peer ID",
                   temp,
                   output);

         /* User agent. */
         temp.clear();
         temp.push_back(std::string("string"));

         formatKey(KEY_MISC_USERAGENT,
                   "String for use as user agent",
                   temp,
                   output);

         temp.clear();
         temp.push_back(std::string("string"));

         formatKey(KEY_MISC_PIDFILE,
                   "PID-file name",
                   temp,
                   output);
         
         /* */

         formatSection(SECTION_LIMIT, output);

         temp.clear();
         temp.push_back("number");

         formatKey(KEY_LIMIT_UPLOAD_RATE_LIMIT,
                   "Limit the download to this amount, in kbit/s",
                   temp,
                   output);

         temp.clear();
         temp.push_back("number");

         formatKey(KEY_LIMIT_DOWNLOAD_RATE_LIMIT,
                   "Limit download to this amount, in kbit/s",
                   temp,
                   output);

         temp.clear();
         temp.push_back("number");

         formatKey(KEY_LIMIT_MAX_UPLOADS,
                   "Max number of uploads",
                   temp,
                   output);

         temp.clear();
         temp.push_back("number");

         formatKey(KEY_LIMIT_MAX_CONNECTIONS,
                   "Max number of connections",
                   temp,
                   output);

         /* */

         formatSection(SECTION_SS, output);

         temp.clear();
         temp.push_back("filename");

         formatKey(KEY_SS_FILENAME,
                   "File used to save sessions to",
                   temp,
                   output);

         temp.clear();
         temp.push_back(std::string(VALUE_ENABLE_TRUE));
         temp.push_back(std::string(VALUE_ENABLE_YES));
         temp.push_back(std::string(VALUE_ENABLE_ONE));
         temp.push_back(std::string(VALUE_ENABLE_FALSE));
         temp.push_back(std::string(VALUE_ENABLE_NO));
         temp.push_back(std::string(VALUE_ENABLE_ZERO));

         formatKey(KEY_SS_ENABLE,
                   "Enables/disables session saving",
                   temp,
                   output);

         temp.clear();
         temp.push_back("timeout");

         formatKey(KEY_SS_TIMEOUT,
                   "How often sessions will be saved periodicly (in seconds)",
                   temp,
                   output);

         /* */

         formatSection(SECTION_AUTH, output);

         temp.clear();
         temp.push_back("filename");

         formatKey(KEY_AUTH_PASSWDFILE,
                   "File used to store auth information",
                   temp,
                   output);

         /* */

         formatSection(SECTION_RUNAS, output);

         temp.clear();
         temp.push_back("*nix user name");

         formatKey(KEY_RUNAS_USER,
                   "The daemon runs as this user",
                   temp,
                   output);

         temp.clear();
         temp.push_back("*nix group name");

         formatKey(KEY_RUNAS_GROUP,
                   "The daemon runs as this group",
                   temp,
                   output);

         return output;
      }

      void daemonConfiguration::setTransport(btg::core::messageTransport::TRANSPORT const _transport)
      {
         def_transport = _transport;
         data_modified = true;
      }

      void daemonConfiguration::setListenTo(btg::core::addressPort const& _addrport)
      {
         def_listenTo  = _addrport;
         data_modified = true;
      }

      void daemonConfiguration::setLog(btg::core::logger::logBuffer::LOGGER_TYPE const _logType)
      {
         def_logType   = _logType;
         data_modified = true;
      }

      void daemonConfiguration::setLogFile(std::string const& _filename)
      {
         def_logFilename = _filename;
         data_modified   = true;
      }

      void daemonConfiguration::setProxy(btg::core::addressPort const& _addrport)
      {
         def_proxy     = _addrport;
         data_modified = true;
      }

      void daemonConfiguration::setFilterFilename(std::string const& _filename)
      {
         def_filterFilename = _filename;
         data_modified      = true;
      }

      void daemonConfiguration::setFilterType(IpFilterIf::TYPE const _type)
      {
         def_filterType = _type;
         data_modified  = true;
      }

      void daemonConfiguration::setLTPortRange(std::pair<t_int, t_int> const& _range)
      {
         def_port_range = _range;
         data_modified  = true;
      }

      void daemonConfiguration::setLTListenTo(btg::core::Address const& _addr)
      {
         def_lt_ip     = _addr;
         data_modified = true;
      }

      btg::core::messageTransport::TRANSPORT daemonConfiguration::getTransport() const
      {
         return def_transport;
      }

      btg::core::addressPort daemonConfiguration::getListenTo() const
      {
         return def_listenTo;
      }

      btg::core::logger::logBuffer::LOGGER_TYPE daemonConfiguration::getLog() const
      {
         return def_logType;
      }

      void daemonConfiguration::setCACert(std::string const& _caCert)
      {
         def_TLS_CA_cert = _caCert;
      }

      std::string daemonConfiguration::getCACert() const
      {
         return def_TLS_CA_cert;
      }

      void daemonConfiguration::setCert(std::string const& _cert)
      {
         def_TLS_cert = _cert;
      }

      std::string daemonConfiguration::getCert() const
      {
         return def_TLS_cert;
      }

      void daemonConfiguration::setPrivKey(std::string const& _key)
      {
         def_TLS_privkey = _key;
      }

      std::string daemonConfiguration::getPrivKey() const
      {
         return def_TLS_privkey;
      }

      std::string daemonConfiguration::getLogFile() const
      {
         return def_logFilename;
      }

      btg::core::addressPort daemonConfiguration::getProxy() const
      {
         return def_proxy;
      }

      const std::string & daemonConfiguration::getPIDFile() const
      {
         return def_pidfname;
      }
      
      void daemonConfiguration::getFilterFilename(std::string & _filename)
      {
         _filename = def_filterFilename;
      }

      IpFilterIf::TYPE daemonConfiguration::getFilterType()
      {
         return def_filterType;
      }

      std::pair<t_int, t_int> daemonConfiguration::getLTPortRange() const
      {
         return def_port_range;
      }

      btg::core::Address daemonConfiguration::getLTListenTo() const
      {
         return def_lt_ip;
      }

      void daemonConfiguration::setUploadRateLimit(t_int const _value)
      {
         def_upload_rate_limit = _value;
      }

      void daemonConfiguration::setDownloadRateLimit(t_int const _value)
      {
         def_download_rate_limit = _value;
      }

      void daemonConfiguration::setMaxUploads(t_int const _value)
      {
         def_max_uploads = _value;
      }

      void daemonConfiguration::setMaxConnections(t_int const _value)
      {
         def_max_connections = _value;
      }

      t_int daemonConfiguration::getUploadRateLimit() const
      {
         return def_upload_rate_limit;
      }

      t_int daemonConfiguration::getDownloadRateLimit() const
      {
         return def_download_rate_limit;
      }

      t_int daemonConfiguration::getMaxUploads() const
      {
         return def_max_uploads;
      }

      t_int daemonConfiguration::getMaxConnections() const
      {
         return def_max_connections;
      }

      std::string daemonConfiguration::getSSFilename() const
      {
         return def_ss_filename;
      }

      bool daemonConfiguration::getSSEnable() const
      {
         return def_ss_enable;
      }

      t_int daemonConfiguration::getSSTimeout() const
      {
         return def_ss_timeout;
      }

      void daemonConfiguration::setSSFilename(std::string const& _filename)
      {
         def_ss_filename      = _filename;
         data_modified = true;
      }

      void daemonConfiguration::setSSEnable(bool const _enable)
      {
         def_ss_enable = _enable;
         data_modified = true;
      }

      void daemonConfiguration::setSSTimeout(t_int const _timeout)
      {
         def_ss_timeout = _timeout;
         data_modified = true;
      }

      std::string daemonConfiguration::getAuthFile() const
      {
         return def_authFile;
      }

      void daemonConfiguration::setAuthFile(std::string const& _authFile)
      {
         def_authFile = _authFile;
      }

      void daemonConfiguration::setRunAsUser(std::string const& _username)
      {
         def_runAsUser = _username;
      }

      std::string daemonConfiguration::getRunAsUser() const
      {
         return def_runAsUser;
      }

      void daemonConfiguration::setRunAsGroup(std::string const& _group)
      {
         def_runAsGroup = _group;
      }

      std::string daemonConfiguration::getRunAsGroup() const
      {
         return def_runAsGroup;
      }

      void daemonConfiguration::setUseTorrentName(bool const _use_torrent_name)
      {
         def_use_torrent_name = _use_torrent_name;
      }

      bool daemonConfiguration::getUseTorrentName() const
      {
         return def_use_torrent_name;
      }

      bool daemonConfiguration::getErrorDescription(std::string & _errordescription) const
      {
         return Configuration::getErrorDescription(_errordescription);
      }

      void daemonConfiguration::setUseUPnP(bool const _useUpnp)
      {
         def_use_upnp = def_use_upnp;
      }

      bool daemonConfiguration::getUseUPnP() const
      {
         return def_use_upnp;
      }

      void daemonConfiguration::readFlag(std::string const& _key,
                                         std::string const& _section,
                                         bool & _value)
      {
         std::string val_enable = inifile->GetValue(_key, _section);
         if (val_enable.size() > 0)
            {
               if ((val_enable == VALUE_ENABLE_TRUE) ||
                   (val_enable == VALUE_ENABLE_YES)  ||
                   (val_enable == VALUE_ENABLE_ONE))
                  {
                     _value = true;
                  }
               else if ((val_enable == VALUE_ENABLE_TRUE) ||
                        (val_enable == VALUE_ENABLE_YES)  ||
                        (val_enable == VALUE_ENABLE_ONE))
                  {
                     _value = false;
                  }
               else
                  {
                     _value = false;
                  }
            }
         else
            {
               _value = false;
            }
      }

      bool daemonConfiguration::writeFlag(std::string const& _key,
                                          std::string const& _section,
                                          std::string const& _description,
                                          bool const& _value)
      {
         std::string temp;

         if (_value)
            {
               temp = VALUE_ENABLE_TRUE;
            }
         else
            {
               temp = VALUE_ENABLE_FALSE;
            }

         if (!inifile->SetValue(_key,
                                temp,
                                _description,
                                _section))
            {
               setErrorDescription(writeOperation, _section, _key);
               return false;
            }

         return true;
      }

      void daemonConfiguration::readEncryptionPolicy(std::string const& _input,
                                                     encryption_policy & _output)
      {
         if (_input == VALUE_LEVEL_FORCE)
            {
               _output = daemonConfiguration::forced;
            }
         else if (_input == VALUE_LEVEL_ENABLE)
            {
               _output = daemonConfiguration::enabled;
            }
         else if (_input == VALUE_LEVEL_DISABLE)
            {
               _output = daemonConfiguration::disabled;
            }
      }

      void daemonConfiguration::readEncryptionLevel(std::string const& _input,
                                                    encryption_level & _output)
      {
         if (_input == VALUE_POLICY_PLAIN)
            {
               _output = daemonConfiguration::plaintext;
            }
         else if (_input == VALUE_POLICY_RC4)
            {
               _output = daemonConfiguration::rc4;
            }
         else if (_input == VALUE_POLICY_BOTH)
            {
               _output = daemonConfiguration::both;
            }
      }

      void daemonConfiguration::readEncryption()
      {
         std::string encryptionInStr  = inifile->GetValue(KEY_POLICY_IN, SECTION_ENC);

         if (encryptionInStr.size() > 0)
            {
               readEncryptionPolicy(encryptionInStr, def_in_enc_policy);
            }

         std::string encryptionOutStr = inifile->GetValue(KEY_POLICY_OUT, SECTION_ENC);
         if (encryptionOutStr.size() > 0)
            {
               readEncryptionPolicy(encryptionOutStr, def_out_enc_policy);
            }

         std::string encryptionLevelStr = inifile->GetValue(KEY_LEVEL, SECTION_ENC);
         if (encryptionLevelStr.size() > 0)
            {
               readEncryptionLevel(encryptionLevelStr, def_enc_level);
            }

         readFlag(KEY_PREFER_RC4, SECTION_ENC, def_prefer_rc4);
      }

      bool daemonConfiguration::writeEncryptionPolicy(encryption_policy const _input,
                                                      std::string const& _key,
                                                      std::string const& _descr)
      {
         std::string output;
         switch (_input)
            {
            case daemonConfiguration::forced:
               output = VALUE_LEVEL_FORCE;
               break;
            case daemonConfiguration::enabled:
               output = VALUE_LEVEL_ENABLE;
               break;
            case daemonConfiguration::disabled:
               output = VALUE_LEVEL_DISABLE;
               break;
            }

         if (!inifile->SetValue(_key,
                                output,
                                _descr,
                                SECTION_ENC))
            {
               setErrorDescription(writeOperation, SECTION_ENC, _key);
               return false;
            }

         return true;
      }

      bool daemonConfiguration::writeEncryptionLevel(encryption_level const _input)
      {
         std::string output;
         switch (_input)
            {
            case daemonConfiguration::plaintext:
               output = VALUE_POLICY_PLAIN;
               break;
            case daemonConfiguration::rc4:
               output = VALUE_POLICY_RC4;
               break;
            case daemonConfiguration::both:
               output = VALUE_POLICY_BOTH;
               break;
            }

         if (!inifile->SetValue(KEY_LEVEL,
                                output,
                                DESCR_LEVEL,
                                SECTION_ENC))
            {
               setErrorDescription(writeOperation, SECTION_ENC, KEY_LEVEL);
               return false;
            }

         return true;
      }

      bool daemonConfiguration::writeEncryption()
      {
         if (!writeEncryptionPolicy(def_out_enc_policy, KEY_POLICY_OUT, DESCR_POLICY_OUT))
            {
               return false;
            }

         if (!writeEncryptionPolicy(def_in_enc_policy, KEY_POLICY_IN, DESCR_POLICY_IN))
            {
               return false;
            }

         if (!writeEncryptionLevel(def_enc_level))
            {
               return false;
            }

         if (!writeFlag(KEY_PREFER_RC4,
                        SECTION_ENC, 
                        DESCR_PREFER_RC4,
                        def_prefer_rc4))
            {
               return false;
            }

         return true;
      }

      void daemonConfiguration::getEncryptionSettings(daemonConfiguration::encryption_policy & _in, 
                                                      daemonConfiguration::encryption_policy & _out, 
                                                      daemonConfiguration::encryption_level & _level,
                                                      bool & _preferRc4) const
      {
         _in        = def_in_enc_policy;
         _out       = def_out_enc_policy;
         _level     = def_enc_level;
         _preferRc4 = def_prefer_rc4;
      }

      std::string daemonConfiguration::getPeerId() const
      {
         return def_peerId;
      }

      std::string daemonConfiguration::getUserAgent() const
      {
         return def_userAgent;
      }

      void daemonConfiguration::readPeerIdAndUserAgent()
      {
         std::string peerIdStr = inifile->GetValue(KEY_MISC_PEERID, SECTION_MISC);
         if (peerIdStr.size() > 0)
            {
               def_peerId = peerIdStr;
            }
         
         std::string userAgentStr = inifile->GetValue(KEY_MISC_USERAGENT, SECTION_MISC);
         if (userAgentStr.size() > 0)
            {
               def_userAgent = userAgentStr;
            }
      }

      bool daemonConfiguration::writePeerIdAndUserAgent()
      {
         if (def_peerId.size() > 0)
            {
               if (!inifile->SetValue(KEY_MISC_PEERID,
                                      def_peerId,
                                      DESCR_MISC_PEERID,
                                      SECTION_MISC))
                  {
                     setErrorDescription(writeOperation, SECTION_MISC, KEY_MISC_PEERID);
                     return false;
                  }
            }
         
         if (def_userAgent.size() > 0)
            {
               if (!inifile->SetValue(KEY_MISC_USERAGENT,
                                      def_userAgent,
                                      DESCR_MISC_USERAGENT,
                                      SECTION_MISC))
                  {
                     setErrorDescription(writeOperation, SECTION_MISC, KEY_MISC_USERAGENT);
                     return false;
                  }
            }

         return true;
      }

      daemonConfiguration::~daemonConfiguration()
      {

      }

   } // namespace daemon
} // namespace btg


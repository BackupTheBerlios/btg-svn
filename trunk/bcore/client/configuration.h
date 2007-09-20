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
 * $Id: configuration.h,v 1.12.4.25 2007/05/28 10:33:02 wojci Exp $
 */

#ifndef CLIENT_CONFIGURATION_H
#define CLIENT_CONFIGURATION_H

#include <bcore/configuration.h>

#include <bcore/transport/transport.h>
#include <bcore/addrport.h>
#include <bcore/logger/logger.h>

#include <string>

namespace btg
{
   namespace core
      {
         namespace client
            {
               /// Client configuration.
               class clientConfiguration: public btg::core::Configuration
                  {
                  public:
                     /// Constructor.
                     /// @param [in] _filename The filename to use for reading/writing.
                     clientConfiguration(std::string const& _filename);

                     /// Read the configuration file.
                     bool read(bool const _force = false);

                     /// Write the configuration file.
                     bool write(bool const _force = false);

                     /// Returns true if the configuration was modified.
                     bool modified() const { return data_modified; };

                     /// Get the syntax of the configuration file.
                     std::string getSyntax() const;

                     /* Directories. */

                     /* Transport */

                     /// Set the default transport.
                     void setTransport(btg::core::messageTransport::TRANSPORT const _transport);

                     /// Get the default transport.
                     btg::core::messageTransport::TRANSPORT getTransport() const;

                     /* Network */

                     /// Set the IPv4:port that the client should connect to.
                     void setDaemonAddressPort(btg::core::addressPort const& _addrport);

                     /// Get the IPv4:port that the client should connect to.
                     btg::core::addressPort getDaemonAddressPort() const;

                     /* TLS */

                     /// Set the CA certificate to be used by the client.
                     void setCACert(std::string const& _caCert);

                     /// Get the CA certificate to be used by the client.
                     std::string getCACert() const;

                     /// Set the certificate to be used by the client.
                     void setCert(std::string const& _cert);

                     /// Get the certificate to be used by the client.
                     std::string getCert() const;

                     /// Set the private key to be used by the client.
                     void setPrivKey(std::string const& _key);

                     /// Get the private key to be used by the client.
                     std::string getPrivKey() const;

                     /* Log */

                     /// Set the type of log used.
                     void setLog(btg::core::logger::logBuffer::LOGGER_TYPE const _logType);

                     /// Get the type of log used.
                     btg::core::logger::logBuffer::LOGGER_TYPE getLog() const;

                     /// Set the filename of the logfile used, if a file based log is used.
                     void setLogFile(std::string const& _filename);

                     /// Get the filename of the logfile used, if a file based log is used.
                     std::string getLogFile() const;

                     /* Misc. */

                     /// Set the leech mode.
                     void setLeechMode(bool const _mode);

                     /// Returns true, if leech mode is enabled.
                     bool getLeechMode() const;

                     /// If mode is set to true, the user is not to be
                     /// bothered by confirmations.
                     void setNeverAskQuestions(bool const _mode);

                     /// If true, the user is not to be bothered by
                     /// confirmations.
                     bool getNeverAskQuestions() const;

                     /// Get the stored username.
                     std::string getUserName() const;

                     /// Set the username.
                     void setUserName(std::string const& _username);

                     /// Get the stored password.
                     std::string getPasswordHash() const;

                     /// Set the password.
                     void setPasswordHash(std::string const& _passwordHash);

                     /// Indicates that username and hash password are
                     /// present in the config file.
                     bool authSet() const;

                     /// Enable/disable DHT.
                     void setUseDHT(bool const _useDHT);

                     /// Get a DHT use indicator.
                     bool getUseDHT() const;

                     /// Enable/disable using encryption.
                     void setUseEncryption(bool const _useEncryption);

                     /// Indicates if encryption shall be used.
                     bool getUseEncryption() const;
                      
                     bool getErrorDescription(std::string & _errordescription) const;

                     /// Destructor.
                     virtual ~clientConfiguration();
                  protected:
                     /* Transport */

                     /// Default transport.
                     btg::core::messageTransport::TRANSPORT      def_transport;

                     /* Network */

                     /// The IPv4:port that the client should connect to.
                     btg::core::addressPort                      def_daemonaddress;

                     /* TLS */

                     /// CA certificate filename.
                     std::string                                 def_TLS_CA_cert;

                     /// Certificate filename.
                     std::string                                 def_TLS_cert;

                     /// Private key filename.
                     std::string                                 def_TLS_privkey;

                     /* Log */

                     /// The type of log used.
                     btg::core::logger::logBuffer::LOGGER_TYPE def_logType;

                     /// The filename of the logfile used, if a file based log is used.
                     std::string                                 def_logFilename;

                     /* Misc */
                     /// True, if leech mode is enabled.
                     bool                                        def_leechmode;

                     /// List of last opened files.
                     t_strList                                   def_lastFiles;

                     /// If true, the user is not to be bothered by
                     /// confirmations.
                     bool                                        def_neverAskQuestions;

                     /* Auth */

                     /// Username to use.
                     std::string                                 def_username;
                     /// Hash of the password to use.
                     std::string                                 def_passwordHash;

                     /// Indicates that username and hash password are
                     /// present in the config file.
                     bool                                        def_auth_set;

                     /// Indicates if DHT should be used.
                     bool                                        def_useDHT;

                     /// Indicates if encryption shall be used.
                     bool                                       def_useEncryption;
                  private:
                     /// Copy constructor.
                     clientConfiguration(clientConfiguration const& _c);
                     /// Assignment operator.
                     clientConfiguration& operator=(clientConfiguration const& _c);
                  };
            } // namespace client
      } // namespace core
} // namespace btg

#endif // CLIENT_CONFIGURATION_H

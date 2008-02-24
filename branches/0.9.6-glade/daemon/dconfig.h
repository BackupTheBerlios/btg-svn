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

#ifndef DAEMON_CONFIGURATION_H
#define DAEMON_CONFIGURATION_H

#include <bcore/configuration.h>

#include <bcore/transport/transport.h>
#include <bcore/addrport.h>
#include <bcore/logger/logger.h>

#include "ipfilter/ipfilterif.h"

#include <string>

namespace btg
{
   namespace daemon
      {
         /// Configuration reader/writer used by the daemon.
         class daemonConfiguration: private btg::core::Configuration
            {
            public:
               /// Constructor.
               /// @param [in] _filename The file name used.
               daemonConfiguration(std::string const& _filename);

               /// Read the config file.
               bool read();

               /// Write the config file.
               bool write(bool const _force = false);

               /// Returns true if the data was modified.
               bool modified() const { return data_modified; };

               /// Return the syntax of the config file used.
               std::string getSyntax() const;

               /* Transport */

               /// Set the default transport.
               void setTransport(btg::core::messageTransport::TRANSPORT const _transport);

               /// Get the default transport.
               btg::core::messageTransport::TRANSPORT getTransport() const;

               /* Network */
               /// Set the IPv4:port that the daemon should listen to.
               void setListenTo(btg::core::addressPort const& _addrport);

               /// Get the IPv4:port that the daemon should listen to.
               btg::core::addressPort getListenTo() const;

               /* TLS */

               /// Set the CA certificate to be used by the daemon.
               void setCACert(std::string const& _caCert);
               /// Get the CA certificate to be used by the daemon.
               std::string getCACert() const;

               /// Set the certificate to be used by the daemon.
               void setCert(std::string const& _cert);
               /// Get the certificate to be used by the daemon.
               std::string getCert() const;

               /// Set the private key to be used by the daemon.
               void setPrivKey(std::string const& _key);
               /// Get the private key to be used by the daemon.
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

               /* Misc */
               /// Set the proxy IPv4:port to be used.
               void setProxy(btg::core::addressPort const& _addrport);

               /// Get the proxy IPv4:port to be used.
               btg::core::addressPort getProxy() const;

               /// Set the filename of the file used for IPv4 filtering.
               void setFilterFilename(std::string const& _filename);

               /// Get the filename of the file used for IPv4 filtering.
               void getFilterFilename(std::string & _filename);

               /// Set the IPv4 filter type.
               void setFilterType(IpFilterIf::TYPE const _type);

               /// Get the IPv4 filter type.
               IpFilterIf::TYPE getFilterType();

               /// Set the port range used by libtorrent.
               void setLTPortRange(std::pair<t_int, t_int> const& _range);
               /// Get the port range used by libtorrent.
               std::pair<t_int, t_int> getLTPortRange() const;

               /// Set the IPv4 used by libtorrent.
               void setLTListenTo(btg::core::Address const& _addr);

               /// Get the IPv4 used by libtorrent.
               btg::core::Address getLTListenTo() const;

               /* Limit */
               /// Set the upload rate limit
               void setUploadRateLimit(t_int const _value);
               /// Get the upload rate limit
               t_int getUploadRateLimit() const;

               /// Set the download rate limit
               void setDownloadRateLimit(t_int const _value);
               /// Get the upload rate limit
               t_int getDownloadRateLimit() const;

               /// Set max uploads
               void setMaxUploads(t_int const _value);
               /// Get max uploads
               t_int getMaxUploads() const;

               /// Set max connections
               void setMaxConnections(t_int const _value);
               /// Get max connections
               t_int getMaxConnections() const;

#if BTG_OPTION_SAVESESSIONS
               /// Set the filename used to store saved sessions.
               void setSSFilename(std::string const& _filename);

               /// Get the filename used to store saved sessions.
               std::string getSSFilename() const;

               /// Enable/disable storing saved sessions.
               void setSSEnable(bool const _enable);

               /// Returns true, if saving sessions is enabled.
               bool getSSEnable() const;

               /// Timeout for periodic session saving 
               void setSSTimeout(t_int const _timeout);

					/// Get Timeout 
               t_int getSSTimeout() const;
#endif // BTG_OPTION_SAVESESSIONS

               /// Set the file name from which autorization information is read from.
               void setAuthFile(std::string const& _authFile);

               /// Get the file name from which autorization information is read from.
               std::string getAuthFile() const;

               /// Set the user as which the daemon should run as.
               void setRunAsUser(std::string const& _username);

               /// Get the user as which the daemon should run as.
               std::string getRunAsUser() const;

               /// Set the group as which the daemon should run as.
               void setRunAsGroup(std::string const& _username);

               /// Get the group as which the daemon should run as.
               std::string getRunAsGroup() const;

               /// Enable/disable using the torrent name from
               /// the tracker.
               /// 
               /// If disabled, torrent file name (given by a client)
               /// is used.
               void setUseTorrentName(bool const _use_torrent_name);

               /// Indicates if the torrent name from the tracker
               /// should be used.
               bool getUseTorrentName() const;

               /// Get the textual description of an error.
               bool getErrorDescription(std::string & _errordescription) const;

#if BTG_OPTION_UPNP
               /// Enable/disable the use of UPnP.
               void setUseUPnP(bool const _useUpnp);

               /// Indicates if UPnP is used.
               bool getUseUPnP() const;
#endif // BTG_OPTION_UPNP

               /// Encryption policy used.
               enum encryption_policy
                  {
                     forced = 0,
                     enabled,
                     disabled
                  };

               /// Encryption level used.
               enum encryption_level
                  {
                     plaintext,
                     rc4,
                     both
                  };

               /// Get the stored encryption policy and level.
               void getEncryptionSettings(daemonConfiguration::encryption_policy & _in, 
                                          daemonConfiguration::encryption_policy & _out, 
                                          daemonConfiguration::encryption_level & _level,
                                          bool & _preferRc4) const;
               
               /// Destructor.
               virtual ~daemonConfiguration();
            private:

               /// Read the encryption policy.
               void readEncryptionPolicy(std::string const& _input,
                                         encryption_policy & _output);

               /// Read the encryption level.
               void readEncryptionLevel(std::string const& _input,
                                        encryption_level & _output);
               
               /// Read all encryption settings.
               void readEncryption();

               /// Write all encryption settings.
               bool writeEncryption();
               
               /// Write the encryption level.
               bool writeEncryptionLevel(encryption_level const _input);

               /// Write the encryption policy.
               bool writeEncryptionPolicy(encryption_policy const _input,
                                          std::string const& _key,
                                          std::string const& _descr);
               
               /// Read a flag from the ini file.
               void readFlag(std::string const& _key, 
                             std::string const& _section,
                             bool & _value);

               /// Write a flag to the ini file.
               /// \note Calls setErrorDescription and returns false
               /// on failture.
               bool writeFlag(std::string const& _key, 
                              std::string const& _section,
                              std::string const& _description,
                              bool const& _value);

               /* Transport */
               /// Default transport.
               btg::core::messageTransport::TRANSPORT      def_transport;

               /* Network */
               /// The IPv4:port that the daemon should listen to.
               btg::core::addressPort                      def_listenTo;

               /* TLS */

               /// CA certificate filename.
               std::string                                 def_TLS_CA_cert;

               /// Certificate filename.
               std::string                                 def_TLS_cert;

               /// Private key filename.
               std::string                                 def_TLS_privkey;

               /* Log */
               /// The type of log used.
               btg::core::logger::logBuffer::LOGGER_TYPE   def_logType;

               /// The filename of the logfile used, if a file based log is used.
               std::string                                 def_logFilename;

               /* Misc */
               /// The proxy IPv4:port to be used.
               btg::core::addressPort                      def_proxy;

               /// The filename of the file used for IPv4 filtering.
               std::string                                 def_filterFilename;

               /// IPv4 filter type.
               IpFilterIf::TYPE                            def_filterType;

               /// Port range used by libtorrent.
               std::pair<t_uint, t_uint>                   def_port_range;

               /// Libtorrent listens to this IPv4 address.
               btg::core::Address                          def_lt_ip;

#if BTG_OPTION_UPNP
               /// Flag, determines if upnp should be used.
               bool                                        def_use_upnp;
#endif // BTG_OPTION_UPNP
               /* Limit */
               /// Upload rate limit
               t_int                                       def_upload_rate_limit;
               /// Download rate limit
               t_int                                       def_download_rate_limit;
               /// Max uploads
               t_int                                       def_max_uploads;
               /// Max connections
               t_int                                       def_max_connections;

#if BTG_OPTION_SAVESESSIONS
               /* Saving sessions */
               /// If session saving is enabled
               bool                                        def_ss_enable;

               /// File to write/read sessions to/from
               std::string                                 def_ss_filename;


					/// Timeout for periodic session saving
					t_int													  def_ss_timeout;
#endif // BTG_OPTION_SAVESESSIONS

               /// File from which read authorization information.
               std::string                                 def_authFile;

               /// Run as this user.
               std::string                                 def_runAsUser;

               /// Run as this group.
               std::string                                 def_runAsGroup;

               /// Enable/disable using the torrent name from the
               /// tracker.
               bool                                        def_use_torrent_name;

               /// Encryption policy, out.
               encryption_policy                           def_out_enc_policy;

               /// Encryption policy, in.
               encryption_policy                           def_in_enc_policy;

               /// Encryption level.
               encryption_level                            def_enc_level;

               /// If encrypted connections are prefered.
               bool                                        def_prefer_rc4;

            };

      } // namespace daemon
} // namespace btg

#endif // DAEMON_CONFIGURATION_H

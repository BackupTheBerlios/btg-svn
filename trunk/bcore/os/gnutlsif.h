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
 * The original class was from an article by Rob Tougher,
 * Linux Socket Programming In C++,
 * http://www.linuxgazette.com/issue74/tougher.html#1.
 */

/*
 * $Id: gnutlsif.h,v 1.1.4.5 2007/01/04 17:29:28 wojci Exp $
 */

#ifndef GNUTLS_IF_H
#define GNUTLS_IF_H

#include <config.h>

extern "C"
{
#include <gnutls/gnutls.h>
} // extern "C"

#include <string>

namespace btg
{
   namespace core
      {
         namespace os
            {
               /**
                * \addtogroup OS
                */
               /** @{ */

               // Typedefs, used to be able to
               // compile using an older gnutls version.

#if (GNUTLS_MAJOR_VER >= 1)
#  if (GNUTLS_MINOR_VER <= 1)
               typedef gnutls_session                 gnutls_session_t;
               typedef gnutls_dh_params               gnutls_dh_params_t;
               typedef gnutls_rsa_params              gnutls_rsa_params_t;
               typedef gnutls_certificate_credentials gnutls_certificate_credentials_t;
               typedef gnutls_x509_crt                gnutls_x509_crt_t;
               typedef gnutls_x509_privkey            gnutls_x509_privkey_t;
               typedef gnutls_datum                   gnutls_datum_t;
               typedef gnutls_credentials_type        gnutls_credentials_type_t;
               typedef gnutls_pk_algorithm            gnutls_pk_algorithm_t;
               typedef gnutls_kx_algorithm            gnutls_kx_algorithm_t;

#define SHA_DIGEST GNUTLS_DIG_SHA
#  else
#define SHA_DIGEST GNUTLS_DIG_SHA1
#  endif
#endif

               //typedef gnutls_session gnutls_session_t;

               /// Generic GnuTLS functions.
               class gtlsGeneric
                  {
                  public:
                     enum
                        {
                           /// Number of bits used.
                           GNUTLSIF_DH_BITS  = 1024,
                           /// Number of bits used.
                           GNUTLSIF_RSA_BITS = 512
                        };

                     /// Do global initialization of gnutls.
                     static void init();

                     /// Function used as a callback to log information from GNU TLS.
                     static void logCallback(int _level, const char* _p);

                     /// Convert a char array to hex.
                     static const char* bin2hex(const char* _bin, size_t _bin_size);

                     /// Generate SHA1 hash of a string. Returns a pointer to 20 bytes of binary data. Note that this data will be overwritten
                     /// next time generateSha1 is called from anywhere.
                     static const unsigned char* generateSha1(const unsigned char* _data, size_t _data_size);

                     /// Verifify that a peers certificate is acceptable.
                     static bool verifyCertificate(gnutls_session_t & _session);

                     /// Log information about the current session.
                     static void logSessionInfo(gnutls_session_t _session);

                     /// Log information about the currently used x509 certificate.
                     static void logX509CertificateInfo(gnutls_session_t session);
                  };

               /// Global data used by a server.
               /// This class must only be constructed and destructed
               /// exacly once.
               class gtlsGlobalServerData
                  {
                  public:
                     /// Constructor.
                     /// @param _CAfile   Filename, certificate authority certificate.
                     /// @param _CertFile Filename, daemon certificate.
                     /// @param _keyFile  Filename, daemon private key.
                     gtlsGlobalServerData(std::string const& _CAfile,
                                          /*std::string const& _CRLFile,*/
                                          std::string const& _CertFile,
                                          std::string const& _keyFile);

                     /// Genereate DH parameters.
                     void generate_dh_params();
                     /// Generate RSA parameters.
                     void generate_rsa_params();

                     /// Return true if an instance of this class is
                     /// initialized, false otherwise.
                     bool initialized() { return initialized_; };

                     /// Destructor.
                     ~gtlsGlobalServerData();

                  public:
                     /// DH params.
                     gnutls_dh_params_t               dh_params;
                     /// RSA params.
                     gnutls_rsa_params_t              rsa_params;
                     /// Certificate structure used.
                     gnutls_certificate_credentials   cert_cred;

                  private:
                     /// True if an instance of this class is initialized, false otherwise.
                     bool initialized_;

                  private:
                     /// Copy constructor.
                     gtlsGlobalServerData(gtlsGlobalServerData const& _gsd);

                     /// Assignment operator.
                     gtlsGlobalServerData& operator=(gtlsGlobalServerData const& _gsd);
                  };

               /// Data used by a server for each connection.
               class gtlsServerData
                  {
                  public:
                     /// Constructor which initializes the data needed
                     /// by a server socket.
                     /// @param _gtlsglobalserverdata Pointer an object containing the global GnuTLS data.
                     gtlsServerData(gtlsGlobalServerData* _gtlsglobalserverdata);

                     /// Initialize a GnuTLS session.
                     /// @param _session The session to initialize.
                     bool initSession(gnutls_session_t & _session);

                     /// Set the session contained in an instance of
                     /// this class.
                     /// @param _session The session to use.
                     void setSession(gnutls_session_t & _session);

                     /// Get the session contained in an instance of
                     /// this class.
                     gnutls_session_t getSession() const;

                     /// Return true if an instance of this class is
                     /// initialized, false otherwise.
                     bool initialized() { return initialized_; };

                     /// Destructor.
                     ~gtlsServerData();
                  private:
                     /// True if an instance of this class is
                     /// initialized, false otherwise.
                     bool initialized_;

                     /// The session used.
                     gnutls_session_t session;

                     /// Pointer an object containing the global GnuTLS data.
                     gtlsGlobalServerData* gtlsglobalserverdata_;
                  private:
                     /// Copy constructor.
                     gtlsServerData(gtlsServerData const& _sd);

                     /// Assignment operator.
                     gtlsServerData& operator=(gtlsServerData const& _sd);
                  };

               /// Data used by a client for each connection.
               class gtlsClientData
                  {
                  public:
                     /// Constructor which initializes the data needed
                     /// by a client socket.
                     /// @param _cafile   Filename, certificate authority certificate.
                     /// @param _certFile Filename, client certificate.
                     /// @param _keyFile  Filename, client private key.
                     gtlsClientData(std::string const& _cafile,
                                    std::string const& _certFile,
                                    std::string const& _keyFile);

                     /// Get the session.
                     gnutls_session_t session();

                     /// Set the session to be used.
                     /// @param _session The session.
                     void setSession(gnutls_session_t _session);

                     /// Destructor.
                     ~gtlsClientData();
                  private:
                     /// Client GnuTLS credentials.
                     gnutls_certificate_credentials_t cred_;
                     /// Session used.
                     gnutls_session_t                 session_;

                     /// Client certificate.
                     gnutls_x509_crt_t                client_crt_;
                     /// Client private key.
                     gnutls_x509_privkey_t            client_key_;
                  private:
                     /// Copy constructor.
                     gtlsClientData(gtlsClientData const& _gsd);

                     /// Assignment operator.
                     gtlsClientData& operator=(gtlsClientData const& _gsd);
                  };

               /** @} */
            } // namespace os
      } // namespace core
} // namespace btg

#endif // GNUTLS_IF_H

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
 * $Id$
 */

#include "gnutlsif.h"
#include "gnutlsmacro.h"

#include <bcore/logmacro.h>

#include <gnutls/x509.h>
#include <stdio.h>

namespace btg
{
   namespace core
   {
      namespace os
      {
         // Log level, 0-9.
         int const loglevel = 2;

         void gtlsGeneric::init()
         {
            GNUTLS_NOTICE("Initializing GNU TLS.");
            gnutls_global_init();
         }

         void gtlsGeneric::logCallback(int _level, const char* _p)
         {
            GNUTLS_NOTICE("GNUTLS(" << _level << "): " << _p);
         }

         const char* gtlsGeneric::bin2hex(const char* _bin, size_t _bin_size)
         {
            static char printable[110];
            //const unsigned char* bin = (const unsigned char*)_bin;
            size_t bin_size          = _bin_size;
            char *print;
            size_t i;

            if (bin_size > 50)
               {
                  bin_size = 50;
               }

            print = printable;
            for (i = 0; i < bin_size; i++)
               {
                  sprintf(print, "%.2x ", _bin[i]);
                  print += 2;
               }

            return printable;
         }

         const unsigned char* gtlsGeneric::generateSha1(const unsigned char* _data, size_t _data_size)
         {
            static unsigned char hash[20];
            size_t result_size = 20;

            unsigned char* buffer = new unsigned char[_data_size];
            memcpy(buffer, _data, _data_size);

            gnutls_datum_t data;
            data.data = buffer;
            data.size = _data_size;

            if (gnutls_fingerprint(SHA_DIGEST, &data, hash, &result_size) < 0)
               {
                  BTG_NOTICE("gtlsGeneric::generateSha1() failed");
                  delete [] buffer;
                  buffer = 0;
                  return 0;
               }

            delete [] buffer;
            buffer = 0;

            return hash;
         }

         bool gtlsGeneric::verifyCertificate(gnutls_session_t & _session)
         {
            bool status = true;

            unsigned int cert_status;
            const gnutls_datum_t *cert_list;
            int cert_list_size, ret;
            gnutls_x509_crt_t cert;

            ret = gnutls_certificate_verify_peers2(_session, &cert_status);

            if (ret < 0)
               {
                  status = false;
                  return status;
               }

            if (cert_status & GNUTLS_CERT_INVALID)
               {
                  BTG_NOTICE("The certificate is not trusted.");
                  status = false;
               }

            if (cert_status & GNUTLS_CERT_SIGNER_NOT_FOUND)
               {
                  BTG_NOTICE("The certificate hasn't got a known issuer.");
                  status = false;
               }

            if (cert_status & GNUTLS_CERT_REVOKED)
               {
                  BTG_NOTICE("The certificate has been revoked.\n");
                  status = false;
               }

            if (gnutls_x509_crt_init(&cert) < 0)
               {
                  BTG_NOTICE("gnutls_x509_crt_init failed.");
                  status = false;
                  return status;
               }

            cert_list = gnutls_certificate_get_peers(_session, reinterpret_cast<unsigned int*>(&cert_list_size));
            if (cert_list == 0)
               {
                  BTG_NOTICE("No certificate was found");
                  status = false;
                  return status;
               }

            // Just check the first certificate.
            if (gnutls_x509_crt_import(cert, &cert_list[0], GNUTLS_X509_FMT_DER) < 0)
               {
                  BTG_NOTICE("error parsing certificate.");
                  status = false;
                  return status;
               }

            time_t now       = time(0);
            time_t cert_exptime = gnutls_x509_crt_get_expiration_time(cert);

            if (cert_exptime == static_cast<time_t>(-1))
               {
                  BTG_NOTICE("gnutls_x509_crt_get_expiration_time failed.");
                  status = false;
                  return status;
               }

            if (cert_exptime < now)
               {
                  BTG_NOTICE("The certificate has expired.");
                  status = false;
                  return status;
               }

            time_t cert_act = gnutls_x509_crt_get_activation_time(cert);

            if (cert_exptime == static_cast<time_t>(-1))
               {
                  BTG_NOTICE("gnutls_x509_crt_get_activation_time failed.");
                  status = false;
                  return status;
               }

            if (cert_act > now)
               {
                  BTG_NOTICE("The certificate is not yet activated.");
                  status = false;
                  return status;
               }

            gnutls_x509_crt_deinit(cert);

            return status;
         }

         void gtlsGeneric::logSessionInfo(gnutls_session_t _session)
         {
            const char *tmp;
            gnutls_credentials_type_t cred;
            gnutls_kx_algorithm_t kx;

            // print the key exchange's algorithm name
            kx = gnutls_kx_get(_session);
            tmp = gnutls_kx_get_name(kx);
            BTG_NOTICE("- Key Exchange: " << tmp);

            // Check the authentication type used and switch
            // to the appropriate.
            cred = gnutls_auth_get_type(_session);
            switch (cred)
               {
               case GNUTLS_CRD_SRP:
                  {
                     BTG_NOTICE("- SRP session");
                     break;
                  }

               case GNUTLS_CRD_ANON:
                  {
                     BTG_NOTICE("- Anonymous DH using prime of " << gnutls_dh_get_prime_bits (_session) << " bits");
                     break;
                  }
               case GNUTLS_CRD_CERTIFICATE:
                  {
                     // Check if we have been using ephemeral Diffie Hellman.

                     if (kx == GNUTLS_KX_DHE_RSA || kx == GNUTLS_KX_DHE_DSS)
                        {
                           BTG_NOTICE("- Ephemeral DH using prime of " << gnutls_dh_get_prime_bits(_session) << " bits");
                        }

                     /* if the certificate list is available, then
                      * print some information about it.
                      */
                     gtlsGeneric::logX509CertificateInfo(_session);
                     break;
                  }
               default:
                  {
                     BTG_NOTICE("Unknown cred.");
                  }
               }

            /* print the protocol's name (ie TLS 1.0)
             */
            tmp = gnutls_protocol_get_name(gnutls_protocol_get_version(_session));
            BTG_NOTICE("- Protocol: " << tmp);

            /* print the certificate type of the peer.
             * ie X.509
             */
            tmp = gnutls_certificate_type_get_name(gnutls_certificate_type_get(_session));

            BTG_NOTICE("- Certificate Type: " << tmp);

            /* print the compression algorithm (if any)
             */
            tmp = gnutls_compression_get_name(gnutls_compression_get(_session));
            BTG_NOTICE("- Compression: " << tmp);

            /* print the name of the cipher used.
             * ie 3DES.
             */
            tmp = gnutls_cipher_get_name(gnutls_cipher_get(_session));
            BTG_NOTICE("- Cipher: " << tmp);

            /* Print the MAC algorithms name.
             * ie SHA1
             */
            tmp = gnutls_mac_get_name(gnutls_mac_get(_session));
            BTG_NOTICE("- MAC: " << tmp);
         }

         void gtlsGeneric::logX509CertificateInfo(gnutls_session_t _session)
         {
            char serial[40];
            char dn[128];
            size_t size;
            unsigned int algo, bits;
            time_t expiration_time, activation_time;
            const gnutls_datum_t *cert_list;
            int cert_list_size = 0;
            gnutls_x509_crt_t cert;

            if (gnutls_certificate_type_get (_session) != GNUTLS_CRT_X509)
               {
                  BTG_NOTICE("Not a x509 certificate, abort.");
                  return;
               }

            cert_list = gnutls_certificate_get_peers(_session, reinterpret_cast<unsigned int*>(&cert_list_size));

            BTG_NOTICE("Peer provided " << cert_list_size << " certificates.");

            if (cert_list_size > 0)
               {

                  /* we only print information about the first certificate.
                   */
                  gnutls_x509_crt_init (&cert);

                  gnutls_x509_crt_import(cert, &cert_list[0], GNUTLS_X509_FMT_DER);

                  BTG_NOTICE("Certificate info:");

                  expiration_time = gnutls_x509_crt_get_expiration_time (cert);
                  activation_time = gnutls_x509_crt_get_activation_time (cert);

                  BTG_NOTICE("   Certificate is valid since: " << ctime (&activation_time));
                  BTG_NOTICE("   Certificate expires: " << ctime (&expiration_time));

                  /* Print the serial number of the certificate.
                   */
                  size = sizeof (serial);
                  gnutls_x509_crt_get_serial (cert, serial, &size);

                  size = sizeof (serial);
                  BTG_NOTICE("   Certificate serial number: " << bin2hex(serial, size));

                  /* Extract some of the public key algorithm's parameters
                   */
                  algo = gnutls_x509_crt_get_pk_algorithm(cert, &bits);

                  BTG_NOTICE("Certificate public key: " << gnutls_pk_algorithm_get_name(static_cast<gnutls_pk_algorithm_t>(algo)));

                  /* Print the version of the X.509
                   * certificate.
                   */
                  BTG_NOTICE("   Certificate version: #" << gnutls_x509_crt_get_version(cert));

                  size = sizeof(dn);
                  gnutls_x509_crt_get_dn(cert, dn, &size);
                  BTG_NOTICE("   DN: " << dn);

                  size = sizeof(dn);
                  gnutls_x509_crt_get_issuer_dn (cert, dn, &size);
                  BTG_NOTICE("   Issuer's DN: " << dn);

                  gnutls_x509_crt_deinit(cert);
               }
         }

         gtlsGlobalServerData::gtlsGlobalServerData(std::string const& _CAfile,
                                                    /*std::string const& _CRLFile,*/
                                                    std::string const& _CertFile,
                                                    std::string const& _keyFile)
            : cert_cred(0),
              initialized_(true)
         {
            int ret = 0;

            // Logging.
            gnutls_global_set_log_level(loglevel);
            gnutls_global_set_log_function(gtlsGeneric::logCallback);

            // Allocate the structures used to keep certificates in.
            ret = gnutls_certificate_allocate_credentials(&cert_cred);
            printerror("gnutls_certificate_allocate_credentials", ret);
            if (ret < 0) { initialized_ = false; }

            // Set the CA certificate.
            GNUTLS_NOTICE("Setting CA, file='" << _CAfile << "'");
            ret = gnutls_certificate_set_x509_trust_file(cert_cred,
                                                         _CAfile.c_str(),
                                                         GNUTLS_X509_FMT_PEM);
            printerror("gnutls_certificate_set_x509_trust_file", ret);
            if (ret < 0) { initialized_ = false; }
            GNUTLS_NOTICE("Processed " << ret << " certificates.");

            // Set the certificate and priate key pair.
            GNUTLS_NOTICE("Setting certificate, file='" << _CertFile << "'");
            GNUTLS_NOTICE("Setting private key, file='" << _keyFile << "'");
            ret = gnutls_certificate_set_x509_key_file(cert_cred,
                                                       _CertFile.c_str(),
                                                       _keyFile.c_str(),
                                                       GNUTLS_X509_FMT_PEM);
            printerror("gnutls_certificate_set_x509_key_file", ret);
            if (ret < 0) { initialized_ = false; }
            GNUTLS_NOTICE("Processed " << ret << " certificate/key pair(s).");

            // Generate and set DH params.
            generate_dh_params();
            gnutls_certificate_set_dh_params(cert_cred, dh_params);
         }

         void gtlsGlobalServerData::generate_dh_params()
         {
            int ret = 0;

            ret = gnutls_dh_params_init(&dh_params);
            printerror("gnutls_dh_params_init", ret);
            if (ret < 0) { initialized_ = false; }

            ret = gnutls_dh_params_generate2(dh_params, gtlsGeneric::GNUTLSIF_DH_BITS);
            printerror("gnutls_dh_params_generate2", ret);
            if (ret < 0) { initialized_ = false; }

         }

         void gtlsGlobalServerData::generate_rsa_params()
         {
            int ret = 0;

            ret = gnutls_rsa_params_init(&rsa_params);
            printerror("gnutls_rsa_params_init", ret);

            /* Generate RSA parameters - for use with RSA-export
             * cipher suites. These should be discarded and regenerated
             * once a day, once every 500 transactions etc. Depends on the
             * security requirements.
             */

            ret = gnutls_rsa_params_generate2(rsa_params, gtlsGeneric::GNUTLSIF_RSA_BITS);
            printerror("gnutls_rsa_params_generate2", ret);
            if (ret < 0) { initialized_ = false; }
         }

         gtlsGlobalServerData::~gtlsGlobalServerData()
         {
            // Free the structures used to keep certificates in.
            gnutls_certificate_free_credentials(cert_cred);

            // Free the dh params used.
            gnutls_dh_params_deinit(dh_params);

            // Shutdown Gnu TLS.
            gnutls_global_deinit();
         }

         // Data used per connection.
         gtlsServerData::gtlsServerData(gtlsGlobalServerData* _gtlsglobalserverdata)
            : initialized_(true),
              gtlsglobalserverdata_(_gtlsglobalserverdata)
         {
         }

         bool gtlsServerData::initSession(gnutls_session_t & _session)
         {
            bool status = true;

            int ret = 0;

            // Create a session.
            ret = gnutls_init (&_session, GNUTLS_SERVER);
            if (ret < 0) { initialized_ = false; }
            printerror("gnutls_init", ret);

            // Set the default priority.
            ret = gnutls_set_default_priority(_session);
            if (ret < 0) { initialized_ = false; }
            printerror("gnutls_set_default_priority", ret);

            ret = gnutls_credentials_set(_session, GNUTLS_CRD_CERTIFICATE, gtlsglobalserverdata_->cert_cred);
            if (ret < 0) { initialized_ = false; }
            printerror("gnutls_credentials_set", ret);

            gnutls_certificate_server_set_request(_session, GNUTLS_CERT_REQUEST);

            gnutls_dh_set_prime_bits(_session, gtlsGeneric::GNUTLSIF_DH_BITS);
            GNUTLS_NOTICE("Created new TLS session with id = " << _session);

            return status;
         }

         void gtlsServerData::setSession(gnutls_session_t & _session)
         {
            session = _session;
         }

         gnutls_session_t gtlsServerData::getSession() const
         {
            return session;
         }

         gtlsServerData::~gtlsServerData()
         {
            gnutls_deinit(session);
         }

         /* -- */
         /* -- */
         /* -- */

         gtlsClientData::gtlsClientData(std::string const& _caFile,
                                        std::string const& _certFile,
                                        std::string const& _keyFile)
         {
            int ret = 0;

            gnutls_global_set_log_level(loglevel);
            gnutls_global_set_log_function(gtlsGeneric::logCallback);

            gnutls_certificate_allocate_credentials(&cred_);
            GNUTLS_NOTICE("Allocted credentials");
            // Add the certificate authority used.
            GNUTLS_NOTICE("Setting CA, file='" << _caFile << "'");
            ret = gnutls_certificate_set_x509_trust_file(cred_, _caFile.c_str(), GNUTLS_X509_FMT_PEM);
            printerror("gnutls_certificate_set_x509_trust_file", ret);
            GNUTLS_NOTICE("Processed " << ret << " certificates.");

            // Set the cert and private key.
            // Set the certificate and priate key pair.
            GNUTLS_NOTICE("Setting certificate, file='" << _certFile << "'");
            GNUTLS_NOTICE("Setting private key, file='" << _keyFile << "'");

            ret = gnutls_certificate_set_x509_key_file(cred_,
                                                       _certFile.c_str(),
                                                       _keyFile.c_str(),
                                                       GNUTLS_X509_FMT_PEM);
            printerror("gnutls_certificate_set_x509_key_file", ret);

            GNUTLS_NOTICE("Processed " << ret << " certificate/key pair(s).");

            ret = gnutls_init(&session_, GNUTLS_CLIENT);
            printerror("gnutls_init", ret);

            ret = gnutls_set_default_priority(session_);
            printerror("gnutls_set_default_priority", ret);

            gnutls_dh_set_prime_bits(session_, gtlsGeneric::GNUTLSIF_DH_BITS);

            const int cert_type_priority[2] = { GNUTLS_CRT_X509, 0 };
            ret = gnutls_certificate_type_set_priority(session_,
                                                       cert_type_priority);
            printerror("gnutls_certificate_type_set_priority", ret);

            ret = gnutls_credentials_set(session_,
                                         GNUTLS_CRD_CERTIFICATE,
                                         cred_);

            printerror("gnutls_credentials_set", ret);
         }

         gnutls_session_t gtlsClientData::session()
         {
            return session_;
         }

         void gtlsClientData::setSession(gnutls_session_t _session)
         {
            session_ = _session;
         }

         gtlsClientData::~gtlsClientData()
         {
            gnutls_deinit(session_);

            gnutls_certificate_free_credentials(cred_);

            gnutls_global_deinit();
         }

      } // namespace os
   } // namespace core
} // namespace btg


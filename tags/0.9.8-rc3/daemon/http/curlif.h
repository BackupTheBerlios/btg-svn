/*
 * btg Copyright (C) 2008 Michael Wojciechowski.
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

#ifndef CURL_IF_H
#define CURL_IF_H

#include <string>
#include <stdexcept> // for std::runtime_error
#include <fstream>

extern "C"
{
#include <curl/curl.h>
} // extern "C"

#include <bcore/sbuf.h>

namespace btg
{
   namespace daemon
   {
      namespace http
      {
         /**
          * \addtogroup http
          */
         /** @{ */

         /// Exception thown by the curl interface constructor.
         class curlException: public std::runtime_error
         {
         public:
            /// Constructor.
            curlException(std::string const& _message)
               : std::runtime_error(_message)
            {
               
            }
         };

         class httpAbortIf;
         class httpProgressIf;

         /// Curl interface.
         class curlInterface: public btg::core::Logable
         {
         public:
            /// Constructor.
            curlInterface(btg::core::LogWrapperType _logwrapper,
                          httpAbortIf const& _abortIf,
                          httpProgressIf & _progressIf);

            /// Download _url into _filename.
            /// @return true - downloaded, false - not downloaded.
            bool Get(std::string const& _url, 
                     std::string const& _filename);

            /// Get the contents of the downloaded file.
            bool Result(btg::core::sBuffer & _buffer);
            
            /// Destructor.
            ~curlInterface();
         
         private:
            /// Reference to the interface used for quering if the
            /// download should terminate.
            httpAbortIf const& abortIf;
            /// Reference to the interface to report download progress
            httpProgressIf & progressIf;
            /// Pointer to curl structure.
            CURL*       curl;
            /// Download status.
            bool        status;
            /// Filename used.
            std::string filename;

            /// file stream to write the data to
            std::ofstream file;
            
            /// Callback from the CURL
            /// called when chunk to write is ready
            /// @param [in] data data to write
            /// @param [in] size size of block
            /// @param [in] nmemb count of blocks
            /// @param [in] self pointer to CURL object owner (this)
            /// @return size * nmemb if success, something else if want to terminate (curl_easy_perform will return CURLE_WRITE_ERROR then) 
            static size_t curl_write_function(void *data, size_t size, size_t nmemb, curlInterface *self);
            
         protected:
            /// Called (from curl_write_function) when data chunk arrived
            /// @param [in] data the chunk
            /// @param [in] size chunk size
            /// @return true - continue, false - terminate
            bool onDataChunk(void *data, size_t size);
            
         private:
            /// Copy constructor.
            curlInterface(curlInterface const& _ci);

            /// Assignment operator.
            curlInterface& operator=(curlInterface const& _ci);
         };

         /** @} */
      } // namespace http
   } // namespace daemon
} // namespace btg

#endif // CURL_IF_H

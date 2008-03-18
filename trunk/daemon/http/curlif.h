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

         class curlException
         {
         public:
            curlException(std::string const& _message);

            std::string message() const;
         private:
            const std::string message_;
         };

         class httpAbortIf;

         /// Curl interface.
         class curlInterface: public btg::core::Logable
         {
         public:
            /// Constructor.
            curlInterface(btg::core::LogWrapperType _logwrapper,
                          httpAbortIf const& _abortIf);

            bool Get(std::string const& _url, 
                     std::string const& _filename);

            bool Result(btg::core::sBuffer & _buffer);
            
            /// Destructor.
            ~curlInterface();
         protected:
            httpAbortIf const& abortIf;
            CURLM*      curlm;
            CURL*       curl;
            bool        status;
            std::string filename;
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

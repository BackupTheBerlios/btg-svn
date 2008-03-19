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

#include "curlif.h"
#include "httpproc.h"

extern "C"
{
#include <stdio.h>
}

#include <bcore/os/sleep.h>
#include <daemon/modulelog.h>

namespace btg
{
   namespace daemon
   {
      namespace http
      {
         const std::string moduleName("cuif");

         curlException::curlException(std::string const& _message)
            : message_(_message)
         {
         }

         std::string curlException::message() const
         {
            return message_;
         }

         /* */

         curlInterface::curlInterface(btg::core::LogWrapperType _logwrapper,
                                      httpAbortIf const& _abortIf)
            : btg::core::Logable(_logwrapper),
              abortIf(_abortIf),
              curl(0),
              filename()
         {
            BTG_MNOTICE(logWrapper(), "constructed");

            curlm = curl_multi_init();
            if (!curlm) 
               {
                  throw curlException("curl_multi_init failed.");
               }

            curl = curl_easy_init();
            if (!curl) 
               {
                  throw curlException("curl_easy_init failed.");
               }
         }
         
         bool curlInterface::Get(std::string const& _url,
                                 std::string const& _filename)
         {
            status = false;

            curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());

            // curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);

            FILE* f = fopen(_filename.c_str(), "w");

            if (f == 0)
               {
                  BTG_MNOTICE(logWrapper(), "fopen failed, filename << '" << _filename << "'.");
                  return status;
               }
            
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
            
            curl_multi_add_handle(curlm, curl);

            bool done = false;
            while (!done && !abortIf.AbortTransfer())
               {
                  int running_handles;
                  CURLMcode code = curl_multi_perform(curlm, &running_handles);
                  switch (code)
                     {
                     case CURLM_CALL_MULTI_PERFORM:
                     case CURLM_OK:
                        // Give curl a chance to download something.
                        btg::core::os::Sleep::sleepMiliSeconds(50);
                        break;
                     default:
                        BTG_MNOTICE(logWrapper(), "CURLM code:" << code);
                        break;
                     }
                  
                  if (running_handles == 0)
                     {
                        BTG_MNOTICE(logWrapper(), "No more curl handles." << code);
                        done = true;
                     }
               }

            if (abortIf.AbortTransfer())
               {
                  BTG_MNOTICE(logWrapper(), "Transfer aborted.");

                  curl_multi_remove_handle(curlm, curl);
                  fclose(f);
                  return status;
               }

            long httpcode = 404;
            CURLcode rcode = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpcode);

            if ((rcode != CURLE_OK) || (httpcode >= 400))
               {
                  BTG_MNOTICE(logWrapper(), "Got error " << httpcode << " while loading '" << _url << "'.");
                  status = false;
               }
            else
               {
                  BTG_MNOTICE(logWrapper(), "Loaded '" << _url << "'.");
                  status   = true;
                  filename = _filename;
               }

            fclose(f);

            curl_multi_remove_handle(curlm, curl);

            return status;
         }

         bool curlInterface::Result(btg::core::sBuffer & _buffer)
         {
            if (!status)
               {
                  BTG_MNOTICE(logWrapper(), "Not reading file");
                  return status;
               }

            BTG_MNOTICE(logWrapper(), "reading file '" << filename << "'");

            status = _buffer.read(filename);
            filename = "";

            return status;
         }

         curlInterface::~curlInterface()
         {
            curl_easy_cleanup(curl);
            curl_multi_cleanup(curlm);

            BTG_MNOTICE(logWrapper(), "destroyed");
         }

      } // namespace http
   } // namespace daemon
} // namespace btg



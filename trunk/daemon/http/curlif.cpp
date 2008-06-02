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

#include <bcore/os/sleep.h>
#include <bcore/os/fileop.h>

#include <daemon/modulelog.h>

namespace btg
{
   namespace daemon
   {
      namespace http
      {
         const std::string moduleName("cuif");

         curlInterface::curlInterface(btg::core::LogWrapperType _logwrapper,
                                      httpAbortIf const& _abortIf,
                                      httpProgressIf & _progressIf)
            : btg::core::Logable(_logwrapper),
              abortIf(_abortIf),
              progressIf(_progressIf),
              curl(0)
         {
            BTG_MNOTICE(logWrapper(), "constructed");

            curl = curl_easy_init();
            if (!curl) 
               {
                  throw curlException("curl_easy_init failed");
               }
         }
         
         bool curlInterface::Get(std::string const& _url,
                                 std::string const& _filename)
         {
            status = true;
            filename.clear();

            status = status && (curl_easy_setopt(curl, CURLOPT_URL, _url.c_str()) == 0);
            // status = status && (curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1) == 0);
            status = status && (curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_function) == 0);
            status = status && (curl_easy_setopt(curl, CURLOPT_WRITEDATA, this) == 0);
            
            if (!status)
            {
               BTG_MNOTICE(logWrapper(), "curl_easy_setopt failed");
               return status;
            }

            file.clear();
            file.open(_filename.c_str());
            status = status && file.good();

            if (!status)
               {
                  BTG_MNOTICE(logWrapper(), "fopen failed, filename << '" << _filename << "'.");
                  return status;
               }
            
            if (status)
            {
               status = status && (curl_easy_perform(curl) == 0);
               
               if (!status)
               {
                  if (abortIf.AbortTransfer())
                  {
                     BTG_MNOTICE(logWrapper(), "Transfer aborted.");
                  }
                  else
                  {
                     BTG_MNOTICE(logWrapper(), "curl_easy_perform returned bad status");
                  }
               }
            }
            
            file.close();
            
            long httpcode = 404;
            if (status)
            {
               status = status && (curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpcode) == 0);
               
               if (!status)
               {
                  BTG_MNOTICE(logWrapper(), "curl_easy_getinfo[CURLINFO_RESPONSE_CODE] failed");
               }
            }
            
            if (status)
            {
               status = status && (httpcode == 200);
               
               if (!status)
               {
                  BTG_MNOTICE(logWrapper(), "Got HTTP error " << httpcode << " while loading '" << _url << "'.");
               }
            }
            
            if (status)
            {
               BTG_MNOTICE(logWrapper(), "Loaded '" << _url << "'.");
               filename = _filename;
            }
            else
            {
               // Remove the filename used.
              btg::core::os::fileOperation::remove(_filename);
            }

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
            // filename = "";

            return status;
         }

         curlInterface::~curlInterface()
         {
            curl_easy_cleanup(curl);

            BTG_MNOTICE(logWrapper(), "destroyed");
         }
         
         size_t curlInterface::curl_write_function(void *data, size_t size, size_t nmemb, curlInterface *self)
         {
            return self->onDataChunk(data, size * nmemb) ? size * nmemb : (size_t)-1;
         }
         
         bool curlInterface::onDataChunk(void *data, size_t size)
         {
            bool bResult = true;
            
            double dltotal, dlnow, dlspeed;
            bResult = bResult && (curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &dltotal) == 0);
            bResult = bResult && (curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &dlnow) == 0);
            bResult = bResult && (curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD, &dlspeed) == 0);
            if (bResult)
            {
               progressIf.ReportDownloadProgress(dltotal, dlnow, dlspeed);
            }
            
            /*
             * for the future (may be)
             * 
            double ultotal, ulnow, ulspeed;
            bResult = bResult && (curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &ultotal) == 0);
            bResult = bResult && (curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &ulnow) == 0);
            bResult = bResult && (curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD, &ulspeed) == 0);
            if (bResult)
            {
               progressIf.ReportUploadProgress(ultotal, ulnow, ulspeed);
            }
            */

            file.write(static_cast<char*>(data), size);
            
            return file.good() && !abortIf.AbortTransfer();            
         }

      } // namespace http
   } // namespace daemon
} // namespace btg



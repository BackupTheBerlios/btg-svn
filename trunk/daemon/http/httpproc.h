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

#ifndef HTTPPROCESS_H
#define HTTPPROCESS_H

#include <string>

#include <bcore/sbuf.h>
#include <bcore/logable.h>

#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include "httpif.h"
#include "curlif.h"

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

         /// Interface used to abort URL download.
         class httpAbortIf
         {
         public:
            /// Abort a download.
            /// @return True - abort download. False - continue download.
            virtual bool AbortTransfer() const = 0;
            
            virtual ~httpAbortIf()
            {
               
            }
         };
         
         /// Interface used to track download progress
         class httpProgressIf
         {
         public:
            /// Report the download progress
            /// @param [in] dltotal total bytes to download
            /// @param [in] dlnow bytes already downloaded
            /// @param [in] dlspeed current download speed (bytes/sec)
            /// @note for faster communication with curl all values are in double
            virtual void ReportDownloadProgress(double dltotal, double dlnow, double dlspeed) = 0;
            
            /*
             * Additional reporting methods (callbacks) also can be defined
             * for communication with another backends
             */
            
            // Report the upload progress
            // @param [in] ultotal total bytes to upload
            // @param [in] ulnow bytes already uploaded
            // @param [in] ulspeed current upload speed (bytes/sec)
            // @note for the future
            //virtual void ReportUploadProgress(double ultotal, double ulnow, double ulspeed) = 0;
            
            virtual ~httpProgressIf()
            {
               
            }
         };

         /// Thread used to download an URL. This thread can be
         /// aborted.
         class httpProcess: public btg::core::Logable, public httpAbortIf, public httpProgressIf
         {
         public:
            /// Constructor.
            httpProcess(btg::core::LogWrapperType _logwrapper,
                        std::string _URL, 
                        std::string _filename);

            /// Get the status of the download.
            httpInterface::Status Status();
            
            /// Get download progress info
            void DlProgress(t_uint &_dltotal, t_uint &_dlnow, t_uint &_dlspeed);

            /// Get the result of the download.
            bool Result(btg::core::sBuffer & _buffer);

            /// Terminate the download.
            void Terminate();

            /// Destructor.
            virtual ~httpProcess();
         protected:
            /// Function used for the thread.
            void work();

            /// Indicate if the transfer should be aborted.
            bool AbortTransfer() const;
            
            /// Download progress reporter callback.
            /// Called by curlInterface.
            void ReportDownloadProgress(double _dltotal, double _dlnow, double _dlspeed);

            /// The URL to download.
            std::string           URL;

            /// The filename to write the result to.
            std::string           filename;

            /// The status of the download.
            httpInterface::Status status;

            /// The CURL interface used.
            curlInterface         ci;

            /// Indicates if the download should terminate.
            bool                  terminate;
            
            /// Content-Length
            double               dltotal;
            
            /// How much data we downloaded
            double               dlnow;
            
            /// Last measured download speed (bytes/sec) 
            double               dlspeed;

            /// Mutex used to control access to the members
            /// of this class from the outside.
            boost::mutex          interfaceMutex;
            
            /// The thread used by this class.
            boost::thread         thread;
         };

         /** @} */

      } // namespace http
   } // namespace daemon
} // namespace btg

#endif // HTTPPROCESS_H


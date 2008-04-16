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

#ifndef HTTPMGR_H
#define HTTPMGR_H

#include <string>
#include <map>

#include <bcore/sbuf.h>
#include <bcore/logable.h>

#include "httpif.h"
#include "curlif.h"
#include "httpproc.h"

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

         /// Http interface.
         class httpDlManager: public httpInterface, public btg::core::Logable
         {
         public:
            /// Constructor.
            httpDlManager(btg::core::LogWrapperType _logwrapper);

            /// Download an URL.
            t_uint Fetch(std::string const& _url,
                         std::string const& _filename);

            /// Get status of a download.
            httpInterface::Status getStatus(const t_uint _id);

            /// Get the downloaded file.
            bool Result(const t_uint _id, 
                        btg::core::sBuffer & _buffer);

            /// Terminate a download.
            void Terminate(const t_uint _id);

            /// Destructor.
            virtual ~httpDlManager();
         protected:
            /// Terminate all downloads.
            void TerminateAll();

            /// Get the current ID - Fetch returns this ID each time
            /// its called.
            t_uint getCurrentId();

            /// The current ID.
            t_uint         current_id;
            /// The max ID - used to decide when to wrap.
            const t_uint   max_id;

            /// Threads used to download.
            std::map<t_uint, boost::shared_ptr<httpProcess> > processes;
         };

         /** @} */

      } // namespace http
   } // namespace daemon
} // namespace btg

#endif // HTTPMGR_H


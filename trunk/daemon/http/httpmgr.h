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

#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

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
         class httpManager: public httpInterface, public btg::core::Logable
         {
         public:
            /// Constructor.
            httpManager(btg::core::LogWrapperType _logwrapper);

            t_uint Fetch(std::string const& _url,
                         std::string const& _filename);

            httpInterface::Status getStatus(const t_uint _id);

            bool Result(const t_uint _id, 
                        btg::core::sBuffer & _buffer);

            void Terminate(const t_uint _id);

            /// Destructor.
            virtual ~httpManager();
         protected:
            /// Terminate all downloads.
            void TerminateAll();

            t_uint getCurrentId();

            t_uint         current_id;
            const t_uint   max_id;

            std::map<t_uint, boost::shared_ptr<httpProcess> > processes;
         };

         /** @} */

      } // namespace http
   } // namespace daemon
} // namespace btg

#endif // HTTPMGR_H


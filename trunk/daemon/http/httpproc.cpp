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

#include "httpproc.h"
#include <boost/bind.hpp>
#include <daemon/modulelog.h>

namespace btg
{
   namespace daemon
   {
      namespace http
      {
         const std::string moduleName("htpr");

         httpProcess::httpProcess(btg::core::LogWrapperType _logwrapper,
                                  std::string _URL, 
                                  std::string _filename)
            : btg::core::Logable::Logable(_logwrapper),
              URL(_URL),
              filename(_filename),
              status(httpInterface::INIT),
              ci(logWrapper(), *this),
              terminate(false),
              thread(
                     boost::bind(&httpProcess::work, boost::ref(*this))
                     )
         {
            BTG_MNOTICE(logWrapper(), "constructed")
         }

         void httpProcess::work()
         {
            {
               boost::mutex::scoped_lock interface_lock(interfaceMutex);
               status = httpInterface::WAIT;
            }

            if (ci.Get(URL, filename))
               {
                  // Got the URL, saved to file.
                  {
                     boost::mutex::scoped_lock interface_lock(interfaceMutex);
                     status = httpInterface::FINISH;
                  }
               }
            else
               {
                  {
                     boost::mutex::scoped_lock interface_lock(interfaceMutex);
                     status = httpInterface::ERROR;
                  }
               }
         }

         httpInterface::Status httpProcess::Status()
         {
            boost::mutex::scoped_lock interface_lock(interfaceMutex);

            return status;
         }

         bool httpProcess::Result(btg::core::sBuffer & _buffer)
         {
            boost::mutex::scoped_lock interface_lock(interfaceMutex);
            if (status != httpInterface::FINISH)
               {
                  return false;
               }
            
            return ci.Result(_buffer);
         }

         void httpProcess::Terminate()
         {
            terminate = true;
         }
         
         bool httpProcess::AbortTransfer() const
         {
            return terminate;
         }

         httpProcess::~httpProcess()
         {
            thread.join();
            BTG_MNOTICE(logWrapper(), "destroyed");
         }

      } // namespace http
   } // namespace daemon
} // namespace btg




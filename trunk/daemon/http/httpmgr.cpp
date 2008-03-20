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

#include "httpmgr.h"
#include <boost/bind.hpp>
#include <daemon/modulelog.h>

namespace btg
{
   namespace daemon
   {
      namespace http
      {

         const std::string moduleName("hmgr");
         const t_uint min_id = 1;

         httpManager::httpManager(btg::core::LogWrapperType _logwrapper)
         : btg::core::Logable(_logwrapper),
           current_id(min_id),
           max_id(0xFFFFFF),
           processes()
         {
            BTG_MNOTICE(logWrapper(), "constructed");
         }

         t_uint httpManager::getCurrentId()
         {
            t_uint id = current_id;
            current_id++;

            if (id > max_id)
               {
                  current_id = min_id;
               }

            return id;
         }

         t_uint httpManager::Fetch(std::string const& _url,
                                   std::string const& _filename)
         {
            t_uint id = getCurrentId();

            BTG_MNOTICE(logWrapper(), "Fetch(" << id << "): " << _url << ", into " << _filename);

            boost::shared_ptr<httpProcess> p(new httpProcess(logWrapper(),
                                                             _url, 
                                                             _filename));
            std::pair<t_uint, boost::shared_ptr<httpProcess> > pair(id, p);

            processes.insert(pair);

            return id;
         }

         httpInterface::Status httpManager::getStatus(const t_uint _id)
         {
            std::map<t_uint, boost::shared_ptr<httpProcess> >::const_iterator i = 
               processes.find(_id);

            if (i != processes.end())
               {
                  httpInterface::Status s = i->second->Status();
                  BTG_MNOTICE(logWrapper(), "Status(" << _id << "): " << s);
                  return s;
               }

            BTG_MNOTICE(logWrapper(), "Status(" << _id << "): httpInterface::ERROR");
            return httpInterface::ERROR;
         }

         bool httpManager::Result(const t_uint _id, 
                                  btg::core::sBuffer & _buffer)
         {
            std::map<t_uint, boost::shared_ptr<httpProcess> >::iterator i = 
               processes.find(_id);

            if (i != processes.end())
               {
                  bool status = i->second->Result(_buffer);

                  processes.erase(i);

                  BTG_MNOTICE(logWrapper(), "Result(" << _id << "): " << status);
                  return status;
               }

            BTG_MNOTICE(logWrapper(), "Result(" << _id << "): false");
            return false;
         }

         void httpManager::Terminate(const t_uint _id)
         {
            std::map<t_uint, boost::shared_ptr<httpProcess> >::const_iterator i = 
               processes.find(_id);

            if (i != processes.end())
               {
                  i->second->Terminate();
               }
         }

         void httpManager::TerminateAll()
         {
            std::map<t_uint, boost::shared_ptr<httpProcess> >::const_iterator i; 
            for (i = processes.begin();
                 i != processes.end();
                 i++)
               {
                  Terminate(i->first);
               }
         }

         httpManager::~httpManager()
         {
            TerminateAll();
            processes.erase(processes.begin(), processes.end());
         }

      } // namespace http
   } // namespace daemon
} // namespace btg




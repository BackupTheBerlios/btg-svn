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
 * $Id$
 */

#include "callbackmgr.h"
#include <bcore/os/exec.h>
#include <bcore/logmacro.h>
#include <bcore/os/sleep.h>

#include "modulelog.h"

#include <boost/bind.hpp>

#include <bcore/verbose.h>

namespace btg
{
   namespace daemon
   {

      const std::string moduleName("cbk");

      callbackManager::callbackManager(btg::core::LogWrapperType _logwrapper,
                                       bool const _verboseFlag)
         : btg::core::Logable(_logwrapper),
           verboseFlag(_verboseFlag),
           interfaceMutex_(),
           interfaceCondition_(),
           die(false),
           thread(
                  boost::bind(&callbackManager::work, boost::ref(*this))
                  )
      {
         eventNameMap[CBM_UNDEF]    = "ERROR";
         eventNameMap[CBM_ADD]      = "ADD";
         eventNameMap[CBM_REM]      = "REMOVE";
         eventNameMap[CBM_FINISHED] = "FINISHED";
         eventNameMap[CBM_SEED]     = "SEEDING";
         eventNameMap[CBM_CLEAN]    = "CLEAN";
      }

      bool callbackManager::add(std::string const& _username,
                                std::string const& _callback)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         bool status = false;

         std::pair<std::string, std::string> p;

         if (!btg::core::os::Exec::isExecutable(_callback))
            {
               BTG_MNOTICE(logWrapper(), "callback is not executable");
               return status;
            }

         p.first  = _username;
         p.second = _callback;

         std::map<std::string, std::string>::iterator iter;

         iter = userCallbackMap.find(_username);

         if (iter == userCallbackMap.end())
            {
               BTG_MNOTICE(logWrapper(), "added callback '" << _callback << "' for user '" << _username << "'");
               userCallbackMap.insert(p);
               status = true;
            }
         else
            {
               BTG_MNOTICE(logWrapper(), "unable to add callback '" << _callback << "' for user '" << _username << "'");
            }

         return status;
      }

      bool callbackManager::remove(std::string const& _username)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         bool status = false;

         std::map<std::string, std::string>::iterator iter;

         iter = userCallbackMap.find(_username);

         if (iter != userCallbackMap.end())
            {
               userCallbackMap.erase(iter);
               status = true;
            }

         return status;
      }

      void callbackManager::event(std::string const& _username,
                                  EVENT const _event,
                                  std::vector<std::string> const& _arguments)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         threadData td;
         td.username = _username;
         td.event    = _event;
         td.arguments = _arguments;

         threaddata.push_back(td);

         //interfaceCondition_.notify_one();
      }

      void callbackManager::work()
      {
         while (!die)
            {
               {
                  boost::mutex::scoped_lock interface_lock(interfaceMutex_);

                  // interfaceCondition_.wait(interface_lock);

                  work_pop();
               }
               
               // btg::core::os::Sleep::sleepMiliSeconds(64);
               btg::core::os::Sleep::sleepSecond(1);
            }
      }

      void callbackManager::work_pop()
      {
         for (std::vector<threadData>::const_iterator tdIter = threaddata.begin();
              tdIter != threaddata.end();
              tdIter++)
            {
               threadData const td = *tdIter;

                BTG_MNOTICE(logWrapper(), "user '" << td.username << "'.");

               std::map<std::string, std::string>::const_iterator iter = 
                  userCallbackMap.find(td.username);

               if (iter != userCallbackMap.end())
                  {
                     std::string callback = iter->second;
                     BTG_MNOTICE(logWrapper(), "executing '" << callback << "':");

                     std::vector<std::string> arguments;

                     std::string eventname;

                     switch (td.event)
                        {
                        case CBM_ADD:
                        case CBM_REM:
                        case CBM_FINISHED:
                        case CBM_SEED:
                        case CBM_CLEAN:
                           eventname = eventNameMap[td.event];
                           break;
                        default:
                           {
                              BTG_MNOTICE(logWrapper(), "unknown event");
                              continue;
                           }
                        }

                     BTG_MNOTICE(logWrapper(), "eventname '" << eventname << "'");

                     arguments.push_back(eventname);

                     std::vector<std::string>::const_iterator argIter;

                     for (argIter = td.arguments.begin();
                          argIter != td.arguments.end();
                          argIter++)
                        {
                           BTG_MNOTICE(logWrapper(), "arg: '" << *argIter << "'");
                           
                           arguments.push_back(*argIter);
                        }

                     if (btg::core::os::Exec::execFile(callback, arguments))
                        {
                           MVERBOSE_LOG(logWrapper(), 
                                        verboseFlag, "Executed " << 
                                        eventname << " for user " << td.username << ".");
                        }
                     else
                        {
                           BTG_MNOTICE(logWrapper(), "execution of callback '" << callback << "' failed");
                        }
                     
                     BTG_MNOTICE(logWrapper(), "execution of callback '" << callback << "' succeded");
                  }
               else
                  {
                     BTG_MNOTICE(logWrapper(), "unable to execute callback for user '" << td.username << "'");
                  }
            }
         // Clear data used by the thread.
         threaddata.clear();
      }

      callbackManager::~callbackManager()
      {
         {
            boost::mutex::scoped_lock interface_lock(interfaceMutex_);
            die = true;
            BTG_MNOTICE(logWrapper(), "stopped");
         }
         //interfaceCondition_.notify_one();

         thread.join();

         // Execute any callbacks left.
         work_pop();

         BTG_MNOTICE(logWrapper(), "destroyed");
      }

   } // namespace daemon
} // namespace btg


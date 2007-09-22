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

#include "modulelog.h"

namespace btg
{
   namespace daemon
   {

      const std::string moduleName("cbk");

      callbackManager::callbackManager()
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
         bool status = false;

         std::pair<std::string, std::string> p;

         if (!btg::core::os::Exec::isExecutable(_callback))
            {
               BTG_MNOTICE("callback is not executable");
               return status;
            }

         p.first  = _username;
         p.second = _callback;

         std::map<std::string, std::string>::iterator iter;

         iter = userCallbackMap.find(_username);

         if (iter == userCallbackMap.end())
            {
               BTG_MNOTICE("added callback '" << _callback << "' for user '" << _username << "'");
               userCallbackMap.insert(p);
               status = true;
            }
         else
            {
               BTG_MNOTICE("unable to add callback '" << _callback << "' for user '" << _username << "'");
            }

         return status;
      }

      bool callbackManager::remove(std::string const& _username)
      {
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
         std::map<std::string, std::string>::const_iterator iter;

         iter = userCallbackMap.find(_username);

         if (iter != userCallbackMap.end())
            {
               std::string callback = iter->second;
               BTG_MNOTICE("executing '" << callback << "':");

               std::vector<std::string> arguments;

               // arguments.push_back(callback);

               std::string eventname;

               switch (_event)
                  {
                  case CBM_ADD:
                  case CBM_REM:
                  case CBM_FINISHED:
                  case CBM_SEED:
                  case CBM_CLEAN:
                     eventname = eventNameMap[_event];
                     break;
                  default:
                     {
                        BTG_MNOTICE("unknown event");
                        return;
                     }
                  }

               BTG_MNOTICE("eventname '" << eventname << "'");

               arguments.push_back(eventname);

               std::vector<std::string>::const_iterator argIter;

               for (argIter = _arguments.begin();
                    argIter != _arguments.end();
                    argIter++)
                  {
                     BTG_MNOTICE("arg: '" << *argIter << "'");

                     arguments.push_back(*argIter);
                  }

               if (!btg::core::os::Exec::execFile(callback, arguments))
                  {
                     BTG_MNOTICE("execution of callback '" << callback << "' failed");
                     return;
                  }

               BTG_MNOTICE("execution of callback '" << callback << "' succeded");
            }
         else
            {
               BTG_MNOTICE("unable to execute callback for user '" << _username << "'");
            }
      }

      callbackManager::~callbackManager()
      {

      }

   } // namespace daemon
} // namespace btg


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

#include "loglevel.h"
#include "carg.h"

namespace btg
{
   namespace core
   {
      namespace client
      {

         void setDefaultLogLevel(btg::core::LogWrapperType _logwrapper, 
                                 const bool _debugFlag,
                                 const bool _verboseFlag)
         {
            using namespace btg::core::logger;

#if BTG_DEBUG
            // Set debug logging, if requested.
            if (_debugFlag)
               {
                  _logwrapper->setMinMessagePriority(logWrapper::PRIO_DEBUG);
               }
            else if (_verboseFlag)
               {
                  _logwrapper->setMinMessagePriority(logWrapper::PRIO_VERBOSE);
               }
            else
               {
                  // Default: only report errors.
                  _logwrapper->setMinMessagePriority(logWrapper::PRIO_ERROR);
               }
#else
            // No debug enabled, check for verbose flag.
            if (_verboseFlag)
               {
                  _logwrapper->setMinMessagePriority(logWrapper::PRIO_VERBOSE);
               }
            else
               {
                  // Default: only report errors.
                  _logwrapper->setMinMessagePriority(logWrapper::PRIO_ERROR);
               }
#endif // BTG_DEBUG

         }

      } // namespace client
   } // namespace core
} // namespace btg





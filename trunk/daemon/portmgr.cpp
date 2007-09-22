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

#include "portmgr.h"
#include <bcore/verbose.h>

namespace btg
{
   namespace daemon
   {
      const std::string portMgrName("pmr:");

      portManager::portManager(bool const _verboseFlag,
                               std::pair<t_uint, t_uint> const& _port_range)
         : verboseFlag_(_verboseFlag),
           ports_()
      {
         VERBOSE_LOG(verboseFlag_, portMgrName << " using ports " << _port_range.first << ":" << _port_range.second << ".");

         for (t_uint counter = _port_range.first;
              counter <= _port_range.second;
              counter++)
            {
               ports_.push_back(counter);
            }

         VERBOSE_LOG(verboseFlag_, portMgrName << " available ports: " << ports_.size() << ".");
      }

      bool portManager::available(t_uint const _numberOfPorts) const
      {
         if (ports_.size() >= _numberOfPorts)
            {
               return true;
            }

         VERBOSE_LOG(verboseFlag_, portMgrName << " unable to get " << _numberOfPorts << " port(s).");
         return false;
      }

      bool portManager::get(std::pair<t_uint, t_uint> & _port)
      {
         if (ports_.size() == 0)
            {
               VERBOSE_LOG(verboseFlag_, portMgrName << " unable to get port.");
               return false;
            }

         _port.first  = *(ports_.begin());
         _port.second = *(ports_.begin());
         ports_.pop_front();

         VERBOSE_LOG(verboseFlag_, portMgrName << " got port: " << _port.first << ".");

         return true;
      }

      void portManager::giveBack(std::pair<t_uint, t_uint> const& _port)
      {
         VERBOSE_LOG(verboseFlag_, portMgrName << " returned port: " << _port.first << ".");

         ports_.push_back(_port.first);
      }

      portManager::~portManager()
      {
         ports_.clear();
      }

   } // namespace daemon
} // namespace btg


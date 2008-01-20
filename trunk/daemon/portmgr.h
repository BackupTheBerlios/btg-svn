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

#ifndef PORTMGR_H
#define PORTMGR_H

#include <bcore/type.h>
#include <bcore/logable.h>

#include <list>

namespace btg
{
   namespace daemon
      {

         /**
          * \addtogroup daemon Daemon
          */
         /** @{ */

         /// Keep track of which ports are used.
         class portManager: public btg::core::Logable
            {
            public:
               /// Constructor.
               /// Set up this instance with a port range.
               /// @param [in] _logwrapper  Pointer used to send logs to.
               /// @param [in] _verboseFlag Indicates if this class should do verbose logging.
               /// @param [in] _port_range  The port range to use.
               portManager(btg::core::LogWrapperType _logwrapper,
                           bool const _verboseFlag, 
                           std::pair<t_uint, t_uint> const& _port_range);

               /// Indicate of a number of ports is available.
               /// @param [in] numberOfPorts The number of ports that will be checked for availability.
               /// @return True - numberOfPorts ports available, false otherwise.
               bool available(t_uint const numberOfPorts = 2) const;

               /// Get a port.
               /// @param [out] _port Reference to a port.
               /// @return True - success. False - otherwise.
               bool get(std::pair<t_uint, t_uint> & _port);

               /// Return a port.
               /// @param [out] _port Port to return.
               void giveBack(std::pair<t_uint, t_uint> const& _port);

               /// Destructor.
               ~portManager();
            private:
               /// Indicates that verbose logging should be performed.
               bool const verboseFlag_;

               /// Contained ports.
               std::list<t_uint> ports_;
            };

         /** @} */

      } // namespace daemon
} // namespace btg

#endif // PORTMGR_H


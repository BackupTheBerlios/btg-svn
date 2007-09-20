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
 * $Id: transport.cpp,v 1.1.2.9 2006/08/05 23:21:10 wojci Exp $
 */

#include "transport.h"

namespace btg
{
   namespace core
   {

      messageTransport::messageTransport(btg::core::externalization::Externalization* _e,
                                         t_int const _buffer_size,
                                         DIRECTION const _direction,
                                         t_uint const _timeout)

         : externalization(_e),
           direction(_direction),
           initialized(true),
           buffer_size(_buffer_size),
           timeout(_timeout),
           dead_connections(0)
      {
      }

      t_intList messageTransport::getDeadConnections()
      {
         t_intList lst = dead_connections;
         dead_connections.clear();
         return lst;
      }

      messageTransport::~messageTransport()
      {
      }
   } // namespace core
} // namespace btg

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
 * The original class was from an article by Rob Tougher,
 * Linux Socket Programming In C++,
 * http://www.linuxgazette.com/issue74/tougher.html#1.
 */

/*
 * $Id$
 */

#ifndef TCPC_H
#define TCPC_H

#include "socket.h"
#include <bcore/externalization/externalization.h>

namespace btg
{
   namespace core
      {
         namespace os
            {
               /**
                * \addtogroup OS
                */
               /** @{ */

               /// A Client socket.
               class ClientSocket : public Socket
                  {
                  public:
                     /// Constructor.
                     ClientSocket(LogWrapperType _logwrapper,
                                  std::string const& _host, 
                                  t_uint const _port);

                     /// Write a byte array.
                     /// @return True - success. False - failture.
                     bool write(t_byteCP _msg, t_int const _size);

                     /// Read, store bytes in an array.
                     /// @return Number of bytes read.
                     t_int read(t_byteP _msg, t_int const _size);

                     /// Destructor.
                     virtual ~ClientSocket(){};
                  };

               /** @} */

            } // namespace os
      } // namespace core
} // namespace btg

#endif

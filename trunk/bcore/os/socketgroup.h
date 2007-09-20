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
 * $Id: socketgroup.h,v 1.1.2.5 2006/10/08 08:01:27 wojci Exp $
 */

#ifndef SOCKETGROUP_H
#define SOCKETGROUP_H

#include "socket.h"

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

               /// A class to group sockets together for doing
               /// select() on them.
               class SocketGroup
                  {
                  public:
                     /// Default constructor.
                     SocketGroup();

                     /// Constructor.
                     /// @param [in] _timeout Max time in milliseconds to block on doSelect().
                     SocketGroup(const t_uint _timeout);

                     /// Set timeout
                     /// @param [in] _timeout Max time in milliseconds to block on doSelect()
                     void setTimeout(const t_uint _timeout);

                     /// Return the timeout used an instance of this
                     /// class.
                     t_uint getTimeout() const;

                     /// Execute select() on one socket.
                     /// Used by clients which only have one socket.
                     ///
                     /// Also used to find out if a certain socket has
                     /// more data that can be read.
                     ///
                     /// @param [in] _socket The socket used for the call to select();
                     /// \return True - data can be read from the socket, false otherwise.
                     bool doSelect(const Socket* _socket);

                     /// Execute select() on all the contained sockets.
                     ///
                     /// The results of this function shall be
                     /// extracted using the getNext() function.
                     ///
                     /// If there are sockets left from the last call
                     /// to doSelect(), this function just returns.
                     void doSelect();

                     //
                     // Functions to get the sockets that can be read.
                     //

                     /// Get the next socket that can be read.
                     /// This function makes sure that all contained
                     /// sockets get their _fair_ share of reading.
                     ///
                     /// \note This function will keep on returning
                     /// socket to read from until max_reads are returned.
                     ///
                     /// @param [in] _socket Reference to a pointer to a socket.
                     /// @return True - success. False - no more sockets to read.
                     bool getNext(Socket* & _socket);

                     /// Get the number of sockets that one can read
                     /// from.
                     t_uint size() const;

                     //
                     // Other functions.
                     //

                     /// Add socket.
                     /// @param [in] _s The socket to add.
                     bool addSocket(Socket *_s);

                     /// Remove socket.
                     /// @param [in] _s The socket to remove
                     bool removeSocket(const Socket *_s);

                     /// Remove all sockets.
                     bool clear();
                  private:
                     /// Setup fd_base and calculate the highest fd
                     /// (nfds).
                     void setupFdSet();

                     /// The max number of reads that can be done.
                     t_uint const max_reads;

                     /// The current number of reads / calls to the getNext() function.
                     t_uint current_reads;

                     /// A map of socket ids and their socket objects.
                     std::map<t_int, Socket*> sockets;

                     /// Frequence of reads.
                     std::map<t_int, t_uint>  socket_freq;

                     /// The max timeout to block on select.
                     t_uint                   timeout;

                     /// A basic fd_set with no fd's set. Is
                     /// copyed. Never select on this!
                     fd_set                   fd_base;

                     /// The nfds param to select(). The highest
                     /// socket id we know about + 1.
                     t_int                    nfds;

                     /// Indicates that select was called, and should
                     /// not be called again before all the sockets
                     /// are read.
                     bool                     select_done;

                     /// The result of the select operation. Least
                     /// read socket comes first.
                     std::vector<Socket*>     select_result;
                  };

               /** @} */
            } // namespace os
      } // namespace core
} // namespace btg

#endif

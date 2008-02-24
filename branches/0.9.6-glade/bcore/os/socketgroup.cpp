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

//#include "socket_exception.h"

#include "socketgroup.h"
#include "fdcopy.h"
#include <bcore/logmacro.h>

namespace btg
{
   namespace core
   {
      namespace os
      {

         t_uint const MAX_READS = 16;

         SocketGroup::SocketGroup()
            : max_reads(MAX_READS),
              current_reads(0),
              sockets(),
              socket_freq(),
              timeout(1000),
              nfds(0),
              select_done(false)
         {
#if BTG_TRANSPORT_DEBUG
            BTG_NOTICE("Created SocketGroup: max_reads = " <<
                       max_reads << ", timeout = " << timeout << ".");
#endif // BTG_TRANSPORT_DEBUG
            FD_ZERO(&fd_base);
         }

         SocketGroup::SocketGroup(const t_uint _timeout)
            : max_reads(MAX_READS),
              current_reads(0),
              sockets(),
              socket_freq(),
              timeout(_timeout),
              nfds(0),
              select_done(false)
         {
#if BTG_TRANSPORT_DEBUG
            BTG_NOTICE("Created SocketGroup: max_reads = " <<
                       max_reads << ", timeout = " << timeout << ".");
#endif // BTG_TRANSPORT_DEBUG
            FD_ZERO(&fd_base);
         }

         void SocketGroup::setTimeout(const t_uint _timeout)
         {
#if BTG_TRANSPORT_DEBUG
            BTG_NOTICE("tcpTransport::setTimeout = " <<_timeout);
#endif // BTG_TRANSPORT_DEBUG
            timeout = _timeout;
         }

         t_uint SocketGroup::getTimeout() const
         {
            return timeout;
         }

         bool SocketGroup::doSelect(const Socket* _socket)
         {
            bool result = false;

            t_int sockId = _socket->getSockId();

            // Guard against deleted sockets.
            if (sockId == -1)
               {
                  return result;
               }
#if BTG_TRANSPORT_DEBUG
            BTG_NOTICE("Calling select(), with socket.");
#endif // BTG_TRANSPORT_DEBUG
            fd_set fd_read;
            struct timeval tv;

            FD_COPY(&fd_base, &fd_read);
            t_uint t = timeout;
            // XXX: Is this correct? t is milliseconds... mS/1000 gives seconds..
            t = tv.tv_sec = t / 1000;
            // % 1000 gives rest in milliseconds
            t=t%1000;
            // and then *1000 gives microseconds
            tv.tv_usec = t * 1000;

            t_int stat;
            stat = select(nfds, &fd_read, NULL, NULL, &tv);

            if (stat <= 0)
               {
                  // Timeout or error.
                  return result;
               }

            // We got fds with data. Is it the one this function is
            // supposed to check?
            if (FD_ISSET(sockId, &fd_read))
               {
#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE("SocketGroup::doSelect, got data on #" << _socket->getSockId());
#endif // BTG_TRANSPORT_DEBUG
                  result = true;
               }

            return result;
         }

         void SocketGroup::doSelect()
         {
            t_uint result_size = select_result.size();

            if (select_done && (result_size == 0))
               {
                  // Nothing left to read from, select should be
                  // called again.
                  select_done = false;
               }

            if (select_done && (result_size > 0))
               {
#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE("Not calling select().");
#endif // BTG_TRANSPORT_DEBUG
                  return;
               }

#if BTG_TRANSPORT_DEBUG
            BTG_NOTICE("Calling select().");
#endif //BTG_TRANSPORT_DEBUG

            fd_set fd_read;
            struct timeval tv;

            FD_COPY(&fd_base, &fd_read);
            t_uint t   = timeout;
            // XXX: Is this correct? t is milliseconds... mS/1000 gives seconds..
            t          = tv.tv_sec = t / 1000;
            // % 1000 gives rest in milliseconds
            t          = t % 1000;
            // and then *1000 gives microseconds
            tv.tv_usec = t * 1000;

            t_int stat = select(nfds, &fd_read, NULL, NULL, &tv);

            if (stat <= 0 )
               {
                  // Timeout or error.
                  return;
               }

            // We got fds with data. Find out which.
            std::map<t_int, Socket*>::iterator iter;
            for (iter = sockets.begin();
                 iter != sockets.end();
                 iter++)
               {
                  if (FD_ISSET(iter->first, &fd_read))
                     {

#if BTG_TRANSPORT_DEBUG
                        BTG_NOTICE("SocketGroup::doSelect, got data on #" << iter->first);
#endif // BTG_TRANSPORT_DEBUG
                        socket_freq[iter->first] ++;

                        // To avoid overflows.
                        if (socket_freq[iter->first] > 1024)
                           {
                              socket_freq[iter->first] = 1;
                           }
                     }
                  else
                     {
                        socket_freq[iter->first] = 0;
                     }
               }

            // socket_freq now contains socket IDs and their
            // frequency. If the frequency is 0, it means that the
            // socket is not ready for reading.

            std::map<t_int, t_uint>::iterator freqIter;
            std::multimap<t_uint, t_int> sortedFreq;

            for (freqIter = socket_freq.begin();
                 freqIter != socket_freq.end();
                 freqIter++)
               {
                  if (freqIter->second > 0)
                     {
                        sortedFreq.insert( std::make_pair(freqIter->second, freqIter->first) );
                     }
               }

            // Convert the above to a vector, sorted by frequency.

#if BTG_TRANSPORT_DEBUG
            BTG_NOTICE("SocketGroup::doSelect, results:");
#endif // BTG_TRANSPORT_DEBUG

            std::multimap<t_uint, t_int>::iterator sortedFreqIter;
            for (sortedFreqIter = sortedFreq.begin();
                 sortedFreqIter != sortedFreq.end();
                 sortedFreqIter++)
               {
                  // Resolve socket id to socket pointer.
                  select_result.push_back(
                                          (sockets.find(sortedFreqIter->second))->second
                                          );

#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE("Socket #" << sortedFreqIter->second << 
                             ", freq = " << sortedFreqIter->first << ".");
#endif // BTG_TRANSPORT_DEBUG

               }

#if BTG_TRANSPORT_DEBUG
            BTG_NOTICE("SocketGroup::doSelect, end of results.");
#endif // BTG_TRANSPORT_DEBUG

            // select() will not be called again before all the
            // sockets which are available for reading are returned
            // using getNext().

            select_done = true;

            // Reset the counter used to limit the number of reads in
            // a row.
            current_reads = 0;
         }

         bool SocketGroup::addSocket(Socket *_s)
         {
#if BTG_TRANSPORT_DEBUG
            BTG_NOTICE("SocketGroup::addSocket, adding socket #" << _s->getSockId());
#endif // BTG_TRANSPORT_DEBUG
            sockets[_s->getSockId()]     = _s;
            socket_freq[_s->getSockId()] = 0;
            setupFdSet();
            return true;
         }

         bool SocketGroup::removeSocket(const Socket *_s)
         {
#if BTG_TRANSPORT_DEBUG
            BTG_NOTICE("SocketGroup::removeSocket, removing socket #" << _s->getSockId());
#endif // BTG_TRANSPORT_DEBUG
            std::map<t_int, Socket*>::iterator iter;
            for (iter = sockets.begin(); 
                 iter != sockets.end(); 
                 iter++)
               {
                  if (iter->second == _s)
                     {
#if BTG_TRANSPORT_DEBUG
                        BTG_NOTICE("SocketGroup::removeSocket, found " << (void*)iter->second << ", erasing it.");
#endif // BTG_TRANSPORT_DEBUG

                        // Erase the frequency.
                        std::map<t_int, t_uint>::iterator freq_iter = socket_freq.find(iter->first);
                        if (freq_iter != socket_freq.end())
                           {
                              socket_freq.erase(freq_iter);
                           }

                        // Erase the socket itself.
                        sockets.erase(iter);

                        // Invalidate the results.
                        // Force re-selecting the sockets.
                        select_done = false;
                        select_result.clear();

                        setupFdSet();
                        return true;
                     }
               }
#if BTG_TRANSPORT_DEBUG
            BTG_NOTICE("SocketGroup::removeSocket, failed to remove socket #" << _s->getSockId());
#endif // BTG_TRANSPORT_DEBUG
            return false;
         }

         bool SocketGroup::clear()
         {
            sockets.clear();
            socket_freq.clear();
            FD_ZERO(&fd_base);
            nfds=0;
            return true;
         }

         bool SocketGroup::getNext(Socket* & _socket)
         {
            bool status = false;

            // No call to select() was performed.
            if (!select_done)
               {
                  return status;
               }

            // No more sockets, enable selecting again.
            if (select_result.size() == 0)
               {
                  select_done = false;

                  return status;
               }

            // This function was called the max amount times in a row.
            if (current_reads > max_reads)
               {
                  current_reads = 0;

                  select_result.clear();
                  select_done = false;
#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE("Max reads reached.");
#endif // BTG_TRANSPORT_DEBUG
                  return status;
               }

            if (select_result.size() > 0)
               {
                  std::vector<Socket*>::iterator iter = select_result.begin();
                  _socket = *iter;
                  status  = true;
                  select_result.erase(iter);

                  current_reads++;
               }

            return status;
         }

         t_uint SocketGroup::size() const
         {
            return select_result.size();
         }

         void SocketGroup::setupFdSet()
         {
            nfds = 0;
            std::map<t_int, Socket*>::iterator iter;
            FD_ZERO(&fd_base);
            for (iter  = sockets.begin();
                 iter != sockets.end();
                 iter++)
               {
                  FD_SET(iter->first, &fd_base);

                  if (iter->first > nfds)
                     {
                        nfds = iter->first;
                     }
               }
            nfds++;
         }

      } // namespace os
   } // namespace core
} // namespace btg


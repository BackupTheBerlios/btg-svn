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

#ifndef HANDLERTHR_H
#define HANDLERTHR_H

#include <bcore/type.h>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/shared_ptr.hpp>

#include <bcore/os/timer.h>

#include <bcore/client/clienthandler.h>
#include <bcore/client/clientcallback.h>

namespace btg
{
   namespace core
      {
         namespace client
            {
               /**
                * \addtogroup client
                */
               /** @{ */

               class handlerThreadIf;

               /// Thread running the handler communicating with the daemon.
               class handlerThread
                  {
                  public:
                     /// Constructor.
                     handlerThread(bool const _verboseFlag,
                                   handlerThreadIf* _handler);

                     /// Get the mutex used by this thread, so that
                     /// other threads which wish to access the
                     /// handler can use it for locking.
                     boost::shared_ptr<boost::mutex> mutex();

                     /// Get a reference to the contained handler.
                     handlerThreadIf* handler() const;

                     /// Force an update of all contexts.
                     /// \note Resets the timer.
                     void forceUpdate();

                     /// Destructor.
                     ~handlerThread();
                  protected:
                     /// Indicates if this class shall be verbose.
                     bool const             verboseFlag_;

                     /// Mutex used to control access to the members
                     /// of this class from the outside.
                     boost::mutex           interfaceMutex_;

                     /// True if the thread should terminate itself.
                     bool                   die_;

                     /// Pointer to the handler.
                     handlerThreadIf*       handler_;

                     /// Timer used to detect when to do updates.
                     btg::core::os::Timer   timer_;

                     /// The thread used by this class.
                     boost::thread          thread_;

                     /// The thread is running this function.
                     void work();
                  };

               /// Client handler class. To be used with a
               /// handlerThread instance.
               class handlerThreadIf : 
                  public btg::core::client::clientCallback,
                  public btg::core::client::clientHandler
                  {
                  public:
                     /// Constructor.
                     handlerThreadIf(btg::core::externalization::Externalization* _e,
                                     btg::core::messageTransport*            _transport,
                                     btg::core::client::clientConfiguration* _config,
                                     btg::core::client::lastFiles*           _lastfiles,
                                     bool const _verboseFlag,
                                     bool const _autoStartFlag);

                     /// Indicates if status list was updated.
                     virtual bool statusListUpdated() const;

                     /// The number of status object received.
                     virtual t_uint statusSize() const;

                     /// Used when an operation was performed, which
                     /// deleted contexts.
                     /// 
                     /// Call this function to make sure that no old
                     /// contexts exist in the handler.
                     virtual void resetStatusList();

                     /// Get list of status objects.
                     virtual void getStatusList(t_statusList & _statusList);

                     /// Destructor.
                     virtual ~handlerThreadIf();
                  protected:
                     /// List of status objects.
                     t_statusList   statusList_;

                     /// Indicates that the status list was updated.
                     bool           statusListUpdated_;

                     /// Number of status objects received.
                     t_uint         statusSize_;

                     /// Last received list of peers.
                     // t_peerList     lastPeerlist_;
                  };

               /** @} */
            } // namespace clien
      } // namespace core
} // namespace btg

#endif // HANDLERTHR_H


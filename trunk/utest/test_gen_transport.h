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
 * $Id: test_gen_transport.h,v 1.1.2.9 2006/08/17 19:56:56 wojci Exp $
 */

#include <cppunit/extensions/HelperMacros.h>

#include <bcore/command/command.h>
#include <bcore/command_factory.h>
#include <bcore/util.h>
#include <bcore/dbuf.h>
#include <bcore/transport/transport.h>
#include <bcore/externalization/externalization.h>

class testGenericTransport
{
 public:
   testGenericTransport(btg::core::externalization::Externalization* _e,
                        btg::core::messageTransport* _mq_server,
                        btg::core::messageTransport* _mq_client);

   ~testGenericTransport();

   // Fill a buffer with some data.
   void fillData(btg::core::dBuffer & _output, const int _size) const;

   // Test functions from the messageQue class, not splitting data.
   void testSmallQue();

   // Test functions from the messageQue class, splitting data.
   void testLargeQue();

   // Test that messages can travel in both directions.
   void testTwoWayCommunication();

   // Send a number of messages.
   void testLimits();

   // Read from a transport, attempt several times and sleep
   // in-between.
   t_int readFromTransport(btg::core::messageTransport* _transport,
                           btg::core::dBuffer & _output,
                           t_int & _connectionID,
                           t_int const _attempts = 1,
                           t_int const _sleepInMs = 0);

 private:
   btg::core::dBuffer          buffer;
   t_long                      session;
   btg::core::messageTransport *mq_server;
   btg::core::messageTransport *mq_client;

   t_int                       readAttempts;
   t_int                       readSleepInMs;

   // After a client writes some data to the daemon, its not avaiable
   // as the daemon must accept the connection first. This function
   // must be called before expecting to be able to read any data from
   // the daemon.
   void readToAcceptConnection(btg::core::messageTransport* _transport);
};

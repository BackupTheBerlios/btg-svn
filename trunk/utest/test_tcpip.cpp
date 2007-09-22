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

#include "test_tcpip.h"
#include "test_gen_transport.h"
#include "testrunner.h"

#include <bcore/externalization/xmlrpc.h>

#include <bcore/transport/direction.h>

#include <cppunit/extensions/HelperMacros.h>



#include <string.h>
#include <vector>

#include <iostream> // for cout, which should not be used here.

#if BTG_UTEST_TCPIP
CPPUNIT_TEST_SUITE_REGISTRATION( testTcpIp );
#endif // BTG_UTEST_TCPIP

using namespace btg::core;

void testTcpIp::setUp()
{
   addressport     = btg::core::addressPort(127,0,0,1,15000);
   externalization = new btg::core::externalization::XMLRPC();
   mq_server       = new tcpTransport(externalization, 1024*10, FROM_SERVER, addressport, 1000);
   mq_client       = new tcpTransport(externalization, 1024*10, TO_SERVER, addressport, 1000);
}

void testTcpIp::tearDown()
{
   delete mq_server;
   mq_server = 0;
   delete mq_client;
   mq_client = 0;
   delete externalization;
   externalization = 0;
}

void testTcpIp::testSmallQue()
{
   testGenericTransport tgt(externalization, mq_server, mq_client);
   tgt.testSmallQue();
}

void testTcpIp::testLargeQue()
{
   testGenericTransport tgt(externalization, mq_server, mq_client);
   tgt.testLargeQue();
}

void testTcpIp::testTwoWayCommunication()
{
   testGenericTransport tgt(externalization, mq_server, mq_client);
   tgt.testTwoWayCommunication();
}

void testTcpIp::testLimits()
{
   testGenericTransport tgt(externalization, mq_server, mq_client);
   tgt.testLimits();
}


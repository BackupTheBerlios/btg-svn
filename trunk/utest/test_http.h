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

#include <cppunit/extensions/HelperMacros.h>

#include <bcore/transport/http.h>
#include <bcore/externalization/externalization.h>
#include <bcore/logable.h>

class testHttp: public CppUnit::TestFixture
{
   CPPUNIT_TEST_SUITE( testHttp );
   
   CPPUNIT_TEST( testSmallQue );
   CPPUNIT_TEST( testLargeQue );

   CPPUNIT_TEST( testTwoWayCommunication );
   CPPUNIT_TEST( testLimits );

   CPPUNIT_TEST_SUITE_END();

 public:
   // Used by the test system.
   void setUp();

   // Used by the test system.
   void tearDown();

   // Test functions from the messageQue class, not splitting data.
   void testSmallQue();

   // Test functions from the messageQue class, splitting data.
   void testLargeQue();

   // Test that messages can travel in both directions.
   void testTwoWayCommunication();

   // Send a number of messages.
   void testLimits();

   // void testInit();

 private:
   btg::core::dBuffer       buffer;
   btg::core::externalization::Externalization* externalization;
   btg::core::httpTransport* mq_server;
   btg::core::httpTransport* mq_client;
   btg::core::addressPort   addressport;
   btg::core::LogWrapperType logwrapper;
};

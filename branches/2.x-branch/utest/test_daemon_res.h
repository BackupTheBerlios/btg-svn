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

#include <bcore/command/command.h>
#include <bcore/command_factory.h>
#include <bcore/util.h>
#include <bcore/dbuf.h>
#include <bcore/logable.h>
#include <bcore/logable.h>

namespace btg
{
   namespace daemon
   {
      class fileTrack;
   }
}

class testDaemonRes : public CppUnit::TestFixture
{
   CPPUNIT_TEST_SUITE( testDaemonRes );

   CPPUNIT_TEST( testAllocationInf );

   CPPUNIT_TEST( testAllocation );

   CPPUNIT_TEST( testAllocation2 );

   CPPUNIT_TEST( testAllocation3 );

   CPPUNIT_TEST( testAllocation4 );

   CPPUNIT_TEST_SUITE_END();

 public:
   // Used by the test system.
   void setUp();
   // Used by the test system.
   void tearDown();

   void testAllocationInf();
   void testAllocation();
   void testAllocation2();
   void testAllocation3();
   void testAllocation4();

 private:
   btg::core::LogWrapperType logwrapper;
};

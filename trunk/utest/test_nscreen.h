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
 * $Id: test_nscreen.h,v 1.4 2005/06/05 18:56:47 wojci Exp $
 */

#include <cppunit/extensions/HelperMacros.h>

#include <UI/cli/nscreen.h>

class testNScreen : public CppUnit::TestFixture
{
   CPPUNIT_TEST_SUITE( testNScreen );

   CPPUNIT_TEST( creation );

   CPPUNIT_TEST_SUITE_END();

 public:
   // Used by the test system.
   void setUp();
   // Used by the test system.
   void tearDown();
   // Tests.

   // Creation.
   void creation();

   // Text output.
   void test_output();

 private:

   btg::UI::cli::ncursesScreen* nscreen;
};


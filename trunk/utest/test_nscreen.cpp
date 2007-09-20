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
 * $Id: test_nscreen.cpp,v 1.4 2005/06/05 18:56:47 wojci Exp $
 */

#include <cppunit/extensions/HelperMacros.h>

#include "test_nscreen.h"

#include <string>

#include <iostream> // for cout, which should not be used here.

CPPUNIT_TEST_SUITE_REGISTRATION( testNScreen );

using namespace btg::UI::cli;

void testNScreen::setUp()
{

}

void testNScreen::tearDown()
{

}

void testNScreen::creation()
{
   nscreen = new ncursesScreen(80);

   delete nscreen;
   nscreen = 0;
}

void testNScreen::test_output()
{
   nscreen = new ncursesScreen(79);

   for (int i=0; i<512; i++)
      {
         nscreen->setOutput(" @@@@@@@  @@@@@@@  @@@@@@@   @@@@@@@ @@@      @@@ ");
         nscreen->setOutput(" @@!  @@@   @@!   !@@       !@@      @@!      @@! ");
         nscreen->setOutput(" @!@!@!@    @!!   !@! @!@!@ !@!      @!!      !!@ ");
         nscreen->setOutput(" !!:  !!!   !!:   :!!   !!: :!!      !!:      !!: ");
         nscreen->setOutput(" :: : ::     :     :: :: :   :: :: : : ::.: : :   ");
         nscreen->setOutput("                                                  ");

         // nscreen->setOutput("test:\t0123456789\n\n", false);
      }
   delete nscreen;
   nscreen = 0;
}

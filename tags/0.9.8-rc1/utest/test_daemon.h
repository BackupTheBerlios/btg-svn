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

class testDaemon : public CppUnit::TestFixture
{
   CPPUNIT_TEST_SUITE( testDaemon );

   CPPUNIT_TEST( testFilter );

   CPPUNIT_TEST( testConfiguration );

#if BTG_OPTION_SAVESESSIONS
   // CPPUNIT_TEST( testSessionSaver );
#endif // BTG_OPTION_SAVESESSIONS

   CPPUNIT_TEST( testPortManager );

   CPPUNIT_TEST( testFileTracker );

   CPPUNIT_TEST( testCommandLineHandler );
#if BTG_OPTION_URL
   CPPUNIT_TEST( testHttpDownload );
#endif // BTG_OPTION_URL
   CPPUNIT_TEST_SUITE_END();

 public:
   // Used by the test system.
   void setUp();
   // Used by the test system.
   void tearDown();

   void testFilter();

   void testConfiguration();
#if BTG_OPTION_SAVESESSIONS
   void testSessionSaver();
#endif // BTG_OPTION_SAVESESSIONS

   void testPortManager();

   void testFileTracker();

   // Helper functions for testFileTracker.
   void addFiles(class btg::daemon::fileTrack* _ft,
                 std::string const& _file0,
                 std::string const& _user0,
                 std::vector<std::string> const& _files0,
                 std::string const& _file1,
                 std::string const& _user1,
                 std::vector<std::string> const& _files1,
                 std::string const& _file2,
                 std::string const& _user2,
                 std::vector<std::string> const& _files2);
                 
   void removeFiles(class btg::daemon::fileTrack* _ft,
                    std::string const& _file0,
                    std::string const& _user0,
                    std::vector<std::string> const& _files0,
                    std::string const& _file1,
                    std::string const& _user1,
                    std::vector<std::string> const& _files1,
                    std::string const& _file2,
                    std::string const& _user2,
                    std::vector<std::string> const& _files2);

   void testCommandLineHandler();
#if BTG_OPTION_URL
   void testHttpDownload();
#endif // BTG_OPTION_URL
 private:
   btg::core::LogWrapperType logwrapper;
};

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

#include <bcore/addrport.h>
#include <bcore/transport/transport.h>
#include <bcore/addrport.h>
#include <bcore/logger/logger.h>
#include <bcore/client/configuration.h>
#include <bcore/logable.h>

#include <string>

class testBcoreClient : public CppUnit::TestFixture
{
   CPPUNIT_TEST_SUITE( testBcoreClient );

   // CPPUNIT_TEST( testProgress );

   CPPUNIT_TEST( testConfiguration );
   CPPUNIT_TEST( testConfigurationLastOpenFiles );
   CPPUNIT_TEST( testCommandLineHandler );

   CPPUNIT_TEST_SUITE_END();

 public:
   // Used by the test system.
   void setUp();
   // Used by the test system.
   void tearDown();

   // void testProgress();

   void testConfiguration();
   void testConfigurationLastOpenFiles();
   void testCommandLineHandler();

 private:
   int                       contextId;

   btg::core::LogWrapperType logwrapper;

   void setConfigDefaults(btg::core::client::clientConfiguration* _config,
                          btg::core::messageTransport::TRANSPORT const _transport,
                          btg::core::logger::logBuffer::LOGGER_TYPE const _logger_type,
                          bool const _leech_mode,
                          std::string const& _logfile,
                          btg::core::addressPort const& _ap);

};

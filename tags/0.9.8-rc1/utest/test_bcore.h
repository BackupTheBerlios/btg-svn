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

#include <bcore/externalization/externalization.h>
#include <bcore/logable.h>

class testBcore : public CppUnit::TestFixture
{
   CPPUNIT_TEST_SUITE( testBcore );

   // Test encoding/decoding simple type using the simple
   // externalization.
   CPPUNIT_TEST( testUtil_simple_types );
   CPPUNIT_TEST( testUtil_str_nocasecmp );
   CPPUNIT_TEST( testDbuffer );
   CPPUNIT_TEST( testSbuffer );
   CPPUNIT_TEST( testStatus );


   /* Tests of the commands. */
   /* Added as new commands are added, as I am lazy. */

   CPPUNIT_TEST( testCommand );
   CPPUNIT_TEST( testCommandNames );

   CPPUNIT_TEST( testListCommand );
   CPPUNIT_TEST( testListCommandResponse );
   CPPUNIT_TEST( testErrorCommand );
   CPPUNIT_TEST( testListContextCommand );
   CPPUNIT_TEST( testListSessionResponseCommand );
   CPPUNIT_TEST( testAttachSessionCommand );

   CPPUNIT_TEST( testContextStatusResponseCommand );
   CPPUNIT_TEST( testContextAllStatusResponseCommand );

   CPPUNIT_TEST( testContextCleanCommand );
   CPPUNIT_TEST( testContextCleanResponseCommand );

   CPPUNIT_TEST( testHumanReadableUnit );
   CPPUNIT_TEST( testHumanReadableTime );

   CPPUNIT_TEST( testFileInfo );

   CPPUNIT_TEST( testFileInfoResponseCommand );

   CPPUNIT_TEST( testSelectedFileEntry );
   CPPUNIT_TEST( testSelectedFileEntryList );

   CPPUNIT_TEST( testAddrPort );

   CPPUNIT_TEST( testPeer );

   CPPUNIT_TEST( testHash );

   CPPUNIT_TEST( testOsFileOp );

   CPPUNIT_TEST( testVersion );

   CPPUNIT_TEST( testXmlRpc );

   CPPUNIT_TEST( testXmlRpcCommands );

   CPPUNIT_TEST( stressTestXmlRpcCommands );

   CPPUNIT_TEST( testPIDFile );

   CPPUNIT_TEST_SUITE_END();

 public:
   // Used by the test system.
   void setUp();
   // Used by the test system.
   void tearDown();
   // Test functions from Util.
   void testUtil_simple_types();
   void testUtil_str_nocasecmp();

   // Test of dBuf.
   void testDbuffer();
   void testSbuffer();
   void testStatus();

   // Test commands, begin
   void testCommand();
   void testCommandNames();
   void testListCommand();
   void testListCommandResponse();
   void testErrorCommand();
   void testListContextCommand();
   void testListSessionResponseCommand();
   void testAttachSessionCommand();

   void testContextStatusResponseCommand();
   void testContextAllStatusResponseCommand();

   void testContextCleanCommand();
   void testContextCleanResponseCommand();

   void testHumanReadableUnit();
   void testHumanReadableTime();

   void testFileInfo();
   void testFileInfoResponseCommand();

   void testSelectedFileEntry();
   void testSelectedFileEntryList();

   // Test commands, end.

   void testAddrPort();

   void testPeer();

   void testHash();

   void testOsFileOp();

   void testVersion();

   // Test the encoding/decoding primitive types using XML-RPC.
   void testXmlRpc();

   // Test the encoding/decoding of commands using XML-RPC.
   void testXmlRpcCommands();

   // Stress test the encoding/decoding of commands using XML-RPC.
   void stressTestXmlRpcCommands();
   
   // test btg::core::os::PIDFile
   void testPIDFile();
 private:
   void createCommands(std::vector<btg::core::Command*> & commands);
   void destroyCommands(std::vector<btg::core::Command*> & commands);

   void XmlRpcSerializeDeserialize(btg::core::externalization::Externalization* _eSource,
                                   btg::core::externalization::Externalization* _eDestin);

   void testListCommandRange(t_uint _start, t_uint _end);
 private:
   btg::core::dBuffer                           buffer;
   btg::core::externalization::Externalization* externalization;
   btg::core::Command*                          command_pointer;
   btg::core::LogWrapperType                    logwrapper;
};

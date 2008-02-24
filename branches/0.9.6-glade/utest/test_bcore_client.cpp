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

#include "test_bcore_client.h"
#include "testrunner.h"

#include <bcore/type.h>

#include "files.h"

#include <bcore/client/lastfiles.h>
#include <bcore/client/clientdynconfig.h>
#include <bcore/client/carg.h>

#if BTG_UTEST_CLIENT
CPPUNIT_TEST_SUITE_REGISTRATION( testBcoreClient );
#endif // BTG_UTEST_CLIENT

using namespace btg::core;
using namespace std;

void testBcoreClient::setUp()
{
   contextId = 100;
}

void testBcoreClient::tearDown()
{
   contextId = -1;
}

/*
  void testBcoreClient::testProgress()
  {
  t_ulong filesize  = 100 * 1024 * 1024; // 100 MiB;
  t_ulong base_rate = 50 * 1024;         // 50 KiB per second.

  btg::core::client::clientProgress cp;

  cp.addContext(contextId, filesize);

  for (t_uint i=0; i<=100; i++)
  {
  cp.updateContext(contextId, base_rate);

  btg::core::client::timeSpecification ts = cp.getTime(contextId, i);
  }
  }
*/

void testBcoreClient::testConfiguration()
{
   using namespace btg::core::client;
   using namespace btg::core;
   using namespace btg::core::logger;

   string config_filename        = TESTFILE_CONFIG;

   clientConfiguration *config = new clientConfiguration(config_filename);

   // Defaults:

   // Set all values:
   messageTransport::TRANSPORT transport = messageTransport::TCP;
   logBuffer::LOGGER_TYPE logger_type  = logBuffer::LOGFILE;
   std::string logfile("/path/to/file");
   addressPort ap(127,0,0,1, 5000);
   bool leech_mode = true;

   setConfigDefaults(config, transport, logger_type,
                     leech_mode, logfile,
                     ap);

   // Write to a file:

   bool result = config->write(true /* Force a write. */);
   if (!result)
      {
         std::string errordescription;
         if (config->getErrorDescription(errordescription))
            {
               std::cout << errordescription << std::endl;
            }
      }

   CPPUNIT_ASSERT(result);

   delete config;
   config = 0;

   // Check the values which were read from the file.
   config = new clientConfiguration(config_filename);
   CPPUNIT_ASSERT(config->read());

   CPPUNIT_ASSERT(config->getTransport() == transport);
   CPPUNIT_ASSERT(config->getLog() == logger_type);
   CPPUNIT_ASSERT(config->getLogFile() == logfile);
   CPPUNIT_ASSERT(config->getDaemonAddressPort() == ap);
   CPPUNIT_ASSERT(config->getLeechMode() == leech_mode);

   delete config;
   config = 0;
}

void testBcoreClient::testConfigurationLastOpenFiles()
{
   using namespace std;
   using namespace btg::core::client;
   using namespace btg::core;
   using namespace btg::core::logger;

   string dynconfig_filename        = std::string(TESTFILE_CONFIG) + ".dynconfig";
   vector<string> last_files;

   last_files.push_back(TESTFILE_LAST_0);
   last_files.push_back(TESTFILE_LAST_1);
   last_files.push_back(TESTFILE_LAST_2);
   last_files.push_back(TESTFILE_LAST_3);
   last_files.push_back(TESTFILE_LAST_4);
   last_files.push_back(TESTFILE_LAST_5);
   last_files.push_back(TESTFILE_LAST_6);
   last_files.push_back(TESTFILE_LAST_7);
   last_files.push_back(TESTFILE_LAST_8);
   last_files.push_back(TESTFILE_LAST_9);
   last_files.push_back(TESTFILE_LAST_10);
   last_files.push_back(TESTFILE_LAST_11);

   clientDynConfig *cliDynConf = new clientDynConfig(dynconfig_filename);
   lastFiles *config = new lastFiles(*cliDynConf);

   config->setLastFiles(last_files);

   delete config;
   config = 0;
   delete cliDynConf;
   cliDynConf = 0;

   cliDynConf = new clientDynConfig(dynconfig_filename);
   config = new lastFiles(*cliDynConf);

   CPPUNIT_ASSERT(config->getLastFiles() == last_files);
   CPPUNIT_ASSERT(config->getLastFiles().size() == last_files.size());

   delete config;
   config = 0;
   delete cliDynConf;
   cliDynConf = 0;
}

void testBcoreClient::setConfigDefaults(btg::core::client::clientConfiguration* _config,
                                        btg::core::messageTransport::TRANSPORT const _transport,
                                        btg::core::logger::logBuffer::LOGGER_TYPE const _logger_type,
                                        bool const _leech_mode,
                                        std::string const& _logfile,
                                        btg::core::addressPort const& _ap)
{
   // Defaults:
   bool leech_mode = true;

   std::string logfile("/path/to/file");

   addressPort ap(127,0,0,1, 5000);

   // Set all values:
   _config->setTransport(_transport); // messageTransport::TCP);
   _config->setLog(_logger_type); // debugBuffer::LOGFILE);
   _config->setLogFile(_logfile);
   _config->setDaemonAddressPort(_ap);

   _config->setLeechMode(_leech_mode);
}

void testBcoreClient::testCommandLineHandler()
{
   btg::core::client::commandLineArgumentHandler* clah = new btg::core::client::commandLineArgumentHandler("client.ini", false);

   clah->setup();
#if BTG_DEBUG
   int argc = 6;
   char* args[argc];
   args[0] = "test_client";
   args[1] = "-A";
   args[2] = "-d 127.0.0.1:16001";
   args[3] = "-o test.torrent";
   args[4] = "--nostart";
   args[5] = "-D";
#else
   int argc = 5;
   char* args[argc];
   args[0] = "test_client";
   args[1] = "-A";
   args[2] = "-d 127.0.0.1:16001";
   args[3] = "-o test.torrent";
   args[4] = "--nostart";
#endif

   char** argv = &args[0];

   CPPUNIT_ASSERT(clah->parse(argc, argv));
   
   CPPUNIT_ASSERT(!clah->configFileSet());

   CPPUNIT_ASSERT(!clah->doList());
   CPPUNIT_ASSERT(!clah->doAttach());
   CPPUNIT_ASSERT(clah->doAttachFirst());
   CPPUNIT_ASSERT(clah->daemonSet());
   CPPUNIT_ASSERT(clah->inputFilenamesPresent());
   CPPUNIT_ASSERT(!clah->automaticStart());

#if BTG_DEBUG
   CPPUNIT_ASSERT(clah->doDebug());
#endif

   delete clah;
   clah = 0;
}

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
 * $Id: test_daemon.cpp,v 1.1.2.28 2007/08/08 21:43:36 wojci Exp $
 */

#include "test_daemon.h"
#include "testrunner.h"

#include <daemon/dconfig.h>
#if BTG_OPTION_SAVESESSIONS
#include <daemon/daemondata.h>
#include <daemon/eventhandler.h>
#include <daemon/session_list.h>
#include <daemon/sessionsaver.h>
#include <daemon/portmgr.h>
#endif // BTG_OPTION_SAVESESSIONS
#include "files.h"

extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
}

#include <daemon/portmgr.h>
#include <daemon/limitmgr.h>

#if BTG_UTEST_DAEMON
CPPUNIT_TEST_SUITE_REGISTRATION( testDaemon );
#endif // BTG_UTEST_DAEMON

void testDaemon::setUp()
{
   /* Make sure the dir for the test files exist */
   if(access(TESTFILE_BASE_DIR, W_OK) == -1)
      {
         /* Is this the correct way to bail? */
         CPPUNIT_ASSERT(mkdir(TESTFILE_BASE_DIR, 0777) != -1);
      }
}

void testDaemon::tearDown()
{

}

void testDaemon::testFilter()
{
   // Old filter test was here. Removed.
}

void testDaemon::testConfiguration()
{
   using namespace btg::daemon;
   using namespace btg::core;
   using namespace btg::core::logger;

   std::string logfile("/path/to/file");
   // std::string banfile("/path/to/ban_file");

   std::string torrent_output_dir("/path/to/torrent_output/dir");

#if BTG_OPTION_SAVESESSIONS
   std::string ss_filename("/path/to/ss_filename");
   bool ss_enabled(true);
#endif // BTG_OPTION_SAVESESSIONS

   std::pair<t_int, t_int> range;
   range.first  = 10024;
   range.second = 11240;

   Address lt_address(192,168,0,1);

   addressPort ap_listen(192,168,0,1,15000);
   addressPort ap_proxy(192,168,0,2,25000);

   daemonConfiguration* config = new daemonConfiguration(TESTFILE_DAEMON_CONFIG);

   config->setTransport(messageTransport::TCP);
   config->setLog(logBuffer::LOGFILE);
   config->setLogFile(logfile);
   config->setListenTo(ap_listen);
   config->setProxy(ap_proxy);
   config->setLTPortRange(range);
   config->setLTListenTo(lt_address);
#if BTG_OPTION_SAVESESSIONS
   config->setSSFilename(ss_filename);
   config->setSSEnable(ss_enabled);
#endif // BTG_OPTION_SAVESESSIONS

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

   config = new daemonConfiguration(TESTFILE_DAEMON_CONFIG);
   CPPUNIT_ASSERT(config->read());

   CPPUNIT_ASSERT(config->getTransport() == messageTransport::TCP);
   CPPUNIT_ASSERT(config->getLog() == logBuffer::LOGFILE);
   CPPUNIT_ASSERT(config->getLogFile() == logfile);
   CPPUNIT_ASSERT(config->getListenTo() == ap_listen);
   CPPUNIT_ASSERT(config->getProxy() == ap_proxy);
   // CPPUNIT_ASSERT(config->getBans() == banfile);
   CPPUNIT_ASSERT(config->getLTPortRange() == range);

#if BTG_OPTION_SAVESESSIONS
   CPPUNIT_ASSERT(config->getSSFilename() == ss_filename);
   CPPUNIT_ASSERT(config->getSSEnable() == ss_enabled);
#endif // BTG_OPTION_SAVESESSIONS

   delete config;
   config = 0;
}

#if BTG_OPTION_SAVESESSIONS
void testDaemon::testSessionSaver()
{
   using namespace btg::daemon;
   using namespace btg::core;
   using namespace btg::core::logger;

   std::pair<t_int, t_int> port_range(1024, 1024 + 20);
   portManager* portMgr = new portManager(false, port_range);
   limitManager* limitMgr = new limitManager(false, 10, -1, -1, -1, -1);

   sessionList sessionlist(false, 10);

   daemonData dd;
   /* Setting up a full environment is much... Too much?
      dd.tempDir = TESTFILE_BASE_DIR;
      dd.workDir = TESTFILE_BASE_DIR;
      dd.outputDir = TESTFILE_BASE_DIR;
      dd.torrentOutputDir = TESTFILE_BASE_DIR;
      dd.portRange = ;
      // Externalization and transport is never used.
      dd.externalization = new btg::core::externalization::Simple();
      dd.transport = new httpTransport(dd.externalization,
      100 * 1024,
      FROM_SERVER,
      ap,
      1000 // 1s timeout
      );
      dd.filetrack
      dd.filter = ;
   */

   SessionSaver *ss = new SessionSaver(false, *portMgr, *limitMgr, sessionlist, dd);
   CPPUNIT_ASSERT(ss->saveSessions(TESTFILE_DAEMON_SESSIONSAVE, false) == 1);

   delete ss;
   ss=0;

   ss = new SessionSaver(false, *portMgr, *limitMgr, sessionlist, dd);
   CPPUNIT_ASSERT(ss->loadSessions(TESTFILE_DAEMON_SESSIONSAVE) == 0);

   std::vector<t_long> sessions;

   sessionlist.getIds("test", sessions);

   CPPUNIT_ASSERT(sessions.size() == 0);

   delete ss;
   ss = 0;

   delete portMgr;
   portMgr = 0;

   delete limitMgr;
   limitMgr = 0;
}
#endif // BTG_OPTION_SAVESESSIONS

void testDaemon::testPortManager()
{
   using namespace btg::daemon;

   t_uint const portBase   = 1024;
   t_uint const portNumber = 100;

   std::pair<t_uint, t_uint> port_range(portBase, portBase+portNumber);

   portManager* portman = new portManager(false, port_range);

   for (t_uint counter = 0;
        counter < portNumber;
        counter++)
      {
         CPPUNIT_ASSERT(portman->available(1));

         std::pair<t_uint, t_uint> port;
         CPPUNIT_ASSERT(portman->get(port));
      }

   for (t_uint counter = 0;
        counter < portNumber;
        counter++)
      {
         std::pair<t_uint, t_uint> port(portBase+counter, portBase+counter);
         portman->giveBack(port);
      }

   for (t_uint counter = 0;
        counter < portNumber;
        counter++)
      {
         CPPUNIT_ASSERT(portman->available(1));

         std::pair<t_uint, t_uint> port;
         CPPUNIT_ASSERT(portman->get(port));
      }

   delete portman;
   portman = 0;
}

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

#include <daemon/filetrack.h>

#include <daemon/darg.h>
#include <bcore/logable.h>

#include <bcore/logger/logger.h>
#include <bcore/logger/console_log.h>

#if BTG_OPTION_URL
#  include <daemon/http/httpmgr.h>
#  include <bcore/os/sleep.h>
#endif // BTG_OPTION_URL

#if BTG_UTEST_DAEMON
CPPUNIT_TEST_SUITE_REGISTRATION( testDaemon );
#endif // BTG_UTEST_DAEMON

#include <iostream>

void testDaemon::setUp()
{
   /* Make sure the dir for the test files exist */
   if(access(TESTFILE_BASE_DIR, W_OK) == -1)
      {
         /* Is this the correct way to bail? */
         CPPUNIT_ASSERT(mkdir(TESTFILE_BASE_DIR, 0777) != -1);
      }

   logwrapper = btg::core::LogWrapperType(new btg::core::logger::logWrapper);
   logwrapper->setMinMessagePriority(btg::core::logger::logWrapper::PRIO_DEBUG);

   boost::shared_ptr<btg::core::logger::logStream> l(new btg::core::logger::logStream(new btg::core::logger::consoleLogger()));
   logwrapper->setLogStream(l);
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

   daemonConfiguration* config = new daemonConfiguration(logwrapper, 
                                                         TESTFILE_DAEMON_CONFIG);

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

   config = new daemonConfiguration(logwrapper,
                                    TESTFILE_DAEMON_CONFIG);
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
   portManager* portMgr = new portManager(logwrapper, false, port_range);
   limitManager* limitMgr = new limitManager(logwrapper, false, 10, -1, -1, -1, -1);

   sessionList sessionlist(logwrapper, false, 10);

   daemonData dd;
   /* Setting up a full environment is much... Too much?
      dd.tempDir = TESTFILE_BASE_DIR;
      dd.workDir = TESTFILE_BASE_DIR;
      dd.outputDir = TESTFILE_BASE_DIR;
      dd.torrentOutputDir = TESTFILE_BASE_DIR;
      dd.portRange = ;
      // Externalization and transport is never used.
      dd.externalization = new btg::core::externalization::XMLRPC();
      dd.transport = new httpTransport(dd.externalization,
      100 * 1024,
      FROM_SERVER,
      ap,
      1000 // 1s timeout
      );
      dd.filetrack
      dd.filter = ;
   */

   btg::core::os::fstream sf(TESTFILE_DAEMON_SESSIONSAVE, std::ios_base::out);

   SessionSaver *ss = new SessionSaver(logwrapper, false, *portMgr, *limitMgr, sessionlist, dd);
   CPPUNIT_ASSERT(ss->saveSessions(sf, false) == 1);

   delete ss;
   ss=0;

   ss = new SessionSaver(logwrapper, false, *portMgr, *limitMgr, sessionlist, dd);
   CPPUNIT_ASSERT(ss->loadSessions(sf) == 0);

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

   portManager* portman = new portManager(logwrapper, false, port_range);

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

void testDaemon::testFileTracker()
{
   using namespace btg::daemon;

   fileTrack* ft = new fileTrack(logwrapper);

   std::string file0("file0.torrent");
   std::string user0("user0");
   std::vector<std::string> files0;
   files0.push_back("dir0/a1");
   files0.push_back("dir1/b2");
   files0.push_back("dir2/c3");

   std::string file1("file1.torrent");
   std::string user1("user1");
   std::vector<std::string> files1;
   files1.push_back("dir3/d1");
   files1.push_back("dir4/e2");
   files1.push_back("dir5/f3");

   std::string file2("file2.torrent");
   std::string user2("user2");
   std::vector<std::string> files2;
   files2.push_back("dir6/g1");
   files2.push_back("dir7/h2");
   files2.push_back("dir8/i3");

   
   addFiles(ft,
            file0, user0, files0,
            file1, user1, files1,
            file2, user2, files2);

   removeFiles(ft,
               file0, user0, files0,
               file1, user1, files1,
               file2, user2, files2);

   addFiles(ft,
            file0, user0, files0,
            file1, user1, files1,
            file2, user2, files2);

   // Adding again.
   delete ft;
   ft = 0;

   // Adding entries to files.
}

void testDaemon::addFiles(btg::daemon::fileTrack* _ft,
                          std::string const& _file0,
                          std::string const& _user0,
                          std::vector<std::string> const& _files0,
                          std::string const& _file1,
                          std::string const& _user1,
                          std::vector<std::string> const& _files1,
                          std::string const& _file2,
                          std::string const& _user2,
                          std::vector<std::string> const& _files2)
{
   // Adding files.
   CPPUNIT_ASSERT(_ft->add(_user0, _file0));
   CPPUNIT_ASSERT(_ft->setFiles(_user0, _file0, _files0));
   CPPUNIT_ASSERT(_ft->add(_user0, _file1));
   CPPUNIT_ASSERT(_ft->setFiles(_user0, _file1, _files1));
   CPPUNIT_ASSERT(_ft->add(_user0, _file2));
   CPPUNIT_ASSERT(_ft->setFiles(_user0, _file2, _files2));

   // Adding the same file for the same user fails.
   CPPUNIT_ASSERT(!_ft->add(_user0, _file0));
   // Adding the same files fails.
   CPPUNIT_ASSERT(!_ft->setFiles(_user0, _file1, _files0));

   CPPUNIT_ASSERT(_ft->add(_user1, _file0));
   CPPUNIT_ASSERT(_ft->setFiles(_user1, _file0, _files0));
   CPPUNIT_ASSERT(_ft->add(_user1, _file1));
   CPPUNIT_ASSERT(_ft->add(_user1, _file2));

   CPPUNIT_ASSERT(_ft->add(_user2, _file0));
   CPPUNIT_ASSERT(_ft->setFiles(_user2, _file0, _files0));
   CPPUNIT_ASSERT(_ft->add(_user2, _file1));
   CPPUNIT_ASSERT(_ft->add(_user2, _file2));
}

void testDaemon::removeFiles(class btg::daemon::fileTrack* _ft,
                             std::string const& _file0,
                             std::string const& _user0,
                             std::vector<std::string> const& _files0,
                             std::string const& _file1,
                             std::string const& _user1,
                             std::vector<std::string> const& _files1,
                             std::string const& _file2,
                             std::string const& _user2,
                             std::vector<std::string> const& _files2)
{
   _ft->remove(_user0, _file0);
   _ft->remove(_user0, _file1);
   _ft->remove(_user0, _file2);

   _ft->remove(_user1, _file0);
   _ft->remove(_user1, _file1);
   _ft->remove(_user1, _file2);

   _ft->remove(_user2, _file0);
   _ft->remove(_user2, _file1);
   _ft->remove(_user2, _file2);
}

void testDaemon::testCommandLineHandler()
{
   btg::daemon::commandLineArgumentHandler* clah = new btg::daemon::commandLineArgumentHandler("daemon.ini");

   clah->setup();
#if BTG_DEBUG
   const int argc = 4;
   const char* coarg[argc] = {
      "test_daemon",
      "-v",
      "-n",
      "-D"
   };
   char* args[argc] = {0, 0, 0, 0};
#else
   const int argc = 3;
   const char* coarg[argc] = {
      "test_daemon",
      "-v",
      "-n"
   };
   char* args[argc] = {0, 0, 0};
#endif

   for (int count = 0;
        count < argc;
        count++)
      {
         int len = strlen(coarg[count]);
         args[count] = new char[len + 1];
         memset(args[count], 0, len+1);
         strncpy(args[count], coarg[count], len);
      }

   char** argv = &args[0];

   CPPUNIT_ASSERT(clah->parse(argc, argv));
   
   CPPUNIT_ASSERT(!clah->configFileSet());
   CPPUNIT_ASSERT(clah->doNotDetach());
   CPPUNIT_ASSERT(clah->beVerbose());

#if BTG_DEBUG
   CPPUNIT_ASSERT(clah->doDebug());
#endif

   delete clah;
   clah = 0;

   for (int count = 0;
        count < argc;
        count++)
      {
         delete [] args[count];
         args[count] = 0;
      }
}

#if BTG_OPTION_URL
// Note: these tests can fail if the required files are not present on
// the http server they are downloaded from.
void testDaemon::testHttpDownload()
{
   using namespace btg::daemon::http;
   using namespace btg::core::os;

   httpDlManager httpm(logwrapper);

   // Download a non existing file.
   t_uint serial = httpm.Fetch("http://127.0.0.1/test.torrent", "test.torrent");
   
   bool done = false;
   httpInterface::Status stat = httpInterface::ERROR;
   while (!done)
      {
         stat = httpm.getStatus(serial);
         if ((stat == httpInterface::FINISH) || (stat == httpInterface::ERROR))
            {
               done = true;
               break;
            }

         Sleep::sleepSecond(1);
      }
   CPPUNIT_ASSERT(stat == httpInterface::ERROR);

   btg::core::sBuffer buffer;
   CPPUNIT_ASSERT(!httpm.Result(serial, buffer));

   // Download a file which exists.
   serial = httpm.Fetch("http://127.0.0.1/test2.torrent", "test2.torrent");
   done = false;
   while (!done)
      {
         stat = httpm.getStatus(serial);
         if ((stat == httpInterface::FINISH) || (stat == httpInterface::ERROR))
            {
               done = true;
               break;
            }

         Sleep::sleepSecond(1);
      }
   CPPUNIT_ASSERT(stat == httpInterface::FINISH);

   btg::core::sBuffer buffer2;
   CPPUNIT_ASSERT(httpm.Result(serial, buffer2));
   CPPUNIT_ASSERT(buffer2.size() > 0);

   std::cout << "Got bytes: " << buffer2.size() << std::endl;
   
   CPPUNIT_ASSERT(unlink("test2.torrent") != -1);
}
#endif // BTG_OPTION_URL

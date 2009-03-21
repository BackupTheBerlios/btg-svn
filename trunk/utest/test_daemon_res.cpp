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

#include "test_daemon_res.h"
#include "testrunner.h"

#include <daemon/limitmgr.h>
#include <daemon/limitval.h>
#include <daemon/resource/allocate_resources.h>

#include <bcore/logable.h>
#include <bcore/logger/logger.h>
#include <bcore/logger/console_log.h>

#if BTG_UTEST_DAEMON
CPPUNIT_TEST_SUITE_REGISTRATION( testDaemonRes );
#endif // BTG_UTEST_DAEMON

#include <iostream>
#include <vector>

void testDaemonRes::setUp()
{
   logwrapper = btg::core::LogWrapperType(new btg::core::logger::logWrapper);
   logwrapper->setMinMessagePriority(btg::core::logger::logWrapper::PRIO_DEBUG);

   boost::shared_ptr<btg::core::logger::logStream> l(new btg::core::logger::logStream(new btg::core::logger::consoleLogger()));
   logwrapper->setLogStream(l);
}

void testDaemonRes::tearDown()
{

}

void createSessions(std::vector<btg::daemon::sessionData> & _sessions, int _number, int _rate)
{
   for (int i = 0; i < _number; i++)
      {
         btg::daemon::sessionData s(0);

         s.val_dl_rate.min       = 1;
         s.val_dl_rate.max       = _rate;
         s.val_dl_rate.given     = 0;
         s.val_dl_rate.leftovers = 0;
         
         s.val_ul_rate.min       = 1;
         s.val_ul_rate.max       = _rate;
         s.val_ul_rate.given     = 0;
         s.val_ul_rate.leftovers = 0;

         s.val_connections.min       = 1;
         s.val_connections.max       = _rate;
         s.val_connections.given     = 0;
         s.val_connections.leftovers = 0;

         s.val_uploads.min       = 1;
         s.val_uploads.max       = _rate;
         s.val_uploads.given     = 0;
         s.val_uploads.leftovers = 0;

         _sessions.push_back(s);
      }
}

void testDaemonRes::testAllocationInf()
{
   int numberSessions = 2;
   int reqLimit = 1024;
   int resources = btg::daemon::limitValue::inf;
   std::vector<btg::daemon::sessionData> sessions;
   createSessions(sessions, numberSessions, reqLimit);

   btg::daemon::allocate_resources(logwrapper,
                                   resources, 
                                   sessions,
                                   &btg::daemon::sessionData::val_dl_rate);

   for (std::vector<btg::daemon::sessionData>::iterator iter = sessions.begin();
        iter != sessions.end();
        iter++)
      {
         CPPUNIT_ASSERT((*iter).val_dl_rate.given == reqLimit);
      }
}

void testDaemonRes::testAllocation()
{
   int numberSessions = 5;
   int time = 60;
   int reqLimit = 1024;
   int resources = 10*reqLimit*time; // 10 KiB for one minute.
   std::vector<btg::daemon::sessionData> sessions;
   createSessions(sessions, numberSessions, reqLimit);

   btg::daemon::allocate_resources(logwrapper,
                                   resources, 
                                   sessions,
                                   &btg::daemon::sessionData::val_ul_rate);

   std::vector<btg::daemon::sessionData>::iterator iter = sessions.begin();

   // 60 seconds later..
   int less = reqLimit*time/2;

   iter->val_ul_rate.used = reqLimit*time;
   iter++;
   iter->val_ul_rate.used = (reqLimit*time)-less;
   iter++;
   iter->val_ul_rate.used = (reqLimit*time)-less;
   iter++;
   iter->val_ul_rate.used = (reqLimit*time)-less;
   iter++;
   iter->val_ul_rate.used = (reqLimit*time)-less;

   btg::daemon::allocate_resources(logwrapper,
                                   resources, 
                                   sessions,
                                   &btg::daemon::sessionData::val_ul_rate);

   createSessions(sessions, 1, reqLimit*2);

   iter->val_ul_rate.used = reqLimit*time;
   iter++;
   iter->val_ul_rate.used = (reqLimit*time)-less;
   iter++;
   iter->val_ul_rate.used = (reqLimit*time)-less;
   iter++;
   iter->val_ul_rate.used = (reqLimit*time)-less;
   iter++;
   iter->val_ul_rate.used = (reqLimit*time)-less;
   iter++;
   iter->val_ul_rate.used = (2*reqLimit*time);

   btg::daemon::allocate_resources(logwrapper,
                                   resources, 
                                   sessions,
                                   &btg::daemon::sessionData::val_ul_rate);
   int ul_given = 0;
   for (iter = sessions.begin();
        iter != sessions.end();
        iter++)
      {
         ul_given += (*iter).val_ul_rate.given;
      }

   CPPUNIT_ASSERT(ul_given <= resources);
}

void testDaemonRes::testAllocation2()
{
   int numberSessions = 5;
   int time = 60;
   int reqLimit = 1024;
   int resources = 10*reqLimit*time; // 10 KiB for one minute.
   std::vector<btg::daemon::sessionData> sessions;
   createSessions(sessions, numberSessions, reqLimit*time);

   std::vector<btg::daemon::sessionData>::iterator iter = sessions.begin();
   iter->val_ul_rate.max = 5*reqLimit*time;

   for (int counter = 0; counter < (10*1024); counter++)
      {
         iter = sessions.begin();
         iter->val_ul_rate.used = 5*reqLimit*time;
         iter++;
         iter->val_ul_rate.used = (counter*time);
         iter++;
         iter->val_ul_rate.used = (counter*time);
         iter++;
         iter->val_ul_rate.used = (counter*time);
         iter++;
         iter->val_ul_rate.used = (counter*time);

         btg::daemon::allocate_resources(logwrapper,
                                         resources, 
                                         sessions,
                                         &btg::daemon::sessionData::val_ul_rate);

         int ul_given = 0;
         for (iter = sessions.begin();
              iter != sessions.end();
              iter++)
            {
               ul_given += (*iter).val_ul_rate.given;
            }

         CPPUNIT_ASSERT(ul_given <= resources);
      }
}

void testDaemonRes::testAllocation3()
{
   int numberSessions = 5;
   int time = 60;
   int reqLimit = 1024;
   int resources = 10*reqLimit*time; // 10 KiB for one minute.
   std::vector<btg::daemon::sessionData> sessions;
   createSessions(sessions, numberSessions, reqLimit*time);

   std::vector<btg::daemon::sessionData>::iterator iter = sessions.begin();
   iter->val_ul_rate.max = 5*reqLimit*time;
   iter++;
   iter++;
   iter++;
   iter++;
   iter->val_ul_rate.max = 3*reqLimit*time;

   for (int counter = 0; counter < (1*1024); counter++)
      {
         iter = sessions.begin();
         iter->val_ul_rate.used = 5*reqLimit*time;
         iter++;
         iter->val_ul_rate.used = 0;
         iter++;
         iter->val_ul_rate.used = (counter*time);
         iter++;
         iter->val_ul_rate.used = 0;
         iter++;
         iter->val_ul_rate.used = 3*reqLimit*time;

         btg::daemon::allocate_resources(logwrapper,
                                         resources, 
                                         sessions,
                                         &btg::daemon::sessionData::val_ul_rate);

         int ul_given = 0;
         for (iter = sessions.begin();
              iter != sessions.end();
              iter++)
            {
               ul_given += (*iter).val_ul_rate.given;
            }

         CPPUNIT_ASSERT(ul_given <= resources);
      }
}

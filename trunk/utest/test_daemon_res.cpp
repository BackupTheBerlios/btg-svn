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

   iter->val_ul_rate.used += reqLimit*time;
   iter++;
   iter->val_ul_rate.used += (reqLimit*time)-less;
   iter++;
   iter->val_ul_rate.used += (reqLimit*time)-less;
   iter++;
   iter->val_ul_rate.used += (reqLimit*time)-less;
   iter++;
   iter->val_ul_rate.used += (reqLimit*time)-less;

   btg::daemon::allocate_resources(logwrapper,
                                   resources, 
                                   sessions,
                                   &btg::daemon::sessionData::val_ul_rate);

   createSessions(sessions, 1, reqLimit*2);

   iter->val_ul_rate.used += reqLimit*time;
   iter++;
   iter->val_ul_rate.used += (reqLimit*time)-less;
   iter++;
   iter->val_ul_rate.used += (reqLimit*time)-less;
   iter++;
   iter->val_ul_rate.used += (reqLimit*time)-less;
   iter++;
   iter->val_ul_rate.used += (reqLimit*time)-less;
   iter++;
   iter->val_ul_rate.used += (2*reqLimit*time);

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

   const int max_res = resources+(numberSessions*(1*resources/100));

   for (int counter = 0; counter < (1024); counter++)
      {
         iter = sessions.begin();
         iter->val_ul_rate.used += 5*reqLimit*time;
         iter++;
         iter->val_ul_rate.used += (counter*time);
         iter++;
         iter->val_ul_rate.used += (counter*time);
         iter++;
         iter->val_ul_rate.used += (counter*time);
         iter++;
         iter->val_ul_rate.used += (counter*time);

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

         //std::cout << "ul_given = " << ul_given << ":" << max_res << std::endl;

         CPPUNIT_ASSERT(ul_given <= max_res);
      }
}

void testDaemonRes::testAllocation3()
{
   int numberSessions = 5;
   int time = 60;
   int reqLimit = 1024;
   int resources = 100*reqLimit*time; // 100 KiB for one minute.
   std::vector<btg::daemon::sessionData> sessions;
   createSessions(sessions, numberSessions, reqLimit*time);

   const int max_res = resources+(numberSessions*(1*resources/100));

   std::vector<btg::daemon::sessionData>::iterator iter = sessions.begin();
   iter->val_ul_rate.max = 49*reqLimit*time;
   iter++;
   iter++;
   iter++;
   iter++;
   iter->val_ul_rate.max = 50*reqLimit*time;

   for (int counter = 0; counter < (1*1024); counter++)
      {
         iter = sessions.begin();
         iter->val_ul_rate.used = (48*reqLimit*time)+(counter/2);
         iter++;
         iter->val_ul_rate.used = 0;
         iter++;
         iter->val_ul_rate.used = (counter*time);
         iter++;
         iter->val_ul_rate.used = 0;
         iter++;
         iter->val_ul_rate.used = (49*reqLimit*time)+counter;

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

         int diff = resources - ul_given;
         // std::cout << "res = " << resources << ", given = " << ul_given << ", diff = " << diff << std::endl;
         // No more resources than the ones available were
         // distributed.
         CPPUNIT_ASSERT(ul_given <= max_res);
         // More or less all available resources were distributed.
         CPPUNIT_ASSERT(diff <= 10);
      }
}

void testDaemonRes::testAllocation4()
{
   /* download,
      upload,
      ignore,
      ignore
   */

   // Real values from 3 sessions.
   int bytes[] = 
      {
         /* 0 */
         7162
         ,0
         ,0
         ,10763
         ,0
         ,0
         ,82
         ,0
         ,0
         ,0
         ,0
         ,0
         /* 1 */
         ,42291
         ,29059
         ,0
         ,466856
         ,49576
         ,0
         ,3
         ,83
         ,0
         ,0
         ,0
         ,0
         ,11428
         ,29898
         ,0
         ,1312513
         ,1171739
         ,0
         ,0
         ,2
         ,0
         ,0
         ,1
         ,0
         ,1562743
         ,727941
         ,0
         ,3588803
         ,5488219
         ,0
         ,0
         ,0
         ,0
         ,21
         ,19
         ,0
         ,1760420
         ,1633224
         ,0
         ,3902900
         ,4355419
         ,0
         ,-2
         ,0
         ,0
         ,4
         ,12
         ,0
         ,1965955
         ,1788212
         ,0
         ,3494937
         ,5777621
         ,0
         ,2
         ,0
         ,0
         ,2
         ,0
         ,0
         ,1998588
         ,1823717
         ,0
         ,3239932
         ,5373301
         ,0
         ,0
         ,0
         ,0
         ,-2
         ,0
         ,0
         ,2023237
         ,1841013
         ,0
         ,3131017
         ,4292190
         ,0
         ,0
         ,0
         ,0
         ,1
         ,1
         ,0
         ,2089104
         ,1863496
         ,0
         ,2832310
         ,5072315
         ,0
         ,0
         ,0
         ,0
         ,0
         ,1
         ,0
         ,2219645
         ,1946559
         ,0
         ,3094622
         ,6197440
         ,0
         ,0
         ,0
         ,0
         ,0
         ,-1
         ,0
         ,2250485
         ,1992420
         ,0
         ,4807007
         ,5444529
         ,0
         ,0
         ,0
         ,0
         ,0
         ,0
         ,0
         ,2402194
         ,2117595
         ,0
         ,5445698
         ,4802837
         ,0
         ,0
         ,0
         ,0
         ,1
         ,-1
         ,0
         ,2361761
         ,2116424
         ,0
         ,4348259
         ,4658108
         ,0
         ,0
         ,0
         ,0
         ,-1
         ,0
         ,0
         ,2468330
         ,2176807
         ,0
         ,4820302
         ,4687705
         ,0
         ,-1
         ,-1
         ,0
         ,0
         ,0
         ,0
         ,2496744
         ,2278375
         ,0
         ,5341013
         ,5131825
         ,0
         ,1
         ,1
         ,0
         ,1
         ,3
         ,0
         ,2533614
         ,2431804
         ,0
         ,6719741
         ,4124653
         ,0
         ,0
         ,0
         ,0
         ,4
         ,3
         ,0
         ,2436024
         ,2420181
         ,0
         ,5810159
         ,3054796
         ,0
         ,-2
         ,-1
         ,0
         ,-1
         ,7
         ,0
         ,2452705
         ,2370636
         ,0
         ,5564586
         ,4533398
         ,0
         ,1
         ,1
         ,0
         ,-2
         ,2
         ,0
         ,2469087
         ,2401375
         ,0
         ,5401306
         ,3948162
         ,0
         ,-1
         ,0
         ,0
         ,0
         ,0
         ,0
         ,2566989
         ,2384470
         ,0
         ,2875827
         ,5327251
         ,0
         ,1
         ,0
         ,0
         ,1
         ,-1
         ,0
         ,2572153
         ,2402563
         ,0
         ,3007395
         ,6190814
         ,0
         ,0
         ,0
         ,0
         ,1
         ,1
         ,0
         ,2571331
         ,2403829
         ,0
         ,3142170
         ,6885275
         ,0
         ,0
         ,0
         ,0
         ,0
         ,-1
         ,0
         ,2614809
         ,2441653
         ,0
         ,2922847
         ,5346249
         ,0
         ,0
         ,0
         ,0
         ,0
         ,1
         ,0
         ,2622199
         ,2465057
         ,0
         ,3169642
         ,4412248
         ,0
         ,0
         ,0
         ,0
         ,0
         ,5
         ,0
         ,2621259
         ,2487877
         ,0
         ,3092793
         ,4331051
         ,0
         ,0
         ,0
         ,0
         ,0
         ,2
         ,0
         ,2631896
         ,2530538
         ,0
         ,2836786
         ,3866310
         ,0
         ,0
         ,0
         ,0
         ,-1
         ,-1
         ,0
         ,2645842
         ,2571479
         ,0
         ,2759170
         ,3381118
         ,0
         ,0
         ,0
         ,0
         ,0
         ,1
         ,0
         ,2642686
         ,2485915
         ,0
         ,2623248
         ,2579980
         ,0
         ,0
         ,-1
         ,0
         ,0
         ,1
         ,0
         ,2644816
         ,2600793
         ,0
         ,3015767
         ,2101161
         ,0
         ,0
         ,1
         ,0
         ,0
         ,-3
         ,0
         ,2637008
         ,2641744
         ,0
         ,2834238
         ,1959174
         ,0
         ,0
         ,0
         ,0
         ,0
         ,1
         ,0
         ,2563347
         ,2550849
         ,0
         ,2858123
         ,1876278
         ,0
         ,0
         ,-1
         ,0
         ,2
         ,1
         ,0
         ,2469943
         ,2574855
         ,0
         ,2748615
         ,2030244
         ,0
         ,0
         ,1
         ,0
         ,0
         ,1
         ,0
         ,2546835
         ,2550796
         ,0
         ,2808279
         ,2328907
         ,0
         ,0
         ,0
         ,0
         ,2
         ,-1
         ,0
         ,2623389
         ,2679153
         ,0
         ,3039726
         ,1824156
         ,0
         ,0
         ,0
         ,0
         ,-2
         ,0
         ,0
         ,2618612
         ,2670739
         ,0
         ,3307330
         ,2332088
         ,0
         ,0
         ,0
         ,0
         ,-2
         ,-2
         ,0
         ,2636268
         ,2650515
         ,0
         ,2933885
         ,2343739
         ,0
         ,0
         ,0
         ,0
         ,0
         ,0
         ,0
         ,2546427
         ,2579167
         ,0
         ,2623647
         ,1796174
         ,0
         ,0
         ,0
         ,0
         ,1
         ,0
         ,0
         ,2647936
         ,2693863
         ,0
         ,2901483
         ,2147726
         ,0
         ,0
         ,0
         ,0
         ,0
         ,0
         ,0
         ,2636931
         ,2593468
         ,0
         ,2649454
         ,1907857
         ,0
         ,0
         ,0
         ,0
         ,0
         ,0
         ,0
         ,2671100
         ,2672312
         ,0
         ,2572510
         ,1973579
         ,0
         ,0
         ,0
         ,0
         ,0
         ,1
         ,0
         ,2646699
         ,2683406
         ,0
         ,2660982
         ,2046115
         ,0
         ,0
         ,0
         ,0
         ,-2
         ,-3
         ,0
         ,2571300
         ,2690136
         ,0
         ,2301903
         ,2115637
         ,0
         ,0
         ,0
         ,0
         ,2
         ,1
         ,0
         ,2646288
         ,2690507
         ,0
         ,2523362
         ,1996209
         ,0
         ,0
         ,0
         ,0
         ,0
         ,-1
         ,0
         ,2583253
         ,2586443
         ,0
         ,2394788
         ,1714992
         ,0
         ,0
         ,0
         ,0
         ,-2
         ,-1
         ,0
         ,2595639
         ,2602390
         ,0
         ,2372480
         ,1981873
         ,0
         ,-1
         ,-1
         ,0
         ,0
         ,2
         ,0
      };
}

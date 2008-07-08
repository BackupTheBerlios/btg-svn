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

#include "testrunner.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TestResult.h>

#include <string>
#include <bcore/util.h>

#include <bcore/project.h>
#include <bcore/logger/logger.h>
#include <bcore/logger/console_log.h>

#include "listener.h"
#include "synchronize.h"

int main(int argc, char **argv)
{
   /*
   using namespace btg::core::logger;
   logWrapper* log = logWrapper::getInstance();
   log->setLogStream(new logStream(new consoleLogger()));

#if BTG_DEBUG
   // Log everything.
   log->setMinMessagePriority(logWrapper::PRIO_DEBUG);
#else
   // Only log errors.
   log->setMinMessagePriority(logWrapper::PRIO_ERROR);
#endif // BTG_DEBUG
   */
   
   // synchronize with others (if any)
   int sync_result = synchronize();
   if (sync_result)
   	return sync_result;

   // Default, run the tests one time.
   t_int repeatCount = 1;

   if (argc >= 2)
      {
         // Running tests a number of times.
         std::string number(argv[1]);
         t_int n;

         if (btg::core::Util::stringToInt(number, n))
            {
               repeatCount = n;
            }
      }

   CppUnit::TextUi::TestRunner runner;
   CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
   runner.addTest( registry.makeTest() );
#if BTG_UTEST_VERBOSE
   BtgTestListener listener;
   // CppUnit::BriefTestProgressListener listener;
   runner.eventManager().addListener(&listener);
#endif // BTG_UTEST_VERBOSE
   bool wasSucessful = false;

   for (t_int i = 0; i < repeatCount; i++)
      {
         wasSucessful = runner.run();

         if (!wasSucessful)
            {
               return wasSucessful ? 0 : 1;
            }
      }

   return wasSucessful ? 0 : 1;
}

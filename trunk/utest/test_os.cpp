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
 * $Id: test_os.cpp,v 1.1.2.9 2006/04/05 20:40:26 wojci Exp $
 */

#include "test_os.h"
#include "testrunner.h"

#include <cppunit/extensions/HelperMacros.h>

#include <string.h>
#include <vector>
#include <iostream> // for cout, which should not be used here.

#include <bcore/externalization/simple.h>

extern "C"
{
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
}

#include <bcore/os/stcps.h>
#include <bcore/os/stcpc.h>

#include "files.h"

#if BTG_UTEST_OS
CPPUNIT_TEST_SUITE_REGISTRATION( testOS );
#endif // BTG_UTEST_OS

using namespace btg::core;
using namespace btg::core::os;

void testOS::setUp()
{
   externalization = new btg::core::externalization::Simple();

   /* Make sure the dir for the test files exist */
   if(access(TESTFILE_BASE_DIR, W_OK) == -1)
      {
         /* Is this the correct way to bail? */
         CPPUNIT_ASSERT(mkdir(TESTFILE_BASE_DIR, 0777) != -1);
      }
}

void testOS::tearDown()
{
}

void testOS::testGNUTLSSockets()
{
   /*
     std::string CAfile   = "/home/wojci/.btg/ca.crt";
     // Empty file.
     std::string CRLFile  = "/home/wojci/.btg/ca.crl";
     std::string CertFile = "/home/wojci/.btg/btgdaemon.crt";
     std::string keyFile  = "/home/wojci/.btg/btgdaemon.key";
     t_int       port     = 5000;

     gtlsGlobalServerData* ggsd = new gtlsGlobalServerData(CAfile, CRLFile, CertFile, keyFile);

     SecureServerSocket* sss = new SecureServerSocket(ggsd, port);

     gtlsClientData* gcd = new gtlsClientData(CAfile);

     SecureClientSocket* scs = new SecureClientSocket(gcd, "127.0.0.1", port);

     SecureServerSocket* acceptSock = new SecureServerSocket();

     if (sss->accept(*acceptSock))
     {
     std::cout << "Accepted." << std::endl;
     }

     delete scs;
     scs = 0;
     delete gcd;
     gcd = 0;

     delete sss;
     sss = 0;

     delete ggsd;
     ggsd = 0;
   */
}

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

#include "test_bcore.h"
#include "testrunner.h"

#include <bcore/type.h>

#include <bcore/trackerstatus.h>

extern "C"
{
#include <string.h>
}

#include <vector>
#include <string>

#include <bcore/dbuf.h>
#include <bcore/sbuf.h>

#include <bcore/status.h>
#include <bcore/file_info.h>
#include <bcore/hru.h>
#include <bcore/hrt.h>
#include <bcore/peer.h>
#include <bcore/addrport.h>
#include <bcore/externalization/xmlrpc.h>
#include <bcore/auth/hash.h>

#include <bcore/os/fileop.h>
#include <bcore/os/pidfile.h>

#include <bcore/project.h>

#include <bcore/command/ack.h>
#include <bcore/command/context_abort.h>
#include <bcore/command/context_clean.h>
#include <bcore/command/context_clean_rsp.h>
#include <bcore/command/context_create.h>
#include <bcore/command/context_fi.h>
#include <bcore/command/context_fi_rsp.h>
#include <bcore/command/context_li_status.h>
#include <bcore/command/context_li_status_rsp.h>
#include <bcore/command/context_limit.h>
#include <bcore/command/context_peers.h>
#include <bcore/command/context_peers_rsp.h>
#include <bcore/command/context_start.h>
#include <bcore/command/context_status.h>
#include <bcore/command/context_status_rsp.h>
#include <bcore/command/context_stop.h>
#include <bcore/command/error.h>
#include <bcore/command/initconnection.h>
#include <bcore/command/list.h>
#include <bcore/command/list_rsp.h>
#include <bcore/command/session_attach.h>
#include <bcore/command/session_detach.h>
#include <bcore/command/session_error.h>
#include <bcore/command/session_list.h>
#include <bcore/command/session_list_rsp.h>
#include <bcore/command/session_quit.h>
#include <bcore/command/setup.h>
#include <bcore/command/version.h>

#include <bcore/command/kill.h>
#include <bcore/command/limit.h>
#include <bcore/command/uptime.h>

#include <bcore/command/context_last.h>
#include <bcore/command/context_file.h>

#include <bcore/logger/console_log.h>

#include "files.h"

#define buffer_size (t_int)1024

#define BASIC_EQUAL1(pointer1, pointer2) pointer1->getType() == pointer2->getType()
#define CLEANUP delete command_pointer; command_pointer=0; buffer.erase();

#if BTG_UTEST_BCORE
CPPUNIT_TEST_SUITE_REGISTRATION( testBcore );
#endif // BTG_UTEST_BCORE

using namespace btg::core;

void testBcore::setUp()
{
   logwrapper      = btg::core::LogWrapperType(new btg::core::logger::logWrapper);

   boost::shared_ptr<btg::core::logger::logStream> l(new btg::core::logger::logStream(new btg::core::logger::consoleLogger()));
   logwrapper->setLogStream(l);
   
   command_pointer = 0;
   externalization = new btg::core::externalization::XMLRPC(logwrapper);
}

void testBcore::tearDown()
{
   buffer.erase();

   delete externalization;
   externalization = 0;
}

void testBcore::testCommand()
{
   btg::core::commandFactory cf(logwrapper, *externalization);
   // A most basic command. This command cannot be serialized, its
   // serialize/deserialize methods are used by its children to store
   // session and command type.
   Command* c = new Command(Command::CN_UNDEFINED);
   CPPUNIT_ASSERT(cf.convertToBytes(c) == 0);

   CPPUNIT_ASSERT(c->getType() == Command::CN_UNDEFINED);
   CPPUNIT_ASSERT(c->getName() == "Undefined");
   commandFactory::decodeStatus error;
   command_pointer = cf.createFromBytes(error);
   CPPUNIT_ASSERT(error != commandFactory::DS_OK);
   CPPUNIT_ASSERT(command_pointer == 0);

   delete c;

   CLEANUP;
}

void testBcore::testCommandNames()
{
   // The usual command names.
   t_uint i = Command::CN_GINITCONNECTION;
   CPPUNIT_ASSERT(Command::getName(i) != Command::getName(Command::CN_UNDEFINED));

   // CN_UNDEFINED
   // Name for an undefined command.
   i += 255;
   CPPUNIT_ASSERT(Command::getName(i) == Command::getName(Command::CN_UNDEFINED));
   
   testListCommandRange(Command::CN_GSETUP, Command::CN_GLISTRSP);
   testListCommandRange(Command::CN_CCREATEWITHDATA, Command::CN_CMOVE);
   testListCommandRange(Command::CN_ERROR, Command::CN_ACK);
   testListCommandRange(Command::CN_SATTACH, Command::CN_MOWRITE);
}

void testBcore::testListCommandRange(t_uint _start, t_uint _end)
{   
   for (t_uint i=_start; i <= _end; i++)
      {
         CPPUNIT_ASSERT(Command::getName(i) != Command::getName(Command::CN_UNDEFINED));
      }

   btg::core::externalization::Externalization* e = new btg::core::externalization::XMLRPC(logwrapper);

   for (t_uint i=_start ; i <= _end; i++)
      {
         CPPUNIT_ASSERT(e->getCommandName(i) != e->getCommandName(Command::CN_UNDEFINED));
      }

   delete e;
   e = 0;
}

void testBcore::testListCommand()
{
   btg::core::commandFactory cf(logwrapper, *externalization);

   // List command
   listCommand *lc = new listCommand();
   lc->serialize(externalization);

   commandFactory::decodeStatus error;
   command_pointer = cf.createFromBytes(error);
   CPPUNIT_ASSERT(error == commandFactory::DS_OK);
   CPPUNIT_ASSERT(command_pointer != 0);
   CPPUNIT_ASSERT(command_pointer->getType() == Command::CN_GLIST);
   CPPUNIT_ASSERT(BASIC_EQUAL1(lc, command_pointer));
   CPPUNIT_ASSERT(buffer.size() == 0);
   delete lc;

   CLEANUP;
}

void testBcore::testListCommandResponse()
{
   btg::core::commandFactory cf(logwrapper, *externalization);

   std::vector<t_int> ids;
   std::vector<std::string> files;

   ids.push_back(1000);   files.push_back("1");
   ids.push_back(5000);   files.push_back("2");
   ids.push_back(1500);   files.push_back("3");
   ids.push_back(1050);   files.push_back("4");
   ids.push_back(10000);  files.push_back("5");
   ids.push_back(50000);  files.push_back("6");
   ids.push_back(100000); files.push_back("7");

   // List response command:
   listCommandResponse *lcr = new listCommandResponse(ids, files);
   lcr->serialize(externalization);

   commandFactory::decodeStatus error;
   command_pointer = cf.createFromBytes(error);
   CPPUNIT_ASSERT(error == commandFactory::DS_OK);
   CPPUNIT_ASSERT(buffer.size() == 0);

   CPPUNIT_ASSERT(command_pointer->getType() == Command::CN_GLISTRSP);
   CPPUNIT_ASSERT(BASIC_EQUAL1(lcr, command_pointer));

   CPPUNIT_ASSERT(lcr->getIDs()       == ids);
   CPPUNIT_ASSERT(lcr->getFilanames() == files);

   CPPUNIT_ASSERT(lcr->getIDs()       == dynamic_cast<listCommandResponse*>(command_pointer)->getIDs());
   CPPUNIT_ASSERT(lcr->getFilanames() == dynamic_cast<listCommandResponse*>(command_pointer)->getFilanames());

   delete lcr;
   CLEANUP;
}

void testBcore::testErrorCommand()
{
   btg::core::commandFactory cf(logwrapper, *externalization);

   // List response command:
   std::string const message = "Message. \nTestMessage. TestTestTest Message          0x0\\\\\0";
   t_int const type     = Command::CN_GLISTRSP;

   errorCommand *ec = new errorCommand(type, message);
   ec->serialize(externalization);

   commandFactory::decodeStatus error;
   command_pointer = cf.createFromBytes(error);
   CPPUNIT_ASSERT(error == commandFactory::DS_OK);
   CPPUNIT_ASSERT(buffer.size() == 0);

   CPPUNIT_ASSERT(command_pointer->getType() == Command::CN_ERROR);
   CPPUNIT_ASSERT(BASIC_EQUAL1(ec, command_pointer));
   CPPUNIT_ASSERT(ec->getMessage() == message);
   CPPUNIT_ASSERT(ec->getMessage() == (dynamic_cast<errorCommand*>(command_pointer)->getMessage()));
   CPPUNIT_ASSERT(ec->getErrorCommand() == type);
   CPPUNIT_ASSERT(ec->getErrorCommand() == (dynamic_cast<errorCommand*>(command_pointer)->getErrorCommand()));
   delete ec;
   CLEANUP;
}

void testBcore::testListContextCommand()
{
   btg::core::commandFactory cf(logwrapper, *externalization);

   listSessionCommand *lcc = new listSessionCommand();
   lcc->serialize(externalization);

   commandFactory::decodeStatus error;
   command_pointer = cf.createFromBytes(error);
   CPPUNIT_ASSERT(error == commandFactory::DS_OK);
   CPPUNIT_ASSERT(buffer.size() == 0);

   CPPUNIT_ASSERT(lcc->getType()             == Command::CN_SLIST);
   CPPUNIT_ASSERT(command_pointer->getType() == Command::CN_SLIST);

   CPPUNIT_ASSERT(BASIC_EQUAL1(lcc, command_pointer));

   delete lcc;
   CLEANUP;
}

void testBcore::testListSessionResponseCommand()
{
   btg::core::commandFactory cf(logwrapper, *externalization);

   std::vector<t_long> vl;
   std::vector<std::string> sn;
   for (t_long i=0; i<50; i++)
      {
         vl.push_back(i);
         sn.push_back("name");
      }

   listSessionResponseCommand *lsrc = new listSessionResponseCommand(vl, sn);

   lsrc->serialize(externalization);
   commandFactory::decodeStatus error;
   command_pointer = cf.createFromBytes(error);
   CPPUNIT_ASSERT(error == commandFactory::DS_OK);
   CPPUNIT_ASSERT(buffer.size() == 0);

   CPPUNIT_ASSERT(lsrc->getType()            == Command::CN_SLISTRSP);
   CPPUNIT_ASSERT(command_pointer->getType() == Command::CN_SLISTRSP);

   CPPUNIT_ASSERT(BASIC_EQUAL1(lsrc, command_pointer));

   CPPUNIT_ASSERT(lsrc->getSessions() == vl);
   CPPUNIT_ASSERT(dynamic_cast<listSessionResponseCommand*>(command_pointer)->getSessions() == vl);
   CPPUNIT_ASSERT(lsrc->getSessions() == dynamic_cast<listSessionResponseCommand*>(command_pointer)->getSessions());
   delete lsrc;
   CLEANUP;
}

void testBcore::testAttachSessionCommand()
{
   btg::core::commandFactory cf(logwrapper, *externalization);

   for (t_long trySession=0; trySession<512; trySession++)
      {
         attachSessionCommand *asc = new attachSessionCommand("build", trySession);
         asc->serialize(externalization);

         commandFactory::decodeStatus error;
         command_pointer = cf.createFromBytes(error);
         CPPUNIT_ASSERT(error == commandFactory::DS_OK);
         CPPUNIT_ASSERT(buffer.size() == 0);

         CPPUNIT_ASSERT(asc->getType()             == Command::CN_SATTACH);
         CPPUNIT_ASSERT(command_pointer->getType() == Command::CN_SATTACH);

         CPPUNIT_ASSERT(BASIC_EQUAL1(asc, command_pointer));

         CPPUNIT_ASSERT(asc->getSession() == trySession);
         CPPUNIT_ASSERT(dynamic_cast<attachSessionCommand*>(command_pointer)->getSession() == trySession);
         CPPUNIT_ASSERT(asc->getSession() == dynamic_cast<attachSessionCommand*>(command_pointer)->getSession());

         delete asc;
         CLEANUP;
      }
}

void testBcore::testContextStatusResponseCommand()
{
   // !!!
   btg::core::externalization::Externalization* eSource = 
      new btg::core::externalization::XMLRPC(logwrapper);
   
   btg::core::commandFactory cf1(logwrapper, *eSource);

   trackerStatus ts(-1, 0);
   Status status(100, 
                 "test", 
                 Status::ts_queued, 
                 100, 
                 101, 
                 102, 
                 1, 
                 1, 
                 50, 
                 1, 
                 100, 
                 200, 
                 0, 
                 0, 
                 0, 
                 0, 
                 ts, 
                 0);

   contextStatusResponseCommand* csrc = new contextStatusResponseCommand(5, status);

   bool result = cf1.convertToBytes(csrc);

   CPPUNIT_ASSERT(result);

   dBuffer dbuffer;

   eSource->getBuffer(dbuffer);

   btg::core::externalization::Externalization* eDestin = 
      new btg::core::externalization::XMLRPC(logwrapper);
   eDestin->setBuffer(dbuffer);

   btg::core::commandFactory cf2(logwrapper, *eDestin);

   // csrc->serialize(externalization);

   commandFactory::decodeStatus error;
   command_pointer = cf2.createFromBytes(error);

   CPPUNIT_ASSERT(error == commandFactory::DS_OK);
   // CPPUNIT_ASSERT(buffer.size() == 0);

   CPPUNIT_ASSERT(csrc->getType()             == Command::CN_CSTATUSRSP);
   CPPUNIT_ASSERT(command_pointer->getType()  == Command::CN_CSTATUSRSP);

   CPPUNIT_ASSERT(BASIC_EQUAL1(csrc, command_pointer));

   Status new_status_1 = csrc->getStatus();
   Status new_status_2 = (dynamic_cast<contextStatusResponseCommand*>(command_pointer))->getStatus();

   CPPUNIT_ASSERT(new_status_1 == status);
   CPPUNIT_ASSERT(new_status_2 == status);

   delete csrc;
   CLEANUP;

   delete eSource;
   eSource = 0;
   delete eDestin;
   eDestin = 0;
}

void testBcore::testContextAllStatusResponseCommand()
{
   btg::core::commandFactory cf(logwrapper, *externalization);

   std::vector<Status> vstatus;
   for (t_int i=0; i<9; i++)
      {
         trackerStatus ts(-1, 0);
         Status status(i, "/path/to/file", Status::ts_queued, 100, 101, 102, 1, 1, 50, 1, 100, 200, 0, 0, 0, 0, ts, 0);
         vstatus.push_back(status);
      }

   contextAllStatusResponseCommand *casrc = new contextAllStatusResponseCommand(2048, vstatus);

   casrc->serialize(externalization);

   commandFactory::decodeStatus error;
   command_pointer = cf.createFromBytes(error);
   CPPUNIT_ASSERT(error == commandFactory::DS_OK);
   CPPUNIT_ASSERT(buffer.size() == 0);

   CPPUNIT_ASSERT(casrc->getType()            == Command::CN_CALLSTATUSRSP);
   CPPUNIT_ASSERT(command_pointer->getType()  == Command::CN_CALLSTATUSRSP);

   CPPUNIT_ASSERT(BASIC_EQUAL1(casrc, command_pointer));

   std::vector<Status> new_vstatus = dynamic_cast<contextAllStatusResponseCommand*>(command_pointer)->getStatus();
   CPPUNIT_ASSERT(new_vstatus == vstatus);

   delete casrc;
   CLEANUP;
}

void testBcore::testUtil_simple_types()
{
   btg::core::commandFactory cf(logwrapper, *externalization);

   dBuffer output_buffer;
   bool bool_value = false;
	t_int command;

   externalization->setCommand(btg::core::Command::CN_GINITCONNECTION);
   externalization->boolToBytes(bool_value);
   CPPUNIT_ASSERT(externalization->getBufferSize() > 0);
   externalization->getBuffer(output_buffer);
   externalization->reset();
   externalization->setBuffer(output_buffer);
   CPPUNIT_ASSERT(externalization->getCommand(command));
   CPPUNIT_ASSERT(command == btg::core::Command::CN_GINITCONNECTION);

   CPPUNIT_ASSERT(externalization->bytesToBool(bool_value));
   CPPUNIT_ASSERT(!bool_value);

   output_buffer.erase();
   externalization->reset();

   bool_value = true;
   externalization->boolToBytes(bool_value);
   externalization->getBuffer(output_buffer);
   externalization->reset();
   externalization->setBuffer(output_buffer);
   CPPUNIT_ASSERT(externalization->bytesToBool(bool_value));
   CPPUNIT_ASSERT(bool_value);

   output_buffer.erase();
   externalization->reset();

   // String:
   std::string string_value = "1234567890Test1234567890Test1234567890Test1234567890Test1234567890TestB";

   externalization->stringToBytes(&string_value);

   externalization->getBuffer(output_buffer);
   externalization->reset();
   externalization->setBuffer(output_buffer);

   std::string string_target("");
   CPPUNIT_ASSERT(externalization->bytesToString(&string_target));
   CPPUNIT_ASSERT(string_target == string_value);
}

void testBcore::testUtil_str_nocasecmp()
{
   std::string s1;
   std::string s2;

   s1="test";
   s2="test";
   CPPUNIT_ASSERT(Util::compareStringNoCase(s1,s2));
   CPPUNIT_ASSERT(Util::compareStringNoCase(s2,s1));
   s2="TEST";
   CPPUNIT_ASSERT(Util::compareStringNoCase(s1,s2));
   CPPUNIT_ASSERT(Util::compareStringNoCase(s2,s1));
   s2="TeSt";
   CPPUNIT_ASSERT(Util::compareStringNoCase(s1,s2));
   CPPUNIT_ASSERT(Util::compareStringNoCase(s2,s1));
   s2="TeSt23";
   CPPUNIT_ASSERT(!Util::compareStringNoCase(s1,s2));
   CPPUNIT_ASSERT(!Util::compareStringNoCase(s2,s1));
   s1="TeSt23";
   CPPUNIT_ASSERT(Util::compareStringNoCase(s1,s2));
   CPPUNIT_ASSERT(Util::compareStringNoCase(s2,s1));

   s1="123123123";
   s2="123123123";
   CPPUNIT_ASSERT(Util::compareStringNoCase(s1,s2));
   CPPUNIT_ASSERT(Util::compareStringNoCase(s2,s1));
}

void testBcore::testDbuffer()
{
   t_uint const size = 31;
   unsigned char *p  = new unsigned char[size];

   t_uint i;

   for (i=0; i<size; i++)
      {
         p[i] = i % 254;
      }

   dBuffer dbuffer;
   for (i=1; i<16; i++)
      {
         dbuffer.addBytes(p, size);
         CPPUNIT_ASSERT(dbuffer.size() == size);

         t_int result = dbuffer.getBytes(p, size);
         CPPUNIT_ASSERT(result > 0);
         t_uint uresult = result;
         CPPUNIT_ASSERT(uresult == size);
      }

   CPPUNIT_ASSERT(buffer.size() == 0);

   delete [] p;
}

void testBcore::testSbuffer()
{
   {
      sBuffer sbuf;
      CPPUNIT_ASSERT(sbuf.size() == 0);
   }

   // Test reading.
   {
      sBuffer read_sbuf;
      CPPUNIT_ASSERT(read_sbuf.read(TESTFILE_FILLED));
      CPPUNIT_ASSERT(read_sbuf.size() > 0);
   }

   const t_int size = 1024;
   t_byte bytes[size];
   memset(&bytes[0], 0, size);

   bytes[0] = 1;
   bytes[1] = 2;

   bytes[511] = 3;
   bytes[512] = 4;

   bytes[1022] = 5;
   bytes[1023] = 6;
   
   // Test splitting.
   {
      const t_int partSize = 510;

      sBuffer sbuf(bytes, size);
      CPPUNIT_ASSERT(sbuf.size() == size);

      t_byte b = 0;
      CPPUNIT_ASSERT(sbuf.getByte(0, b));
      CPPUNIT_ASSERT(b == 1);
      CPPUNIT_ASSERT(sbuf.getByte(1, b));
      CPPUNIT_ASSERT(b == 2);

      CPPUNIT_ASSERT(sbuf.getByte(511, b));
      CPPUNIT_ASSERT(b == 3);
      CPPUNIT_ASSERT(sbuf.getByte(512, b));
      CPPUNIT_ASSERT(b == 4);

      CPPUNIT_ASSERT(sbuf.getByte(1022, b));
      CPPUNIT_ASSERT(b == 5);
      CPPUNIT_ASSERT(sbuf.getByte(1023, b));
      CPPUNIT_ASSERT(b == 6);

      std::vector<sBuffer> dest;
      sbuf.split(partSize, dest);
      CPPUNIT_ASSERT(dest.size() == 3);

      for (std::vector<sBuffer>::const_iterator iter = dest.begin();
           iter != dest.end();
           iter++)
         {
            CPPUNIT_ASSERT(iter->size() <= partSize);
         }

      sBuffer sbuf2(dest);
      CPPUNIT_ASSERT(sbuf2.size() == size);

      CPPUNIT_ASSERT(sbuf2 == sbuf);

      CPPUNIT_ASSERT(sbuf2.getByte(0, b));
      CPPUNIT_ASSERT(b == 1);
      CPPUNIT_ASSERT(sbuf2.getByte(1, b));
      CPPUNIT_ASSERT(b == 2);

      CPPUNIT_ASSERT(sbuf2.getByte(511, b));
      CPPUNIT_ASSERT(b == 3);
      CPPUNIT_ASSERT(sbuf2.getByte(512, b));
      CPPUNIT_ASSERT(b == 4);

      CPPUNIT_ASSERT(sbuf2.getByte(1022, b));
      CPPUNIT_ASSERT(b == 5);
      CPPUNIT_ASSERT(sbuf2.getByte(1023, b));
      CPPUNIT_ASSERT(b == 6);
   }
}
void testBcore::testStatus()
{
   btg::core::commandFactory cf(logwrapper, *externalization);

   t_int context_id     = 1024;
   std::string filename("/tmp/test.torrent");
   Status::torrent_status
      tstatus           = Status::ts_finished;
   t_ulong dn_total     = 1024*1024*1024;
   t_ulong ul_total     = dn_total * 5;
   t_ulong failed_bytes = 512;
   t_ulong dn_rate      = 1024 * 1024 * 10;
   t_ulong ul_rate      = dn_rate * 2;
   t_int   done         = 100;
   t_ulong filesize     = 1024ul * 1024ul * 1024ul * 10ul;
   t_int   leechers     = 100000;
   t_int   seeders      = 500000;

   trackerStatus ts(-1, 0);
   Status* status = new Status(context_id, filename, tstatus, dn_total, ul_total, failed_bytes, dn_rate, ul_rate, done, filesize, leechers, seeders, 0, 0, 0, 0, ts, 0);

   CPPUNIT_ASSERT(status->contextID() == context_id);
   CPPUNIT_ASSERT(status->filename() == filename);
   CPPUNIT_ASSERT(status->status() == tstatus);
   CPPUNIT_ASSERT(status->downloadTotal() == dn_total);
   CPPUNIT_ASSERT(status->uploadTotal() == ul_total);
   CPPUNIT_ASSERT(status->failedBytes() == failed_bytes);
   CPPUNIT_ASSERT(status->downloadRate() == dn_rate);
   CPPUNIT_ASSERT(status->uploadRate() == ul_rate);
   CPPUNIT_ASSERT(status->done() == done);
   CPPUNIT_ASSERT(status->filesize() == filesize);
   CPPUNIT_ASSERT(status->seeders() == seeders);
   CPPUNIT_ASSERT(status->leechers() == leechers);

   delete status;
   status = 0;

   status = new Status();
   status->set(context_id, filename, tstatus, dn_total, ul_total, failed_bytes, dn_rate, ul_rate, done, filesize, leechers, seeders, 0, 0, 0, 0, ts, 0);

   CPPUNIT_ASSERT(status->contextID() == context_id);
   CPPUNIT_ASSERT(status->filename() == filename);
   CPPUNIT_ASSERT(status->status() == tstatus);
   CPPUNIT_ASSERT(status->downloadTotal() == dn_total);
   CPPUNIT_ASSERT(status->uploadTotal() == ul_total);
   CPPUNIT_ASSERT(status->failedBytes() == failed_bytes);
   CPPUNIT_ASSERT(status->downloadRate() == dn_rate);
   CPPUNIT_ASSERT(status->uploadRate() == ul_rate);
   CPPUNIT_ASSERT(status->done() == done);
   CPPUNIT_ASSERT(status->filesize() == filesize);
   CPPUNIT_ASSERT(status->seeders() == seeders);
   CPPUNIT_ASSERT(status->leechers() == leechers);

   delete status;
   status = 0;
}

void testBcore::testContextCleanCommand()
{
   btg::core::commandFactory cf(logwrapper, *externalization);

   contextCleanCommand* ccc = new contextCleanCommand();

   ccc->serialize(externalization);

   commandFactory::decodeStatus error;
   command_pointer = cf.createFromBytes(error);
   CPPUNIT_ASSERT(error == commandFactory::DS_OK);
   CPPUNIT_ASSERT(buffer.size() == 0);

   CPPUNIT_ASSERT(ccc->getType()              == Command::CN_CCLEAN);
   CPPUNIT_ASSERT(command_pointer->getType()  == Command::CN_CCLEAN);

   CPPUNIT_ASSERT(BASIC_EQUAL1(ccc, command_pointer));

   delete ccc;
   CLEANUP;
}

void testBcore::testContextCleanResponseCommand()
{
   btg::core::commandFactory cf(logwrapper, *externalization);

   std::vector<std::string> filenames;
   std::vector<t_int>  ids;

   filenames.push_back("/path/to/test0");
   ids.push_back(0);
   filenames.push_back("/path/to/test1");
   ids.push_back(1);
   filenames.push_back("/path/to/test2");
   ids.push_back(2);
   filenames.push_back("/path/to/test3");
   ids.push_back(3);
   filenames.push_back("/path/to/test4");
   ids.push_back(4);
   filenames.push_back("/path/to/test5");
   ids.push_back(5);
   filenames.push_back("/path/to/test6");
   ids.push_back(6);
   filenames.push_back("/path/to/test7");
   ids.push_back(7);
   filenames.push_back("/path/to/test8");
   ids.push_back(8);
   filenames.push_back("/path/to/test9");
   ids.push_back(9);

   contextCleanResponseCommand *ccrc = new contextCleanResponseCommand(filenames, ids);

   ccrc->serialize(externalization);

   commandFactory::decodeStatus error;
   command_pointer = cf.createFromBytes(error);
   CPPUNIT_ASSERT(error == commandFactory::DS_OK);
   CPPUNIT_ASSERT(buffer.size() == 0);

   CPPUNIT_ASSERT(ccrc->getType()            == Command::CN_CCLEANRSP);
   CPPUNIT_ASSERT(command_pointer->getType() == Command::CN_CCLEANRSP);

   CPPUNIT_ASSERT(BASIC_EQUAL1(ccrc, command_pointer));

   CPPUNIT_ASSERT(dynamic_cast<contextCleanResponseCommand*>(command_pointer)->getFilenames() == filenames);

   delete ccrc;
   CLEANUP;
}

void testBcore::testHumanReadableUnit()
{
   // Zero.
   t_ullong bytes        = 0;
   humanReadableUnit hru = humanReadableUnit::convert(bytes);

   // Bytes.
   bytes = 1023ull;
   hru   = humanReadableUnit::convert(bytes);

   CPPUNIT_ASSERT(hru.getValue() == 1023u);
   CPPUNIT_ASSERT(hru.getValueUnit() == humanReadableUnit::B);

   // KiB.
   bytes = 1024ull;
   hru   = humanReadableUnit::convert(bytes);

   CPPUNIT_ASSERT(hru.getValue() == 1);
   CPPUNIT_ASSERT(hru.getValueUnit() == humanReadableUnit::KiB);

   // KiB, remainder.
   bytes = 1025ull;
   hru   = humanReadableUnit::convert(bytes);

   CPPUNIT_ASSERT(hru.getValue() == 1);
   CPPUNIT_ASSERT(hru.getValueUnit() == humanReadableUnit::KiB);

   CPPUNIT_ASSERT(hru.getRemainder() == 1);
   CPPUNIT_ASSERT(hru.getRemainderUnit() == humanReadableUnit::B);

   // MiB.
   bytes = 1024ull * 1024ull;
   hru   = humanReadableUnit::convert(bytes);

   CPPUNIT_ASSERT(hru.getValue() == 1);
   CPPUNIT_ASSERT(hru.getValueUnit() == humanReadableUnit::MiB);

   // MiB, remainder.
   bytes = (1024ull * 1024ull) + (512ull * 1024ull);
   hru   = humanReadableUnit::convert(bytes);

   CPPUNIT_ASSERT(hru.getValue() == 1);
   CPPUNIT_ASSERT(hru.getValueUnit() == humanReadableUnit::MiB);

   CPPUNIT_ASSERT(hru.getRemainder() == 512);
   CPPUNIT_ASSERT(hru.getRemainderUnit() == humanReadableUnit::KiB);

   // GiB.
   bytes = 1024ull * 1024ull * 1024ull;
   hru   = humanReadableUnit::convert(bytes);

   CPPUNIT_ASSERT(hru.getValue() == 1);
   CPPUNIT_ASSERT(hru.getValueUnit() == humanReadableUnit::GiB);

   // TiB.
   bytes = 1024ull * 1024ull * 1024ull * 1024ull;
   hru   = humanReadableUnit::convert(bytes);

   CPPUNIT_ASSERT(hru.getValue() == 1);
   CPPUNIT_ASSERT(hru.getValueUnit() == humanReadableUnit::TiB);



   // PiB.
   bytes = 1024ull * 1024ull * 1024ull * 1024ull * 1024ull;
   hru   = humanReadableUnit::convert(bytes);

   CPPUNIT_ASSERT(hru.getValue() == 1);
   CPPUNIT_ASSERT(hru.getValueUnit() == humanReadableUnit::PiB);

}

void testBcore::testHumanReadableTime()
{
   // Zero.
   t_ullong seconds      = 0;
   humanReadableTime hrt = humanReadableTime::convert(seconds);

   // Seconds.
   seconds = 59ull;
   hrt     = humanReadableTime::convert(seconds);

   CPPUNIT_ASSERT(hrt.getValue() == 59);
   CPPUNIT_ASSERT(hrt.getValueUnit() == humanReadableTime::SECOND);

   // Minutes.
   seconds = 60ull;
   hrt     = humanReadableTime::convert(seconds);

   CPPUNIT_ASSERT(hrt.getValue() == 1);
   CPPUNIT_ASSERT(hrt.getValueUnit() == humanReadableTime::MINUTE);

   // Minutes, remainder.
   seconds = 61ull;
   hrt     = humanReadableTime::convert(seconds);

   CPPUNIT_ASSERT(hrt.getValue() == 1);
   CPPUNIT_ASSERT(hrt.getValueUnit() == humanReadableTime::MINUTE);

   CPPUNIT_ASSERT(hrt.getRemainder() == 1);
   CPPUNIT_ASSERT(hrt.getRemainderUnit() == humanReadableTime::SECOND);

   // Hours.
   seconds = 60ull * 60ull;
   hrt     = humanReadableTime::convert(seconds);

   CPPUNIT_ASSERT(hrt.getValue() == 1);
   CPPUNIT_ASSERT(hrt.getValueUnit() == humanReadableTime::HOUR);

   // Hours, remainder.
   seconds  = 60ull * 60ull;
   seconds += 60;

   hrt   = humanReadableTime::convert(seconds);

   CPPUNIT_ASSERT(hrt.getValue() == 1);
   CPPUNIT_ASSERT(hrt.getValueUnit() == humanReadableTime::HOUR);

   CPPUNIT_ASSERT(hrt.getRemainder() == 1);
   CPPUNIT_ASSERT(hrt.getRemainderUnit() == humanReadableTime::MINUTE);

   // Days.
   seconds = 60ull * 60ull * 24;
   hrt     = humanReadableTime::convert(seconds);

   CPPUNIT_ASSERT(hrt.getValue() == 1);
   CPPUNIT_ASSERT(hrt.getValueUnit() == humanReadableTime::DAY);

   // Week.
   seconds = 60ull * 60ull * 24ull * 7ull;
   hrt     = humanReadableTime::convert(seconds);

   CPPUNIT_ASSERT(hrt.getValue() == 1);
   CPPUNIT_ASSERT(hrt.getValueUnit() == humanReadableTime::WEEK);

   // Month.
   seconds = 60ull * 60ull * 24ull * 7ull * 4ull;
   hrt     = humanReadableTime::convert(seconds);

   CPPUNIT_ASSERT(hrt.getValue() == 1);
   CPPUNIT_ASSERT(hrt.getValueUnit() == humanReadableTime::MONTH);
}

void testBcore::testFileInfo()
{
   t_uint const bits          = 1024;
   t_uint const bytesPerPiece = 1024;
   t_ulong const filesize     = 1024 * 1024 * 1024;

   std::vector<bool> boolvect;

   for (t_uint i=0; i<(bits / 2); i++)
      {
         boolvect.push_back(false);
      }
   for (t_uint i=0; i<(bits / 2); i++)
      {
         boolvect.push_back(true);
      }
   fileInformation fi("max_size", boolvect, bytesPerPiece, filesize);
   CPPUNIT_ASSERT(fi.size() == bits);

   // Add more data, double the input size.
   for (t_uint i=0; i<(bits / 2); i++)
      {
         boolvect.push_back(false);
      }
   for (t_uint i=0; i<(bits / 2); i++)
      {
         boolvect.push_back(true);
      }
   fi = fileInformation("double_size", boolvect, bytesPerPiece, filesize);

   CPPUNIT_ASSERT(fi.size() == (2*bits));

   // Add even modre data, uneven data size.
   boolvect.push_back(true);
   fi = fileInformation("uneven", boolvect, bytesPerPiece, filesize);

   CPPUNIT_ASSERT(fi.size() == ((2*bits)+1));

   // Create less input.
   boolvect.clear();
   for (t_uint i=0; i<(bits/2); i++)
      {
         if (i % 2) { boolvect.push_back(true); }
         else { boolvect.push_back(false); }
      }
   fi = fileInformation("less", boolvect, bytesPerPiece, filesize);

   CPPUNIT_ASSERT(fi.size() == (bits/2));

   boolvect.clear();
   fi = fileInformation("zero", boolvect, bytesPerPiece, filesize);
   CPPUNIT_ASSERT(fi.size() == 0);

   // Test that constructor takes huge amounts of bits.
   // 4 x fileInformation::MAX_PIECES.
   boolvect.clear();
   for (t_uint i=0; i<(4*bits); i++)
      {
         boolvect.push_back(false);
         boolvect.push_back(true);
         boolvect.push_back(false);
         boolvect.push_back(true);
      }
   fi = fileInformation("huge", boolvect, bytesPerPiece, filesize);
   CPPUNIT_ASSERT(fi.size() == (4 * bits * 4));
}

void testBcore::testFileInfoResponseCommand()
{
   btg::core::commandFactory cf(logwrapper, *externalization);

   std::vector<bool> boolvect;

   for (t_int i=0; i<256; i++)
      {
         boolvect.push_back(false);
      }
   for (t_int i=0; i<256; i++)
      {
         boolvect.push_back(true);
      }

   fileInformation fileinfo("test_file", boolvect, 2048, 1024*1024);


   t_fileInfoList fileinfolist;
   for (t_int i=0; i<16; i++)
      {
         fileinfolist.push_back(fileinfo);
      }

   contextFileInfoResponseCommand *cfirc = new contextFileInfoResponseCommand(0, fileinfolist);

   cfirc->serialize(externalization);

   commandFactory::decodeStatus error;
   command_pointer = cf.createFromBytes(error);
   CPPUNIT_ASSERT(error == commandFactory::DS_OK);
   CPPUNIT_ASSERT(buffer.size() == 0);

   CPPUNIT_ASSERT(cfirc->getType()            == Command::CN_CFILEINFORSP);
   CPPUNIT_ASSERT(command_pointer->getType()  == Command::CN_CFILEINFORSP);

   CPPUNIT_ASSERT(BASIC_EQUAL1(cfirc, command_pointer));

   CPPUNIT_ASSERT(dynamic_cast<contextFileInfoResponseCommand*>(command_pointer)->getFileInfoList() == fileinfolist);

   delete cfirc;
   CLEANUP;
}

void testBcore::testSelectedFileEntry()
{
   const std::string filename("/tmp/files/file1.45");
   const bool selected = false;

   // Default constructor.
   btg::core::selectedFileEntry* sfe = new btg::core::selectedFileEntry;

   CPPUNIT_ASSERT(sfe->filename() == "none");
   CPPUNIT_ASSERT(sfe->selected());

   delete sfe;
   sfe = 0;

   // Constructor with filename only.
   sfe = new selectedFileEntry(filename);
   CPPUNIT_ASSERT(sfe->filename() == filename);
   CPPUNIT_ASSERT(sfe->selected());

   delete sfe;
   sfe = 0;
   
   // Constructor with filename and selection.
   sfe = new selectedFileEntry(filename, selected);
   CPPUNIT_ASSERT(sfe->filename() == filename);
   CPPUNIT_ASSERT(sfe->selected() == selected);

   delete sfe;
   sfe = 0;

   // Copy constructor.
   selectedFileEntry original(filename, selected);
   selectedFileEntry copied(original);

   CPPUNIT_ASSERT(copied.filename() == filename);
   CPPUNIT_ASSERT(copied.selected() == selected);

   // Assignment.
   copied.deSelect();
   original = copied;

   CPPUNIT_ASSERT(original.filename() == filename);
   CPPUNIT_ASSERT(!original.selected());
}

void testBcore::testSelectedFileEntryList()
{
   btg::core::selectedFileEntryList* sfel = new selectedFileEntryList;

   sfel->add("filename1", true);
   sfel->add("filename2", true);
   sfel->add("filename3", true);
   sfel->add("filename4", false);
   sfel->add("filename5", true);
   sfel->add("filename6", true);
   sfel->add("filename7", false);
   sfel->add("filename8", true);
   sfel->add("filename9", true);
   sfel->add("filename10", true);

   std::vector<selectedFileEntry> entries = sfel->files();

   CPPUNIT_ASSERT(entries[0].filename() == "filename1");
   CPPUNIT_ASSERT(entries[0].selected());
   CPPUNIT_ASSERT(entries[1].filename() == "filename2");
   CPPUNIT_ASSERT(entries[1].selected());
   CPPUNIT_ASSERT(entries[2].filename() == "filename3");
   CPPUNIT_ASSERT(entries[2].selected());
   CPPUNIT_ASSERT(entries[3].filename() == "filename4");
   CPPUNIT_ASSERT(!entries[3].selected());
   CPPUNIT_ASSERT(entries[4].filename() == "filename5");
   CPPUNIT_ASSERT(entries[4].selected());
   CPPUNIT_ASSERT(entries[5].filename() == "filename6");
   CPPUNIT_ASSERT(entries[5].selected());
   CPPUNIT_ASSERT(entries[6].filename() == "filename7");
   CPPUNIT_ASSERT(!entries[6].selected());
   CPPUNIT_ASSERT(entries[7].filename() == "filename8");
   CPPUNIT_ASSERT(entries[7].selected());
   CPPUNIT_ASSERT(entries[8].filename() == "filename9");
   CPPUNIT_ASSERT(entries[8].selected());
   CPPUNIT_ASSERT(entries[9].filename() == "filename10");
   CPPUNIT_ASSERT(entries[9].selected());

   delete sfel;
   sfel = 0;

   // List constructor.
   sfel = new selectedFileEntryList(entries);

   std::vector<selectedFileEntry> entries2 = sfel->files();

   CPPUNIT_ASSERT(entries2[0].filename() == "filename1");
   CPPUNIT_ASSERT(entries2[0].selected());
   CPPUNIT_ASSERT(entries2[1].filename() == "filename2");
   CPPUNIT_ASSERT(entries2[1].selected());
   CPPUNIT_ASSERT(entries2[2].filename() == "filename3");
   CPPUNIT_ASSERT(entries2[2].selected());
   CPPUNIT_ASSERT(entries2[3].filename() == "filename4");
   CPPUNIT_ASSERT(!entries2[3].selected());
   CPPUNIT_ASSERT(entries2[4].filename() == "filename5");
   CPPUNIT_ASSERT(entries2[4].selected());
   CPPUNIT_ASSERT(entries2[5].filename() == "filename6");
   CPPUNIT_ASSERT(entries2[5].selected());
   CPPUNIT_ASSERT(entries2[6].filename() == "filename7");
   CPPUNIT_ASSERT(!entries2[6].selected());
   CPPUNIT_ASSERT(entries2[7].filename() == "filename8");
   CPPUNIT_ASSERT(entries2[7].selected());
   CPPUNIT_ASSERT(entries2[8].filename() == "filename9");
   CPPUNIT_ASSERT(entries2[8].selected());
   CPPUNIT_ASSERT(entries2[9].filename() == "filename10");
   CPPUNIT_ASSERT(entries2[9].selected());

   delete sfel;
   sfel = 0;

   // Copy constructor.
   selectedFileEntryList original(entries2);
   selectedFileEntryList copied(original);

   CPPUNIT_ASSERT(original == copied);

   std::vector<selectedFileEntry> entries3 = copied.files();

   CPPUNIT_ASSERT(entries3[0].filename() == "filename1");
   CPPUNIT_ASSERT(entries3[0].selected());
   CPPUNIT_ASSERT(entries3[1].filename() == "filename2");
   CPPUNIT_ASSERT(entries3[1].selected());
   CPPUNIT_ASSERT(entries3[2].filename() == "filename3");
   CPPUNIT_ASSERT(entries3[2].selected());
   CPPUNIT_ASSERT(entries3[3].filename() == "filename4");
   CPPUNIT_ASSERT(!entries3[3].selected());
   CPPUNIT_ASSERT(entries3[4].filename() == "filename5");
   CPPUNIT_ASSERT(entries3[4].selected());
   CPPUNIT_ASSERT(entries3[5].filename() == "filename6");
   CPPUNIT_ASSERT(entries3[5].selected());
   CPPUNIT_ASSERT(entries3[6].filename() == "filename7");
   CPPUNIT_ASSERT(!entries3[6].selected());
   CPPUNIT_ASSERT(entries3[7].filename() == "filename8");
   CPPUNIT_ASSERT(entries3[7].selected());
   CPPUNIT_ASSERT(entries3[8].filename() == "filename9");
   CPPUNIT_ASSERT(entries3[8].selected());
   CPPUNIT_ASSERT(entries3[9].filename() == "filename10");
   CPPUNIT_ASSERT(entries3[9].selected());

   // The equality operator.
   copied.deSelect("filename10");
   original = copied;

   std::vector<selectedFileEntry> entries4 = copied.files();

   CPPUNIT_ASSERT(original == copied);

   CPPUNIT_ASSERT(entries4[8].filename() == "filename9");
   CPPUNIT_ASSERT(entries4[8].selected());
   CPPUNIT_ASSERT(entries4[9].filename() == "filename10");
   CPPUNIT_ASSERT(!entries4[9].selected());

   std::vector<selectedFileEntry> entries5 = entries4;

   CPPUNIT_ASSERT(entries5[0].filename() == "filename1");
   CPPUNIT_ASSERT(entries5[0].selected());
   CPPUNIT_ASSERT(entries5[1].filename() == "filename2");
   CPPUNIT_ASSERT(entries5[1].selected());
   CPPUNIT_ASSERT(entries5[2].filename() == "filename3");
   CPPUNIT_ASSERT(entries5[2].selected());
   CPPUNIT_ASSERT(entries5[3].filename() == "filename4");
   CPPUNIT_ASSERT(!entries5[3].selected());
   CPPUNIT_ASSERT(entries5[4].filename() == "filename5");
   CPPUNIT_ASSERT(entries5[4].selected());
   CPPUNIT_ASSERT(entries5[5].filename() == "filename6");
   CPPUNIT_ASSERT(entries5[5].selected());
   CPPUNIT_ASSERT(entries5[6].filename() == "filename7");
   CPPUNIT_ASSERT(!entries5[6].selected());
   CPPUNIT_ASSERT(entries5[7].filename() == "filename8");
   CPPUNIT_ASSERT(entries5[7].selected());
   CPPUNIT_ASSERT(entries5[8].filename() == "filename9");
   CPPUNIT_ASSERT(entries5[8].selected());
   CPPUNIT_ASSERT(entries5[9].filename() == "filename10");
   CPPUNIT_ASSERT(!entries5[9].selected());

}

void testBcore::testAddrPort()
{
   std::string input("127.0.0.1:60000");

   addressPort ap;

   CPPUNIT_ASSERT(ap.fromString(input));

   input = "254.254.254.0:1024";
   CPPUNIT_ASSERT(ap.fromString(input));

   // Invalid:
   input = "127.0.0.1";
   CPPUNIT_ASSERT(!ap.fromString(input));

   // Invalid:
   input = "300.400.500.600:0";
   CPPUNIT_ASSERT(!ap.fromString(input));

   addressPort ap1(127,0,0,1,105);
   addressPort ap2(127,0,0,1,105);
   addressPort ap3;

   CPPUNIT_ASSERT(ap1 == ap2);

   ap3 = ap1;

   CPPUNIT_ASSERT(ap3 == ap1);

   addressPort ap4(ap3);

   CPPUNIT_ASSERT(ap4 == ap3);
   
   input = "10.255.255.217:16000";
   CPPUNIT_ASSERT(ap.fromString(input));
   CPPUNIT_ASSERT(ap == addressPort(10,255,255,217,16000));
}

void testBcore::testPeer()
{
   btg::core::commandFactory cf(logwrapper, *externalization);

   t_peerList peerlist;

   for (t_int i = 0; i<12; i++)
      {
         peerAddress pa(i, 127, i, 254);

         bool seed = false;
         if (i % 2)
            {
               seed = true;
            }

         Peer peer(pa, seed, "info");

         peerlist.push_back(peer);
      }

   contextPeersResponseCommand* cprc = new contextPeersResponseCommand(0, peerlist);

   cprc->serialize(externalization);

   commandFactory::decodeStatus error;
   command_pointer = cf.createFromBytes(error);
   CPPUNIT_ASSERT(error == commandFactory::DS_OK);
   CPPUNIT_ASSERT(buffer.size() == 0);

   CPPUNIT_ASSERT(cprc->getType()             == Command::CN_CPEERSRSP);
   CPPUNIT_ASSERT(command_pointer->getType()  == Command::CN_CPEERSRSP);

   CPPUNIT_ASSERT(BASIC_EQUAL1(cprc, command_pointer));

   CPPUNIT_ASSERT(dynamic_cast<contextPeersResponseCommand*>(command_pointer)->getList() == peerlist);

   delete cprc;
   cprc = 0;

   CLEANUP;
}

void testBcore::testHash()
{
   // Check that saving/restoring to/from string works.
   std::string text = "hash123";
   std::string saved_text;

   Hash* h = new Hash;

   h->generate(text);
   h->get(saved_text);

   Hash* h1 = new Hash;
   h1->set(saved_text);

   std::string saved_text_again;
   h1->get(saved_text_again);

   CPPUNIT_ASSERT(*h == *h1);
   CPPUNIT_ASSERT(saved_text == saved_text_again);

   delete h;
   h  = 0;

   delete h1;
   h1 = 0;

   // Check that two text strings have different hashes.
   text = "teststringa";
   std::string text2 = "teststringb";

   h  = new Hash;
   h1 = new Hash;

   h->generate(text);
   h1->generate(text2);

   CPPUNIT_ASSERT(*h != *h1);

   delete h;
   h  = 0;

   delete h1;
   h1 = 0;

   // Check that the same text string give the same hash.
   text  = "teststringaa";
   text2 = text;

   h  = new Hash;
   h1 = new Hash;

   h->generate(text);
   h1->generate(text2);

   h->get(saved_text);
   h1->get(saved_text_again);

   CPPUNIT_ASSERT(saved_text == saved_text_again);
   CPPUNIT_ASSERT(*h == *h1);

   delete h;
   h = 0;

   delete h1;
   h1 = 0;

   // Check some known hashes
   text  = "abc";
   h  = new Hash;
   h->generate(text);
   h->get(saved_text);
   CPPUNIT_ASSERT(saved_text == "a9993e364706816aba3e25717850c26c9cd0d89d");

   delete h;
   h = 0;

   text  = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
   h  = new Hash;
   h->generate(text);
   h->get(saved_text);
   CPPUNIT_ASSERT(saved_text == "84983e441c3bd26ebaae4aa1f95129e5e54670f1");

   delete h;
   h = 0;
}

void testBcore::testOsFileOp()
{
   std::string fileExists(OSFILEOPDIR);
   std::string fileDoesNotExist(OSFILEOPFILE);
   fileDoesNotExist += ".not";

   std::string dirExists(OSFILEOPDIR);
   std::string dirDoesNotExist(OSFILEOPDIR);
   dirDoesNotExist += ".not";

   std::string errorMessage;

   // A file exists.
   CPPUNIT_ASSERT(btg::core::os::fileOperation::check(fileExists, errorMessage, false));
   // A file does not exist.
   CPPUNIT_ASSERT(!btg::core::os::fileOperation::check(fileDoesNotExist, errorMessage, false));

   // A dir exists.
   CPPUNIT_ASSERT(btg::core::os::fileOperation::check(dirExists, errorMessage, true));
   // A dir does not exist.
   CPPUNIT_ASSERT(!btg::core::os::fileOperation::check(dirDoesNotExist, errorMessage, true));
}

void testBcore::testVersion()
{
   CPPUNIT_ASSERT((
                   GPD->iMAJORVERSION() != 0 ||
                   GPD->iMINORVERSION() != 0 ||
                   GPD->iREVISIONVERSION() != 0));

   // This is required by libtorrent.
   CPPUNIT_ASSERT( (GPD->iMAJORVERSION() >= 0) && (GPD->iMAJORVERSION() <= 9) );
   CPPUNIT_ASSERT( (GPD->iMINORVERSION() >= 0) && (GPD->iMINORVERSION() <= 9) );
   CPPUNIT_ASSERT( (GPD->iREVISIONVERSION() >= 0 ) && (GPD->iREVISIONVERSION() <= 9) );

   // Test the version command.
   versionResponseCommand vrc(GPD->iMAJORVERSION(), 
                              GPD->iMINORVERSION(),
                              GPD->iREVISIONVERSION());

   vrc.setOption(versionResponseCommand::SS);
   vrc.setOption(versionResponseCommand::PERIODIC_SS);
   vrc.setOption(versionResponseCommand::UPNP);
   vrc.setOption(versionResponseCommand::DHT);
   vrc.setOption(versionResponseCommand::ENCRYPTION);
   vrc.setOption(versionResponseCommand::URL);
   // vrc.setOption(versionResponseCommand::SELECTIVE_DL);
   vrc.setOption(versionResponseCommand::OPTION_7);
   vrc.setOption(versionResponseCommand::OPTION_8);

   CPPUNIT_ASSERT( vrc.getOption(versionResponseCommand::SS));
   CPPUNIT_ASSERT( vrc.getOption(versionResponseCommand::URL));
   CPPUNIT_ASSERT( !vrc.getOption(versionResponseCommand::SELECTIVE_DL));
   CPPUNIT_ASSERT( vrc.getOption(versionResponseCommand::OPTION_7));
   CPPUNIT_ASSERT( vrc.getOption(versionResponseCommand::OPTION_8));
}

void testBcore::testXmlRpc()
{
   /* Attempt to test all the primitive types. */

   btg::core::externalization::Externalization* e = new btg::core::externalization::XMLRPC(logwrapper);
   t_int command_id   = btg::core::Command::CN_GINITCONNECTION;
   t_int context_id   = 100;
   bool flag          = true;
   t_byte byte        = 127;
   t_uint unsign_int  = 1000;
   t_long long_int    = 1543222;
   t_ulong ulong_int  = 31543222;
   t_float float_numb = 0.2143;

   t_intList  integer_list;
   integer_list.push_back(10);
   integer_list.push_back(0);
   integer_list.push_back(1);
   integer_list.push_back(100);
   integer_list.push_back(104);
   integer_list.push_back(1510);
   integer_list.push_back(2110);
   integer_list.push_back(67543);
   integer_list.push_back(2);

   t_longList long_list;
   long_list.push_back(10);
   long_list.push_back(0);
   long_list.push_back(1);
   long_list.push_back(44100);
   long_list.push_back(44104);
   long_list.push_back(31510);
   long_list.push_back(232110);
   long_list.push_back(267543);
   long_list.push_back(2);

   std::string string_entity("!%\"asfd//2399GGFFaKKKKKK4D00000000000001312");
   t_strList  string_list;
   string_list.push_back("asdf43234234");
   string_list.push_back("asdf123");
   string_list.push_back("asdfzxfdsfsdf");
   string_list.push_back("asdf2344");
   string_list.push_back("asdfFFFFFFF");
   string_list.push_back("asdfafdsasdfdsafadsf2432432G");
   string_list.push_back("asdfasdf#");
   string_list.push_back("asdfADSFDSAFDSAF");
   string_list.push_back("asdf1213123!#");
   string_list.push_back("!asdf-");

   btg::core::dBuffer buffer;

   e->setParamInfo("command id", true);
   e->setCommand(command_id);
  
   e->setParamInfo("integer", true);
   e->intToBytes(&context_id);
   CPPUNIT_ASSERT(e->status());

   e->setParamInfo("boolean", true);
   e->boolToBytes(flag);
   CPPUNIT_ASSERT(e->status());

   e->setParamInfo("byte", true);
   e->addByte(byte);
   CPPUNIT_ASSERT(e->status());

   e->setParamInfo("unsigned integer", true);
   e->uintToBytes(&unsign_int);
   CPPUNIT_ASSERT(e->status());

   e->setParamInfo("long integer", true);
   e->longToBytes(&long_int);
   CPPUNIT_ASSERT(e->status());

   e->setParamInfo("unsigned long integer", true);
   e->uLongToBytes(&ulong_int);
   CPPUNIT_ASSERT(e->status());

   e->setParamInfo("float", true);
   e->floatToBytes(&float_numb);
   CPPUNIT_ASSERT(e->status());

   e->setParamInfo("integer list", true);
   e->intListToBytes(&integer_list);
   CPPUNIT_ASSERT(e->status());

   e->setParamInfo("long list", true);
   e->longListToBytes(&long_list);
   CPPUNIT_ASSERT(e->status());

   e->setParamInfo("string", true);
   e->stringToBytes(&string_entity);
   CPPUNIT_ASSERT(e->status());

   e->setParamInfo("string list", true);
   e->stringListToBytes(&string_list);
   CPPUNIT_ASSERT(e->status());

   e->getBuffer(buffer);

   delete e;
   e = 0;

   /* */

   e = new btg::core::externalization::XMLRPC(logwrapper);

   e->setBuffer(buffer);

   t_int context_id_new   = -1;
   bool flag_new          = false;
   t_int command_id_new   = -1;
   t_byte byte_new        = 254;
   t_uint unsign_int_new  = 1;
   t_long long_int_new    = 543222;
   t_ulong ulong_int_new  = 1443222;
   t_float float_numb_new = 0.1243;

   t_intList  integer_list_new;
   integer_list_new.push_back(10);
   integer_list_new.push_back(0);
   integer_list_new.push_back(1);
   integer_list_new.push_back(100);
   integer_list_new.push_back(104);
   integer_list_new.push_back(1510);
   integer_list_new.push_back(2110);
   integer_list_new.push_back(67543);
   integer_list_new.push_back(2);

   t_longList long_list_new;
   long_list_new.push_back(10);
   long_list_new.push_back(0);
   long_list_new.push_back(1);
   long_list_new.push_back(44100);
   long_list_new.push_back(44104);
   long_list_new.push_back(31510);
   long_list_new.push_back(232110);
   long_list_new.push_back(267543);
   long_list_new.push_back(2);

   std::string string_entity_new("!%\"asfd//2399GGFFaKKKKKK4D00000000000001312");
   t_strList  string_list_new;
   string_list_new.push_back("asdf43234234");
   string_list_new.push_back("asdf123");
   string_list_new.push_back("asdfzxfdsfsdf");
   string_list_new.push_back("asdf2344");
   string_list_new.push_back("asdfFFFFFFF");
   string_list_new.push_back("asdfafdsasdfdsafadsf2432432G");
   string_list_new.push_back("asdfasdf#");
   string_list_new.push_back("asdfADSFDSAFDSAF");
   string_list_new.push_back("asdf1213123!#");
   string_list_new.push_back("!asdf-");

   e->setParamInfo("command id", true);
   CPPUNIT_ASSERT(e->getCommand(command_id_new));

   e->setParamInfo("integer", true);
   e->bytesToInt(&context_id_new);
   CPPUNIT_ASSERT(e->status());

   e->setParamInfo("boolean", true);
   e->bytesToBool(flag_new);
   CPPUNIT_ASSERT(e->status());

   e->setParamInfo("byte", true);
   e->getByte(byte_new);
   CPPUNIT_ASSERT(e->status());

   e->setParamInfo("unsigned integer", true);
   e->bytesToUint(&unsign_int_new);
   CPPUNIT_ASSERT(e->status());

   e->setParamInfo("long integer", true);
   e->bytesToLong(&long_int_new);
   CPPUNIT_ASSERT(e->status());

   e->setParamInfo("unsigned long integer", true);
   e->bytesToULong(&ulong_int_new);
   CPPUNIT_ASSERT(e->status());

   e->setParamInfo("float", true);
   e->bytesToFloat(&float_numb_new);
   CPPUNIT_ASSERT(e->status());

   e->setParamInfo("integer list", true);
   e->bytesToIntList(&integer_list_new);
   CPPUNIT_ASSERT(e->status());

   e->setParamInfo("long list", true);
   e->bytesToLongList(&long_list_new);
   CPPUNIT_ASSERT(e->status());

   e->setParamInfo("string", true);
   e->bytesToString(&string_entity_new);
   CPPUNIT_ASSERT(e->status());

   e->setParamInfo("string list", true);
   e->bytesToStringList(&string_list_new);
   CPPUNIT_ASSERT(e->status());

   delete e;
   e = 0;

   CPPUNIT_ASSERT(context_id_new == context_id);
   CPPUNIT_ASSERT(flag_new       == flag);
   CPPUNIT_ASSERT(command_id_new == command_id);
   CPPUNIT_ASSERT(unsign_int_new == unsign_int);
   CPPUNIT_ASSERT(long_int_new   == long_int);
   CPPUNIT_ASSERT(ulong_int_new  == ulong_int);
   CPPUNIT_ASSERT(float_numb_new == float_numb);
   CPPUNIT_ASSERT(integer_list_new  == integer_list);
   CPPUNIT_ASSERT(long_list_new     == long_list);
   CPPUNIT_ASSERT(string_entity_new == string_entity);
   CPPUNIT_ASSERT(string_list_new   == string_list_new);
}

void testBcore::testXmlRpcCommands()
{
   btg::core::externalization::Externalization* eSource = 
      new btg::core::externalization::XMLRPC(logwrapper);
   btg::core::externalization::Externalization* eDestin = 
      new btg::core::externalization::XMLRPC(logwrapper);

   XmlRpcSerializeDeserialize(eSource, eDestin);

   delete eSource;
   eSource = 0;

   delete eDestin;
   eDestin = 0;
}

void testBcore::stressTestXmlRpcCommands()
{
   btg::core::externalization::Externalization* eSource = 
      new btg::core::externalization::XMLRPC(logwrapper);
   btg::core::externalization::Externalization* eDestin = 
      new btg::core::externalization::XMLRPC(logwrapper);

   for (t_int i = 0;
        i < 254;
        i++)
      {
         XmlRpcSerializeDeserialize(eSource, eDestin);
      }

   delete eSource;
   eSource = 0;

   delete eDestin;
   eDestin = 0;
}

// also tests btg::core::os::fstream
void testBcore::testPIDFile()
{
   char fname[] = "/tmp/testPIDFileXXXXXX";
   CPPUNIT_ASSERT(mkstemp(fname) != -1);
   
   {
      btg::core::os::PIDFile pf(fname);
      CPPUNIT_ASSERT(pf.error() == false);
      CPPUNIT_ASSERT(pf.exists() == false);
      
      pf.write();
      
      {
         btg::core::os::PIDFile pf(fname);
         CPPUNIT_ASSERT(pf.error() == false);
         CPPUNIT_ASSERT(pf.exists() == true);
      }
      
      {
         // test that file isn't deleted
         std::ifstream ifs(fname);
         CPPUNIT_ASSERT(ifs.is_open() == true);
      }
      int a = 1;
   }

   {
      // test that file is deleted
      std::ifstream ifs(fname);
      CPPUNIT_ASSERT(ifs.is_open() == false);
   }
   
   {
      btg::core::os::PIDFile pf(fname);
      pf.clear();
   }

   {
      // test that file isn't deleted
      std::ifstream ifs(fname);
      CPPUNIT_ASSERT(ifs.is_open() == true);
   }
   
   {
      // empty file, should be OK
      btg::core::os::PIDFile pf(fname);
      CPPUNIT_ASSERT(pf.error() == false);
      CPPUNIT_ASSERT(pf.exists() == false);
      pf.write();
      
      int pid;
      std::ifstream ifs(fname);
      ifs >> pid;
      CPPUNIT_ASSERT(pid == getpid());
   }
   
   {
      // correct pidfile re-writing
      std::ofstream ofs(fname);
      ofs << "999999"; // unexistent PID, hope
      ofs.close();
      
      btg::core::os::PIDFile pf(fname);
      CPPUNIT_ASSERT(pf.error() == false);
      CPPUNIT_ASSERT(pf.exists() == false);
      pf.write();
      
      /* 
       * fighting against filesystem cache
       */
      
      std::stringstream ss;
      ss << "cp " << fname << " " << fname << ".new";
      CPPUNIT_ASSERT(system(ss.str().c_str()) == 0);
      
      int pid = 0;
      ss.str("");
      ss << fname << ".new";
      std::ifstream ifs(ss.str().c_str());
      ifs >> pid;
      CPPUNIT_ASSERT(unlink(ss.str().c_str()) == 0);
      CPPUNIT_ASSERT(pid == getpid());
   }
}

void testBcore::XmlRpcSerializeDeserialize(btg::core::externalization::Externalization* _eSource,
                                           btg::core::externalization::Externalization* _eDestin)
{
   btg::core::commandFactory cf1(logwrapper, *_eSource);
   btg::core::commandFactory cf2(logwrapper, *_eDestin);

   std::vector<btg::core::Command*> org_commands;
   std::vector<btg::core::Command*> restored_commands;
   createCommands(org_commands);

   dBuffer buffer;

   std::vector<btg::core::Command*>::iterator iter;
   for (iter= org_commands.begin();
        iter != org_commands.end();
        iter++)
      {
         Command* c = *iter;

         CPPUNIT_ASSERT(cf1.convertToBytes(c));
         
         _eSource->getBuffer(buffer);
         _eSource->reset();

         _eDestin->setBuffer(buffer);

         commandFactory::decodeStatus ds;
         Command* c2 = cf2.createFromBytes(ds);
         CPPUNIT_ASSERT(ds == commandFactory::DS_OK);
         CPPUNIT_ASSERT(c2 != 0);

         restored_commands.push_back(c2);

         buffer.erase();
      }

   CPPUNIT_ASSERT(restored_commands.size() > 0);
   CPPUNIT_ASSERT(org_commands.size() == restored_commands.size());

   std::vector<btg::core::Command*>::iterator org_iter;
   std::vector<btg::core::Command*>::iterator res_iter = restored_commands.begin();

   for (org_iter = org_commands.begin();
        org_iter != org_commands.end();
        org_iter++)
      {
         CPPUNIT_ASSERT(res_iter != restored_commands.end());

         Command* c  = *org_iter;
         CPPUNIT_ASSERT(c != 0);

         Command* c2 = *res_iter;
         CPPUNIT_ASSERT(c2 != 0);

         CPPUNIT_ASSERT(c->getType() == c2->getType());
         CPPUNIT_ASSERT(c->getName() == c2->getName());

         res_iter++;
      }

   destroyCommands(org_commands);
   destroyCommands(restored_commands);
}

void testBcore::createCommands(std::vector<btg::core::Command*> & commands)
{
   t_int which_command = Command::CN_GINITCONNECTION;

   commands.push_back(
                      new errorCommand(which_command, "test")
                      );

   commands.push_back(
                      new ackCommand(which_command)
                      );

   commands.push_back(
                      new listCommand()
                      );

   t_intList context_ids;
   context_ids.push_back(1);
   context_ids.push_back(10);
   context_ids.push_back(11);
   context_ids.push_back(15);
   context_ids.push_back(100);
   context_ids.push_back(145);
   context_ids.push_back(1000);
   context_ids.push_back(1050);
   context_ids.push_back(1500);

   t_strList filenames;
   filenames.push_back("/path/test1");
   filenames.push_back("/path/test2");
   filenames.push_back("/path/test3");
   filenames.push_back("/path/test4");
   filenames.push_back("/path/test5");
   filenames.push_back("/path/test6");
   filenames.push_back("/path/test7");
   filenames.push_back("/path/test8");
   filenames.push_back("/path/test9");

   commands.push_back(
                      new listCommandResponse(context_ids, filenames)
                      );

   requiredSetupData rsd("testbuild",
                         -1,
                         -1,
                         false,
                         true);

   commands.push_back(
                      new setupCommand(rsd)
                      );

   t_long sessionId = 5;

   commands.push_back(
                      new setupResponseCommand(sessionId)
                      );

   std::string filename("/path/to/file.torrent");
   sBuffer torrent_file;
   CPPUNIT_ASSERT(torrent_file.read(TESTFILE_FILLED));

   commands.push_back(
                      new contextCreateWithDataCommand(filename, torrent_file, true)
                      );
   
   commands.push_back(
                      new lastCIDCommand()
                      );

   t_int cid = 104;

   commands.push_back(
                      new lastCIDResponseCommand(cid)
                      );

   commands.push_back(
                      new contextStartCommand(cid, false)
                      );

   commands.push_back(
                      new contextStopCommand(cid, false)
                      );

   commands.push_back(
                      new contextAbortCommand(cid, false)
                      );

   commands.push_back(
                      new contextStatusCommand(cid, false)
                      );

   trackerStatus ts(-1, 0);
   Status status(100, 
                 filename, 
                 Status::ts_queued, 
                 100, 
                 101, 
                 102, 
                 1, 
                 1, 
                 50, 
                 1, 
                 100, 
                 200, 
                 0, 
                 0, 
                 0, 
                 0, 
                 ts, 
                 0);

   commands.push_back(
                      new contextStatusResponseCommand(cid, status)
                      );

   t_statusList statuslst;
   statuslst.push_back(status);
   statuslst.push_back(status);

   commands.push_back(
                      new contextAllStatusResponseCommand(cid, statuslst)
                      );

   commands.push_back(
                      new contextFileInfoCommand(cid, false)
                      );

   t_bitList pieces;
   pieces.push_back(true);
   pieces.push_back(true);
   pieces.push_back(true);
   pieces.push_back(true);
   pieces.push_back(true);
   pieces.push_back(true);
   pieces.push_back(true);
   pieces.push_back(true);

   pieces.push_back(false);
   pieces.push_back(false);
   pieces.push_back(false);
   pieces.push_back(false);
   pieces.push_back(false);
   pieces.push_back(false);
   pieces.push_back(false);
   pieces.push_back(false);

   fileInformation fi(filename, pieces, 1, 16);
   t_fileInfoList fileinfolist;
   fileinfolist.push_back(fi);

   commands.push_back(
                      new contextFileInfoResponseCommand(cid, fileinfolist)
                      );

   commands.push_back(
                      new contextCleanCommand(cid, false)
                      );

   commands.push_back(
                      new contextCleanResponseCommand(filenames, context_ids)
                      );

   commands.push_back(
                      new contextLimitCommand(cid, 100, 200, 300, 400, false)
                      );

   commands.push_back(
                      new contextLimitStatusCommand(cid, false)
                      );

   commands.push_back(
                      new contextLimitStatusResponseCommand(cid, 100, 200, 300, 400)
                      );
   
   commands.push_back(
                      new contextPeersCommand(cid, false)
                      );

   peerAddress peeraddress(127, 0, 0, 1);
   Peer p(peeraddress, false, "test");

   t_peerList peerlist;
   peerlist.push_back(p);

   commands.push_back(
                      new contextPeersResponseCommand(cid, peerlist)
                      );

   selectedFileEntry sfe("test", false);

   std::vector<selectedFileEntry> vfiles;
   vfiles.push_back(sfe);

   selectedFileEntryList sfiles(vfiles);

   commands.push_back(
                      new contextSetFilesCommand(cid, sfiles)
                      );

   commands.push_back(
                      new contextGetFilesCommand(cid)
                      );

   commands.push_back(
                      new contextGetFilesResponseCommand(cid, sfiles)
                      );

   commands.push_back(
                      new listSessionCommand()
                      );

   t_longList sessions;
   sessions.push_back(1);
   sessions.push_back(5);
   sessions.push_back(14);
   sessions.push_back(104);
   sessions.push_back(150);

   t_strList sessionNames;
   sessionNames.push_back("1");
   sessionNames.push_back("5");
   sessionNames.push_back("14");
   sessionNames.push_back("104");
   sessionNames.push_back("150");

   commands.push_back(
                      new listSessionResponseCommand(sessions, sessionNames)
                      );
   
   std::string buildId("testbuild");

   commands.push_back(
                      new attachSessionCommand(buildId, sessionId)
                      );

   commands.push_back(
                      new quitSessionCommand()
                      );

   commands.push_back(
                      new sessionErrorCommand(which_command)
                      );
   
   commands.push_back(
                      new detachSessionCommand()
                      );
   
   commands.push_back(
                      new killCommand()
                      );

   commands.push_back(
                      new limitCommand(1000, 2000, 3000, 4000)
                      );

   commands.push_back(new limitStatusCommand()
                      );
   
   commands.push_back(
                      new limitStatusResponseCommand(1000, 2000, 3000, 4000)
                      );

   commands.push_back(
                      new uptimeCommand()
                      );

   commands.push_back(
                      new uptimeResponseCommand(1024)
                      );

   commands.push_back(
                      new initConnectionCommand("test0", "test1")
                      );
}

void testBcore::destroyCommands(std::vector<btg::core::Command*> & commands)
{
   std::vector<btg::core::Command*>::iterator iter;
   for (iter= commands.begin();
        iter != commands.end();
        iter++)
      {
         Command* c = *iter;
         delete c;
         c = 0;
      }

   commands.clear();
}

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
 * $Id: test_gen_transport.cpp,v 1.1.2.22 2006/08/17 19:56:56 wojci Exp $
 */

#include "test_gen_transport.h"
#include "testrunner.h"
#include <cppunit/extensions/HelperMacros.h>
#include <string.h>
#include <vector>

#include <iostream> // for cout, which should not be used here.

#include <bcore/os/sleep.h>

using namespace btg::core;

testGenericTransport::testGenericTransport(btg::core::externalization::Externalization* _e,
                                           messageTransport* _mq_server,
                                           messageTransport* _mq_client)
   : mq_server(_mq_server),
     mq_client(_mq_client),
     readAttempts(10),
     readSleepInMs(512)
{
}

testGenericTransport::~testGenericTransport()
{
   // The used pointers are deleted elsewhere.
}

void testGenericTransport::testSmallQue()
{
   CPPUNIT_ASSERT (mq_client != 0);
   CPPUNIT_ASSERT (mq_client->isInitialized());

   CPPUNIT_ASSERT (mq_server != 0);
   CPPUNIT_ASSERT (mq_server->isInitialized());

   // Create a simple message.
   // Used with write.
   dBuffer db_output;
   // Used with read.
   dBuffer db_input;

	t_int connectionID;

   for (t_int i=0; i<9; i++)
      {
         db_output.addByte(i);
      }

   CPPUNIT_ASSERT( db_output.size() == 9 );
   CPPUNIT_ASSERT( mq_client->write(db_output) > 0 );

   // The write command clears the input buffer.
   CPPUNIT_ASSERT(db_output.size() == 0);

   // Try to read the message back.
   readToAcceptConnection(mq_server);
   t_int status = readFromTransport(mq_server, db_input, connectionID, readAttempts, readSleepInMs);

   CPPUNIT_ASSERT( status > 0 );
	BTG_NOTICE("dbinput.size() = " << db_input.size());
   CPPUNIT_ASSERT( db_input.size() == 9 );

   // Get all the data contained in the buffer.
   t_byte c = 0;
   for (t_int i=0; i<9; i++)
      {
         CPPUNIT_ASSERT(db_input.getByte(&c) == 1);
      }

   // The buffer is empty now.
   CPPUNIT_ASSERT(db_input.size() == 0);
}

void testGenericTransport::testLargeQue()
{
   CPPUNIT_ASSERT (mq_client != 0);
   CPPUNIT_ASSERT (mq_client->isInitialized());

   CPPUNIT_ASSERT (mq_server != 0);
   CPPUNIT_ASSERT (mq_server->isInitialized());

   // Create a big message.
   // Used with write.
   dBuffer db_output;
   // Used with read.
   dBuffer db_input;

   for (t_int i=0; i<1024; i++)
      {
         db_output.addByte(i%254);
      }
   CPPUNIT_ASSERT( db_output.size() == (1024) );

   t_int status = mq_client->write(db_output);

   CPPUNIT_ASSERT( status > 0 );

   // The write command clears the input buffer.
   CPPUNIT_ASSERT(db_output.size() == 0);

   // Try to read the message back.
   readToAcceptConnection(mq_server);
   status = readFromTransport(mq_server, db_input, readAttempts, readSleepInMs);
   CPPUNIT_ASSERT( status == 1024 );
}

void testGenericTransport::fillData(dBuffer & _output, const int _size) const
{
   _output.erase();

   for (t_int j=0; j<_size; j++)
      {
         _output.addByte(10);
      }

}

void testGenericTransport::testTwoWayCommunication()
{
   CPPUNIT_ASSERT (mq_client != 0);
   CPPUNIT_ASSERT (mq_client->isInitialized());

   CPPUNIT_ASSERT (mq_server != 0);
   CPPUNIT_ASSERT (mq_server->isInitialized());

   t_int const data_size = 15;
	t_int connectionID;

   dBuffer db_original;

   dBuffer db_output;
   dBuffer db_input;

   this->fillData(db_original, data_size);

   db_output = db_original;

   CPPUNIT_ASSERT (db_output == db_original);

   mq_client->write(db_output);
   readToAcceptConnection(mq_server);
   readFromTransport(mq_server, db_input, connectionID, readAttempts, readSleepInMs);

   db_output = db_input;

   mq_server->write(db_output, connectionID);

   mq_client->read(db_input);

   db_input.resetRWCounters();
   db_original.resetRWCounters();

   CPPUNIT_ASSERT (db_input == db_original);
}

void testGenericTransport::testLimits()
{
   CPPUNIT_ASSERT (mq_client != 0);
   CPPUNIT_ASSERT (mq_client->isInitialized());

   CPPUNIT_ASSERT (mq_server != 0);
   CPPUNIT_ASSERT (mq_server->isInitialized());

   dBuffer db_srv_input;
   dBuffer db_srv_output;

   t_int const sizeMultiplier = 2;
   t_int status               = 0;
	t_int connectionID;

   bool once = false;

   for (t_int messageNo = 100; 
        messageNo <= 124; 
        messageNo++)
      {
         db_srv_input.erase();
         db_srv_output.erase();
         // Generate data.
         this->fillData(db_srv_input, messageNo * sizeMultiplier);

         // Send the data to the server.
         t_int const data_size = db_srv_input.size();

			// std::cout << "Writing " << data_size << " bytes\n";
         status = mq_client->write(db_srv_input);
         CPPUNIT_ASSERT(status == data_size);

         // Receive the data in the client.
         if (!once)
            {
               once = true;
               readToAcceptConnection(mq_server);
            }
         status = readFromTransport(mq_server, db_srv_output, connectionID, readAttempts, readSleepInMs);
			// std::cout << "Expecting " << data_size << " bytes, got " << status << "\n";
         CPPUNIT_ASSERT(status == data_size);

         db_srv_input = db_srv_output;

         db_srv_input.resetRWCounters();
         db_srv_output.resetRWCounters();

         CPPUNIT_ASSERT(db_srv_input == db_srv_output);

         // Send the data back.
         status = mq_server->write(db_srv_input, connectionID);

         // std::cout << "Wrote " << status << " bytes.\n";

         CPPUNIT_ASSERT(status == data_size);

         status = readFromTransport(mq_client, db_srv_output, readAttempts, readSleepInMs);

         // std::cout << "Expecting " << data_size << " bytes, got " << status << "\n";

         CPPUNIT_ASSERT(status == data_size);
      }

   // Nothing is left in the que.
}

t_int testGenericTransport::readFromTransport(btg::core::messageTransport* _transport,
                                              dBuffer & _output,
															 t_int & _connectionID,
                                              t_int const _attempts,
                                              t_int const _sleepInMs)
{
   bool  done      = false;
   t_int stat      = 0;
   t_int stat_sum  = 0;
   t_int connectionID = -1;
   dBuffer output;

   using namespace btg::core;

   while (!done)
      {
         stat = _transport->read(output, connectionID);
         if (stat != messageTransport::OPERATION_FAILED)
            {
               stat_sum     += stat;
               _output      += output;
               _connectionID = connectionID;

               // std::cout << "read, stat = " << stat << std::endl;
            }
         else
            {
               done = true;
               break;
            }
      }

   if (stat_sum <= 0)
      {
         stat_sum = messageTransport::OPERATION_FAILED;
      }

   if (_output.size() > 0)
      {
         // std::cout << _output.toString() << std::endl;
      }

   // std::cout << "read, stat_sum = " << stat_sum << std::endl;

   return stat_sum;
}

void testGenericTransport::readToAcceptConnection(btg::core::messageTransport* _transport)
{
   using namespace btg::core;

   dBuffer output;
   t_int connectionID;
   _transport->read(output, connectionID);
}

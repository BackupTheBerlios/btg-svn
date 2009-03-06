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

#include "test_btgncli.h"
#include "testrunner.h"

#include <bcore/type.h>

#include "files.h"

#include <bcore/client/lastfiles.h>
#include <bcore/client/clientdynconfig.h>
#include <bcore/client/carg.h>
#include <bcore/client/urlhelper.h>

#if BTG_UTEST_CLIENT
CPPUNIT_TEST_SUITE_REGISTRATION( testbtgncli );
#endif // BTG_UTEST_CLIENT

#include <UI/cli/ncurses/se.h>
#include <UI/cli/ncurses/dm.h>
#include <iostream>

using namespace btg::core;

class coutDisplayModelIf: public btg::UI::cli::DisplayModelIf
{
public:
   coutDisplayModelIf()
      : printed(0),
        current(-1),
        size_from_findSizes(255)
   {
   }

   void pstart()
   {

   }

   void pend()
   {

   }

   void findSizes(std::vector<btg::UI::cli::statusEntry> const& _l)
   {
      size_from_findSizes = _l.size();
   }

   void reset()
   {
      printed             = 0;
      current             = -1;
      size_from_findSizes = 255;
   }

   void paint(const t_uint _number, 
              const bool _current,
              const btg::UI::cli::statusEntry & _se,
              const t_uint _total)
   {
      if (_current)
      {
         current = _se.status.contextID();
         // std::cout << "!CURRENT!" << std::endl;
      }
      /*
      std::cout << "(" << _number << ") " << 
         _se.status.contextID() << ": " << 
         std::endl;
      */
      printed++;
   }
   
   t_uint printed;
   t_int current;
   t_uint size_from_findSizes;
};

void testbtgncli::setUp()
{
}

void testbtgncli::tearDown()
{
}

void testbtgncli::testDisplayModel()
{
   using namespace btg::UI::cli;
   using namespace btg::core;

   coutDisplayModelIf* dmif = new coutDisplayModelIf;

   t_uint visible = 10;
   btg::UI::cli::DisplayModel* dm = new btg::UI::cli::DisplayModel(visible, dmif);

   addData(dm, dmif, visible);
   testMoving(dm, dmif, visible);
   removeData(dm, dmif, visible);

   delete dm;
   dm = 0;
   delete dmif;
   dmif = 0;
}

void testbtgncli::testDisplayModelResize()
{
   using namespace btg::UI::cli;
   using namespace btg::core;

   coutDisplayModelIf* dmif = new coutDisplayModelIf;

   // Try with a different number of visible elements.
   t_uint visible = 20;
   btg::UI::cli::DisplayModel* dm = new btg::UI::cli::DisplayModel(visible, dmif);

   addData(dm, dmif, visible);
   testMoving(dm, dmif, visible);
   removeData(dm, dmif, visible);

   delete dm;
   dm = 0;
   delete dmif;
   dmif = 0;

   // Changing the number of displayed elements.
   dmif = new coutDisplayModelIf;

   visible = 20;
   dm = new btg::UI::cli::DisplayModel(visible, dmif);
   addData(dm, dmif, visible);
   visible = 10;
   dm->resize(visible);
   testMoving(dm, dmif, visible);
   removeData(dm, dmif, visible);

   delete dm;
   dm = 0;
   delete dmif;
   dmif = 0;
}

void testbtgncli::testRemove()
{
   using namespace btg::UI::cli;
   using namespace btg::core;

   coutDisplayModelIf* dmif = new coutDisplayModelIf;

   // Try with a different number of visible elements.
   t_uint visible = 20;
   btg::UI::cli::DisplayModel* dm = new btg::UI::cli::DisplayModel(visible, dmif);

   addData(dm, dmif, visible);

   dmif->reset();
   dm->draw();

   std::cout << "dmif->printed:" << dmif->printed << std::endl;
   CPPUNIT_ASSERT(dmif->printed == visible);
   CPPUNIT_ASSERT(dmif->size_from_findSizes == visible);
   

   std::vector<t_int> id_list;
   for (t_int i = 0;
        i < 100;
        i++)
      {
         id_list.push_back(i);
      }

   dm->remove(id_list);

   dmif->reset();
   dm->draw();

   CPPUNIT_ASSERT(dmif->printed == 0);
   CPPUNIT_ASSERT(dmif->size_from_findSizes == 0);

   CPPUNIT_ASSERT(dmif->current == -1);

   CPPUNIT_ASSERT(dm->size() == 0);

   delete dm;
   dm = 0;
   delete dmif;
   dmif = 0;
}

void testbtgncli::testRemoveLast()
{
   using namespace btg::UI::cli;
   using namespace btg::core;

   coutDisplayModelIf* dmif = new coutDisplayModelIf;

   // Try with a different number of visible elements.
   t_uint visible = 20;
   btg::UI::cli::DisplayModel* dm = new btg::UI::cli::DisplayModel(visible, dmif);
   t_int last_id = 1;

   std::vector<btg::core::Status> l;
   trackerStatus ts(-1, 0);
   Status s(last_id, 
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
            0,
            "hash",
            "url");
         
   l.push_back(s);

   dm->update(l);

   // Draw.
   dmif->reset();
   dm->draw();
   CPPUNIT_ASSERT(dmif->current == 0);

   std::vector<t_int> id_list;
   id_list.push_back(last_id);
   dm->remove(id_list);

   dmif->reset();
   dm->draw();

   CPPUNIT_ASSERT(dmif->printed == 0);
   CPPUNIT_ASSERT(dmif->size_from_findSizes == 0);

   CPPUNIT_ASSERT(dmif->current == -1);

   CPPUNIT_ASSERT(dm->size() == 0);

   delete dm;
   dm = 0;
   delete dmif;
   dmif = 0;
}


void testbtgncli::addData(btg::UI::cli::DisplayModel* _dm, 
                          coutDisplayModelIf* _dmif,
                          t_uint _visible)
{
   // Prepare data.
   std::vector<btg::core::Status> l;

   for (t_int i = 0;
        i < 100;
        i++)
      {
         trackerStatus ts(-1, 0);
         Status s(i, 
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
                  0,
                  "hash",
                  "url");
         
         l.push_back(s);
      }

   _dm->update(l);

   // Draw.
   _dmif->reset();
   _dm->draw();
   CPPUNIT_ASSERT(_dmif->current == 0);
}

void testbtgncli::testMoving(btg::UI::cli::DisplayModel* _dm,
                             coutDisplayModelIf* _dmif,
                             t_uint _visible)
{
   _dm->down();
   _dmif->reset();
   _dm->draw();
   CPPUNIT_ASSERT(_dmif->current == 1);
   CPPUNIT_ASSERT(_dmif->printed == _visible);

   // Moving down..
   for (t_uint i = 0; i < 105; i++)
   {
      _dm->down();
   }
   _dmif->reset();
   _dm->draw();
   CPPUNIT_ASSERT(_dmif->current == 99);
   CPPUNIT_ASSERT(_dmif->printed == _visible);

   // Moving up..
   for (t_uint i = 0; i < 105; i++)
   {
      _dm->up();
   }
   _dmif->reset();
   _dm->draw();
   CPPUNIT_ASSERT(_dmif->printed == _visible);
   CPPUNIT_ASSERT(_dmif->current == 0);
   
   // End.
   _dm->end();
   _dmif->reset();
   _dm->draw();
   CPPUNIT_ASSERT(_dmif->printed == _visible);
   CPPUNIT_ASSERT(_dmif->current == 99);

   // Begin.
   _dm->begin();
   _dmif->reset();
   _dm->draw();
   CPPUNIT_ASSERT(_dmif->printed == _visible);
   CPPUNIT_ASSERT(_dmif->current == 0);
}

void testbtgncli::removeData(btg::UI::cli::DisplayModel* _dm, 
                             coutDisplayModelIf* _dmif,
                             t_uint _visible)
{
   // Remove elements.
   std::vector<btg::core::Status> l;
   l.clear();
   for (t_int i = 0;
        i < 99;
        i++)
      {
         trackerStatus ts(-1, 0);
         Status s(i, 
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
                  0,
                  "hash",
                  "url");
         
         l.push_back(s);
      }

   _dm->update(l);

   CPPUNIT_ASSERT(_dm->sizeChanged());

   _dmif->reset();
   _dm->draw();
   CPPUNIT_ASSERT(_dmif->printed == _visible);
   CPPUNIT_ASSERT(_dmif->current == 0);

   // Remove all but one element.
   l.clear();
   trackerStatus ts(-1, 0);
   Status s(0, 
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
            0,
            "hash",
            "url");

   l.push_back(s);
   _dm->update(l);
   CPPUNIT_ASSERT(_dm->sizeChanged());
   CPPUNIT_ASSERT(_dm->size() == 1);
   _dmif->reset();
   _dm->draw();
   CPPUNIT_ASSERT(_dmif->printed == 1);
   CPPUNIT_ASSERT(_dmif->current == 0);
}

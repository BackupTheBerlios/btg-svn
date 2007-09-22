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

#include "ui.h"
#include <iostream>

static Uint32 update_event(void *obj, Uint32 ival, void *arg)
{
   std::cout << "Timer!" << std::endl;

   timerData* timerdata = (timerData*)arg;

   std::vector<tableData> data;

   updateTable(timerdata->gui, data);

	return ival;
}

void createTimer(btgvsGui & _gui, timerData* _timerdata)
{
   // Create a new timer.
   AG_SetTimeout(&_gui.timer, update_event, _timerdata, 0);
	AG_AddTimeout(_gui.table, &_gui.timer, 1000);
}

void createGui(btgvsGui & _gui)
{ 
   AG_SetRefreshRate(25);

   AG_InitConfigWin(0);

	AG_BindGlobalKey(SDLK_ESCAPE, KMOD_NONE, AG_Quit);

	// Create a new window.
	_gui.window = AG_WindowNewNamed(AG_WINDOW_NOTITLE | AG_WINDOW_NOBORDERS, "%s", "BTGVS");
   AG_WindowSetCaption(_gui.window, "%s", "TEST");

   _gui.contents_box = AG_VBoxNew(_gui.window, AG_VBOX_EXPAND);
   _gui.notebook     = AG_NotebookNew(_gui.contents_box, AG_NOTEBOOK_EXPAND);
   AG_NotebookSetTabAlignment(_gui.notebook, AG_NOTEBOOK_TABS_BOTTOM);

   // Status bar.
   _gui.statusbar      = AG_StatusbarNew(_gui.contents_box, 1);
   _gui.statusbarlabel = AG_StatusbarAddLabel(_gui.statusbar, AG_LABEL_STATIC, "Ready.");

   _gui.tabs[0] = AG_NotebookAddTab(_gui.notebook, "Torrents", AG_BOX_HORIZ);
   _gui.tabs[1] = AG_NotebookAddTab(_gui.notebook, "Graph",    AG_BOX_HORIZ);
   _gui.tabs[2] = AG_NotebookAddTab(_gui.notebook, "Info",     AG_BOX_HORIZ);

   AG_NotebookSelectTab(_gui.notebook, _gui.tabs[0]);

	// Create a new table that expands to fill the window.
	_gui.table = AG_TableNew(_gui.tabs[0], AG_TABLE_EXPAND);
 
	/* Insert the columns. */
   char* sizeSpecifier = "<01234567890123456789>";
	AG_TableAddCol(_gui.table, "Filename", sizeSpecifier, 0/*SortString*/);
	AG_TableAddCol(_gui.table, "Status", sizeSpecifier, 0 /*SortInt*/);
   AG_TableAddCol(_gui.table, "Progress", sizeSpecifier, 0 /*SortInt*/);
   AG_TableAddCol(_gui.table, "Dl/Ul", sizeSpecifier, 0 /*SortInt*/);
   AG_TableAddCol(_gui.table, "Size", sizeSpecifier, 0 /*SortInt*/);
   AG_TableAddCol(_gui.table, "Peer/Seed", sizeSpecifier, 0 /*SortInt*/);

	/* Insert the rows. */
   /*
   for (unsigned int i = 0;
        i < 100;
        i ++)
      {
         AG_TableAddRow(_gui.table, "%s:%s:%s:%s:%s:%s", 
                        "Torrent filename",
                        "Seed",
                        "00:21:23",
                        "50 Kib/s / 60 Kib/s",
                        "1024 MiB",
                        "10/500");
      }
   */   
   // Plot.
   
   _gui.plot     = AG_GraphNew(_gui.tabs[1], AG_GRAPH_LINES, AG_GRAPH_EXPAND);
   _gui.plotItem = AG_GraphAddItem(_gui.plot, "Plot 1", 255, 255, 255, 10000);

   for (unsigned int i = 0;
        i < 5000;
        i ++)
      {
         AG_GraphPlot(_gui.plotItem, i);
      }

	// Show the window and enter event loop.
	AG_WindowShow(_gui.window);
   AG_WindowMaximize(_gui.window);
}

void run()
{
   AG_EventLoop();
}

void destroyGui(btgvsGui & _gui)
{
   
}

void updateTable(btgvsGui & _gui, std::vector<tableData> const& _data)
{
   AG_TableBegin(_gui.table);
   
   for (std::vector<tableData>::const_iterator iter = _data.begin();
        iter != _data.end();
        iter++)
      {
         const tableData & td = *iter;
         AG_TableAddRow(_gui.table, "%s:%s:%s:%s:%s:%s", 
                        td.filename.c_str(),
                  td.status.c_str(),
                  td.progress.c_str(),
                  td.dlul.c_str(),
                  td.size.c_str(),
                  td.peers.c_str());
      }

   AG_TableEnd(_gui.table);

   AG_TableRedrawCells(_gui.table);
}

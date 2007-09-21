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
 * $Id: aboutdialog.h,v 1.4 2005/07/10 21:55:06 wojci Exp $
 */

#include "btgvs.h"

#include <iostream>
 
int main(int argc, char *argv)
{
   // Handle command line options:
   // TODO: implement this.

	/* Initialize Agar. */
	if (AG_InitCore("BTGVS", 0) == -1)
      {
         std::cerr << "Unable to initialize agar subsystem." << std::endl;
         return -1;
      }

   /* AG_VIDEO_FULLSCREEN */
   if (AG_InitVideo(800, 600, 32, AG_VIDEO_DOUBLEBUF | AG_VIDEO_RESIZABLE) == -1) 
      {
         std::cerr << "Unable to initialize agar gfx subsystem." << std::endl;
         return -1;
      }

   btgvsGui gui;
   createGui(gui);

   AG_EventLoop();

   destroyGui(gui);

	return 0;
}

void createGui(btgvsGui & _gui)
{ 
   AG_SetRefreshRate(25);

   AG_InitConfigWin(0);

	AG_BindGlobalKey(SDLK_ESCAPE, KMOD_NONE, AG_Quit);

	/* Create a new window. */
	_gui.window = AG_WindowNewNamed(AG_WINDOW_NOTITLE | AG_WINDOW_NOBORDERS, "%s", "BTGVS");
   AG_WindowSetCaption(_gui.window, "%s", "TEST");

   _gui.contents_box = AG_VBoxNew(_gui.window, AG_VBOX_EXPAND);
   _gui.notebook     = AG_NotebookNew(_gui.contents_box, AG_NOTEBOOK_EXPAND);
   AG_NotebookSetTabAlignment(_gui.notebook, AG_NOTEBOOK_TABS_BOTTOM);

   // Status bar.
   _gui.statusbar      = AG_StatusbarNew(_gui.contents_box, 1);
   _gui.statusbarlabel = AG_StatusbarAddLabel(_gui.statusbar, AG_LABEL_STATIC, "Ready.");

   _gui.tabs[0] = AG_NotebookAddTab(_gui.notebook, "Torrents", AG_BOX_HORIZ);
   _gui.tabs[1] = AG_NotebookAddTab(_gui.notebook, "Stats",    AG_BOX_HORIZ);
   _gui.tabs[2] = AG_NotebookAddTab(_gui.notebook, "Info",     AG_BOX_HORIZ);

   AG_NotebookSelectTab(_gui.notebook, _gui.tabs[1]);

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
   
   // Plot.
   
   _gui.plot     = AG_GraphNew(_gui.tabs[1], AG_GRAPH_LINES, AG_GRAPH_EXPAND);
   _gui.plotItem = AG_GraphAddItem(_gui.plot, "Plot 1", 255, 255, 255, 10000);

   for (unsigned int i = 0;
        i < 255;
        i ++)
      {
         AG_GraphPlot(_gui.plotItem, i);
      }

	// Show the window and enter event loop.
	AG_WindowShow(_gui.window);
   AG_WindowMaximize(_gui.window);
}

void destroyGui(btgvsGui & _gui)
{
   
}

void updateTable(btgvsGui & _gui, std::vector<tableData> const& _data)
{
   
}

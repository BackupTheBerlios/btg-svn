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
#include "ui.h"

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

   tableData td;
   td.filename = "Torrent 0.torrent";
   td.status   = "Downloading";
   td.progress = "00:00:50";
   td.dlul     = "10 KiB/sec / 25 KiB/sec";
   td.size     = "1024 GiB";
   td.peers    = "10/200";

   std::vector<tableData> tdv;
   tdv.push_back(td);

   td.filename = "Torrent 1.torrent";
   td.progress = "00:01:50";
   tdv.push_back(td);
   td.filename = "Torrent 2.torrent";
   td.progress = "00:02:50";
   tdv.push_back(td);
   td.filename = "Torrent 3.torrent";
   td.progress = "00:03:50";
   tdv.push_back(td);
   td.filename = "Torrent 4.torrent";
   td.progress = "00:04:50";
   tdv.push_back(td);

   // Initial update of the torrent table.
   updateTable(gui, tdv);

   // Create a timer which will refresh the UI.
   timerData timerdata(gui);
   createTimer(gui, &timerdata);

   run();

   destroyGui(gui);

	return 0;
}


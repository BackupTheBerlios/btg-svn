#include "btgvs.h"

#include <iostream>

extern "C"
{
#include <agar/core/core.h>
#include <agar/gui/gui.h>
}
 
int main(int argc, char *argv)
{
	AG_Window *win;
	AG_Table *tbl;
 
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

   AG_SetRefreshRate(25);

   AG_InitConfigWin(0);

	AG_BindGlobalKey(SDLK_ESCAPE, KMOD_NONE, AG_Quit);
	//AG_BindGlobalKey(SDLK_F1, KMOD_NONE, AG_ShowSettings);
	//AG_BindGlobalKey(SDLK_F8, KMOD_NONE, AG_ViewCapture);
 
	/* Create a new window. */
	win = AG_WindowNewNamed(AG_WINDOW_NOTITLE | AG_WINDOW_NOBORDERS, "%s", "BTGVS");
   AG_WindowSetCaption (win, "%s", "TEST");

   // AG_Box* contents_box = AG_BoxNew (win, AG_BOX_VERT, AG_BOX_HOMOGENOUS);
   AG_VBox* contents_box = AG_VBoxNew(win, AG_VBOX_EXPAND);

   AG_Notebook* notebook = AG_NotebookNew(contents_box, AG_NOTEBOOK_EXPAND);

   AG_NotebookSetTabAlignment(notebook, AG_NOTEBOOK_TABS_BOTTOM);

   // Status bar.
   AG_Statusbar* statusbar = AG_StatusbarNew(contents_box, 1);
   AG_Label* label = AG_StatusbarAddLabel(statusbar, AG_LABEL_STATIC, "Ready.");

   AG_NotebookTab* tabs[3];
   tabs[0] = AG_NotebookAddTab(notebook, "Torrents", AG_BOX_HORIZ);
   tabs[1] = AG_NotebookAddTab(notebook, "Stats", AG_BOX_HORIZ);
   tabs[2] = AG_NotebookAddTab(notebook, "Info", AG_BOX_HORIZ);

   AG_NotebookSelectTab(notebook, tabs[1]);

   //AG_Box* table_box = AG_BoxNew(tabs[0], AG_BOX_HORIZ, AG_BOX_HOMOGENOUS);
   //AG_BoxSetHomogenous(table_box, 1);

	/* Create a new table that expands to fill the window. */
	tbl = AG_TableNew(tabs[0] /* notebooktable_box */, AG_TABLE_EXPAND);
 
	/* Insert the columns. */
   char* sizeSpecifier = "<01234567890123456789>";
	AG_TableAddCol(tbl, "Filename", sizeSpecifier, 0/*SortString*/);
	AG_TableAddCol(tbl, "Status", sizeSpecifier, 0 /*SortInt*/);
   AG_TableAddCol(tbl, "Progress", sizeSpecifier, 0 /*SortInt*/);
   AG_TableAddCol(tbl, "Dl/Ul", sizeSpecifier, 0 /*SortInt*/);
   AG_TableAddCol(tbl, "Size", sizeSpecifier, 0 /*SortInt*/);
   AG_TableAddCol(tbl, "Peer/Seed", sizeSpecifier, 0 /*SortInt*/);

	/* Insert the rows. */
   for (unsigned int i = 0;
        i < 100;
        i ++)
      {
         AG_TableAddRow(tbl, "%s:%s:%s:%s:%s:%s", 
                        "Torrent filename",
                        "Seed",
                        "00:21:23",
                        "50 Kib/s / 60 Kib/s",
                        "1024 MiB",
                        "10/500");
      }
   
	//AG_TableAddRow(tbl, "%s:%i", "Foo", 10);
	//AG_TableAddRow(tbl, "%s:%i", "Bar", 100);
	//AG_TableAddRow(tbl, "%s:%i", "Baz", 1000);

   // Plot.

   AG_Graph* plot = AG_GraphNew(tabs[1], AG_GRAPH_LINES, AG_GRAPH_EXPAND);

   AG_GraphItem* item = AG_GraphAddItem(plot, "Plot 1", 255, 255, 255, 10000);

   for (unsigned int i = 0;
        i < 255;
        i ++)
      {
         AG_GraphPlot(item, i);
      }

	/* Show the window and enter event loop. */
	AG_WindowShow(win);
   AG_WindowMaximize(win);

	AG_EventLoop();

	return 0;
}

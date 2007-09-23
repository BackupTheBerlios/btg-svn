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

#include "handler.h"

#include <bcore/hrr.h>
#include <bcore/hru.h>
#include <bcore/t_string.h>
#include <bcore/util.h>

namespace btg
{
   namespace UI
   {
      namespace gui
      {
         namespace viewer
         {
            using namespace btg::core;

            void scaleBW(t_uint & _max, 
                         t_uint &_min, 
                         t_uint _bw, 
                         t_uint & _scaled);

            static Uint32 update_event(void *obj, Uint32 ival, void *arg)
            {
               timerData* timerdata = (timerData*)arg;

               t_statusList statuslist;
               std::vector<tableData> data;

               timerdata->handler->reqStatus(-1, true);

               if (timerdata->handler->statusListUpdated())
                  {
                     timerdata->handler->getStatusList(statuslist);
                     timerdata->handler->resetStatusList();

                     tableData td;

                     for (t_statusListI iter = statuslist.begin();
                          iter != statuslist.end();
                          iter++)
                        {
                           std::string filename;

                           if (Util::getFileFromPath(iter->filename(), filename))
                              {
                                 td.filename = filename;
                              }
                           else
                              {
                                 td.filename = iter->filename();
                              }

                           switch(iter->status())
                              {
                              case btg::core::Status::ts_undefined:
                                 td.status = "Undefined";
                                 break;
                              case btg::core::Status::ts_queued:
                                 td.status = "Queued";
                                 break;
                              case btg::core::Status::ts_checking:
                                 td.status = "Checking file";
                                 break;
                              case btg::core::Status::ts_connecting:
                                 td.status = "Connecting";
                                 break;
                              case btg::core::Status::ts_downloading:
                                 td.status = "Downloading";
                                 break;
                              case btg::core::Status::ts_seeding:
                                 td.status = "Seeding";
                                 break;
                              case btg::core::Status::ts_stopped:
                                 td.status = "Stopped";
                                 break;
                              case btg::core::Status::ts_finished:
                                 td.status = "Finished";
                                 break;
                              default:
                                 td.status = "ERROR";
                                 break;
                              }

                           humanReadableRate hrr = humanReadableRate::convert(iter->downloadRate());
                           td.dlul = hrr.toString();

                           td.dlul += " / ";

                           hrr = humanReadableRate::convert(iter->uploadRate());
                           td.dlul += hrr.toString();
                           
                           // Rates in KiB/sec.
                           td.dlCount = iter->downloadRate() / 1024;
                           td.ulCount = iter->uploadRate() / 1024;

                           std::string progress;

                           std::string done_str = convertToString<t_int>(iter->done()) + " %";

                           if (iter->status() == btg::core::Status::ts_downloading)
                              {
                                 if (iter->validTimeLeft())
                                    {
                                       std::string timespec;
                                       iter->timeLeftToString(timespec);
                                       progress = timespec + " (" + done_str + ")";
                                    }
                                 else
                                    {
                                       // Invalid progress, show percent.
                                       progress = done_str;
                                    }
                              } // downloading
                           else
                              {
                                 // Not downloading.
                                 progress = done_str;
                              }

                           td.progress = progress;

                           // Convert the filesize to human readable form.
                           t_ullong bytes = iter->filesize();
                           hRU hru        = hRU::convert(bytes);
                           td.size        = hru.toString();

                           // Update the peer count.
                           td.peers = convertToString<t_int>(iter->leechers()) + "/" +
                              convertToString<t_int>(iter->seeders());

                           data.push_back(td);
                        }
                  }

               updateTable(timerdata->gui, data);
               updateGraph(timerdata->gui, data);

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
 
               // Insert the columns.
               char* sizeSpecifier = "<01234567890123456789>";
               AG_TableAddCol(_gui.table, "Filename", sizeSpecifier, 0);
               AG_TableAddCol(_gui.table, "Status", sizeSpecifier, 0);
               AG_TableAddCol(_gui.table, "Progress", sizeSpecifier, 0);
               AG_TableAddCol(_gui.table, "Dl/Ul", sizeSpecifier, 0);
               AG_TableAddCol(_gui.table, "Size", sizeSpecifier, 0);
               AG_TableAddCol(_gui.table, "Peer/Seed", sizeSpecifier, 0);

               // Plot.
               _gui.plot     = AG_GraphNew(_gui.tabs[1], AG_GRAPH_LINES, AG_GRAPH_SCROLL | AG_GRAPH_EXPAND);

               _gui.plotDlItem   = AG_GraphAddItem(_gui.plot, "Download", 255, 255, 0,   512);
               _gui.plotZeroItem = AG_GraphAddItem(_gui.plot, "Zero",     127, 127, 127, 512);
               _gui.plotUlItem   = AG_GraphAddItem(_gui.plot, "Upload",   255, 0,   0,   512);

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

            void scaleBW(t_uint & _max, 
                         t_uint &_min, 
                         t_uint _bw, 
                         t_uint & _scaled)
            {
               if (_bw > _max)
                  {
                     _max = _bw;
                  }

               if (_bw < _min )
                  {
                     _min = _bw;
                  }

               t_int delta = _max-_min;

               if (delta <= 0)
                  {
                     delta = 1;
                  }
               
               _scaled = (_bw - _min) * 40 / delta;
            }

            void updateGraph(btgvsGui & _gui, std::vector<tableData> const& _data)
            {
               t_uint ul = 0;
               t_uint dl = 0;
               for (std::vector<tableData>::const_iterator iter = _data.begin();
                    iter != _data.end();
                    iter++)
                  {
                     ul += iter->ulCount;
                     dl += iter->dlCount;
                  }

               t_uint ul_scaled = 0;
               scaleBW(_gui.ul_max, _gui.ul_min, ul, ul_scaled);
               t_uint dl_scaled = 0;
               scaleBW(_gui.ul_max, _gui.ul_min, dl, dl_scaled);

               // std::cout << ul_scaled << ":" << ul << std::endl;
               // std::cout << dl_scaled << ":" << dl << std::endl;

               //AG_GraphScale(_gui.plotDlItem, 512, dl);
               //AG_GraphScale(_gui.plotUlItem, 512, ul);

               // dl_scaled = 50;

               AG_GraphPlot(_gui.plotDlItem, dl_scaled);
               AG_GraphPlot(_gui.plotZeroItem, 0);
               AG_GraphPlot(_gui.plotUlItem, -1*ul_scaled);
            }

         } // namespace viewer
      } // namespace gui
   } // namespace UI
} // namespace btg


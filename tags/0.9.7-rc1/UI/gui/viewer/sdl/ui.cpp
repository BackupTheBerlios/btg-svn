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

#include <algorithm>

namespace btg
{
   namespace UI
   {
      namespace gui
      {
         namespace viewer
         {
            using namespace btg::core;

            static Uint32 update_event(void *obj, Uint32 ival, void *arg)
            {
               timerData* timerdata = (timerData*)arg;               

               timerdata->count++;

               if (timerdata->count < timerdata->gui->updateDelay)
                  {
                     update_statusbar(timerdata);
                  }
               else
                  {
                     update_ui(timerdata);
                  }

               return ival;
            }

            btgvsGui::btgvsGui()
               : timer(0),
                 window(0),
                 contents_box(0),
                 statusbar(0),
                 statusbarlabel(0),
                 table(0),
                 ul_max(100),
                 ul_min(0),
                 dl_max(100),
                 dl_min(0),
                 updateDelay(1000),
                 bwLabel(0),
                 uploadStr("0 K/s"),
                 downloadStr("0 K/s"),
                 peLabel(0),
                 peersStr("0"),
                 seedsStr("0")
            {
               timer = new AG_Timeout;
            }

            btgvsGui::~btgvsGui()
            {
               delete timer;
               timer = 0;
            }

            void update_ui(timerData* _timerdata)
            {
               t_statusList statuslist;
               std::vector<tableData> data;

               _timerdata->handler->reqStatus(-1, true);

               if (_timerdata->handler->statusListUpdated())
                  {
                     _timerdata->handler->getStatusList(statuslist);
                     _timerdata->handler->resetStatusList();

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

                           td.dlRate = iter->downloadRate();
                           td.ulRate = iter->uploadRate();

                           humanReadableRate hrr = humanReadableRate::convert(iter->downloadRate());
                           td.dlul = hrr.toString();

                           td.dlul += " / ";

                           hrr = humanReadableRate::convert(iter->uploadRate());
                           td.dlul += hrr.toString();
                           
                           // Rates in KiB/sec.
                           td.dlCount   = iter->downloadRate() / 1024;
                           td.ulCount   = iter->uploadRate() / 1024;

                           td.peerCount = iter->leechers();
                           td.seedCount = iter->seeders();

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

               // Sort the array by download bandwidth.
               std::sort(data.begin(), data.end());

               updateTable(*_timerdata->gui, data);
               updateGlobalStats(*_timerdata->gui, data);

               // Reset the counter used for detecting if its time to
               // update.
               _timerdata->count = 0;

               AG_LabelPrintf(_timerdata->gui->statusbarlabel, "Updated.");
            }


            void createTimer(btgvsGui & _gui, timerData* _timerdata)
            {
               // Create a new timer.
               AG_SetTimeout(_gui.timer, update_event, _timerdata, 0);
               AG_AddTimeout(_gui.table, _gui.timer, 1000 /* 1 sec */);
            }

            void createGui(btgvsGui & _gui)
            { 
               AG_SetRefreshRate(25);

               // AG_InitConfigWin(0);

               // Create a new window.
               _gui.window = AG_WindowNewNamed(AG_WINDOW_NOTITLE | AG_WINDOW_NOBORDERS, "%s", "BTGVS");
               AG_WindowSetCaption(_gui.window, "%s", "TEST");

               _gui.contents_box = AG_VBoxNew(_gui.window, AG_VBOX_EXPAND);

               // Create a new table that expands to fill the window.
               _gui.table = AG_TableNew(_gui.contents_box, AG_TABLE_EXPAND);
 
               // Insert the columns.
               std::string sizeSpecifier("<01234567890123456789>");
               AG_TableAddCol(_gui.table, "Filename", sizeSpecifier.c_str(), 0);
               AG_TableAddCol(_gui.table, "Status", sizeSpecifier.c_str(), 0);
               AG_TableAddCol(_gui.table, "Progress", sizeSpecifier.c_str(), 0);
               AG_TableAddCol(_gui.table, "Dl/Ul", sizeSpecifier.c_str(), 0);
               AG_TableAddCol(_gui.table, "Size", sizeSpecifier.c_str(), 0);
               AG_TableAddCol(_gui.table, "Peer/Seed", sizeSpecifier.c_str(), 0);
#if AGAR_1_2
               AG_LabelNew(_gui.contents_box, AG_LABEL_STATIC, "Totals:");
               _gui.bwLabel = AG_LabelNew(_gui.contents_box, AG_LABEL_STATIC,
                                          "Upload %s. Download %s.",
                                          _gui.uploadStr.c_str(), 
                                          _gui.downloadStr.c_str());
               _gui.peLabel = AG_LabelNew(_gui.contents_box, AG_LABEL_STATIC,
                                          "Peers %s. Seeds %s.",
                                          _gui.peersStr.c_str(), 
                                          _gui.seedsStr.c_str());
#elsif AGAR_1_3
               AG_LabelNewStatic(_gui.contents_box, AG_LABEL_STATIC, "Totals:");
               _gui.bwLabel = AG_LabelNewStatic(_gui.contents_box, AG_LABEL_STATIC,
                                                "Upload %s. Download %s.",
                                                _gui.uploadStr.c_str(), 
                                                _gui.downloadStr.c_str());
               _gui.peLabel = AG_LabelNewStatic(_gui.contents_box, AG_LABEL_STATIC,
                                                "Peers %s. Seeds %s.",
                                                _gui.peersStr.c_str(), 
                                                _gui.seedsStr.c_str());
#endif

               AG_SeparatorNew(_gui.contents_box, AG_SEPARATOR_HORIZ);

               // Status bar.
               _gui.statusbar      = AG_StatusbarNew(_gui.contents_box, 1);
               _gui.statusbarlabel = AG_StatusbarAddLabel(_gui.statusbar, AG_LABEL_STATIC, "Ready.");

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

            void updateGlobalStats(btgvsGui & _gui, std::vector<tableData> const& _data)
            {
               t_ulong ulRate = 0;
               t_ulong dlRate = 0;
               t_ulong seeds = 0;
               t_ulong peers = 0;

               for (std::vector<tableData>::const_iterator iter = _data.begin();
                    iter != _data.end();
                    iter++)
                  {
                     const tableData & td = *iter;
                     
                     ulRate += td.ulRate;
                     dlRate += td.dlRate;

                     seeds  += td.peerCount;
                     peers  += td.seedCount;
                  }

               // Rates.
               humanReadableRate hrr = humanReadableRate::convert(ulRate);
               _gui.uploadStr = hrr.toString();

               hrr = humanReadableRate::convert(dlRate);
               _gui.downloadStr = hrr.toString();

               // Peers and seeds.
               _gui.peersStr = convertToString<t_uint>(peers);
               _gui.seedsStr = convertToString<t_uint>(seeds);

               // Print on screen.
               AG_LabelPrintf(_gui.bwLabel, "Upload: %s. Download: %s.",
                              _gui.uploadStr.c_str(), 
                              _gui.downloadStr.c_str());

               AG_LabelPrintf(_gui.peLabel, "Peers: %s. Seeds: %s",
                              _gui.peersStr.c_str(),
                              _gui.seedsStr.c_str());

                
            }

            void update_statusbar(timerData* _timerdata)
            {
               t_int diff = _timerdata->gui->updateDelay - _timerdata->count;

               AG_LabelPrintf(_timerdata->gui->statusbarlabel, 
                              "Next update in %d second(s).", 
                              diff);
            }

         } // namespace viewer
      } // namespace gui
   } // namespace UI
} // namespace btg


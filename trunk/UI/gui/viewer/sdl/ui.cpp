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
#include <bcore/client/ratio.h>

#include <algorithm>

#include "agar-if.h"

#include <unistd.h>
#include <fcntl.h>

#include <SDL.h>

namespace btg
{
   namespace UI
   {
      namespace gui
      {
         namespace viewer
         {
            using namespace btg::core;

            int quitRequested = 0;

            static Uint32 update_event(void *obj, Uint32 ival, void *arg)
            {
               timerData* timerdata = (timerData*)arg;               

               timerdata->count++;

               if ((timerdata->gui->autoChange) && (timerdata->count%timerdata->gui->autoUpdateFreq)==0)
               {
                  timerdata->gui->direction = btgvsGui::DIR_RIGHT;
                  move(timerdata);
                  update_ui_display(timerdata);
               }
                     
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

            btgvsGui::btgvsGui(bool _auto, t_uint _autoUpdateFreq)
               : timer(0),
                 window(0),
                 basics_box(0),
                 statusbar(0),
                 statusbarlabel(0),
                 ul_max(100),
                 ul_min(0),
                 dl_max(100),
                 dl_min(0),
                 updateDelay(1000),
                 bwLabel(0),
                 uploadStr("0 K/s"),
                 downloadStr("0 K/s"),
                 peLabel(0),
                 peerSeedsStr("0/0"),
                 autoChange(_auto),
                 autoUpdateFreq(_autoUpdateFreq),
                 position(0),
                 size(0)
#if HAVE_LIRC
               , lircConfig(0)
               , lirc_socket(-1)
#endif
            {
               timer = new AG_Timeout;
            }

            btgvsGui::~btgvsGui()
            {
               delete timer;
               timer = 0;
            }

            void update_ui_display(timerData* _timerdata)
            {
               updateTorrentList(*_timerdata->gui, _timerdata->data);
               //updateGlobalStats(*_timerdata->gui, _timerdata->data);
            }
            
            void update_ui(timerData* _timerdata)
            {
               t_statusList statuslist;

               _timerdata->handler->reqStatus(-1, true);

               if (_timerdata->handler->statusListUpdated())
                  {
                     _timerdata->data.clear();
                     
                     _timerdata->handler->getStatusList(statuslist);
                     _timerdata->handler->resetStatusList();

                     torrentData td;

                     for (t_statusListI iter = statuslist.begin();
                          iter != statuslist.end();
                          iter++)
                        {
                           td.id = convertToString<t_int>(iter->contextID());
                           
                           std::string filename;

                           if (Util::getFileFromPath(iter->filename(), filename))
                              {
                                 td.filename = filename;
                              }
                           else
                              {
                                 td.filename = iter->filename();
                              }

                           td.done = iter->done();
                           
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

                           // Ratio.
                           btg::core::client::CalculateUlDlRatio(*iter, td.ratio);

                           td.dlRate = iter->downloadRate();
                           td.ulRate = iter->uploadRate();

                           humanReadableRate hrr = humanReadableRate::convert(iter->downloadRate());
                           td.dl = hrr.toString();

                           hrr = humanReadableRate::convert(iter->uploadRate());
                           td.ul = hrr.toString();
                           
                           // Rates in KiB/sec.
                           td.dlCount   = iter->downloadRate() / 1024;
                           td.ulCount   = iter->uploadRate() / 1024;

                           td.peerCount = iter->leechers();
                           td.seedCount = iter->seeders();

                           td.nof = "0";
                           
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

                           _timerdata->data.push_back(td);
                        }
                  }

               // Sort the array by download bandwidth.
               std::sort(_timerdata->data.begin(), _timerdata->data.end());

               _timerdata->gui->size = (_timerdata->data.size()-1);
               if (_timerdata->gui->size < 0)
               {
                  _timerdata->gui->size = 0;
               }
               
               update_ui_display(_timerdata);

               // Reset the counter used for detecting if its time to
               // update.
               _timerdata->count = 0;
            }


            void createTimer(btgvsGui & _gui, timerData* _timerdata)
            {
               // Create a new timer.
               AG_SetTimeout(_gui.timer, update_event, _timerdata, 0);
               AG_AddTimeout(_gui.window, _gui.timer, 1000 /* 1 sec */);
            }
            
#if HAVE_LIRC
            bool initLIRC(btgvsGui & _gui)
            {
               /* Setup LIRC client. */
               char appname[] = "btgvs";
               _gui.lirc_socket = lirc_init(appname,1);

               if (_gui.lirc_socket == -1)
               {
                  return false;
               }

               if (lirc_readconfig(0,&_gui.lircConfig,0) == -1)
               {
                  return false;
               }

               // Use a non blocking socket for LIRC.
               fcntl(_gui.lirc_socket, F_SETOWN, getpid());
               int flags = fcntl(_gui.lirc_socket, F_GETFL, 0);
               if (flags == -1) 
               {
                  return false;
               }

               if (fcntl(_gui.lirc_socket, F_SETFL, flags|O_NONBLOCK) == -1)
               {
                  return false;
               }

               return true;
            }

            bool pollLirc(btgvsGui & _gui)
            {
               char* code = 0;
               char* c    = 0;
               bool cont  = true;

               while (cont)
               {
                  int ret = lirc_nextcode(&code);

                  if (ret == -1)
                  {
                     return false;
                  }

                  if (code == 0)
                  {
                     cont = false;
                     continue;
                  }

                  while((ret=lirc_code2char(_gui.lircConfig,code,&c))==0 && c!=0)
                  {
                     const int size = strlen(c);
                     
                     if (size == 2) 
                     {
                        if (strcmp(c, "up") == 0)
                        {
                           pushEvent(SDLK_UP);
                        }
                     }
                     else if (size == 4)
                     {
                        if (strcmp(c, "down") == 0)
                        {
                           pushEvent(SDLK_DOWN);
                        }
                        else if (strcmp(c, "left") == 0)
                        {
                           pushEvent(SDLK_LEFT);
                        }
                        else if (strcmp(c, "quit") == 0)
                        {
                           pushEvent(SDLK_ESCAPE);
                        }
                        else if (strcmp(c, "back") == 0)
                        {
                           pushEvent(SDLK_BACKSPACE);
                        }
                     }
                     else if (size == 5)
                     {
                        if (strcmp(c, "right") == 0)
                        {
                           pushEvent(SDLK_RIGHT);
                        }
                        else if (strcmp(c, "enter") == 0)
                        {
                           pushEvent(SDLK_RETURN);
                        }
                     }
                  }
                  free(code);
                }
               return true;
            }
#endif

            void move(timerData* _td)
            {
               int pos = _td->gui->position;
               switch (_td->gui->direction)
               {
                  case btgvsGui::DIR_UP:
                     _td->gui->position=_td->gui->size;
                     break;
                  case btgvsGui::DIR_DOWN:
                     _td->gui->position=0;
                     break;
                  case btgvsGui::DIR_LEFT:
                     _td->gui->position--;
                     break;
                  case btgvsGui::DIR_RIGHT:
                     _td->gui->position++;
                     break;
               }
               
               if (_td->gui->position < 0)
               {
                  _td->gui->position = _td->gui->size;
               }
               
               if (_td->gui->position > _td->gui->size)
               {
                  _td->gui->position = 0;
               }
               
               if (pos != _td->gui->position)
               {
                  // Position changed, update ui.
                  updateTorrentList(*_td->gui, _td->data);
               }
            }
            
            void KeyDown(AG_Event *event)
            {
               if (strcmp(event->name, "window-keydown") == 0)
               {
                  timerData* td = (timerData*)AG_PTR(1);
                  int keysym  = AG_INT(2);
                  
                  if (td->gui->autoChange && 
                  ((keysym == SDLK_UP) || (keysym == SDLK_DOWN) || (keysym == SDLK_LEFT) || (keysym == SDLK_RIGHT))
                  )
                  {
                     // When the viewer is scrolling trough the torrents 
                     // on its own, do not allow key input.
                     return;
                  }
                  
                  switch (keysym)
                  {
                     case SDLK_UP:
                     {
                       std::cout << "<up>" << std::endl;
                       td->gui->direction = btgvsGui::DIR_UP;
                       move(td);
                       break;
                     }
                     case SDLK_DOWN:
                     {
                       std::cout << "<down>" << std::endl;
                       td->gui->direction = btgvsGui::DIR_DOWN;
                       move(td);
                       break;
                     }
                     case SDLK_LEFT:
                     {
                       std::cout << "<left>" << std::endl;
                       td->gui->direction = btgvsGui::DIR_LEFT;
                       move(td);
                       break;
                     }
                     case SDLK_RIGHT:
                     {
                       std::cout << "<right>" << std::endl;
                       td->gui->direction = btgvsGui::DIR_RIGHT;
                       move(td);
                       break;
                     }
                     case SDLK_RETURN:
                     {
                       std::cout << "<enter>" << std::endl;
                       break;
                     }
                     case SDLK_BACKSPACE:
                     {
                       std::cout << "<back>" << std::endl;
                       break;
                     }
                     case SDLK_ESCAPE:
                     {
                       std::cout << "<quit>" << std::endl;
                       quitRequested = 1;
                       break;
                     }
                  }
               }
            }

            void createGui(btgvsGui & _gui, timerData & _timerData)
            { 
               //AG_SetRefreshRate(25);

               setColors();

               setUIStyle(_gui.style);

               // Create a new window.
               _gui.window = AG_WindowNewNamed(AG_WINDOW_NOTITLE | AG_WINDOW_NOBORDERS, 
                                               "%s", "BTGVS");
               AG_SetStyle(_gui.window, &_gui.style);
               AG_WindowSetCaption(_gui.window, "%s", "BTGVS");
               
               _gui.top_box = AG_BoxNew(_gui.window, AG_BOX_VERT, AG_BOX_EXPAND);

               _gui.torrentNumberLabel = AG_LabelNew(_gui.top_box, AG_LABEL_HFILL, "0");

               AG_SpacerNewVert (_gui.top_box);
                                                  
               // Divide the screen into four sub-parts.
               _gui.hp = AG_PaneNewVert(_gui.top_box, AG_PANE_FRAME|AG_PANE_DIV|AG_PANE_FORCE_DIV);
               // AG_Expand(_gui.hp);
               
               _gui.vp = AG_PaneNewHoriz(_gui.hp->div[0], AG_PANE_FORCE_DIV);
               _gui.dp = AG_PaneNewHoriz(_gui.hp->div[1], AG_PANE_FORCE_DIV);

               int padding = 10;
               int spacing = 10;
               int depth = 0;
               
               _gui.basics_box = AG_BoxNew(_gui.vp, AG_BOX_VERT, AG_BOX_EXPAND);
               // AG_BoxSetLabel(_gui.basics_box, "%s", "Basic");
               AG_BoxSetPadding(_gui.basics_box, padding);
               AG_BoxSetSpacing(_gui.basics_box, spacing);
               AG_BoxSetDepth(_gui.basics_box, depth);

               _gui.done_box  = AG_BoxNew(_gui.vp, AG_BOX_VERT, AG_BOX_EXPAND);
               //AG_BoxSetLabel(_gui.done_box, "%s", "Done");
               AG_BoxSetPadding(_gui.done_box, padding);
               AG_BoxSetSpacing(_gui.done_box, spacing);
               AG_BoxSetDepth(_gui.done_box, depth);
               
               
               _gui.speed_box = AG_BoxNew(_gui.dp, AG_BOX_VERT, AG_BOX_EXPAND);
               //AG_BoxSetLabel(_gui.speed_box, "%s", "Speed");
               AG_BoxSetPadding(_gui.speed_box, padding);
               AG_BoxSetSpacing(_gui.speed_box, spacing);
               AG_BoxSetDepth(_gui.speed_box, depth);
               

               _gui.files_box = AG_BoxNew(_gui.dp, AG_BOX_VERT, AG_BOX_EXPAND);
               //AG_BoxSetLabel(_gui.files_box, "%s", "Files");
               AG_BoxSetPadding(_gui.files_box, padding);
               AG_BoxSetSpacing(_gui.files_box, spacing);
               AG_BoxSetDepth(_gui.files_box, depth);
               
               _gui.panes[0] = _gui.basics_box;
               _gui.panes[1] = _gui.done_box;
               _gui.panes[2] = _gui.speed_box;
               _gui.panes[3] = _gui.files_box;

               AG_PaneAttachBox(_gui.vp, 0, _gui.panes[0]);
               AG_PaneAttachBox(_gui.vp, 1, _gui.panes[1]);
               AG_PaneAttachBox(_gui.dp, 0, _gui.panes[2]);
               AG_PaneAttachBox(_gui.dp, 1, _gui.panes[3]);
               
               // Basic information.
               AG_LabelNew(_gui.basics_box, 0, "Id:");
               _gui.idLabel = AG_LabelNew(_gui.basics_box, AG_LABEL_HFILL, "0");
               
               
               AG_LabelNewStatic(_gui.basics_box, 0, "Filename:");
               _gui.fileLabel = AG_LabelNew(_gui.basics_box, AG_LABEL_HFILL, "0");
               
               AG_LabelNew(_gui.basics_box, 0, "Status:");
               _gui.statusLabel = AG_LabelNew(_gui.basics_box, AG_LABEL_HFILL, "0");
               
               // How far is the torrent.
               AG_LabelNew(_gui.done_box, 0, "Done:");

               _gui.pb_value = 0;
               _gui.pb_min   = 0;
               _gui.pb_max   = 100; 
               AG_ProgressBarNewInt (_gui.done_box, 
                                     AG_PROGRESS_BAR_HORIZ, 
                                     AG_PROGRESS_BAR_SHOW_PCT, 
                                     &_gui.pb_value, &_gui.pb_min, &_gui.pb_max);
               
               AG_LabelNew(_gui.done_box, 0, "ETA:");
               _gui.etaLabel = AG_LabelNew(_gui.done_box, AG_LABEL_HFILL, "0");
               
               // Speed ..
               AG_LabelNew(_gui.speed_box, 0, "Download:");
               _gui.dlSpeedLabel = AG_LabelNew(_gui.speed_box, AG_LABEL_HFILL, "0");
               
               AG_LabelNew(_gui.speed_box, 0, "Upload:");
               _gui.ulSpeedLabel = AG_LabelNew(_gui.speed_box, AG_LABEL_HFILL, "0");
                            
               // Files ..
               AG_LabelNew(_gui.files_box, 0, "Number of files:");
               _gui.nofLabel = AG_LabelNew(_gui.files_box, AG_LABEL_HFILL, "0");

               AG_LabelNew(_gui.files_box, 0, "Total size:");
               _gui.fileSizeLabel = AG_LabelNew(_gui.files_box, AG_LABEL_HFILL, "0");

               AG_LabelNew(_gui.files_box, 0, "Seeds/leeches:");
               _gui.seedsPeersLabel = AG_LabelNew(_gui.files_box, AG_LABEL_HFILL, "0");
               
               const char spaces[] = "                                                                                ";
               AG_LabelNew(_gui.basics_box, 0, "%s", &spaces[0]);
               AG_LabelNew(_gui.done_box,   0, "%s", &spaces[0]);
               AG_LabelNew(_gui.speed_box,  0, "%s", &spaces[0]);
               AG_LabelNew(_gui.files_box,  0, "%s", &spaces[0]);
               
               // Status bar.
               _gui.statusbar      = AG_StatusbarNew(_gui.top_box, 1);
               _gui.statusbarlabel = AG_StatusbarAddLabel(_gui.statusbar, AG_LABEL_STATIC, "Ready.");
               
               // Show the window and enter event loop.
               AG_WindowShow(_gui.window);
               //AG_WindowMaximize(_gui.window);

               AG_AddEvent(_gui.window, "window-keydown", KeyDown, "%p", &_timerData);
               
               // Set the focus on the table.
               // AG_WidgetFocus(_gui.table);
            }

            void run(btgvsGui & _gui)
            {
               SDL_Event ev;
               AG_Window* win = 0;
               Uint32 Tr1     = SDL_GetTicks();
               Uint32 Tr2     = 0;
               for (;;) 
                  {
                     Tr2 = SDL_GetTicks();
                     if (Tr2-Tr1 >= agView->rNom) 
                     {		
                        /* Time to redraw? */
                        AG_LockVFS(agView);

                        /* Render GUI elements */
                        AG_BeginRendering();
                        AG_TAILQ_FOREACH(win, &agView->windows, windows) 
                           {
                              AG_ObjectLock(win);
                              AG_WindowDraw(win);
                              AG_ObjectUnlock(win);
                           }
                        AG_EndRendering();
                        AG_UnlockVFS(agView);

                        /* Recalibrate the effective refresh rate. */
                        Tr1 = SDL_GetTicks();
                        agView->rCur = agView->rNom - (Tr1-Tr2);
                        if (agView->rCur < 1) 
                        {
                           agView->rCur = 1;
                        }
                     } 
                     else if (SDL_PollEvent(&ev) != 0) 
                     {
                        /* Send all SDL events to Agar-GUI. */
                        AG_ProcessEvent(&ev);
                     } 
                     else if (AG_TAILQ_FIRST(&agTimeoutObjQ) != NULL) 
                     {
                        /* Advance the timing wheels. */
                        AG_ProcessTimeout(Tr2);
                     } 
                     else if (agView->rCur > agIdleThresh) 
                     {
                        /* Idle the rest of the time. */
                        SDL_Delay(agView->rCur - agIdleThresh);
                     }
                     else if (agView->rCur > agIdleThresh)
                     {
                        /* Idle the rest of the time. */
                        SDL_Delay(agView->rCur - agIdleThresh);
                     }


#if HAVE_LIRC
                     // Translate LIRC input into SDL key press events.
                     // These will be delivered to AGAR for processing 
                     // at some point.
                     if (!pollLirc(_gui))
                     {
                        // Reading from LIRC failed.
                        return;
                     }
#endif
                    // Exit from application requested, so stop processing.
                    if (quitRequested == 1)
                    {
                      return;
                    }
                  }
            }

            void pushEvent(SDLKey _k)
            {
              std::cout << "Sending key press event 0x" << std::hex << (int)_k << std::dec << "." << std::endl;
              SDL_Event e;
              e.key.type   = SDL_KEYDOWN;
              e.key.state  = SDL_PRESSED;
              e.key.keysym.scancode = 0;
              e.key.keysym.sym      = _k;
              e.key.keysym.mod      = KMOD_NONE;
              e.key.keysym.unicode  = 0;
              SDL_PushEvent(&e);

              e.key.type   = SDL_KEYUP;
              e.key.state  = SDL_RELEASED;
              SDL_PushEvent(&e);

            }

            void destroyGui(btgvsGui & _gui)
            {
   
            }

            void updateTorrentDetails(btgvsGui & _gui, torrentData const& _data)
            {
               AG_LabelPrintf(_gui.idLabel,      "%s", _data.id.c_str());
               AG_LabelPrintf(_gui.fileLabel,    "%s", _data.filename.c_str());
               AG_LabelPrintf(_gui.statusLabel,  "%s", _data.status.c_str());
               AG_LabelPrintf(_gui.etaLabel,     "%s", _data.progress.c_str());
               AG_LabelPrintf(_gui.dlSpeedLabel, "%s", _data.dl.c_str());
               AG_LabelPrintf(_gui.ulSpeedLabel, "%s", _data.ul.c_str());
               AG_LabelPrintf(_gui.nofLabel,     "%s", _data.nof.c_str());
               AG_LabelPrintf(_gui.fileSizeLabel,   "%s", _data.size.c_str());
               AG_LabelPrintf(_gui.seedsPeersLabel, "%s", _data.peers.c_str());

               _gui.pb_value = _data.done;
            }
            
            void updateTorrentList(btgvsGui & _gui, std::vector<torrentData> const& _data)
            {
               if (_data.size() == 0)
               {
                  AG_LabelPrintf(_gui.torrentNumberLabel, "%s" "No torrents to show.");
                  return;
               }
               
               if (_gui.position > _data.size())
               {
                  _gui.position = 0;  
               }
                  
               AG_LabelPrintf(_gui.torrentNumberLabel, "Showing torrent %d out of %d.",
                              _gui.position,
                              _data.size()-1);

               updateTorrentDetails(_gui, _data[_gui.position]);
            }

            void updateGlobalStats(btgvsGui & _gui, std::vector<torrentData> const& _data)
            {
               t_ulong ulRate = 0;
               t_ulong dlRate = 0;
               t_ulong seeds = 0;
               t_ulong peers = 0;

               for (std::vector<torrentData>::const_iterator iter = _data.begin();
                    iter != _data.end();
                    iter++)
                  {
                     const torrentData & td = *iter;
                     
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
               /*
               _gui.peerSeedsStr = convertToString<t_uint>(peers);
               _gui.peerSeedsStr += "/";
               _gui.peerSeedsStr += convertToString<t_uint>(seeds);
               */
               // Print on screen.
               /*
               AG_LabelPrintf(_gui.bwLabel, "Upload: %s. Download: %s.",
                              _gui.uploadStr.c_str(), 
                              _gui.downloadStr.c_str());

               AG_LabelPrintf(_gui.peLabel, "Peers: %s. Seeds: %s",
                              _gui.peersStr.c_str(),
                              _gui.seedsStr.c_str());
               */
                
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


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

#include <bcore/logmacro.h>

#include "detailwindow.h"
#include "helpwindow.h"
#include "filelist.h"
#include "fileview.h"
#include "peerlist.h"

#include "basemenu.h"
#include "limitwindow.h"

#include <bcore/client/handlerthr.h>
#include "handler.h"

#include <bcore/command/limit_base.h>
#include <bcore/hrr.h>

#define GET_HANDLER_INST \
   boost::shared_ptr<boost::mutex> ptr = handlerthread_.mutex(); \
   boost::mutex::scoped_lock interface_lock(*ptr); \
   Handler* handler = dynamic_cast<Handler*>(&handlerthread_.handler());

namespace btg
{
   namespace UI
   {
      namespace cli
      {
         using namespace btg::core;

         void UI::handleMenuEntry(menuEntries const _menuentry,
                                  t_int const _context_id,
                                  std::string const& _filename)
         {
            switch (_menuentry)
               {
               case UI::M_START:
                  {
                     {
                        GET_HANDLER_INST;

                        handler->reqStart(_context_id);

                        if (handler->commandSuccess())
                           {
                              actionSuccess("Start", _filename);
                           }
                        else
                           {
                              actionFailture("Start", _filename);
                           }
                     }
                     break;
                  }
               case UI::M_STOP:
                  {
                     {
                        GET_HANDLER_INST;

                        handler->reqStop(_context_id);

                        if (handler->commandSuccess())
                           {
                              actionSuccess("Stop", _filename);
                           }
                        else
                           {
                              actionFailture("Stop", _filename);
                           }
                     }

                     break;
                  }
               case UI::M_ABORT:
                  {
                     {
                        GET_HANDLER_INST;

                        handler->reqAbort(_context_id, false);

                        if (handler->commandSuccess())
                           {
                              handler->resetStatusList();

                              std::vector<t_int> id_list;
                              id_list.push_back(_context_id);
                              mainwindow_.remove(id_list);
                              refresh();

                              actionSuccess("Abort", _filename);
                           }
                        else
                           {
                              actionFailture("Abort", _filename);
                           }
                     }
                     break;
                  }
               case UI::M_ERASE:
                  {
                     {
                        GET_HANDLER_INST;

                        handler->reqAbort(_context_id, true /* erase */);

                        if (handler->commandSuccess())
                           {
                              handler->resetStatusList();

                              std::vector<t_int> id_list;
                              id_list.push_back(_context_id);
                              mainwindow_.remove(id_list);
                              refresh();

                              actionSuccess("Erase", _filename);
                           }
                        else
                           {
                              actionFailture("Erase", _filename);
                           }
                     }
                     break;
                  }
               case UI::M_LIMIT:
                  {
                     statuswindow_.setStatus("Limiting torrent.");

                     t_int download    = limitBase::LIMIT_DISABLED;
                     t_int upload      = limitBase::LIMIT_DISABLED;
                     t_int seedTimeout = limitBase::LIMIT_DISABLED;
                     t_int seedPercent = limitBase::LIMIT_DISABLED;

                     if (limitPopup(_context_id,
                                    download,
                                    upload,
                                    seedTimeout,
                                    seedPercent))
                        {
                           setLimit(_context_id,
                                    _filename,
                                    download,
                                    upload,
                                    seedTimeout,
                                    seedPercent);
                        }
                     else
                        {
                           statuswindow_.setStatus("Setting limits cancelled.");
                        }

                     break;
                  }
               case UI::M_CLEAN:
                  {
                     {
                        GET_HANDLER_INST;

                        handler->reqClean(_context_id);

                        if (handler->commandSuccess())
                           {
                              handler->resetStatusList();

                              std::vector<t_int> id_list;
                              id_list.push_back(_context_id);
                              mainwindow_.remove(id_list);
                              refresh();

                              actionSuccess("Clean", _filename);
                           }
                        else
                           {
                              actionFailture("Clean", _filename);
                           }
                     }
                     break;
                  }
               case UI::M_SHOWFILES:
                  {
                     {
                        GET_HANDLER_INST;

                        handler->reqFileInfo(_context_id);

                        if (handler->commandSuccess())
                           {
                              actionSuccess("Show files", _filename);
                              std::vector<btg::core::fileInformation> fi_lst;
                              handler->getLastFileInfoList(fi_lst);
                              handleShowFiles(fi_lst);
                           }
                        else
                           {
                              actionFailture("Show files", _filename);
                           }
                     }
                     break;
                  }
               case UI::M_SELECTFILES:
                  {
                     {
                        GET_HANDLER_INST;

                        handler->reqGetFiles(_context_id);
                              
                        if (handler->commandSuccess())
                           {
                              btg::core::selectedFileEntryList files;
                              handler->getLastSelectedFiles(files);
                              switch (handleSelectFiles(_context_id, files))
                                 {
                                 case sS_SELECT_RESIZE:
                                    {
                                       actionAbortedByResize("Select files", _filename);
                                       break;
                                    }
                                 case sS_SELECT_SUCCESS:
                                    {
                                       actionSuccess("Select files", _filename);
                                       break;
                                    }
                                 case sS_SELECT_FAILURE:
                                    {
                                       actionFailture("Select files", _filename);
                                       break;
                                    }
                                 case sS_SELECT_ABORT:
                                    {
                                       actionAborted("Select files", _filename);
                                       break;
                                    }
                                 }

                           }
                        else
                           {
                              actionFailture("Select files", _filename);
                           }
                     }
                     break;
                  }
               case UI::M_SHOWPEERS:
                  {
                     {
                        GET_HANDLER_INST;

                        handler->reqPeers(_context_id);

                        if (handler->commandSuccess())
                           {
                              actionSuccess("Show peers", _filename);
                              t_peerList peerlist;
                              handler->getLastPeers(peerlist);
                              handleShowPeers(peerlist);
                           }
                        else
                           {
                              actionFailture("Show peers", _filename);
                           }
                     }
                     break;
                  }
               case UI::M_MOVE:
                  {
                     handleMoveToSession(_context_id, _filename);
                     break;
                  }
               case UI::M_UNDEF:
                  {
                     break;
                  }
               }
         }

         void UI::handleMenu()
         {
            btg::core::Status teststatus;
            if (!mainwindow_.getSelection(teststatus))
               {
                  // There is nothing to show a menu for.
                  statuswindow_.setError("Not showing menu, no torrents.");

                  return;
               }

            statuswindow_.setStatus("Select entry.");

            std::vector<menuEntry> contents;

            contents.push_back(menuEntry(UI::M_START, "Start", "Start a torrent"));
            contents.push_back(menuEntry(UI::M_STOP,  "Stop",  "Stop a torrent"));
            contents.push_back(menuEntry(UI::M_ABORT, "Abort", "Abort a torrent"));
            contents.push_back(menuEntry(UI::M_ERASE, "Erase", "Erase a torrent"));
            contents.push_back(menuEntry(UI::M_LIMIT, "Limit", "Limit a torrent"));
            contents.push_back(menuEntry(UI::M_CLEAN, "Clean", "Clean a torrent"));
            contents.push_back(menuEntry(UI::M_SHOWFILES, "Show files", 
                                         "Show the files contained in a torrent"));
            contents.push_back(menuEntry(UI::M_SELECTFILES, "Select files", 
                                         "Select files to download"));
            contents.push_back(menuEntry(UI::M_SHOWPEERS, "Show peers", 
                                         "Show the peers for a torrent"));
            contents.push_back(menuEntry(UI::M_MOVE, "Move to session", 
                                         "Move to another session"));
            
            windowSize menudimensions;
            mainwindow_.getSize(menudimensions);

            baseMenu bm(keymap_, menudimensions, "Torrent menu", contents, statuswindow_);

            switch(bm.run())
               {
               case dialog::R_RESIZE:
                  {
                     // the window was resized.
                     handleResizeMainWindow();
		  
                     // abort showing menu.
                     return;
                     break;
                  }
               case dialog::R_NCREAT:
                  {
                     return;
                     break;
                  }
               default:
                  {
                     break;
                  }
               }

            if (bm.getResult() == baseMenu::BM_cancel)
               {
                  statuswindow_.setStatus("Selection aborted.");
                  return;
               }

            menuEntries entry = UI::M_UNDEF;
            switch (bm.getResult())
               {
               case UI::M_START:
                  entry = UI::M_START;
                  break;
               case UI::M_STOP:
                  entry = UI::M_STOP;
                  break;
               case UI::M_ABORT:
                  entry = UI::M_ABORT;
                  break;
               case UI::M_ERASE:
                  entry = UI::M_ERASE;
                  break;
               case UI::M_LIMIT:
                  entry = UI::M_LIMIT;
                  break;
               case UI::M_CLEAN:
                  entry = UI::M_CLEAN;
                  break;
               case UI::M_SHOWFILES:
                  entry = UI::M_SHOWFILES;
                  break;
               case UI::M_SELECTFILES:
                  entry = UI::M_SELECTFILES;
                  break;
               case UI::M_SHOWPEERS:
                  entry = UI::M_SHOWPEERS;
                  break;
               case UI::M_MOVE:
                  entry = UI::M_MOVE;
                  break;
               case UI::M_UNDEF:
                  break;
               default:
                  break;
               }

            std::vector<t_int> selected_ids;
            mainwindow_.getMarked(selected_ids);

            btg::core::Status status;

            if (selected_ids.size() > 0)
               {
                  // A number of torrents was marked.
                  //

                  // Handle setting more limits than one.
                  if (entry == UI::M_LIMIT)
                     {
                        handleManyLimits(selected_ids);
                        return;
                     }

                  std::vector<t_int>::const_iterator iter;
                  for (iter = selected_ids.begin();
                       iter != selected_ids.end();
                       iter++)
                     {
                        if (mainwindow_.get(*iter, status))
                           {
                              t_int context_id     = status.contextID();
                              std::string filename = status.filename();

                              handleMenuEntry(entry,
                                              context_id,
                                              filename);
                           }
                     }
                  
                  // Force updating of contexts.
                  handlerthread_.forceUpdate();
               }
            else
               {
                  // Nothing was marked, use the currently selected
                  // torrent.
                  //
                  if (!mainwindow_.getSelection(status))
                     {
                        return;
                     }

                  // Something was marked.

                  t_int context_id     = status.contextID();
                  std::string filename = status.filename();

                  handleMenuEntry(entry,
                                  context_id,
                                  filename);

                  // Force updating of contexts.
                  handlerthread_.forceUpdate();
               }
         }

      } // namespace cli
   } // namespace UI
} // namespace btg


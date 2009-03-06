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

#include <bcore/client/handlerthr.h>
#include <bcore/client/urlhelper.h>
#include <bcore/client/loadhelper.h>
#include <bcore/command/limit_base.h>
#include <bcore/hrr.h>
#include <bcore/os/sleep.h>
#include <bcore/opstatus.h>
#include <bcore/logmacro.h>

#include "handler.h"
#include "detailwindow.h"
#include "helpwindow.h"
#include "filelist.h"
#include "fileview.h"
#include "fileselect.h"
#include "peerlist.h"
#include "basemenu.h"
#include "limitwindow.h"
#include "snwindow.h"
#include "textinput.h"
#include "progress.h"
#include "sessionselect.h"
#include "dm.h"

#define GET_HANDLER_INST                                                \
   boost::shared_ptr<boost::mutex> ptr = handlerthread_.mutex();       \
   boost::mutex::scoped_lock interface_lock(*ptr);                      \
   Handler* handler = dynamic_cast<Handler*>(&handlerthread_.handler());

namespace btg
{
   namespace UI
   {
      namespace cli
      {
         using namespace btg::core;

         bool UI::handleKeyboard()
         {
            bool status = true; // Continue reading.

            keyMapping::KEYLABEL label = mainwindow_.handleKeyboard();

            switch (label)
               {
               case keyMapping::K_UNDEF:
                  {
                     // Handle status updates, used to fill the list
                     // of torrents.
                     {
                        GET_HANDLER_INST;

                        bool statusUpdated = handler->statusListUpdated();
                        if (statusUpdated)
                           {
                              t_statusList statusList;
                              handler->getStatusList(statusList);
                              update(statusList);
                              updateBandwidth(statusList);
                           }
                        // No torrents, showing empty list.
                        if (handler->statusSize() == 0)
                           {
                              t_statusList statusList;
                              update(statusList);
                              updateBandwidth(statusList);
                              clear();
                           }

                        refresh();
                     }
                     break;
                  }
               case keyMapping::K_QUIT:
                  {
                     status = handleQuit();
                     switch (status)
                        {
                        case true:
                           statuswindow_.setStatus("Quit aborted.");
                           break;
                        case false:
                           statuswindow_.setStatus("Quitting.");
                           break;
                        }

                     refresh();
                     break;
                  }
               case keyMapping::K_DETACH:
                  {
                     {
                        GET_HANDLER_INST;

                        handler->reqDetach();
                     }

                     status = false;
                     break;
                  }
               case keyMapping::K_DOWN:
                  {
                     mainwindow_.moveDown();
                     refresh();
                     break;
                  }
               case keyMapping::K_UP:
                  {
                     mainwindow_.moveUp();
                     refresh();
                     break;
                  }
               case keyMapping::K_LIST_START:
                  {
                     mainwindow_.toStart();
                     refresh();
                     break;
                  }
               case keyMapping::K_LIST_END:
                  {
                     mainwindow_.toEnd();
                     refresh();
                     break;
                  }
               case keyMapping::K_SELECT:
                  {
                     handleShowDetails();
                     refresh();
                     break;
                  }
               case keyMapping::K_MARK:
                  {
                     handleMark();
                     break;
                  }
               case keyMapping::K_MARK_ALL:
                  {
                     handleMarkAll();
                     break;
                  }
               case keyMapping::K_MENU:
                  {
                     handleMenu();
                     refresh();
                     break;
                  }
               case keyMapping::K_SESNAME:
                  {
                     handleSessionName();
                     refresh();
                     break;
                  }
               case keyMapping::K_LOAD:
                  {
                     handleLoad();
                     refresh();
                     break;
                  }
               case keyMapping::K_LOAD_URL:
                  {
                     if (urlDlEnabled_)
                        {
                           handleLoadUrl();
                           refresh();
                        }
                     else
                        {
                           statuswindow_.setError("URL downloading not supported by daemon.");
                           refresh();
                        }
                     break;
                  }
               case keyMapping::K_GLIMIT:
                  {
                     handleGlobalLimit();
                     refresh();
                     break;
                  }
               case keyMapping::K_HELP:
                  {
                     switch (handleHelp())
                        {
                        case dialog::R_RESIZE:
                           {
                              handleResizeMainWindow();
                              break;
                           }
                        case dialog::R_NCREAT:
                           {
                              statuswindow_.setError("Unable to show help - resize window.");
                              break;
                           }
                        default:
                           {
                              setDefaultStatusText();
                              break;
                           }
                        }

                     refresh();
                     break;
                  }
               case keyMapping::K_SORT:
                  {
                     handleSort();
                     break;
                  }
               case keyMapping::K_RESIZE:
                  {
                     handleResizeMainWindow();
                     break;
                  }
               default:
                  {
                     break;
                  }
               }

            return status;
         }

         dialog::RESULT UI::handleShowDetailsHelp()
         {
            // Show a help window on the middle of the screen.
            std::vector<std::string> helpText;

            helpText.push_back("Help");
            helpText.push_back("    ");

            std::string keyDescr;

            if (helpWindow::generateHelpForKey(keymap_,
                                               keyMapping::K_BACK,
                                               "to go back",
                                               keyDescr))
               {
                  helpText.push_back(keyDescr);
               }

            helpText.push_back(" ");

            helpWindow hw(keymap_, helpText);

            return hw.run();
         }

         void UI::handleShowDetails()
         {
            btg::core::Status teststatus;
            if (!mainwindow_.getSelection(teststatus))
               {
                  // Nothing to display.
                  return;
               }

            bool details_resized = false;

            windowSize detailsdimension;
            mainwindow_.getSize(detailsdimension);
            {
               setDefaultStatusText();

               mainwindow_.clear();
               detailWindow dw(keymap_, mainwindow_);
               dw.init(detailsdimension);
               dw.refresh();

               keyMapping::KEYLABEL label;

               bool cont = true;

               while (cont)
                  {
                     label = dw.handleKeyboard();
                     switch (label)
                        {
                        case keyMapping::K_UNDEF:
                           {
                              {
                                 GET_HANDLER_INST;

                                 bool statusUpdated = handler->statusListUpdated();
                                 if (statusUpdated)
                                    {
                                       t_statusList statusList;
                                       handler->getStatusList(statusList);
                                       update(statusList);
                                       // Redraw the contents.
                                       dw.refresh();
                                    }
                              }
                              break;
                           }
                        case keyMapping::K_SELECT:
                        case keyMapping::K_BACK:
                           {
                              cont = false;
                              dw.clear();
                              break;
                           }
                        case keyMapping::K_HELP:
                           {
                              switch (handleShowDetailsHelp())
                                 {
                                 case dialog::R_RESIZE:
                                    {
                                       // window was resized.
                                       cont            = false;
                                       details_resized = true;
                                       dw.clear();
                                       break;
                                    }
                                 default:
                                    {
                                       break;
                                    }
                                 }
                              break;
                           }
                        case keyMapping::K_RESIZE:
                           {
                              cont            = false;
                              details_resized = true;
                              dw.clear();
                              break;
                           }
                        default:
                           {
                              // Do nothing.
                              break;
                           }
                        }
                  }

               // dw.destroy();
            }

            setDefaultStatusText();

            if (details_resized)
               {
                  handleResizeMainWindow();
               }

            mainwindow_.refresh();
         }

         void UI::handleMark()
         {
            mainwindow_.mark();
            mainwindow_.moveDown();
            refresh();
         }

         void UI::handleMarkAll()
         {
            mainwindow_.markAll();
            refresh();
         }

         void UI::handleSessionName()
         {
            GET_HANDLER_INST;
            statuswindow_.setStatus("Setting session name.");

            std::string session_name;

            handler->reqSessionName();
            session_name = handler->getCurrentSessionName();

            windowSize sndimensions;
            mainwindow_.getSize(sndimensions);

            sessionNameWindow snw(keymap_,
                                  sndimensions,
                                  session_name);

            switch (snw.run())
               {
               case dialog::R_NCREAT:
                  {
                     statuswindow_.setStatus("Unable to set session name.");
                     break;
                  }
               case dialog::R_RESIZE:
                  {
                     // the window was resized.
                     handleResizeMainWindow();
                     statuswindow_.setStatus("Setting session name aborted.");
                     break;
                  }
               case dialog::R_QUIT:
                  {
                     std::string newname = snw.getName();
                     if (session_name != newname)
                        {
                           handler->reqSetSessionName(newname);
                           if (handler->commandSuccess())
                              {
                                 statuswindow_.setStatus("Session name set.");
                              }
                           else
                              {
                                 statuswindow_.setStatus("Session name not set.");
                              }
                        }
                     else
                        {
                           statuswindow_.setStatus("Session name not changed.");
                        }
                     break;
                  }
               }
         }

         void UI::handleManyLimits(std::vector<t_int> const& _context_ids)
         {
            statuswindow_.setStatus("Limiting torrents.");

            t_int download    = limitBase::LIMIT_DISABLED;
            t_int upload      = limitBase::LIMIT_DISABLED;
            t_int seedTimeout = limitBase::LIMIT_DISABLED;
            t_int seedPercent = limitBase::LIMIT_DISABLED;

            std::vector<t_int>::const_iterator iter = _context_ids.begin();

            t_int context_id = *iter;

            if (limitPopup(context_id,
                           download,
                           upload,
                           seedTimeout,
                           seedPercent))
               {
                  // Got limits, now set them on all the contexts.

                  t_int succ = 0;
                  t_int fail = 0;

                  for (iter = _context_ids.begin();
                       iter != _context_ids.end();
                       iter++)
                     {
                        // Find the filename.
                        btg::core::Status status;
                        mainwindow_.get(*iter, status);

                        std::string filename = status.filename();

                        if (setLimit(*iter,
                                     filename,
                                     download,
                                     upload,
                                     seedTimeout,
                                     seedPercent))
                           {
                              succ++;
                           }
                        else
                           {
                              fail++;
                           }
                     }

                  if (fail == 0)
                     {
                        // Success for all torrents.
                        statuswindow_.setStatus("Limited selected torrents.");
                     }
                  else
                     {
                        statuswindow_.setError("Unable to limit all selected torrents.");
                     }
               }
            else
               {
                  statuswindow_.setStatus("Setting limits aborted.");
               }
         }

         dialog::RESULT UI::handleHelp()
         {
            // Show a help window on the middle of the screen.
            std::vector<std::string> helpText;

            helpText.push_back("Help");
            helpText.push_back("    ");
            helpText.push_back("For each torrent the following information is shown:");
            helpText.push_back("<filename> <status> <percent> <estimate> <peers/seeds>");
            helpText.push_back("    ");
            std::string keyDescr;

            if (helpWindow::generateHelpForKey(keymap_,
                                               keyMapping::K_HELP,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to show help");
               }

            if (helpWindow::generateHelpForKey(keymap_,
                                               keyMapping::K_LOAD,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to load a torrent");
               }

            if (helpWindow::generateHelpForKey(keymap_,
                                               keyMapping::K_LOAD_URL,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to load a torrent from URL");
               }

            if (helpWindow::generateHelpForKey(keymap_,
                                               keyMapping::K_DOWN,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to move down the list");
               }

            if (helpWindow::generateHelpForKey(keymap_,
                                               keyMapping::K_UP,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to move up the list");
               }


            if (helpWindow::generateHelpForKey(keymap_,
                                               keyMapping::K_LIST_START,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to move to the beg. of the list");
               }

            if (helpWindow::generateHelpForKey(keymap_,
                                               keyMapping::K_LIST_END,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to move to the end of the list");
               }

            if (helpWindow::generateHelpForKey(keymap_,
                                               keyMapping::K_MARK,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to mark a torrent");
               }

            if (helpWindow::generateHelpForKey(keymap_,
                                               keyMapping::K_MARK_ALL,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to mark all torrents");
               }

            if (helpWindow::generateHelpForKey(keymap_,
                                               keyMapping::K_SELECT,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to see details about a torrent");
               }

            if (helpWindow::generateHelpForKey(keymap_,
                                               keyMapping::K_MENU,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to show menu (start/stop torrent etc)");
               }

            if (helpWindow::generateHelpForKey(keymap_,
                                               keyMapping::K_GLIMIT,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to set global limits");
               }

            if (helpWindow::generateHelpForKey(keymap_,
                                               keyMapping::K_SORT,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to change the way ");
                  helpText.push_back("  the list of torrents is sorted");
               }

            if (helpWindow::generateHelpForKey(keymap_,
                                               keyMapping::K_DETACH,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to detach from the daemon");
               }

            if (helpWindow::generateHelpForKey(keymap_,
                                               keyMapping::K_SESNAME,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to set the name of the session");
               }

            if (helpWindow::generateHelpForKey(keymap_,
                                               keyMapping::K_QUIT,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to quit the application");
               }

            helpText.push_back(" ");

            helpWindow hw(keymap_, helpText);

            return hw.run();
         }

         void UI::handleLoadUrl()
         {
            std::string helpText;
            genHelpText(helpText);

            statuswindow_.setStatus("Loading URL. " + helpText);

            windowSize dimensions;
            mainwindow_.getSize(dimensions);

            textInput ti_url(keymap_, dimensions, "URL", "");

            if (ti_url.run() == dialog::R_RESIZE)
               {
                  // the window was resized.
                  handleResizeMainWindow();

                  statuswindow_.setStatus("Loading URL aborted.");

                  return;
               }

            std::string url;

            if (!ti_url.getText(url))
               {
                  statuswindow_.setStatus("Loading URL aborted.");
                  return;
               }

            if (!btg::core::client::isUrlValid(url))
               {
                  statuswindow_.setStatus("Invalid URL entered.");
                  return;
               }

            bool gotFilename = true;
            std::string url_filename;
            if (!btg::core::client::getFilenameFromUrl(url, url_filename))
               {
                  gotFilename = false;
               }
            
            if (!gotFilename)
               {
                  textInput ti_fn(keymap_, dimensions, "Filename", "url.torrent");

                  if (ti_fn.run() == dialog::R_RESIZE)
                     {
                        // the window was resized.
                        handleResizeMainWindow();
                        
                        statuswindow_.setStatus("Loading URL aborted.");
                        
                        return;
                     }

                  if (!ti_fn.getText(url_filename))
                     {
                        statuswindow_.setStatus("Loading URL aborted.");
                        return;
                     }

                  if (url_filename.size() == 0)
                     {
                        statuswindow_.setStatus("Invalid URL entered.");
                        return;
                     }
               }

            // Got both URL and filename.
            {
               GET_HANDLER_INST;

               bool created = btg::core::client::createUrl(logWrapper(),
                                                           *handler,
                                                           *this,
                                                           url_filename,
                                                           url);

               if (created)
                  {
                     actionSuccess("Load URL", url_filename);
                  }
               else
                  {
                     actionFailture("Load URL", url_filename);
                  }
            }

            refresh();
         }

         void UI::handleUrl(t_strList const& _filelist)
         {
            t_strListCI iter;
            for (iter = _filelist.begin(); iter != _filelist.end(); iter++)
               {
                  std::string url = *iter;

                  if (!btg::core::client::isUrlValid(url))
                     {
                        continue;
                     }

                  std::string url_filename;
                  if (!btg::core::client::getFilenameFromUrl(url, url_filename))
                     {
                        continue;
                     }

                  // Got both URL and filename.
                  {
                     GET_HANDLER_INST;

                     handler->reqCreateFromUrl(url_filename, url);
               
                     if (handler->commandSuccess())
                        {
                           t_uint hid = handler->UrlId();
                              
                           if (handleUrlProgress(hid))
                              {
                                 actionSuccess("Load URL", url_filename);
                              }
                           else
                              {
                                 actionFailture("Load URL", url_filename);
                              }
                        }
                     else
                        {
                           actionFailture("Load URL", url_filename);
                        }
                  }
               }
            // Force updating of contexts.
            handlerthread_.forceUpdate();
         }

         bool UI::handleUrlProgress(t_uint _hid)
         {
            bool res = false;

            windowSize dimensions;
            mainwindow_.getSize(dimensions);

            progressWindow pwin(dimensions, keymap_);

            bool cont = true;

            Handler* handler = dynamic_cast<Handler*>(&handlerthread_.handler());

            while (cont)
               {
                  handler->reqUrlStatus(_hid);
                  if (!handler->commandSuccess())
                     {
                        return res;
                     }
                  t_uint id = 0;
                  t_uint status;

                  handler->UrlStatusResponse(id, status);
                  
                  switch (status)
                     {
                     case btg::core::OP_UNDEF:
                        {
                           break;
                        }
                     case btg::core::OP_WORKING:
                        {
                           pwin.updateProgress(50, "Working.");
                           break;
                        }
                     case btg::core::OP_FINISHED:
                        {
                           pwin.updateProgress(80, "Loaded URL.");
                           break;
                        }
                     case btg::core::OP_ERROR:
                        {
                           pwin.updateProgress(100, "Error loading URL.");
                           cont = false;
                           break;
                        }
                     case btg::core::OP_CREATE:
                        {
                           pwin.updateProgress(100, "Torrent created.");
                           res  = true;
                           cont = false;
                           break;
                        }
                     case btg::core::OP_CREATE_ERR:
                        {
                           pwin.updateProgress(100, "Unable to create torrent.");
                           res  = false;
                           cont = false;
                           break;
                        }
                     }
                  btg::core::os::Sleep::sleepMiliSeconds(500);
               }

            return res;
         }

         void UI::handleLoad()
         {
            std::string helpText;
            genHelpText(helpText);

            statuswindow_.setStatus("Loading file. " + helpText);

            windowSize fldimensions;
            mainwindow_.getSize(fldimensions);

            // Show a window with a file list.
            fileList fl(keymap_, fldimensions, load_directory_);

            // Use the same dimenstions as the main window.
            if (fl.run() == dialog::R_RESIZE)
               {
                  // the window was resized.
                  handleResizeMainWindow();

                  statuswindow_.setStatus("Loading file aborted.");

                  return;
               }

            std::string filenameToLoad;

            if (fl.getFile(filenameToLoad))
               {
                  {
                     GET_HANDLER_INST;

                     if (btg::core::client::createParts(logWrapper(), 
                                                        *handler, 
                                                        *this, 
                                                        filenameToLoad))
                        {
                           actionSuccess("Load", filenameToLoad);
                        }
                     else
                        {
                           actionFailture("Load", filenameToLoad);
                        }
                  }

                  // Force updating of contexts.
                  handlerthread_.forceUpdate();
               }
            else
               {
                  statuswindow_.setStatus("Loading file aborted.");
               }

            load_directory_ = fl.getLastDirectory();

            refresh();
         }

         void UI::handleLoad(t_strList const& _filelist)
         {
            t_strListCI iter;
            for (iter = _filelist.begin(); iter != _filelist.end(); iter++)
               {
                  {
                     GET_HANDLER_INST;
	    
                     handler->reqCreate(*iter);
	    
                     if (handler->commandSuccess())
                        {
                           actionSuccess("Load", *iter);
                        }
                     else
                        {
                           actionFailture("Load", *iter);
                        }
                  }
               }
            // Force updating of contexts.
            handlerthread_.forceUpdate();
         }

         void UI::handleSort()
         {
            std::string name;
            switch (sortby_)
               {
               case UI::sB_Id:
                  {
                     sortby_ = UI::sB_Name;
                     mainwindow_.setSortBy(DisplayModel::sB_Name);
                     name    = "name";
                     break;
                  }
               case UI::sB_Name:
                  {
                     sortby_ = UI::sB_Size;
                     mainwindow_.setSortBy(DisplayModel::sB_Size);
                     name    = "size";
                     break;
                  }
               case UI::sB_Size:
                  {
                     sortby_ = UI::sB_UlSpeed;
                     mainwindow_.setSortBy(DisplayModel::sB_UlSpeed);
                     name    = "upload speed";
                     break;
                  }
               case UI::sB_UlSpeed:
                  {
                     sortby_ = UI::sB_DlSpeed;
                     mainwindow_.setSortBy(DisplayModel::sB_DlSpeed);
                     name    = "download speed";
                     break;
                  }
               case UI::sB_DlSpeed:
                  {
                     sortby_ = UI::sB_Peers;
                     mainwindow_.setSortBy(DisplayModel::sB_Peers);
                     name    = "number of peers";
                     break;
                  }
               case UI::sB_Peers:
                  {
                     sortby_ = UI::sB_Done;
                     mainwindow_.setSortBy(DisplayModel::sB_Done);
                     name    = "percent done";
                     break;
                  }
               case UI::sB_Done:
                  {
                     sortby_ = UI::sB_Id;
                     mainwindow_.setSortBy(DisplayModel::sB_Id);
                     name    = "id";
                     break;
                  }
               }

            statuswindow_.setStatus("Sorting by '" + name + "'.");
         }

         void UI::handleGlobalLimit()
         {
            std::string helpText;
            genHelpText(helpText);

            statuswindow_.setStatus("Setting global limits. " + helpText);

            t_int download         = limitBase::LIMIT_DISABLED;
            t_int upload           = limitBase::LIMIT_DISABLED;
            t_int max_uploads      = limitBase::LIMIT_DISABLED;
            t_long max_connections = limitBase::LIMIT_DISABLED;

            {
               GET_HANDLER_INST;

               handler->reqGlobalLimitStatus();

               if (handler->commandSuccess())
                  {
                     handler->getLimitStatus(upload, download);
                     handler->getDaemonLimitStatus(max_uploads, max_connections);

                     if (upload != limitBase::LIMIT_DISABLED)
                        {
                           upload      /= limitBase::KiB_to_B;
                        }

                     if (download != limitBase::LIMIT_DISABLED)
                        {
                           download    /= limitBase::KiB_to_B;
                        }
                  }
            }

            windowSize limitdimensions;
            mainwindow_.getSize(limitdimensions);

            limitWindow limitwindow(keymap_,
                                    limitdimensions,
                                    upload,
                                    limitBase::LIMIT_DISABLED, /* min */
                                    65536,                     /* max */
                                    "Global upload (KiB/sec)", /* label */
                                    download,
                                    limitBase::LIMIT_DISABLED,
                                    65536,
                                    "Global download (KiB/sec)",
                                    max_uploads,
                                    limitBase::LIMIT_DISABLED,
                                    65536,
                                    "Global max uploads",
                                    static_cast<t_int>(max_connections),
                                    limitBase::LIMIT_DISABLED,
                                    65536,
                                    "Global max connections");

            if (limitwindow.run() == dialog::R_RESIZE)
               {
                  // the window was resized.
                  handleResizeMainWindow();

                  statuswindow_.setStatus("Setting global limits aborted.");

                  // abort setting of limits, if the window was resized.
                  return;
               }

            t_int tmp_max_connections;
            if (limitwindow.getLimits(upload,
                                      download,
                                      max_uploads,
                                      tmp_max_connections))
               {
                  max_connections = tmp_max_connections; // int -> long
                  {
                     GET_HANDLER_INST;

                     if (upload != limitBase::LIMIT_DISABLED)
                        {
                           upload      *= limitBase::KiB_to_B;
                        }

                     if (download != limitBase::LIMIT_DISABLED)
                        {
                           download    *= limitBase::KiB_to_B;
                        }

                     handler->reqGlobalLimit(upload,
                                             download,
                                             max_uploads,
                                             max_connections);
                     if (handler->commandSuccess())
                        {
                           statuswindow_.setStatus("Global limits set.");
                        }
                     else
                        {
                           statuswindow_.setError("Unable to set global limits.");
                        }
                  }
               }
            else
               {
                  statuswindow_.setStatus("Setting global limits aborted.");
               }
         }

         bool UI::handleQuit()
         {
            bool status = true;

            if (!neverAskQuestions_)
               {
                  statuswindow_.setStatus("Quit?");

                  std::vector<menuEntry> contents;

                  enum
                  {
                     QUIT_YES = 1,
                     QUIT_NO  = 2
                  };

                  contents.push_back(menuEntry(QUIT_NO,  "No",  "Do not quit"));
                  contents.push_back(menuEntry(QUIT_YES, "Yes", "Quit"));

                  windowSize menudimensions;
                  mainwindow_.getSize(menudimensions);

                  baseMenu bm(keymap_, menudimensions, "Quit?", contents, statuswindow_);

                  switch(bm.run())
                     {
                     case dialog::R_RESIZE:
                        {
                           // the window was resized.
                           handleResizeMainWindow();
			
                           statuswindow_.setStatus("Quit aborted.");

                           // abort showing menu.
                           return status;
                           break;
                        }
                     case dialog::R_NCREAT:
                        {
                           return status;
                           break;
                        }
                     default:
                        {
                           break;
                        }
                     }

                  if (bm.getResult() == baseMenu::BM_cancel)
                     {
                        statuswindow_.setStatus("Quit aborted.");
                        return status;
                     }
		  
                  switch (bm.getResult())
                     {
                     case QUIT_YES:
                        status = false;
                        break;
                     case QUIT_NO:
                        status = true;
                        break;
                     }
               }
            else
               {
                  // No questions are asked, so just quit.
                  status = false;
               }

            if (status)
               {
                  return status;
               }

            // Quit confirmed.
            {
               GET_HANDLER_INST;

               handler->reqQuit();
            }

            return status;
         }

         void UI::handleResizeMainWindow()
         {
            windowSize ws;

            baseWindow::getScreenSize(ws.width,
                                      ws.height);
            // Main window.
            windowSize mainwndSize = mainwindow_.calculateDimenstions(ws);
            mainwindow_.resize(mainwndSize);

            // Top window.
            windowSize topwndSize = topwindow_.calculateDimenstions(ws);
            topwindow_.resize(topwndSize);

            // Status window.
            windowSize statuswndSize = statuswindow_.calculateDimenstions(ws);
            statuswindow_.resize(statuswndSize);

            refresh();
         }

         void UI::handleShowFiles(std::vector<btg::core::fileInformation> const& _fi)
         {
            std::string helpText;
            genHelpText(helpText);

            std::string numberOfEntries = btg::core::convertToString<t_ulong>(_fi.size());
            statuswindow_.setStatus("Showing " + numberOfEntries + " file(s). " + helpText);

            windowSize fldimensions;
            mainwindow_.getSize(fldimensions);
            
            // Show a window with file information about a torrent.
            fileView fv(keymap_, fldimensions, _fi);

            // Use the same dimenstions as the main window.
            if (fv.run() == dialog::R_RESIZE)
               {
                  // the window was resized.
                  handleResizeMainWindow();

                  statuswindow_.setStatus("Showing file(s) aborted.");

                  return;
               }

            setDefaultStatusText();

            refresh();
         }

         void UI::handleShowPeers(t_peerList & _peerlist)
         {
            std::string helpText;
            genHelpText(helpText);
	  
            statuswindow_.setStatus("Showing peer(s). " + helpText);
	  
            windowSize fldimensions;
            mainwindow_.getSize(fldimensions);
            
            // Show a window with file information about a torrent.
            peerList pl(keymap_, fldimensions, _peerlist);

            // Use the same dimenstions as the main window.
            if (pl.run() == dialog::R_RESIZE)
               {
                  // the window was resized.
                  handleResizeMainWindow();
	      
                  statuswindow_.setStatus("Showing peer(s) aborted.");
	      
                  return;
               }

            setDefaultStatusText();

            refresh();            
         }

         UI::selectState UI::handleSelectFiles(t_int const _context_id,
                                               btg::core::selectedFileEntryList const& _files)
         {
            std::string helpText;
            genHelpText(helpText);

            statuswindow_.setStatus("Selecting files. " + helpText);

            windowSize fldimensions;
            mainwindow_.getSize(fldimensions);
            
            // Show a window with file information about a torrent.
            fileSelect fs(keymap_, fldimensions, _files);

            // Use the same dimenstions as the main window.
            if (fs.run() == dialog::R_RESIZE)
               {
                  // the window was resized.
                  handleResizeMainWindow();

                  return UI::sS_SELECT_RESIZE;
               }

            // User selected the list.
            if (fs.pressed_select() && fs.changed())
               {
                  {
                     // Got mutex already - in the calling function,
                     // the menu handling.
                     Handler* handler = dynamic_cast<Handler*>(&handlerthread_.handler());

                     btg::core::selectedFileEntryList filesToSet;
                     fs.getFiles(filesToSet);
                     handler->reqSetFiles(_context_id, filesToSet);

                     if (handler->commandSuccess())
                        {
                           return UI::sS_SELECT_SUCCESS;
                        }
                     else
                        {
                           return UI::sS_SELECT_FAILURE;
                        }
                  }
               }
            else if ((fs.pressed_select()) && (!fs.changed()))
               {
                  // No change to the list of selected files.
                  // No reason to tell the daemon to set the list.
                  return UI::sS_SELECT_SUCCESS;
               }

            setDefaultStatusText();

            refresh();

            return UI::sS_SELECT_ABORT;
         }

         void UI::handleMoveToSession(t_int const _context_id,
                                      const std::string& _filename)
         {
            GET_HANDLER_INST;
            handler->reqList();
            
            if (!handler->commandSuccess())
               {
                  actionFailture("Move to session", _filename);
                  return;
               }

            t_longList sessions    = handler->getSessionList();
            t_strList  names       = handler->getSessionNames();
            t_long current_session = handler->session();

            windowSize ssdimensions;
            mainwindow_.getSize(ssdimensions);

            sessionSelect ss(keymap_, 
                             ssdimensions,
                             sessions,
                             names,
                             current_session);

            // Use the same dimenstions as the main window.
            if (ss.run() == dialog::R_RESIZE)
               {
                  // the window was resized.
                  handleResizeMainWindow();

                  actionFailture("Move to session", _filename);

                  return;
               }

            t_long newsession;
            if (!ss.getSession(newsession))
               {
                  actionFailture("Move to session", _filename);
                  return;
               }
            
            
            handler->reqMoveContext(_context_id, newsession);

            if (!handler->commandSuccess())
               {
                  actionFailture("Move to session", _filename);
                  return;
               }

            // setDefaultStatusText();

            refresh();

            actionSuccess("Move to session", _filename);
         }

         void UI::getTrackers(t_int _contextID, 
                              t_strList & _trackers)
         {
            GET_HANDLER_INST;
            handler->reqTrackers(_contextID);
            
            if (handler->commandSuccess())
               {
                  _trackers = handler->getTrackerList();
               }
         }

      } // namespace cli
   } // namespace UI
} // namespace btg


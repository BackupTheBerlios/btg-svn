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

#include "mainwindow.h"

#include "mainmenubar.h"
#include "maintreeview.h"
#include "mainfiletreeview.h"
#include "selectfiletreeview.h"
#include "mainstatusbar.h"
//#include "mainnotebook.h" // included in header
#include "traficplot.h"
#include "peertreeview.h"
#include "detailstreeview.h"

#include <bcore/type.h>
#include <bcore/util.h>

#include "guihandler.h"

// Dialogs:
#include "aboutdialog.h"
#include "questiondialog.h"
#include "prefdialog.h"
#include "limitdialog.h"
#include "errordialog.h"
#include "sndialog.h"
#include "sessionselection.h"
#include "logtextview.h"
#include "progressdialog.h"
#include "urldialog.h"

#include "usermessage.h"

#include <bcore/trackerstatus.h>
#include <bcore/command/limit_base.h>

#include <string>

#include <bcore/verbose.h>
#include <bcore/file_info.h>

#include <bcore/client/clientdynconfig.h>
#include <bcore/client/loadhelper.h>
#include <bcore/project.h>
#include <bcore/btg_assert.h>
#include <bcore/os/sleep.h>
#include <bcore/opstatus.h>

#define GET_HANDLER_INST \
   boost::shared_ptr<boost::mutex> ptr = handlerthread.mutex(); \
   boost::mutex::scoped_lock interface_lock(*ptr); \
   guiHandler* handler = dynamic_cast<guiHandler*>(&handlerthread.handler());

namespace btg
{
   namespace UI
   {
      namespace gui
      {
         using namespace btg::core::logger;

         mainWindow::mainWindow(btg::core::LogWrapperType _logwrapper,
                                std::string const& _session,
                                bool const _verboseFlag,
                                bool const _neverAskFlag,
                                btg::core::client::handlerThread& _handlerthread,
                                btg::core::client::clientDynConfig& _CDC)
            : Gtk::Window(Gtk::WINDOW_TOPLEVEL),
              btg::core::Logable(_logwrapper),
              verboseFlag(_verboseFlag),
              neverAskFlag(_neverAskFlag),
              timeout_connection(),
              mtw(0),
              msb(0),
              mmb(0),
              mnb(0),
              handlerthread(_handlerthread),
              aboutdialog(0),
              preferencesdialog(0),
              lastOpenDirectory(""),
              updateContexts(true),
              progressCounter(0),
              progressMax(5),
              trackerstatSerial(),
              m_clientDynConfig(_CDC),
              m_bMultipleContinue(false), // actually not necessary
              m_limitDialog(new limitDialog()),
              m_sessionSelectionDialog(0),
              last_url(),
              last_url_file(),
              urlDlEnabled(true),
              upload_progressdialog(0),
              peersCounter(0),
              peersMax(5),
              last_mtw_selection(-1),
              last_mnb_selection(mainNotebook::GRAPH)
         {
            {
               GET_HANDLER_INST;
               handler->reqVersion();
               if (handler->commandSuccess())
                  {
                     const btg::core::OptionBase & o = handler->getOption();
                     urlDlEnabled = o.getOption(btg::core::OptionBase::URL); 
                  }
            }

            mmb                       = Gtk::manage(new mainMenubar(this));
            mtw                       = Gtk::manage(new mainTreeview);
            Gtk::ScrolledWindow* mtsw = Gtk::manage(new Gtk::ScrolledWindow);
            mnb                       = Gtk::manage(new mainNotebook);
            Gtk::VPaned* contentsVPaned = Gtk::manage(new Gtk::VPaned);
            msb                       = Gtk::manage(new mainStatusbar);
            Gtk::VBox *mainVbox       = Gtk::manage(new Gtk::VBox);

            mtsw->set_flags(Gtk::CAN_FOCUS);
            mtsw->set_shadow_type(Gtk::SHADOW_IN);
            mtsw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
            mtsw->property_window_placement().set_value(Gtk::CORNER_TOP_LEFT);
            mtsw->add(*mtw);

            contentsVPaned->pack1(*mtsw, true, false);
            contentsVPaned->pack2(*mnb, true, false);

            mainVbox->pack_start(*mmb, Gtk::PACK_SHRINK, 0);

            // Toolbar
            Gtk::Toolbar *tbar = Gtk::manage(new Gtk::Toolbar);
            Gtk::ToolButton *tbutton;

            // Open file.
            tbutton = Gtk::manage(new Gtk::ToolButton);
            tbutton->set_label("Open");
            tbutton->set_stock_id(Gtk::Stock::NEW);
            tbar->append(*tbutton,sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*this, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_LOAD ));

            if (urlDlEnabled)
               {
                  // Open URL.
                  tbutton = Gtk::manage(new Gtk::ToolButton);
                  tbutton->set_label("Open URL");
                  tbutton->set_stock_id(Gtk::Stock::NEW);
                  tbar->append(*tbutton,sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*this, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_LOAD_URL ));
                  
                  tbar->append(*Gtk::manage(new Gtk::SeparatorToolItem));
               }

            tbutton = Gtk::manage(new Gtk::ToolButton);
            tbutton->set_label("Start");
            tbutton->set_stock_id(Gtk::Stock::MEDIA_PLAY);
            tbar->append(*tbutton,sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*this, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_START ));
            torrentControlButtons.push_back(tbutton);

            tbutton = Gtk::manage(new Gtk::ToolButton);
            tbutton->set_label("Stop");
            tbutton->set_stock_id(Gtk::Stock::MEDIA_PAUSE);
            tbar->append(*tbutton,sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*this, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_STOP ));
            torrentControlButtons.push_back(tbutton);

            tbar->append(*Gtk::manage(new Gtk::SeparatorToolItem));

            tbutton = Gtk::manage(new Gtk::ToolButton);
            tbutton->set_label("Abort");
            tbutton->set_stock_id(Gtk::Stock::CANCEL);
            tbar->append(*tbutton,sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*this, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_ABORT ));
            torrentControlButtons.push_back(tbutton);
            
            tbutton = Gtk::manage(new Gtk::ToolButton);
            tbutton->set_label("Clean");
            tbutton->set_stock_id(Gtk::Stock::APPLY);
            tbar->append(*tbutton,sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*this, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_CLEAN ));
            torrentControlButtons.push_back(tbutton);
            
            tbar->append(*Gtk::manage(new Gtk::SeparatorToolItem));

            tbutton = Gtk::manage(new Gtk::ToolButton);
            tbutton->set_label("Limit");
            tbutton->set_stock_id(Gtk::Stock::PREFERENCES);
            tbar->append(*tbutton,sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*this, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_LIMIT ));
            torrentControlButtons.push_back(tbutton);

            mainVbox->pack_start(*tbar,Gtk::PACK_SHRINK);
            
            mainVbox->pack_start(*contentsVPaned);
            mainVbox->pack_start(*msb, Gtk::PACK_SHRINK, 0);
            set_title("btgui (session #" + _session + ")");
            set_modal(false);
            property_window_position().set_value(Gtk::WIN_POS_NONE);
            set_resizable(true);
            property_destroy_with_parent().set_value(false);
            add(*mainVbox);

            show_all();

            {
               GET_HANDLER_INST;
               // Update the last opened file list and URL list in menubar.
               // can hide items, so should be called after show_all()
               mmb->updateLastFileList(handler->getLastFiles());
               mmb->updateLastURLList(handler->getLastURLs(),handler->getLastURLFiles());
            }

            // Restore window geometry
            move(m_clientDynConfig.get_gui_window_x(), m_clientDynConfig.get_gui_window_y());
            resize(m_clientDynConfig.get_gui_window_width(), m_clientDynConfig.get_gui_window_height());
            if (m_clientDynConfig.get_gui_window_maximized())
               maximize();

            // Create/connect a timer used for updating the list of
            // contexts.
            sigc::slot<bool> slot = sigc::bind(sigc::mem_fun(*this, &mainWindow::on_refresh_timeout), 0);
            timeout_connection    = Glib::signal_timeout().connect(slot, 1024);

            // When the main window is closed, call this signal.
            signal_delete_event().connect(sigc::mem_fun(*this, &mainWindow::onWindowClose));
            
            // When peersTreeView scrolled - refresh peers immediately
            mnb->peersVScrollbar()->signal_value_changed().connect(sigc::mem_fun(*this, &mainWindow::on_peers_scrolled));
            // Also when it's window's size changed
            mnb->peersVScrollbar()->signal_size_allocate().connect(sigc::hide(sigc::mem_fun(*this, &mainWindow::on_peers_scrolled)));

            /// Tell the gui handler about the statusbar, so it can
            /// write messages on it.  As for now only used for daemon
            /// uptime.
            {
               GET_HANDLER_INST;
               handler->setStatusBar(msb);
            }

            // Start with the torrent controls disabled.
            setControlFunction(false);
            
            {
               // Log the options the daemon uses.
               GET_HANDLER_INST;
               const btg::core::OptionBase & o = handler->getOption();
               logVerboseMessage(o.toString());
            }
         }

         bool mainWindow::on_refresh_timeout(int)
         {
            if (!updateContexts)
               {
                  return true;
               }

            msb->trigger();

            bool doUpdateProgress = false;

            if (progressCounter >= progressMax)
               {
                  doUpdateProgress = true;
                  progressCounter  = 0;
               }

            progressCounter++;

            bool gotStatus = false;
            t_statusList statuslist;

            {
               GET_HANDLER_INST;

               if (handler->statusListUpdated())
                  {
                     gotStatus = true;
                     handler->getStatusList(statuslist);
                     handler->resetStatusList();
                  }
            }

            if (gotStatus)
               {
                  mnb->getPlot()->resetCounters();

                  for (t_statusListI iter = statuslist.begin();
                       iter != statuslist.end();
                       iter++)
                     {
                        mtw->updateStatus(iter->contextID(),
                                          *iter,
                                          doUpdateProgress);
                        // Update the trafic graph.
                        mnb->getPlot()->updateCounter(*iter);

                        updateTrackerStatus(*iter);
                     }

                  mnb->getPlot()->updateModel();
               }
            else
               {
                  // Check to see if the session still belongs to this
                  // instance..
                  // Or if the statemachine returned a fatal error.
                  // In both situations the client should just quit.
                  {
                     GET_HANDLER_INST;

                     if (handler->sessionError() || handler->fatalError())
                        {
                           // Present a dialog to the user informing him
                           // that the session is fubar. _Then_ quit.
                           errorDialog::showAndDie("Connection to the daemon was lost. Terminating.");
                           Gtk::Main::quit();
                        }
                  }
               }

            mainNotebook::CurrentSelection currentselection = mnb->selected();

            // Update the plot even when no torrents are selected.
            if (currentselection == mainNotebook::GRAPH)
               {
                  updatePlot();
               }

            // If a torrent is selected, attempt to get a file info
            // and list of peers.
            if (mtw->gotSelection())
               {
                  setControlFunction(true);

                  t_uint numberOfSelectedItems = mtw->numberOfSelected();

                  if (numberOfSelectedItems == 1)
                     {
                        // Find out what was selected.
                        elemMap m        = mtw->getSelectedIds();
                        elemMap_citer ci = m.begin();
                        int id           = ci->first;

                        switch (currentselection)
                           {
                           case mainNotebook::DETAILS:
                              updateDetails(id, statuslist);
                              break;
                           case mainNotebook::ALL_FILES:
                              updateFiles(id);
                              break;
                           case mainNotebook::SELECTED_FILES:
                              checkSelectedFiles();
                              updateSelectedFiles(id);
                              break;
                           case mainNotebook::PEERS:
                              updatePeers(id);
                              break;
                           case mainNotebook::LOG:
                              break;
                           default:
                              break;
                           }
                        
                        /* *************************************
                         * Use mtw selection _before_ this point
                         * *************************************/
                        
                        last_mtw_selection = id;
                     }
                  else if (numberOfSelectedItems == 0)
                     {
                        // If nothing is selected, disable certain
                        // items in the control menu and toolbar.
                        setControlFunction(false);
                     }
                  else
                     {
                        clearSelection();
                     }
               }
            else
               {
                  setControlFunction(false);
               }
            
            /* *************************************
             * Use mnb selection _before_ this point
             * *************************************/
            
            last_mnb_selection = currentselection;

            t_intList idstoremove;
            {
               GET_HANDLER_INST;

               idstoremove = handler->getIdsToRemove();
            }

            if (idstoremove.size() > 0)
               {
                  for (t_intListI j=idstoremove.begin(); 
                       j!= idstoremove.end(); 
                       j++)
                     {
                        mtw->removeStatus(*j);
                     }
               }

            return true;
         }

         void mainWindow::on_menu_item_selected(buttonMenuIds::MENUID _which_item)
         {
            bool non_context = false;

            // During execution of this function, no updates of
            // contexts should be done.
            updateContexts = false;
            
            switch (_which_item)
               {
               case buttonMenuIds::BTN_LOAD:
                  {
                     handle_btn_load();
                     non_context = true;
                     break;
                  }
               case buttonMenuIds::BTN_LOAD_URL:
                  {
                     handle_btn_load_url();
                     non_context = true;
                     break;
                  }
               case buttonMenuIds::BTN_PREFERENCES:
                  {
                     if ( !mtw->gotSelection() )
                     {
                        errorDialog::showAndDie("Please, select a torrent.");
                     }
                     break;
                  }
               case buttonMenuIds::BTN_DETACH:
                  {
                     handle_btn_detach();
                     non_context = true;
                     break;
                  }
               case buttonMenuIds::BTN_ABOUT:
                  {
                     handle_btn_about();
                     non_context = true;
                     break;
                  }
               case buttonMenuIds::BTN_QUIT:
                  {
                     handle_btn_quit();
                     non_context = true;
                     break;
                  }
               case buttonMenuIds::BTN_KILL:
                  {
                     handle_btn_kill();
                     non_context = true;
                     break;
                  }
               case buttonMenuIds::BTN_GLIMIT:
                  {
                     handle_btn_glimit();
                     non_context = true;
                     break;
                  }
               case buttonMenuIds::BTN_UPTIME:
                  {
                     handle_btn_uptime();
                     non_context = true;
                     break;
                  }
               case buttonMenuIds::BTN_SESNAME:
                  {
                     handle_btn_sesname();
                     non_context = true;
                     break;
                  }
               case buttonMenuIds::BTN_LASTFILE:
                  {
                     handle_btn_lastfile_all();
                     non_context = true;
                     break;
                  }
               case buttonMenuIds::BTN_LASTURL:
                  {
                     handle_btn_lasturl_all();
                     non_context = true;
                     break;
                  }
               default:
                  {
                     // Ignore the rest of buttons, handled elsewhere.
                     break;
                  }
               } // switch

            if (_which_item > buttonMenuIds::BTN_LASTFILE && \
               _which_item < buttonMenuIds::BTN_LASTFILE + btg::core::projectDefaults::iMAXLASTFILES() + 1)
            {
               handle_btn_lastfile(_which_item - buttonMenuIds::BTN_LASTFILE - 1);
               non_context = true;
            }
            
            if (_which_item > buttonMenuIds::BTN_LASTURL && \
               _which_item < buttonMenuIds::BTN_LASTURL + btg::core::projectDefaults::iMAXLASTURLS() + 1)
            {
               handle_btn_lasturl(_which_item - buttonMenuIds::BTN_LASTURL - 1);
               non_context = true;
            }
            
            if (non_context)
               {
                  updateContexts = true;
                  return;
               }

            if (mtw->gotSelection())
               {
                  // disable controls
                  setControlFunction(false);

                  elemMap m = mtw->getSelectedIds();
                  m_bMultipleContinue = false;

                  for (elemMap_citer i=m.begin(); i!=m.end(); ++i)
                     {
                        int id = i->first;
                        switch(_which_item)
                           {
                           case buttonMenuIds::BTN_START:
                              handle_btn_start(id);
                              break;
                           case buttonMenuIds::BTN_STOP:
                              handle_btn_stop(id);
                              break;
                           case buttonMenuIds::BTN_ABORT:
                              handle_btn_abort(id);
                              break;
                           case buttonMenuIds::BTN_ERASE:
                              handle_btn_erase(id);
                              break;
                           case buttonMenuIds::BTN_CLEAN:
                              handle_btn_clean(id);
                              break;
                           case buttonMenuIds::BTN_LIMIT:
                              handle_btn_limit(id);
                              break;
                           case buttonMenuIds::BTN_MOVE:
                              handle_btn_move(id);
                              break;
                           case buttonMenuIds::BTN_PREFERENCES:
                              handle_btn_prefs(id);
                              break;
                           default:
                              // Ignore the rest of buttons, handled elsewhere.
                              break;
                           } // switch
                        if(!m_bMultipleContinue)
                           {
                              break; // for (elemMap_citer i=m.begin(); i!=m.end(); ++i)
                           }
                     }
                  // enable controls
                  setControlFunction(true);
               }
            updateContexts = true;
         }

         void mainWindow::handle_btn_load()
         {
            Gtk::FileChooserDialog dialog("Please choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN);

            dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
            dialog.add_button("_OK",     Gtk::RESPONSE_OK);

            // Add filters, so that only certain file types can be selected:

            Gtk::FileFilter filter_torrent;
            filter_torrent.set_name("BitTorrent files");
            filter_torrent.add_mime_type("application/x-bittorrent");
            filter_torrent.add_pattern("*.torrent");
            dialog.add_filter(filter_torrent);

            Gtk::FileFilter filter_any;
            filter_any.set_name("Any files");
            filter_any.add_pattern("*");
            dialog.add_filter(filter_any);

            // Set the directory, saved last time this dialog was
            // used.
            if (lastOpenDirectory != "")
               {
                  dialog.set_current_folder(lastOpenDirectory);
               }

            // Show the dialog and wait for a user response:
            int result = dialog.run();

            switch(result)
               {
               case(Gtk::RESPONSE_OK):
                  {
                     lastOpenDirectory = dialog.get_current_folder();
                     openFile(dialog.get_filename());
                     {
                        GET_HANDLER_INST;
                        handler->addLastFile(dialog.get_filename());
                        mmb->updateLastFileList(handler->getLastFiles());
                     }
                     break;
                  }
               case(Gtk::RESPONSE_CANCEL):
                  {
                     break;
                  }
               default:
                  {
                     break;
                  }
               } // switch
         }

         void mainWindow::handle_btn_load_url()
         {
            // Show a dialog where one can input the URL and filename.
            urlDialog ud(last_url, last_url_file);
            if (ud.run() != Gtk::RESPONSE_APPLY)
               {
                  ud.hide();
                  msb->set("Loading URL aborted.");
                  logVerboseMessage("Loading URL aborted.");
                  return;
               }
            ud.hide();

            std::string filename;
            std::string url;

            if (!ud.GetResult(url, filename))
               {
                  msb->set("Loading URL aborted.");
                  logVerboseMessage("Loading URL aborted.");
                  return;
               }
            
            openURL(url, filename);

            {
               GET_HANDLER_INST;
               handler->addLastURL(url, filename);
               mmb->updateLastURLList(handler->getLastURLs(), handler->getLastURLFiles());
            }
         }

         void mainWindow::openFile(std::string const& _filename)
         {
            GET_HANDLER_INST;

            if (btg::core::client::createParts(logWrapper(), *handler, *this, _filename))
               {
                  msb->set(USERMESSAGE_ADDED_B + _filename + USERMESSAGE_ADDED_E);
                  logVerboseMessage(USERMESSAGE_ADDED_B + _filename + USERMESSAGE_ADDED_E);
               }
            else
               {
                  msb->set(USERMESSAGE_ADD_FAILED_B + _filename + USERMESSAGE_ADD_FAILED_E);
                  logVerboseMessage(USERMESSAGE_ADD_FAILED_B + _filename + USERMESSAGE_ADD_FAILED_E);
               }
         }

         void mainWindow::openURL(std::string const& _url, std::string const& _filename)
         {
            msb->set("Loading URL: " + _url);
            logVerboseMessage("Loading URL: " + _url);

            GET_HANDLER_INST;
            bool created = btg::core::client::createUrl(logWrapper(),
                                                        *handler,
                                                        *this,
                                                        _filename,
                                                        _url);
            if (created)
               {
                  msb->set("Created torrent from URL '" +_url + "'.");
                  logVerboseMessage("Created torrent from URL '" + _url + "'.");
               }
            else
               {
                  msb->set("Unable to load URL: " + _filename);
                  logVerboseMessage("Unable to load URL: " + _filename);
               }
            
            // sleep for a second so user can see our message
            for (int i=0; i<100; ++i)
            {
               Gtk::Main::iteration(false);
               btg::core::os::Sleep::sleepMiliSeconds(10);
            }
         }
         
         bool mainWindow::onWindowClose(GdkEventAny *)
         {
            on_menu_item_selected(buttonMenuIds::BTN_DETACH);
            Gtk::Main::quit();
            return true;
         }

         void mainWindow::logMessage(std::string const& _msg)
         {
            if (mnb)
               {
                  mnb->getTextView()->writeLog(_msg);
               }
         }

         void mainWindow::logVerboseMessage(std::string const& _msg)
         {
            if (!verboseFlag)
               {
                  return;
               }
            VERBOSE_LOG(logWrapper(), verboseFlag, _msg);
            logMessage(_msg);
         }

         bool mainWindow::showTrackerStatus(t_int _contextId, btg::core::trackerStatus const& _ts)
         {
            bool result = true;

            std::map<t_int, t_int>::iterator trackerStatIter =
               trackerstatSerial.find(_contextId);

            if (trackerStatIter == trackerstatSerial.end())
               {
                  // Just added the context id, so show the tracker status.
                  trackerstatSerial.insert(std::pair<t_int, t_int>(_contextId, _ts.serial()));
                  return result;
               }

            trackerStatIter = trackerstatSerial.find(_contextId);

            if (_ts.serial() == trackerStatIter->second)
               {
                  // This status was seen before.
                  result = false;
               }

            return result;
         }

         void mainWindow::removeTrackerStatus(t_int const _contextId)
         {
            std::map<t_int, t_int>::iterator eraseIter = trackerstatSerial.find(_contextId);

            if (eraseIter != trackerstatSerial.end())
               {
                  trackerstatSerial.erase(eraseIter);
               }
         }

         void mainWindow::updateTrackerStatus(btg::core::Status const& _status)
         {
            t_int contextId                    = _status.contextID();
            btg::core::trackerStatus const& ts = _status.trackerStatus();

            if (!showTrackerStatus(contextId, ts))
               {
                  return;
               }

            if (ts.status() != btg::core::trackerStatus::undefined)
               {
                  std::map<t_int, t_int>::iterator updateIter = trackerstatSerial.find(contextId);
                  updateIter->second = ts.serial();

                  std::string filename = _status.filename();
                  logMessage(filename +": " + ts.getDescription());
               }
         }

         void mainWindow::updatePlot()
         {
            // Redraw the bandwidth plot.
            mnb->getPlot()->redraw();
         }

         void mainWindow::checkSelectedFiles()
         {
            GET_HANDLER_INST;

            btg::core::selectedFileEntryList sfel = handler->getLastSelectedFiles();

            t_int toggle_id;
            if (mnb->getSelectFileTreeview()->getToogled(toggle_id, sfel))
               {
                  handler->reqSetFiles(toggle_id, sfel);
               }
         }

         void mainWindow::setControlFunction(const bool bSensitive)
         {
            if (bSensitive)
               {
                  mmb->enableControlFunction();
               }
            else
               {
                  mmb->disableControlFunction();
               }

            for (std::list<Gtk::ToolButton*>::iterator ibutton = torrentControlButtons.begin();
               ibutton != torrentControlButtons.end();
               ++ibutton)
               {
                  (*ibutton)->set_sensitive(bSensitive);
               }
         }
         
         void mainWindow::updateFiles(t_int const _id)
         {
            GET_HANDLER_INST;

            // Get a list of files.
            handler->reqFileInfo(_id);
            if (handler->commandSuccess())
               {
                  mnb->getFileTreeview()->update(_id, handler->getFileInfoList());
               }
            else
               {
                  // Could not get a status from the
                  // daemon, clear the list.
                  mnb->getFileTreeview()->clear();
               }
         }

         void mainWindow::updatePeers(t_int const _id)
         {
            if (_id == last_mtw_selection // still staying on the same torrent
               && ++peersCounter < peersMax // no timeout elapsed
               && last_mnb_selection == mainNotebook::PEERS // still staying on the Peers tab
               )
               return; // no update needed

            peersCounter = 0;
            
            GET_HANDLER_INST;

            Gtk::TreeModel::Path pbegin, pend;
            if (mnb->getPeerTreeview()->get_visible_range(pbegin, pend)
               && pbegin.begin() && pend.begin() // sometimes can be NULL
               )
            {
               t_uint offset = *pbegin.begin();
               t_uint count = *pend.begin() - *pbegin.begin() + 1;
               
               // Get a list of peers with extended info.
               handler->reqPeers(_id, false, &offset, &count);
            }
            else
            {
               // Get a list of peers.
               handler->reqPeers(_id);
               
               // No extended info requested, refresh immediately,
               // (will refresh with extended info)
               peersCounter = peersMax;
            }
            
            if (handler->commandSuccess())
               {
                  mnb->getPeerTreeview()->clear();
                  mnb->getPeerTreeview()->update(handler->getPeers());
                  if (handler->haveExPeers())
                     mnb->getPeerTreeview()->update(handler->getExPeersOffset(), handler->getExPeers());
               }
            else
               {
                  // Could not get a list of peers from
                  // the daemon, clear the list.
                  
                  mnb->getPeerTreeview()->clear();
               }
         }

         void mainWindow::clearSelection()
         {
            // Nothing is selected or several contexts are selected,
            // so clear the file list.
            mnb->getFileTreeview()->clear();
            
            // Also clear the peer list.
            mnb->getPeerTreeview()->clear();
            
            // Clear the details.
            mnb->getDetails()->clear();
            
            // Clear the list of selected files.
            mnb->getSelectFileTreeview()->clear();
         }

         void mainWindow::updateSelectedFiles(t_int const _id)
         {
            GET_HANDLER_INST;

            handler->reqGetFiles(_id);
            if (handler->commandSuccess())
               {
                  mnb->getSelectFileTreeview()->update(_id, 
                                                       handler->getLastSelectedFiles()
                                                       );
               }
         }

         void mainWindow::updateDetails(t_int const _id,
                                        t_statusList const& _statuslist)
         {
            // Update the details.
            for (t_statusListCI iter = _statuslist.begin();
                 iter != _statuslist.end();
                 iter++)
               {
                  if (iter->contextID() == _id)
                     {
                        mnb->getDetails()->update(*iter);
                        break;
                     }
               }
         }

         void mainWindow::handle_btn_lastfile(int _which_item)
         {
            std::string filename;
            {
               GET_HANDLER_INST;
               filename = handler->getLastFiles().at(_which_item);
            }
            openFile(filename);
         }

         void mainWindow::handle_btn_lastfile_all()
         {
            t_strList lastfiles;
            {
               GET_HANDLER_INST;
               lastfiles = handler->getLastFiles();
            }
            for (t_strListCI lastIter=lastfiles.begin();
                 lastIter != lastfiles.end();
                 lastIter++)
               {
                  openFile(*lastIter);
               }
         }

         void mainWindow::handle_btn_lasturl(int _which_item)
         {
            std::string url;
            std::string filename;
            {
               GET_HANDLER_INST;
               url      = handler->getLastURLs().at(_which_item);
               filename = handler->getLastURLFiles().at(_which_item);
            }
            openURL(url,filename);
         }

         void mainWindow::handle_btn_lasturl_all()
         {
            t_strList lastURLs;
            t_strList lastURLFiles;
            {
               GET_HANDLER_INST;
               lastURLs = handler->getLastURLs();
               lastURLFiles = handler->getLastURLFiles();
            }
            for (int i=0; i<lastURLs.size(); ++i)
               {
                  openURL(lastURLs[i],lastURLFiles[i]);
               }
         }

         void mainWindow::handle_btn_detach()
         {
            GET_HANDLER_INST;

            handler->reqDetach();
            if (handler->commandSuccess())
               {
                  Gtk::Main::quit();
               }
            else
               {
                  logVerboseMessage(USERMESSAGE_DETACHED);
                  msb->set(USERMESSAGE_DETACHED);
               }
         }

         void mainWindow::handle_btn_about()
         {
            if (aboutdialog == 0)
               {
                  aboutdialog = new aboutDialog();
               }
            aboutdialog->show();
         }

         void mainWindow::handle_btn_quit()
         {
            GET_HANDLER_INST;

            logVerboseMessage(USERMESSAGE_QUIT);
            msb->set(USERMESSAGE_QUIT);
            
            // Query the user .. or not is its disabled.
            bool doQuit = false;
            
            if (!neverAskFlag)
               {
                  doQuit = questionDialog::showAndDie("Quit?", "Are you sure you really want to quit?");
               }
            else
               {
                  doQuit = true;
               }
            
            if (doQuit)
               {
                  // Tell the daemon that this client quits.
                  handler->reqQuit();
                  // Quit gtkmm.
                  Gtk::Main::quit();
               }
            else
               {
                  logVerboseMessage(USERMESSAGE_QUIT_ABORTED);
                  msb->set(USERMESSAGE_QUIT_ABORTED);
               }
         }

         void mainWindow::handle_btn_kill()
         {
            GET_HANDLER_INST;

            logVerboseMessage(USERMESSAGE_KILL);
            msb->set(USERMESSAGE_KILL);
            
            // Query the user about if he wishes to kill the daemon.
            bool suicide = false;
            
            if (!neverAskFlag)
               {
                  std::auto_ptr<questionDialog> qd( new questionDialog(Gtk::MESSAGE_WARNING,
                     "Kill the daemon?",
                     "Are you sure you really want to kill the daemon?" ));
                  suicide = qd->status();
               }
            else
               {
                  suicide = true;
               }
            
            if (suicide)
               {
                  // Tell the daemon to kill itself.
                  handler->reqKill();
                  
                  // Quit gtkmm.
                  Gtk::Main::quit();
               }
            else
               {
                  logVerboseMessage(USERMESSAGE_KILL_ABORTED);
                  msb->set(USERMESSAGE_KILL_ABORTED);
               }
         }

         void mainWindow::handle_btn_sesname()
         {
            GET_HANDLER_INST;

            std::string session_name;

            handler->reqSessionName();
            session_name = handler->getCurrentSessionName();

            sessionNameDialog snd(session_name);
            
            int result = snd.run();

            switch(result)
               {
               case(Gtk::RESPONSE_OK):
                  {
                     std::string newname = snd.getName();
                     if (session_name != newname)
                        {
                           handler->reqSetSessionName(newname);
                           if (handler->commandSuccess())
                              {
                                 msb->set(USERMESSAGE_SNAMESET);
                              }
                           else
                              {
                                 msb->set(USERMESSAGE_NOT_SNAMESET);
                              }
                        }
                     else
                        {
                           msb->set(USERMESSAGE_NOT_SNAMESET);
                        }

                     break;
                  }
               case(Gtk::RESPONSE_CANCEL):
                  {
                     msb->set(USERMESSAGE_NOT_SNAMESET);
                     break;
                  }
               default:
                  {
                     break;
                  }
               } // switch
         }

         void mainWindow::handle_btn_uptime()
         {
            GET_HANDLER_INST;

            logVerboseMessage(USERMESSAGE_UPTIME);
            msb->set(USERMESSAGE_UPTIME);
            handler->reqUptime();
         }

         void mainWindow::handle_btn_glimit()
         {
            GET_HANDLER_INST;

            t_int up_limit   = 0;
            t_int down_limit = 0;
            t_int max_uploads  = 0;
            t_long max_connections = 0;
            
            std::auto_ptr<limitDialog> ld(new limitDialog(NULL,NULL,"Max uploads","Max connections"));

            handler->reqGlobalLimitStatus();

            if (handler->commandSuccess())
               {
                  handler->getLimitStatus(up_limit, down_limit);
                  handler->getDaemonLimitStatus(max_uploads, max_connections);
               }
            else
               {
                  errorDialog::showAndDie("Error getting global limit settings.");
               }

            ld->update("Global",
                                up_limit, down_limit,
                                max_uploads, max_connections);
            ld->run();

            if (ld->limitSelected())
               {
                  up_limit     = btg::core::limitBase::LIMIT_DISABLED;
                  down_limit   = btg::core::limitBase::LIMIT_DISABLED;
                  max_uploads = btg::core::limitBase::LIMIT_DISABLED;
                  max_connections = btg::core::limitBase::LIMIT_DISABLED;
      
                  // User selected limits.
                  if (!ld->uploadLimitDisabled())
                     {
                        up_limit = ld->getUploadLimit();
                     }
      
                  // User selected limits.
                  if (!ld->downloadLimitDisabled())
                     {
                        down_limit = ld->getDownloadLimit();
                     }
      
                  // User selected seed limits.
                  if (!ld->param3Disabled())
                     {
                        max_uploads = ld->getParam3();
                     }
      
                  // User selected seed limits.
                  if (!ld->param4Disabled())
                     {
                        max_connections = ld->getParam4();
                     }
      
                  handler->reqGlobalLimit(up_limit, down_limit, max_uploads, max_connections);
      
                  if (handler->commandSuccess())
                     {
                        msb->set(USERMESSAGE_LIMIT_B + std::string("Global") + USERMESSAGE_LIMIT_E);
                        logVerboseMessage(USERMESSAGE_LIMIT_B + std::string("Global") + USERMESSAGE_LIMIT_E);
                     }
                  else
                     {
                        errorDialog::showAndDie("Error setting global limits.");
                     }
               }
         }

         void mainWindow::handle_btn_start(t_int const _id)
         {
            GET_HANDLER_INST;
            
            m_bMultipleContinue = true; // continue processing multiple selected torrents

            std::string filename;
            handler->idToFilename(_id, filename);

            handler->reqStart(_id);
            if (handler->commandSuccess())
               {
                  logVerboseMessage(USERMESSAGE_STARTED_B + filename + USERMESSAGE_STARTED_E);
                  msb->set(USERMESSAGE_STARTED_B + filename + USERMESSAGE_STARTED_E);
               }
            else
               {
                  errorDialog::showAndDie("Error starting " + filename);
               }
         }

         void mainWindow::handle_btn_stop(t_int const _id)
         {
            GET_HANDLER_INST;

            m_bMultipleContinue = true; // continue processing multiple selected torrents

            std::string filename;
            handler->idToFilename(_id, filename);
            
            handler->reqStop(_id);
            if (handler->commandSuccess())
               {
                  logVerboseMessage(USERMESSAGE_STOPPED_B + filename + USERMESSAGE_STOPPED_E);
                  msb->set(USERMESSAGE_STOPPED_B + filename + USERMESSAGE_STOPPED_E);
               }
            else
               {
                  errorDialog::showAndDie("Error stopping " + filename);
               }
         }

         void mainWindow::handle_btn_erase(t_int const _id)
         {
            if (!m_bMultipleContinue)
               {
                  std::auto_ptr<questionDialog> qd(new questionDialog(Gtk::MESSAGE_WARNING, "Erase", "Are you sure ?")); // shows dialog
                  if ( !(m_bMultipleContinue = qd->status()) )
                     {
                        return;
                     }
               }
            
            GET_HANDLER_INST;

            std::string filename;
            handler->idToFilename(_id, filename);

            handler->reqAbort(_id, true /* erase data */, false);
            if (handler->commandSuccess())
               {
                  removeTrackerStatus(_id);
                  logVerboseMessage(USERMESSAGE_ERASED_B + filename + USERMESSAGE_ERASED_E);
                  msb->set(USERMESSAGE_ERASED_B + filename + USERMESSAGE_ERASED_E);
               }
            else
               {
                  errorDialog::showAndDie("Error erase " + filename);
               }
         }

         void mainWindow::handle_btn_abort(t_int const _id)
         {
            if (!m_bMultipleContinue)
               {
                  std::auto_ptr<questionDialog> qd(new questionDialog(Gtk::MESSAGE_WARNING, "Abort", "Are you sure ?")); // shows dialog
                  if ( !(m_bMultipleContinue = qd->status()) )
                     {
                        return;
                     }
               }

            GET_HANDLER_INST;

            std::string filename;
            handler->idToFilename(_id, filename);
            
            handler->reqAbort(_id, false, false);
            if (handler->commandSuccess())
               {
                  removeTrackerStatus(_id);
                  logVerboseMessage(USERMESSAGE_ABORTED_B + filename + USERMESSAGE_ABORTED_E);
                  msb->set(USERMESSAGE_ABORTED_B + filename + USERMESSAGE_ABORTED_E);
               }
            else
               {
                  errorDialog::showAndDie("Error abort " + filename);
               }
         }

         void mainWindow::handle_btn_clean(t_int const _id)
         {
            GET_HANDLER_INST;

            m_bMultipleContinue = true; // continue processing multiple selected torrents
            
            std::string filename;
            handler->idToFilename(_id, filename);

            handler->reqClean(_id, false);
            if (handler->commandSuccess())
               {
                  removeTrackerStatus(_id);
                  logVerboseMessage(USERMESSAGE_CLEAN_B + filename + USERMESSAGE_CLEAN_E);
                  msb->set(USERMESSAGE_CLEAN_B + filename + USERMESSAGE_CLEAN_E);
               }
            else
               {
                  logVerboseMessage(USERMESSAGE_NOT_CLEAN_B + filename + USERMESSAGE_NOT_CLEAN_E);
                  msb->set(USERMESSAGE_NOT_CLEAN_B + filename + USERMESSAGE_NOT_CLEAN_E);
               }
         }

         void mainWindow::handle_btn_limit(t_int const _id)
         {
            GET_HANDLER_INST;

            t_int up_limit   = btg::core::limitBase::LIMIT_DISABLED;
            t_int down_limit = btg::core::limitBase::LIMIT_DISABLED;
            t_int seed_percent  = btg::core::limitBase::LIMIT_DISABLED;
            t_long seed_timeout = btg::core::limitBase::LIMIT_DISABLED;

            std::string limit_filename;
            
            handler->idToFilename(_id, limit_filename);

            if (!m_bMultipleContinue)
               {
               
                  /*
                   * Ugly in some way
                   * Request and show current limits for first selection only
                   * but set limit settings for all selected
                   */
                  
                  handler->reqLimitStatus(_id);

                  if (handler->commandSuccess())
                     {
                        handler->getLimitStatus(up_limit, down_limit);
                        handler->getSeedLimitStatus(seed_percent, seed_timeout);
                     }
                  else
                     {
                        errorDialog::showAndDie("Error getting limit settings for " + limit_filename);
                     }

                  m_limitDialog->update(limit_filename,
                                      up_limit, down_limit,
                                      seed_percent, seed_timeout);

                  m_limitDialog->run(); // also shows it
                  
                  if ( !(m_bMultipleContinue = m_limitDialog->limitSelected()) )
                     {
                        return;
                     }
               }
            
            up_limit     = btg::core::limitBase::LIMIT_DISABLED;
            down_limit   = btg::core::limitBase::LIMIT_DISABLED;
            seed_percent = btg::core::limitBase::LIMIT_DISABLED;
            seed_timeout = btg::core::limitBase::LIMIT_DISABLED;

            // User selected limits.
            if (!m_limitDialog->uploadLimitDisabled())
               {
                  up_limit = m_limitDialog->getUploadLimit();
               }

            // User selected limits.
            if (!m_limitDialog->downloadLimitDisabled())
               {
                  down_limit = m_limitDialog->getDownloadLimit();
               }

            // User selected seed limits.
            if (!m_limitDialog->param3Disabled())
               {
                  seed_percent = m_limitDialog->getParam3();
               }

            // User selected seed limits.
            if (!m_limitDialog->param4Disabled())
               {
                  seed_timeout = m_limitDialog->getParam4();
               }

            handler->reqLimit(_id,
                             up_limit, down_limit,
                             seed_percent, seed_timeout,
                             false);

            if (handler->commandSuccess())
               {
                  msb->set(USERMESSAGE_LIMIT_B + limit_filename + USERMESSAGE_LIMIT_E);
                  logVerboseMessage(USERMESSAGE_LIMIT_B + limit_filename + USERMESSAGE_LIMIT_E);
               }
            else
               {
                  errorDialog::showAndDie("Error setting limits for " + limit_filename);
               }
         }
         
         void mainWindow::handle_btn_move(t_int const _id)
         {
            GET_HANDLER_INST;

            std::string movefilename;
            handler->idToFilename(_id, movefilename);
            
            if (!m_bMultipleContinue)
            {
               
               /*
                * actually it can be done in constructor
                * but then we'll have constant session list
                * and can get moving error
                * if attempted to move to session
                * deleted by other client instance
                * during our execution
                */ 
               
               handler->reqList();
               
               if (!handler->commandSuccess())
                  {
                     msb->set(USERMESSAGE_NOT_MOVED_B + movefilename + USERMESSAGE_NOT_MOVED_E);
                     logVerboseMessage(USERMESSAGE_NOT_MOVED_B + movefilename + USERMESSAGE_NOT_MOVED_E);
                     return;
                  }

               t_longList sessions    = handler->getSessionList();
               t_strList  names       = handler->getSessionNames();
               t_long current_session = handler->session();

               // Remove the current sessions from session to which this
               // context can be moved to.
               t_strListI nameIter = names.begin();
               for (t_longListI idIter = sessions.begin();
                    idIter != sessions.end();
                    idIter++)
                  {
                     if (*idIter == current_session)
                        {
                           sessions.erase(idIter);
                           names.erase(nameIter);
                           break;
                        }
                     nameIter++;
                  }

               btg::core::btg_assert(names.size() == sessions.size(),
                                     logWrapper(),
                                     "number of sessions equal to number of session names");
               
               if (m_sessionSelectionDialog)
               {
                  delete m_sessionSelectionDialog;
               }
               m_sessionSelectionDialog = new sessionSelectionDialog("Select session to move torrent to", 
                                          sessions,
                                          names);
               m_sessionSelectionDialog->run();
            }

            t_long newsession;
            if ( !(m_bMultipleContinue = m_sessionSelectionDialog->getSelectedSession(newsession)) )
               {
                  msb->set(USERMESSAGE_NOT_MOVED_B + movefilename + USERMESSAGE_NOT_MOVED_E);
                  logVerboseMessage(USERMESSAGE_NOT_MOVED_B + movefilename + USERMESSAGE_NOT_MOVED_E);
                  return;
               }
            
            handler->reqMoveContext(_id, newsession);

            if (handler->commandSuccess())
               {
                  removeTrackerStatus(_id);
                  msb->set(USERMESSAGE_MOVED_B + movefilename + USERMESSAGE_MOVED_E);
                  logVerboseMessage(USERMESSAGE_MOVED_B + movefilename + USERMESSAGE_MOVED_E);
               }
            else
               {
                  msb->set(USERMESSAGE_NOT_MOVED_B + movefilename + USERMESSAGE_NOT_MOVED_E);
                  logVerboseMessage(USERMESSAGE_NOT_MOVED_B + movefilename + USERMESSAGE_NOT_MOVED_E);
               }
         }

         void mainWindow::handle_btn_prefs(t_int const _id)
         {
            errorDialog::showAndDie("Torrent preferences not available yet.");

            m_bMultipleContinue = false; // discontinue for multiple selection, show the error only once
         }
         
         bool mainWindow::on_window_state_event (GdkEventWindowState* event)
         {
            m_clientDynConfig.set_gui_window_maximized(event->new_window_state && Gdk::WINDOW_STATE_MAXIMIZED);
            return Gtk::Window::on_window_state_event(event);
         }
         
         bool mainWindow::on_configure_event (GdkEventConfigure* event)
         {
            // Ugly hack related to state/configure events order.
            if (event->x)
            {
               m_clientDynConfig.set_gui_window_position(event->x,event->y);
               m_clientDynConfig.set_gui_window_dimensions(event->width,event->height);
            }
            return Gtk::Window::on_configure_event(event);
         }

         bool mainWindow::isUrlDlEnabled() const
         {
            return urlDlEnabled;
         }

         mainWindow::~mainWindow()
         {
            timeout_connection.disconnect();

            delete aboutdialog;
            aboutdialog = 0;

            delete preferencesdialog;
            preferencesdialog = 0;
            
            delete m_limitDialog;
            m_limitDialog = 0;

            delete upload_progressdialog;
            upload_progressdialog = 0;
         }
         
         void mainWindow::on_peers_scrolled()
         {
            peersCounter = peersMax; // update peers immediately (this second, actually)
         }

      } // namespace gui
   } // namespace UI
} // namespace btg

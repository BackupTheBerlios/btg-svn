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
#include <gdk/gdkkeysyms.h>
#include <gtkmm/accelgroup.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/paned.h>

#include "mainmenubar.h"
#include "maintreeview.h"
#include "mainfiletreeview.h"
#include "selectfiletreeview.h"
#include "mainstatusbar.h"
#include "mainnotebook.h"
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

#include "logtextview.h"

#include "usermessage.h"

#include <bcore/trackerstatus.h>
#include <bcore/command/limit_base.h>

#include <string>

#include <bcore/verbose.h>
#include <bcore/file_info.h>

#define GET_HANDLER_INST \
   boost::shared_ptr<boost::mutex> ptr = handlerthread->mutex(); \
   boost::mutex::scoped_lock interface_lock(*ptr); \
   guiHandler* handler = dynamic_cast<guiHandler*>(handlerthread->handler());

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         using namespace btg::core::logger;
         using namespace std;

         mainWindow::mainWindow(std::string const& _session,
                                bool const _verboseFlag,
                                bool const _neverAskFlag,
                                btg::core::client::handlerThread* _handlerthread)
            : Gtk::Window(Gtk::WINDOW_TOPLEVEL),
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
              limitdialog(0),
              lastOpenDirectory(""),
              updateContexts(true),
              progressCounter(0),
              progressMax(5),
              trackerstatSerial()
         {
            mmb                       = Gtk::manage(new class mainMenubar(this));
            mtw                       = Gtk::manage(new class mainTreeview());
            Gtk::ScrolledWindow* mtsw = Gtk::manage(new class Gtk::ScrolledWindow());
            Gtk::ScrolledWindow* ntsw = Gtk::manage(new class Gtk::ScrolledWindow());
            mnb                       = Gtk::manage(new class mainNotebook());
            Gtk::VPaned* contentsVPaned = Gtk::manage(new class Gtk::VPaned);
            msb                       = Gtk::manage(new class mainStatusbar());
            Gtk::VBox *mainVbox       = Gtk::manage(new class Gtk::VBox(false, 0));

            mtsw->set_flags(Gtk::CAN_FOCUS);
            mtsw->set_shadow_type(Gtk::SHADOW_IN);
            mtsw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
            mtsw->property_window_placement().set_value(Gtk::CORNER_TOP_LEFT);
            mtsw->add(*mtw);

            ntsw->set_flags(Gtk::CAN_FOCUS);
            ntsw->set_shadow_type(Gtk::SHADOW_IN);
            ntsw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
            ntsw->property_window_placement().set_value(Gtk::CORNER_TOP_LEFT);
            ntsw->add(*mnb);

            contentsVPaned->pack1(*mtsw, true, false);
            contentsVPaned->pack2(*ntsw, true, false);

            mainVbox->pack_start(*mmb, Gtk::PACK_SHRINK, 0);

            // Start with the control menu disabled.
            mmb->disableControlFunction();

            mainVbox->pack_start(*contentsVPaned);
            mainVbox->pack_start(*msb, Gtk::PACK_SHRINK, 0);
            set_title("btgui (session #" + _session + ")");
            set_modal(false);
            property_window_position().set_value(Gtk::WIN_POS_NONE);
            set_resizable(true);
            property_destroy_with_parent().set_value(false);
            add(*mainVbox);

            {
               GET_HANDLER_INST;

               // Update the last opened file list in menubar.
               mmb->updateLastFileList(handler->getLastFiles());
            }

            mtsw->show();
            ntsw->show();

            contentsVPaned->show();
            mainVbox->show();
            show();

            // Create/connect a timer used for updating the list of
            // contexts.
            sigc::slot<bool> slot = sigc::bind(sigc::mem_fun(*this, &mainWindow::on_refresh_timeout), 0);
            timeout_connection    = Glib::signal_timeout().connect(slot, 1024);

            // When the main window is closed, call this signal.
            signal_delete_event().connect(sigc::mem_fun(*this, &mainWindow::onWindowClose));

            /// Tell the gui handler about the statusbar, so it can
            /// write messages on it.  As for now only used for daemon
            /// uptime.

            {
               GET_HANDLER_INST;
               handler->setStatusBar(msb);
            }

            BTG_NOTICE("Main window created.\n");
         }

         bool mainWindow::on_refresh_timeout(int)
         {
            if (!updateContexts)
               {
                  BTG_NOTICE("Not updating contexts");
                  return true;
               }

            msb->trigger();

            mnb->getPlot()->resetCounters();

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

            // If a torrent is selected, attempt to get a file info
            // and list of peers.
            if (mtw->gotSelection())
               {
                  mmb->enableControlFunction();

                  t_uint numberOfSelectedItems = mtw->numberOfSelected();

                  if (numberOfSelectedItems == 1)
                     {
                        // Find out what was selected.
                        elemMap m        = mtw->getSelectedIds();
                        elemMap_citer ci = m.begin();
                        int id           = ci->first;

                        mainNotebook::CurrentSelection currentselection = mnb->selected();

                        switch (currentselection)
                           {
                           case mainNotebook::GRAPH:
                              updatePlot();
                              break;
                           case mainNotebook::DETAILS:
                              updateDetails(id, statuslist);
                              break;
                           case mainNotebook::ALL_FILES:
                              updateFiles(id);
                              break;
                           case mainNotebook::SELECTED_FILES:
                              {
                                 checkSelectedFiles();
                                 updateSelectedFiles(id);
                                 break;
                              }
                           case mainNotebook::PEERS:
                              updatePeers(id);
                              break;
                           case mainNotebook::LOG:
                              break;
                           }

                        
                     }
                  else if (numberOfSelectedItems == 0)
                     {
                        // If nothing is selected, disable certain
                        // items in the control menu.
                        mmb->disableControlFunction();
                     }
                  else
                     {
                        clearSelection();
                     }
               }
            else
               {
                  mmb->disableControlFunction();
               }

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

            BTG_NOTICE("updateContexts = " << updateContexts);

            // During execution of this function, no updates of
            // contexts should be done.
            this->updateContexts = false;

            switch (_which_item)
               {
               case buttonMenuIds::BTN_LASTFILE0:
               case buttonMenuIds::BTN_LASTFILE1:
               case buttonMenuIds::BTN_LASTFILE2:
               case buttonMenuIds::BTN_LASTFILE3:
               case buttonMenuIds::BTN_LASTFILE4:
               case buttonMenuIds::BTN_LASTFILE5:
               case buttonMenuIds::BTN_LASTFILE6:
               case buttonMenuIds::BTN_LASTFILE7:
               case buttonMenuIds::BTN_LASTFILE8:
               case buttonMenuIds::BTN_LASTFILE9:
                  {
                     handle_btn_lastfile(_which_item);
                     non_context = true;
                     break;
                  }
               case buttonMenuIds::BTN_ALL_LAST:
                  {
                     handle_btn_lastfile_all(_which_item);
                     non_context = true;
                     break;
                  }
               case buttonMenuIds::BTN_LOAD:
                  {
                     handle_btn_load();
                     BTG_NOTICE("(global) mainToolbar::BTN_LOAD");
                     non_context = true;
                     break;
                  }
               case buttonMenuIds::BTN_PREFERENCES:
                  {
                     handle_btn_prefs();
                     non_context = true;
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
                     non_context = true;
                     break;
                  }
               case buttonMenuIds::BTN_UPTIME:
                  {
                     handle_btn_uptime();
                     non_context = true;
                     break;
                  }
               default:
                  {
                     // Ignore the rest of buttons, handled elsewhere.
                     break;
                  }
               } // switch

            if (non_context)
               {
                  this->updateContexts = true;
                  return;
               }

            if (mtw->gotSelection())
               {
                  mmb->disableControlFunction();

                  elemMap m = mtw->getSelectedIds();

                  for (elemMap_citer i=m.begin(); i!=m.end(); i++)
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
                              {
                                 handle_btn_erase(id);
                                 break;
                              }
                           case buttonMenuIds::BTN_CLEAN:
                              {
                                 handle_btn_clean(id);
                                 break;
                              }
                           case buttonMenuIds::BTN_LIMIT:
                              {
                                 handle_btn_limit(id);
                                 break;
                              }
                           default:
                              {
                                 // Ignore the rest of buttons, handled elsewhere.
                                 break;
                              }
                           } // switch
                     }
                  mmb->enableControlFunction();
               }
            this->updateContexts = true;
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
                     this->lastOpenDirectory = dialog.get_current_folder();
                     this->openFile(dialog.get_filename());
                     break;
                  }
               case(Gtk::RESPONSE_CANCEL):
                  {
                     BTG_NOTICE("Cancel clicked.");
                     break;
                  }
               default:
                  {
                     BTG_NOTICE("Unexpected button clicked.");
                     break;
                  }
               } // switch
         }

         void mainWindow::openFile(std::string const& _filename)
         {
            GET_HANDLER_INST;

            handler->reqCreate(_filename);

            if (handler->commandSuccess())
               {
                  // Update the status bar.
                  msb->set(USERMESSAGE_ADDED_B + _filename + USERMESSAGE_ADDED_E);
                  logVerboseMessage(USERMESSAGE_ADDED_B + _filename + USERMESSAGE_ADDED_E);

                  // Update the last opened file list in menubar.
                  mmb->updateLastFileList(handler->getLastFiles());
               }
            else
               {
                  msb->set(USERMESSAGE_ADD_FAILED_B + _filename + USERMESSAGE_ADD_FAILED_E);
                  logVerboseMessage(USERMESSAGE_ADD_FAILED_B + _filename + USERMESSAGE_ADD_FAILED_E);
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

            VERBOSE_LOG(verboseFlag, _msg);
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
            mnb->getPlot()->updateModel();
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
            GET_HANDLER_INST;

            // Get a list of peers.
            handler->reqPeers(_id);
            if (handler->commandSuccess())
               {
                  t_peerList peerlist = handler->getPeers();
                  
                  mnb->getPeerTreeview()->clear();
                  mnb->getPeerTreeview()->update(peerlist);
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

         void mainWindow::handle_btn_lastfile(buttonMenuIds::MENUID _which_item)
         {
            std::string filename;
            {
               GET_HANDLER_INST;
               filename = handler->getLastFiles().at(_which_item);
            }
            this->openFile(filename);
         }

         void mainWindow::handle_btn_lastfile_all(buttonMenuIds::MENUID _which_item)
         {
            t_strList lastfiles;
            {
               GET_HANDLER_INST;

               // Open all last files.
               lastfiles = handler->getLastFiles();
            }

            for (t_strListCI lastIter=lastfiles.begin();
                 lastIter != lastfiles.end();
                 lastIter++)
               {
                  this->openFile(*lastIter);
               }
         }

         void mainWindow::handle_btn_prefs()
         {
            // Disable updating contexts while this dialog is running.
            this->updateContexts = false;
            
            BTG_NOTICE("(global) mainToolbar::BTN_PREFERENCES");
            
            // Enable updating contexts.
            this->updateContexts = true;
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
                  doQuit = questionDialog::showAndDie("Quit?",
                                                      "Are you sure you really want to quit?");
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
                  suicide = questionDialog::showAndDie("Kill the daemon?",
                                                       "Are you sure you really want to kill the daemon?");
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

         void mainWindow::handle_btn_uptime()
         {
            GET_HANDLER_INST;

            logVerboseMessage(USERMESSAGE_UPTIME);
            msb->set(USERMESSAGE_UPTIME);
            handler->reqUptime();
         }

         void mainWindow::handle_btn_start(t_int const _id)
         {
            GET_HANDLER_INST;

            BTG_NOTICE("mainToolbar::BTN_START");
            handler->reqStart(_id);
            if (handler->commandSuccess())
               {
                  string filename("");
                  handler->idToFilename(_id, filename);
                  logVerboseMessage(USERMESSAGE_STARTED_B + filename + USERMESSAGE_STARTED_E);
                  msb->set(USERMESSAGE_STARTED_B + filename + USERMESSAGE_STARTED_E);
               }
         }

         void mainWindow::handle_btn_stop(t_int const _id)
         {
            GET_HANDLER_INST;

            BTG_NOTICE("mainToolbar::BTN_STOP");
            handler->reqStop(_id);
            if (handler->commandSuccess())
               {
                  string filename("");
                  handler->idToFilename(_id, filename);
                  logVerboseMessage(USERMESSAGE_STOPPED_B + filename + USERMESSAGE_STOPPED_E);
                  msb->set(USERMESSAGE_STOPPED_B + filename + USERMESSAGE_STOPPED_E);
               }
         }

         void mainWindow::handle_btn_erase(t_int const _id)
         {
            GET_HANDLER_INST;

            BTG_NOTICE("mainToolbar::BTN_ERASE");
            handler->reqAbort(_id, true /* erase data */, false);
            if (handler->commandSuccess())
               {
                  removeTrackerStatus(_id);
                  string filename("");
                  handler->idToFilename(_id, filename);
                  logVerboseMessage(USERMESSAGE_ERASED_B + filename + USERMESSAGE_ERASED_E);
                  msb->set(USERMESSAGE_ERASED_B + filename + USERMESSAGE_ERASED_E);
               }
         }

         void mainWindow::handle_btn_abort(t_int const _id)
         {
            GET_HANDLER_INST;

            BTG_NOTICE("mainToolbar::BTN_ABORT");
            handler->reqAbort(_id, false, false);
            if (handler->commandSuccess())
               {
                  removeTrackerStatus(_id);
                  string filename("");
                  handler->idToFilename(_id, filename);
                  logVerboseMessage(USERMESSAGE_ABORTED_B + filename + USERMESSAGE_ABORTED_E);
                  msb->set(USERMESSAGE_ABORTED_B + filename + USERMESSAGE_ABORTED_E);
               }
         }

         void mainWindow::handle_btn_clean(t_int const _id)
         {
            GET_HANDLER_INST;

            BTG_NOTICE("(global) mainToolbar::BTN_CLEAN");
            handler->reqClean(_id, false);
            string filename("");
            handler->idToFilename(_id, filename);
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

            t_int up_limit   = 0;
            t_int down_limit = 0;
            t_int seed_percent  = 0;
            t_long seed_timeout = 0;

            // Only popup the limit dialog _once_.
            bool limit_popup = false;

            string limit_filename;
            
            if (limitdialog == 0)
               {
                  limitdialog = new limitDialog();
               }
            
            handler->idToFilename(_id, limit_filename);

            if (!limit_popup)
               {
                  handler->reqLimitStatus(_id);

                  if (handler->commandSuccess())
                     {
                        handler->getLimitStatus(up_limit, down_limit);

                        handler->getSeedLimitStatus(seed_percent, seed_timeout);
                     }

                  limitdialog->update(limit_filename,
                                      up_limit, down_limit,
                                      seed_percent, seed_timeout);
                  limitdialog->show();
                  limitdialog->run();
               }

            if (limitdialog->limitSelected())
               {
                  limit_popup  = true;
                  up_limit     = btg::core::limitBase::LIMIT_DISABLED;
                  down_limit   = btg::core::limitBase::LIMIT_DISABLED;
                  seed_percent = btg::core::limitBase::LIMIT_DISABLED;
                  seed_timeout = btg::core::limitBase::LIMIT_DISABLED;

                  // User selected limits.
                  if (!limitdialog->uploadLimitDisabled())
                     {
                        up_limit = limitdialog->getUploadLimit();
                     }

                  // User selected limits.
                  if (!limitdialog->downloadLimitDisabled())
                     {
                        down_limit = limitdialog->getDownloadLimit();
                     }

                  // User selected seed limits.
                  if (!limitdialog->seedPercentDisabled())
                     {
                        seed_percent = limitdialog->getSeedPercent();
                     }

                  // User selected seed limits.
                  if (!limitdialog->seedTimeDisabled())
                     {
                        seed_timeout = limitdialog->getSeedTimeout();
                     }

                  BTG_NOTICE("mainToolbar::BTN_LIMIT");

                  handler->reqLimit(_id,
                                   up_limit, down_limit,
                                   seed_percent, seed_timeout,
                                   false);

                  if (handler->commandSuccess())
                     {
                        msb->set(USERMESSAGE_LIMIT_B + limit_filename + USERMESSAGE_LIMIT_E);
                        logVerboseMessage(USERMESSAGE_LIMIT_B + limit_filename + USERMESSAGE_LIMIT_E);
                        BTG_NOTICE("limit set, upload = " << up_limit
                                   << ", download = " << down_limit
                                   << ", seed percent = " << seed_percent
                                   << ", seed timeout = " << seed_timeout);
                     }
               }
            else
               {
                  limit_popup = true;
               }
         }
         
         mainWindow::~mainWindow()
         {
            timeout_connection.disconnect();

            delete aboutdialog;
            aboutdialog = 0;

            delete preferencesdialog;
            preferencesdialog = 0;
         }


      } // namespace gui
   } // namespace UI
} // namespace btg

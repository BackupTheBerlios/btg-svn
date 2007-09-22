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

#include "mainmenubar.h"
#include <gtkmm/menuitem.h>
#include <gtkmm/menu.h>
#include <gtkmm/main.h>

#include <iostream>

#include <bcore/util.h>

#include "sig_button_menu.h"

#include "mainwindow.h"

namespace btg
{
   namespace UI
   {
      namespace gui
      {
         using namespace std;
         using namespace btg::core;

         mainMenubar::mainMenubar(mainWindow* _mainwindow)
            : controlMenuItem(0),
              controlFunctionEnabled(false),
              lastFiles_separator(0),
              lastFiles(0),
              open_all_last(0),
              startMenuitem(0),
              stopMenuitem(0),
              abortMenuitem(0),
              eraseMenuitem(0),
              cleanMenuitem(0),
              limitMenuitem(0)
         {
            Gtk::MenuBar *mainMenubar          = this;
            Gtk::MenuItem *load_menuitem       = 0;
            Gtk::MenuItem *detachMenuitem      = 0;
            Gtk::MenuItem *file_separator      = 0;
            Gtk::MenuItem *preferencesMenuitem = 0;
            Gtk::MenuItem *quitMenuitem        = 0;
            Gtk::Menu *fileMenu                = Gtk::manage(new class Gtk::Menu());

            Gtk::Menu *recentFileMenu          = Gtk::manage(new class Gtk::Menu());
            Gtk::MenuItem *RecentMenuitem      = 0;

            Gtk::MenuItem *fileMenuitem        = 0;

            Gtk::MenuItem *control_separator1  = 0;

            Gtk::Menu* controlMenu             = Gtk::manage(new class Gtk::Menu());
            controlMenuItem                    = 0;

            Gtk::Menu *daemonMenu              = Gtk::manage(new class Gtk::Menu());
            Gtk::MenuItem *daemonMenuitem      = 0;
            Gtk::MenuItem *globallimitMenuitem = 0;
            Gtk::MenuItem *killMenuitem        = 0;
            Gtk::MenuItem *uptimeMenuitem      = 0;

            Gtk::MenuItem *aboutMenuitem       = 0;
            Gtk::Menu *helpMenu                = Gtk::manage(new class Gtk::Menu());
            Gtk::MenuItem *helpMenuitem        = 0;
            Gtk::MenuItem *last_separator      = 0;

            mainMenubar->items().push_back(Gtk::Menu_Helpers::MenuElem("_File", *fileMenu));
            fileMenuitem = reinterpret_cast<Gtk::MenuItem *>(&mainMenubar->items().back());

            mainMenubar->items().push_back(Gtk::Menu_Helpers::MenuElem("_Control", *controlMenu));
            controlMenuItem = reinterpret_cast<Gtk::MenuItem *>(&mainMenubar->items().back());

            mainMenubar->items().push_back(Gtk::Menu_Helpers::MenuElem("_Daemon", *daemonMenu));
            daemonMenuitem = reinterpret_cast<Gtk::MenuItem *>(&mainMenubar->items().back());

            mainMenubar->items().push_back(Gtk::Menu_Helpers::MenuElem("_Help", *helpMenu));
            helpMenuitem = reinterpret_cast<Gtk::MenuItem *>(&mainMenubar->items().back());

            fileMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Open"));
            load_menuitem = reinterpret_cast<Gtk::MenuItem *>(&fileMenu->items().back());

            fileMenu->items().push_back(Gtk::Menu_Helpers::SeparatorElem());
            file_separator = reinterpret_cast<Gtk::MenuItem *>(&fileMenu->items().back());

            fileMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Preferences"));
            preferencesMenuitem = reinterpret_cast<Gtk::MenuItem *>(&fileMenu->items().back());

            fileMenu->items().push_back(Gtk::Menu_Helpers::SeparatorElem());
            last_separator = reinterpret_cast<Gtk::MenuItem *>(&fileMenu->items().back());

            // Recent files, sub-menu.
            fileMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Recent", *recentFileMenu));
            RecentMenuitem = reinterpret_cast<Gtk::MenuItem *>(&fileMenu->items().back());

            // Used to open all files.
            recentFileMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("Open _all files"));
            open_all_last = reinterpret_cast<Gtk::MenuItem *>(&recentFileMenu->items().back());
            open_all_last->hide();
            open_all_last->signal_activate().connect(sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_ALL_LAST) );

            // List of recent files, in the submenu.
            Gtk::MenuItem* temp = 0;

            for (t_int i=0; i<10; i++)
               {
                  recentFileMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("LastFile"));
                  temp = reinterpret_cast<Gtk::MenuItem *>(&recentFileMenu->items().back());
                  temp->hide();

                  temp->signal_activate().connect(sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), static_cast<buttonMenuIds::MENUID>(i) ));
                  lastFiles.push_back(temp);
               }

            fileMenu->items().push_back(Gtk::Menu_Helpers::SeparatorElem());
            lastFiles_separator = reinterpret_cast<Gtk::MenuItem *>(&fileMenu->items().back());
            lastFiles_separator->hide();

            fileMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Detach"));
            detachMenuitem = reinterpret_cast<Gtk::MenuItem *>(&fileMenu->items().back());

            fileMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Quit"));
            quitMenuitem = reinterpret_cast<Gtk::MenuItem *>(&fileMenu->items().back());

            /* */
            /* */

            controlMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Start"));
            startMenuitem = reinterpret_cast<Gtk::MenuItem *>(&controlMenu->items().back());

            controlMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("S_top"));
            stopMenuitem = reinterpret_cast<Gtk::MenuItem *>(&controlMenu->items().back());

            controlMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Abort"));
            abortMenuitem = reinterpret_cast<Gtk::MenuItem *>(&controlMenu->items().back());

            controlMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Erase"));
            eraseMenuitem = reinterpret_cast<Gtk::MenuItem *>(&controlMenu->items().back());

            controlMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Clean"));
            cleanMenuitem = reinterpret_cast<Gtk::MenuItem *>(&controlMenu->items().back());

            controlMenu->items().push_back(Gtk::Menu_Helpers::SeparatorElem());
            control_separator1 = reinterpret_cast<Gtk::MenuItem *>(&controlMenu->items().back());

            controlMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Limit"));
            limitMenuitem = reinterpret_cast<Gtk::MenuItem *>(&controlMenu->items().back());

            // Control should be disabled per default.
            this->disableControlFunction(true);

            /* */
            /* */

            daemonMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Global limit"));
            globallimitMenuitem = reinterpret_cast<Gtk::MenuItem *>(&daemonMenu->items().back());

            daemonMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Kill"));
            killMenuitem = reinterpret_cast<Gtk::MenuItem *>(&daemonMenu->items().back());

            daemonMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Uptime"));
            uptimeMenuitem = reinterpret_cast<Gtk::MenuItem *>(&daemonMenu->items().back());

            /* */
            /* */

            helpMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("_About"));
            aboutMenuitem = reinterpret_cast<Gtk::MenuItem *>(&helpMenu->items().back());

            load_menuitem->show();
            detachMenuitem->show();
            file_separator->show();
            preferencesMenuitem->show();
            last_separator->show();
            quitMenuitem->show();

            killMenuitem->show();
            uptimeMenuitem->show();

            fileMenuitem->show();
            RecentMenuitem->show();

            startMenuitem->show();
            stopMenuitem->show();

            abortMenuitem->show();
            eraseMenuitem->show();

            cleanMenuitem->show();
            control_separator1->show();
            limitMenuitem->show();
            controlMenuItem->show();

            aboutMenuitem->show();
            helpMenuitem->show();

            mainMenubar->show();

            load_menuitem->signal_activate().connect(
                                                     sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_LOAD )
                                                     );

            detachMenuitem->signal_activate().connect(
                                                      sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_DETACH )
                                                      );
            preferencesMenuitem->signal_activate().connect(
                                                           sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_PREFERENCES )
                                                           );
            quitMenuitem->signal_activate().connect(
                                                    sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_QUIT )
                                                    );
            startMenuitem->signal_activate().connect(
                                                     sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_START )
                                                     );
            stopMenuitem->signal_activate().connect(
                                                    sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_STOP )
                                                    );
            abortMenuitem->signal_activate().connect(
                                                     sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_ABORT )
                                                     );
            eraseMenuitem->signal_activate().connect(
                                                     sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_ERASE )
                                                     );
            cleanMenuitem->signal_activate().connect(
                                                     sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_CLEAN )
                                                     );

            limitMenuitem->signal_activate().connect(
                                                     sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_LIMIT )
                                                     );

            killMenuitem->signal_activate().connect(
                                                    sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_GLIMIT )
                                                    );

            killMenuitem->signal_activate().connect(
                                                    sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_KILL )
                                                    );

            uptimeMenuitem->signal_activate().connect(
                                                      sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_UPTIME )
                                                      );

            aboutMenuitem->signal_activate().connect(
                                                     sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_ABOUT )
                                                     );
         }

         void mainMenubar::updateLastFileList(t_strList const& _lastFileList)
         {
            vector<Gtk::MenuItem*>::iterator iter;
            t_uint counter = 0;

            // First hide all the elements from the array.
            for (iter  = lastFiles.begin();
                 iter != lastFiles.end();
                 iter++)
               {
                  Gtk::MenuItem* temp = *iter;
                  temp->hide();
               }
            // Then hide the one item used to open all files.
            open_all_last->hide();

            // Hide the separator.
            lastFiles_separator->hide();

            // Set the filenames.
            for (iter  = lastFiles.begin();
                 iter != lastFiles.end();
                 iter++)
               {
                  if (counter < _lastFileList.size())
                     {
                        Gtk::MenuItem* temp = *iter;
                        Gtk::Label* label = dynamic_cast<Gtk::Label*>(temp->get_child());
                        // Attempt to remove path information, just keep the filename.
                        string long_filename = _lastFileList.at(counter);
                        string short_filename;
                        if (Util::getFileFromPath(long_filename, short_filename) == true)
                           {
                              label->set_text(short_filename);
                           }
                        else
                           {
                              label->set_text(long_filename);
                           }
                        temp->show();
                     }
                  counter++;
               }

            if (_lastFileList.size() > 0)
               {
                  open_all_last->show();
                  lastFiles_separator->show();
               }
         }

         void mainMenubar::enableControlFunction()
         {
            if (controlFunctionEnabled == false)
               {
                  startMenuitem->set_sensitive(true);
                  stopMenuitem->set_sensitive(true);
                  abortMenuitem->set_sensitive(true);
                  eraseMenuitem->set_sensitive(true);
                  limitMenuitem->set_sensitive(true);
                  cleanMenuitem->set_sensitive(true);

                  // controlMenuItem->set_sensitive(true);
                  controlFunctionEnabled = true;
               }
         }

         void mainMenubar::disableControlFunction(bool const _force)
         {
            if ((controlFunctionEnabled == true) || (_force == true))
               {
                  // controlMenuItem->set_sensitive(false);

                  startMenuitem->set_sensitive(false);
                  stopMenuitem->set_sensitive(false);
                  abortMenuitem->set_sensitive(false);
                  eraseMenuitem->set_sensitive(false);
                  limitMenuitem->set_sensitive(false);
                  cleanMenuitem->set_sensitive(false);

                  controlFunctionEnabled = false;
               }
         }

         mainMenubar::~mainMenubar()
         {
         }

      } // namespace gui
   } // namespace UI
} // namespace btg

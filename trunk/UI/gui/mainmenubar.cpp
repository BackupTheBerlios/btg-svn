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
#include <gtkmm.h>

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
         using namespace btg::core;

         mainMenubar::mainMenubar(mainWindow* _mainwindow)
            : controlMenuItem(0),
              controlFunctionEnabled(true),
              lastFilesMenu(0),
              lastFiles(0),
              startMenuitem(0),
              stopMenuitem(0),
              abortMenuitem(0),
              eraseMenuitem(0),
              cleanMenuitem(0),
              limitMenuitem(0),
              moveMenuitem(0),
              preferencesMenuitem(0)
         {
            Gtk::MenuBar *mainMenubar          = this;
            
            Gtk::MenuItem *detachMenuitem      = 0;
            Gtk::Menu *fileMenu                = Gtk::manage(new class Gtk::Menu());

            lastFilesMenu                      = Gtk::manage(new class Gtk::Menu());
            Gtk::MenuItem *openAllLastMenuitem = 0;

            Gtk::MenuItem *fileMenuitem        = 0;

            Gtk::Menu* controlMenu             = Gtk::manage(new class Gtk::Menu());
            controlMenuItem                    = 0;

            Gtk::Menu *daemonMenu              = Gtk::manage(new class Gtk::Menu());
            Gtk::MenuItem *daemonMenuitem      = 0;
            Gtk::MenuItem *globallimitMenuitem = 0;
            Gtk::MenuItem *aboutMenuitem       = 0;
            Gtk::Menu *helpMenu                = Gtk::manage(new class Gtk::Menu());
            Gtk::MenuItem *helpMenuitem        = 0;

            /*
             * Main menu
             */

            mainMenubar->items().push_back(Gtk::Menu_Helpers::MenuElem("_File", *fileMenu));
            fileMenuitem = & mainMenubar->items().back();

            mainMenubar->items().push_back(Gtk::Menu_Helpers::MenuElem("_Control", *controlMenu));
            controlMenuItem = & mainMenubar->items().back();

            mainMenubar->items().push_back(Gtk::Menu_Helpers::MenuElem("_Daemon", *daemonMenu));
            daemonMenuitem = & mainMenubar->items().back();

            mainMenubar->items().push_back(Gtk::Menu_Helpers::MenuElem("_Help", *helpMenu));
            helpMenuitem = & mainMenubar->items().back();

            /*
             * File menu
             */
            
            Gtk::MenuItem* load_menuitem = new Gtk::ImageMenuItem(*Gtk::manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU)), "_Open", true);
            load_menuitem->signal_activate().connect( sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_LOAD ) );
            fileMenu->items().push_back(*load_menuitem);

            Gtk::MenuItem* loadurl_menuitem = new Gtk::ImageMenuItem(*Gtk::manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU)), "Open _URL", true);
            loadurl_menuitem->signal_activate().connect( sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_LOAD_URL ) );
            fileMenu->items().push_back(*loadurl_menuitem);

            fileMenu->items().push_back(Gtk::Menu_Helpers::SeparatorElem());

            preferencesMenuitem = new Gtk::ImageMenuItem(*Gtk::manage(new Gtk::Image(Gtk::Stock::PROPERTIES, Gtk::ICON_SIZE_MENU)), "_Preferences", true);
            preferencesMenuitem->signal_activate().connect( sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_PREFERENCES ) );
            fileMenu->items().push_back(*preferencesMenuitem);

            fileMenu->items().push_back(Gtk::Menu_Helpers::SeparatorElem());

            // Recent files, sub-menu.
            fileMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Recent", *lastFilesMenu));

            // Used to open all files.
            openAllLastMenuitem = new Gtk::ImageMenuItem(*Gtk::manage(new Gtk::Image(Gtk::Stock::OPEN, Gtk::ICON_SIZE_MENU)), "Open _all files", true);
            openAllLastMenuitem->signal_activate().connect(sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_ALL_LAST) );
            lastFilesMenu->items().push_back(*openAllLastMenuitem);

            lastFilesMenu->items().push_back(Gtk::Menu_Helpers::SeparatorElem());

            // List of recent files, in the submenu.
            for (t_int i=0; i<10; i++)
               {
                  lastFilesMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("LastFile"));

                  Gtk::MenuItem* temp = & lastFilesMenu->items().back();
                  temp->signal_activate().connect(sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), static_cast<buttonMenuIds::MENUID>(i) ));

                  lastFiles.push_back(temp);
               }

            fileMenu->items().push_back(Gtk::Menu_Helpers::SeparatorElem());

            detachMenuitem = new Gtk::ImageMenuItem(*Gtk::manage(new Gtk::Image(Gtk::Stock::QUIT, Gtk::ICON_SIZE_MENU)), "_Detach", true);
            detachMenuitem->signal_activate().connect( sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_DETACH ) );
            fileMenu->items().push_back(*detachMenuitem);

            fileMenu->items().push_back(
               Gtk::Menu_Helpers::MenuElem(
                  "_Quit",
                  sigc::bind<buttonMenuIds::MENUID>(
                     sigc::mem_fun(
                        *_mainwindow,
                        &mainWindow::on_menu_item_selected
                     ),
                     buttonMenuIds::BTN_QUIT
                  )
               )
            );

            /* */
            /* */

            startMenuitem = new Gtk::ImageMenuItem(*Gtk::manage(new Gtk::Image(Gtk::Stock::MEDIA_PLAY, Gtk::ICON_SIZE_MENU)), "_Start", true);
            startMenuitem->signal_activate().connect( sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_START ) );
            controlMenu->items().push_back(*startMenuitem);

            stopMenuitem = new Gtk::ImageMenuItem(*Gtk::manage(new Gtk::Image(Gtk::Stock::MEDIA_PAUSE, Gtk::ICON_SIZE_MENU)), "S_top", true);
            stopMenuitem->signal_activate().connect( sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_STOP ) );
            controlMenu->items().push_back(*stopMenuitem);

            abortMenuitem = new Gtk::ImageMenuItem(*Gtk::manage(new Gtk::Image(Gtk::Stock::CANCEL, Gtk::ICON_SIZE_MENU)), "_Abort", true);
            abortMenuitem->signal_activate().connect( sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_ABORT ) );
            controlMenu->items().push_back(*abortMenuitem);

            eraseMenuitem = new Gtk::ImageMenuItem(*Gtk::manage(new Gtk::Image(Gtk::Stock::DELETE, Gtk::ICON_SIZE_MENU)), "Erase"); // dangerous item - without accelerator
            eraseMenuitem->signal_activate().connect( sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_ERASE ) );
            controlMenu->items().push_back(*eraseMenuitem);

            cleanMenuitem = new Gtk::ImageMenuItem(*Gtk::manage(new Gtk::Image(Gtk::Stock::APPLY, Gtk::ICON_SIZE_MENU)), "_Clean", true);
            cleanMenuitem->signal_activate().connect( sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_CLEAN ) );
            controlMenu->items().push_back(*cleanMenuitem);

            controlMenu->items().push_back(Gtk::Menu_Helpers::SeparatorElem());

            limitMenuitem = new Gtk::ImageMenuItem(*Gtk::manage(new Gtk::Image(Gtk::Stock::PREFERENCES, Gtk::ICON_SIZE_MENU)), "_Limit", true);
            limitMenuitem->signal_activate().connect( sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_LIMIT ) );
            controlMenu->items().push_back(*limitMenuitem);

            controlMenu->items().push_back(
               Gtk::Menu_Helpers::MenuElem(
                  "_Move to session",
                  sigc::bind<buttonMenuIds::MENUID>(
                     sigc::mem_fun(
                        *_mainwindow,
                        &mainWindow::on_menu_item_selected
                     ),
                     buttonMenuIds::BTN_MOVE
                  )
               )
            );
            moveMenuitem = & controlMenu->items().back();
            
            // Control should be disabled per default.
            disableControlFunction();

            /* */
            /* */

            globallimitMenuitem = new Gtk::ImageMenuItem(*Gtk::manage(new Gtk::Image(Gtk::Stock::PREFERENCES, Gtk::ICON_SIZE_MENU)), "_Global limit", true);
            globallimitMenuitem->signal_activate().connect( sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_GLIMIT ) );
            daemonMenu->items().push_back(*globallimitMenuitem);

            daemonMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("Kill", sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_KILL ) ) );

            daemonMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Uptime", sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_UPTIME ) ) );

            daemonMenu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Session Name", sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_SESNAME ) ) );

            /* */
            /* */

            aboutMenuitem = new Gtk::ImageMenuItem(*Gtk::manage(new Gtk::Image(Gtk::Stock::ABOUT, Gtk::ICON_SIZE_MENU)), "_About", true);
            aboutMenuitem->signal_activate().connect( sigc::bind<buttonMenuIds::MENUID>( sigc::mem_fun(*_mainwindow, &mainWindow::on_menu_item_selected), buttonMenuIds::BTN_ABOUT ) );
            helpMenu->items().push_back(*aboutMenuitem);

            /* */

         }

         void mainMenubar::updateLastFileList(t_strList const& _lastFileList)
         {
            std::vector<Gtk::MenuItem*>::iterator iter;
            t_uint counter = 0;

            // First hide all the elements from the array.
            for (iter  = lastFiles.begin();
                 iter != lastFiles.end();
                 iter++)
               {
                  Gtk::MenuItem* temp = *iter;
                  temp->hide();
               }
            // Then hide the recent file menu item.
            lastFilesMenu->set_sensitive(false);

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
                        std::string long_filename = _lastFileList.at(counter);
                        std::string short_filename;
                        if (Util::getFileFromPath(long_filename, short_filename))
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
                  lastFilesMenu->set_sensitive();
               }
         }

         void mainMenubar::enableControlFunction()
         {
            if (!controlFunctionEnabled)
               {
                  startMenuitem->set_sensitive(true);
                  stopMenuitem->set_sensitive(true);
                  abortMenuitem->set_sensitive(true);
                  eraseMenuitem->set_sensitive(true);
                  limitMenuitem->set_sensitive(true);
                  cleanMenuitem->set_sensitive(true);
                  moveMenuitem->set_sensitive(true);
                  preferencesMenuitem->set_sensitive(true);
                  
                  controlFunctionEnabled = true;
               }
         }

         void mainMenubar::disableControlFunction()
         {
            if (controlFunctionEnabled)
               {
                  startMenuitem->set_sensitive(false);
                  stopMenuitem->set_sensitive(false);
                  abortMenuitem->set_sensitive(false);
                  eraseMenuitem->set_sensitive(false);
                  limitMenuitem->set_sensitive(false);
                  cleanMenuitem->set_sensitive(false);
                  moveMenuitem->set_sensitive(false);
                  preferencesMenuitem->set_sensitive(false);
                  
                  controlFunctionEnabled = false;
               }
         }

         mainMenubar::~mainMenubar()
         {
         }

      } // namespace gui
   } // namespace UI
} // namespace btg

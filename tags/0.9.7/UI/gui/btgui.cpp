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

#include <bcore/project.h>
#include <bcore/type.h>
#include <bcore/util.h>
#include <bcore/client/carg.h>
#include <bcore/client/configuration.h>
#include <bcore/client/lastfiles.h>

#include <bcore/externalization/xmlrpc.h>

#include <bcore/os/id.h>
#include <bcore/os/fileop.h>

#include <gtkmm/main.h>

#include "mainwindow.h"

#include "initwindow.h"
#include "errordialog.h"

#include "guihandler.h"
#include "sessiondialog.h"

#include <bcore/logger/logger.h>
#include <bcore/logger/file_log.h>
#include <bcore/t_string.h>
#include <bcore/crashlog.h>
#include <bcore/logable.h>
#include <bcore/client/handlerthr.h>
#include <bcore/client/clientdynconfig.h>
#include <bcore/client/loglevel.h>

using namespace btg::core;
using namespace btg::core::logger;
using namespace btg::core::client;
using namespace btg::UI::gui;

int main(int argc, char **argv)
{
   LogWrapperType logwrapper(new btg::core::logger::logWrapper);
 
   btg::core::crashLog::init();

   commandLineArgumentHandler* cla = new commandLineArgumentHandler(GPD->sGUI_CONFIG());

   cla->setup();

   // Parse command line arguments.
   if (!cla->parse(argc, argv))
      {
         delete cla;
         cla    = 0;

         projectDefaults::killInstance();

         return BTG_ERROR_EXIT;
      }

   // Before doing anything else, check if the user wants to get the
   // syntax of the configuration file.
   if (cla->listConfigFileSyntax())
      {
         clientConfiguration non_existing(logwrapper, "non_existing");

         std::cout << non_existing.getSyntax() << std::endl;

         delete cla;
         cla = 0;

         projectDefaults::killInstance();

         return BTG_NORMAL_EXIT;
      }

   bool verboseFlag = cla->beVerbose();

   Gtk::Main* initMain = new Gtk::Main(&argc, &argv);

   BTG_NOTICE(logwrapper, "Creating the init window.");
   initWindow* iw = new initWindow();
   iw->show();

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_START);

   iw->updateProgress(initWindow::IEV_RCONF);

   // Open the configuration file:
   std::string config_filename = GPD->sCLI_CONFIG();

   if (cla->configFileSet())
      {
         config_filename = cla->configFile();
      }

   std::string errorString;
   if (!btg::core::os::fileOperation::check(config_filename, errorString, false))
      {
         BTG_FATAL_ERROR(logwrapper,
                         GPD->sGUI_CLIENT(), "Could not open file '" << config_filename << "'");

         return BTG_ERROR_EXIT;
      }

   // Open the configuration file:
   clientConfiguration* config = new clientConfiguration(logwrapper, config_filename);

   bool const gotConfig = config->read();

   bool neverAskFlag = config->getNeverAskQuestions();

   clientDynConfig cliDynConf(logwrapper, GPD->sCLI_DYNCONFIG());
   lastFiles* lastfiles = new lastFiles(logwrapper, cliDynConf);

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_RCONF_DONE);

   // Update init dialog.
   // iw->updateProgress(initWindow::IEV_CLA);

   if (!gotConfig)
      {
         BTG_FATAL_ERROR(logwrapper,
                         GPD->sGUI_CLIENT(), "Could not read the config file, '" << GPD->sGUI_CONFIG() << "'. Create one.");
         delete config;
         config = 0;

         delete lastfiles;
         lastfiles = 0;

         delete cla;
         cla    = 0;

         delete iw;
         iw = 0;

         delete initMain;
         initMain = 0;

         projectDefaults::killInstance();

         return BTG_ERROR_EXIT;
      }

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_CLA_DONE);

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_TRANSP);

   // Create a transport to the daemon:
   btg::core::externalization::Externalization* externalization = 0;
   messageTransport* transport                                  = 0;

   transportHelper* transporthelper = new transportHelper(logwrapper,
                                                          GPD->sGUI_CLIENT(),
                                                          config,
                                                          cla);

   if (!transporthelper->initTransport(externalization, transport))
      {
         errorDialog::showAndDie(transporthelper->getMessages());

         delete config;
         config = 0;

         delete lastfiles;
         lastfiles = 0;

         delete cla;
         cla    = 0;

         delete transport;
         transport = 0;

         delete externalization;
         externalization = 0;

         delete iw;
         iw = 0;

         delete initMain;
         initMain = 0;

         delete transporthelper;
         transporthelper = 0;

         projectDefaults::killInstance();

         return BTG_ERROR_EXIT;
      }

   delete transporthelper;
   transporthelper = 0;

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_TRANSP_DONE);

   guiHandler* guihandler = new guiHandler(logwrapper,
                                           externalization,
                                           transport,
                                           config,
                                           lastfiles,
                                           verboseFlag,
                                           cla->automaticStart(),
                                           0 /* initially null */);

   std::string initialStatusMessage("");

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_SETUP);

   // Create a helper to do the initial setup of this client.
   startupHelper* starthelper = new guiStartupHelper(logwrapper,
                                                     config,
                                                     cla,
                                                     transport,
                                                     guihandler);

   if (!starthelper->init())
      {
         errorDialog::showAndDie(starthelper->getMessages());
         BTG_FATAL_ERROR(logwrapper,
                         GPD->sGUI_CLIENT(), "Internal error: start up helper not initialized.");

         delete starthelper;
         starthelper = 0;

         delete guihandler;
         guihandler = 0;

         delete externalization;
         externalization = 0;

         delete cla;
         cla = 0;

         delete iw;
         iw = 0;

         delete initMain;
         initMain = 0;

         delete config;
         config = 0;

         delete lastfiles;
         lastfiles = 0;

         projectDefaults::killInstance();

         return BTG_ERROR_EXIT;
      }

   setDefaultLogLevel(logwrapper, cla->doDebug(), verboseFlag);

   // Initialize logging.
   if (starthelper->execute(startupHelper::op_log) != startupHelper::or_log_success)
      {
         BTG_FATAL_ERROR(logwrapper,
                         GPD->sCLI_CLIENT(), "Unable to initialize logging");

         delete starthelper;
         starthelper = 0;

         delete guihandler;
         guihandler = 0;

         delete externalization;
         externalization = 0;

         delete cla;
         cla = 0;

         delete iw;
         iw = 0;

         delete initMain;
         initMain = 0;

         delete config;
         config = 0;

         delete lastfiles;
         lastfiles = 0;

         projectDefaults::killInstance();

         return BTG_ERROR_EXIT;
      }

   if (config->authSet())
      {
         // Auth info is in the config.
         starthelper->setUser(config->getUserName());
         starthelper->setPasswordHash(config->getPasswordHash());
      }
   else
      {
         // Ask the user about which username and password to use.
         if (starthelper->execute(startupHelper::op_auth) != startupHelper::or_auth_success)
            {
               BTG_FATAL_ERROR(logwrapper,
                               GPD->sCLI_CLIENT(), "Unable to initialize logging");
               delete starthelper;
               starthelper = 0;

               delete guihandler;
               guihandler = 0;

               delete externalization;
               externalization = 0;

               delete cla;
               cla = 0;

               delete iw;
               iw = 0;

               delete initMain;
               initMain = 0;

               delete config;
               config = 0;

               delete lastfiles;
               lastfiles = 0;

               projectDefaults::killInstance();

               return BTG_ERROR_EXIT;
            }
      }

   /// Initialize the transport
   starthelper->execute(startupHelper::op_init);

   // Handle command line options:
   if (cla->doList())
      {
         if (starthelper->execute(startupHelper::op_list) == startupHelper::or_list_failture)
            {
               errorDialog::showAndDie(starthelper->getMessages());
            }

         // Clean up, before quitting.
         delete starthelper;
         starthelper = 0;

         delete guihandler;
         guihandler = 0;

         delete externalization;
         externalization = 0;

         delete cla;
         cla = 0;

         delete iw;
         iw = 0;

         delete initMain;
         initMain = 0;

         delete config;
         config = 0;

         delete lastfiles;
         lastfiles = 0;

         projectDefaults::killInstance();

         return BTG_NORMAL_EXIT;
      }
   else if (cla->doAttachFirst())
      {
         // Attach to the first available session.

         if (starthelper->execute(startupHelper::op_attach_first) == startupHelper::or_attach_first_failture)
            {
               errorDialog::showAndDie(starthelper->getMessages());
               BTG_FATAL_ERROR(logwrapper,
                               GPD->sGUI_CLIENT(), "Unable to attach to session");

               // Clean up, before quitting.
               delete starthelper;
               starthelper = 0;

               delete guihandler;
               guihandler = 0;

               delete cla;
               cla = 0;

               delete externalization;
               externalization = 0;

               delete iw;
               iw = 0;

               delete initMain;
               initMain = 0;

               delete config;
               config = 0;

               delete lastfiles;
               lastfiles = 0;

               projectDefaults::killInstance();

               return BTG_ERROR_EXIT;
            }

         initialStatusMessage = "Attached to session.";
      }
   else if (cla->doAttach())
      {
         // Attach to a certain session, either specified on the
         // command line or chosen by the user from a list.

         startupHelper::operationResult result = starthelper->execute(startupHelper::op_attach);

         if (result == startupHelper::or_attach_failture)
            {
               errorDialog::showAndDie(starthelper->getMessages());
               BTG_FATAL_ERROR(logwrapper,
                               GPD->sGUI_CLIENT(), "Unable to attach to session");
            }
         
         if ((result == startupHelper::or_attach_failture) || 
             (result == startupHelper::or_attach_cancelled))
            {
               // Clean up, before quitting.
               delete starthelper;
               starthelper = 0;
	     
               delete guihandler;
               guihandler = 0;
	     
               delete externalization;
               externalization = 0;
	     
               delete cla;
               cla = 0;
	     
               delete iw;
               iw = 0;
	     
               delete initMain;
               initMain = 0;
	     
               delete config;
               config = 0;
	     
               delete lastfiles;
               lastfiles = 0;
	     
               projectDefaults::killInstance();

               return BTG_ERROR_EXIT;
            }

         initialStatusMessage = "Attached to session.";

      } // Attach to as session.

   bool executeSetup        = false;
   bool attachToSession     = false;
   t_long sessionToAttachTo = Command::INVALID_SESSION;

   if ((!cla->doAttach()) && (!cla->doAttachFirst()))
      {
         // Not attaching to a session. Show a dialog that lets one
         // select which session to use or to create a new one.

         // Get a list of sessions.
         guihandler->reqGetActiveSessions();
         
         t_longList sessionlist = guihandler->getSessionList();
         t_strList sessionsIDs = guihandler->getSessionNames();

         sessionDialog* sd = new sessionDialog(sessionlist, sessionsIDs);
         sd->run();

         switch (sd->getResult())
            {
            case sessionDialog::QUIT:
               {
                  delete sd;
                  sd = 0;

                  // Clean up, before quitting.
                  delete starthelper;
                  starthelper = 0;
                  
                  delete guihandler;
                  guihandler = 0;
                  
                  delete externalization;
                  externalization = 0;
                  
                  delete cla;
                  cla = 0;
	     
                  delete iw;
                  iw = 0;
                  
                  delete initMain;
                  initMain = 0;
                  
                  delete config;
                  config = 0;
                  
                  delete lastfiles;
                  lastfiles = 0;
                  
                  projectDefaults::killInstance();

                  return BTG_ERROR_EXIT;
                  break;
               }
            case sessionDialog::NEW_SESSION:
               {
                  executeSetup    = true;
                  attachToSession = false;
                  // Create a new session.
                  break;
               }
            case sessionDialog::SELECTED:
               {
                  // A session was selected.
                  executeSetup    = false;
                  attachToSession = true;
                  sd->getSelectedSession(sessionToAttachTo);
                  break;
               }
            }

         delete sd;
         sd = 0;
      }
   
   // Only execute setup if we are not attaching to an existing session.
   if (executeSetup)
      {
         if (starthelper->execute(startupHelper::op_setup) == startupHelper::or_setup_failture)
            {
               errorDialog::showAndDie(starthelper->getMessages());

               // Clean up, before quitting.
               delete starthelper;
               starthelper = 0;

               delete guihandler;
               guihandler = 0;

               delete externalization;
               externalization = 0;

               delete cla;
               cla = 0;

               delete iw;
               iw = 0;

               delete initMain;
               initMain = 0;

               delete config;
               config = 0;

               delete lastfiles;
               lastfiles = 0;

               projectDefaults::killInstance();

               return BTG_ERROR_EXIT;
            }

         initialStatusMessage = "Established session.";
      }

   if (attachToSession)
      {
         std::string errorMessage;
         guihandler->reqSetupAttach(sessionToAttachTo);

         if (!guihandler->isAttachDone())
            {
               errorDialog::showAndDie("Unable to attach to session");
               BTG_FATAL_ERROR(logwrapper,
                               GPD->sGUI_CLIENT(), "Unable to attach to session");

               // Clean up, before quitting.
               delete starthelper;
               starthelper = 0;

               delete guihandler;
               guihandler = 0;

               delete externalization;
               externalization = 0;

               delete cla;
               cla = 0;

               delete iw;
               iw = 0;

               delete initMain;
               initMain = 0;

               delete config;
               config = 0;

               delete lastfiles;
               lastfiles = 0;

               projectDefaults::killInstance();

               return BTG_ERROR_EXIT;
            }
      }

   // If the user requested to open any files, do it.
   if (cla->inputFilenamesPresent())
      {
         t_strList filelist = cla->getInputFilenames();
         t_strListCI iter;
         for (iter = filelist.begin(); iter != filelist.end(); iter++)
            {
               guihandler->reqCreate(*iter);
            }
      }

   // Done using arguments.
   delete cla;
   cla = 0;

   // Done using the start up helper.
   delete starthelper;
   starthelper = 0;

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_SETUP_DONE);

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_END);

   t_long session = guihandler->session();
   std::string str_session = btg::core::convertToString<t_long>(session);

   // Get some info about the current session, so it can be displayed
   // to the user.
   guihandler->reqSessionInfo();
   
   if (guihandler->dht())
      {
         str_session += " D";
      }

   if (guihandler->encryption())
      {
         str_session += " E";
      }

   // Start a thread that takes care of communicating with the daemon.
   handlerThread* handlerthr = new handlerThread(logwrapper,
                                                 verboseFlag, 
                                                 guihandler);

   mainWindow* mainWindow = new class mainWindow(logwrapper,
                                                 str_session, 
                                                 verboseFlag, 
                                                 neverAskFlag,
                                                 handlerthr,
                                                 cliDynConf
                                                 );

   BTG_NOTICE(logwrapper, initialStatusMessage);

   BTG_NOTICE(logwrapper, "Deleting the init window.");

   iw->hide();
   delete iw;
   iw = 0;

   initMain->run(*mainWindow);

   delete mainWindow;
   mainWindow = 0;

   delete handlerthr;
   handlerthr = 0;

   // By deleting the gui handler, the transport used is also deleted.
   delete guihandler;
   guihandler = 0;

   delete externalization;
   externalization = 0;

   delete initMain;
   initMain = 0;

   if (config->modified())
      {
         config->write();
      }

   delete config;
   config = 0;

   delete lastfiles;
   lastfiles = 0;

   projectDefaults::killInstance();

   return BTG_NORMAL_EXIT;
}

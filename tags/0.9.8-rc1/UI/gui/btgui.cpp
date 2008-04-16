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
#include <bcore/client/urlhelper.h>
#include <bcore/os/sleep.h>

#include <bcore/verbose.h>

using namespace btg::core;
using namespace btg::core::logger;
using namespace btg::core::client;
using namespace btg::UI::gui;

void OpenUrls(guiHandler & _handler, 
              t_strList const& _filelist);

int main(int argc, char **argv)
{
   btg::core::crashLog::init();
   projectDefaultsKiller PDK__; // need to be before anything uses GPD
   LogWrapperType logwrapper(new btg::core::logger::logWrapper);

   commandLineArgumentHandler cla(GPD->sGUI_CONFIG());
   cla.setup();

   // Parse command line arguments.
   if (!cla.parse(argc, argv))
      {
         return BTG_ERROR_EXIT;
      }

   // Before doing anything else, check if the user wants to get the
   // syntax of the configuration file.
   if (cla.listConfigFileSyntax())
      {
         clientConfiguration non_existing(logwrapper, "non_existing");

         std::cout << non_existing.getSyntax() << std::endl;

         return BTG_NORMAL_EXIT;
      }

   bool verboseFlag = cla.beVerbose();

   setDefaultLogLevel(logwrapper, cla.doDebug(), verboseFlag);
   
   Gtk::Main initMain(&argc, &argv);

   BTG_NOTICE(logwrapper, "Creating the init window.");
   std::auto_ptr<initWindow> iw(new initWindow());
   iw->show();

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_START);

   iw->updateProgress(initWindow::IEV_RCONF);

   // Open the configuration file:
   std::string config_filename = GPD->sCLI_CONFIG();

   if (cla.configFileSet())
      {
         config_filename = cla.configFile();
      }

   VERBOSE_LOG(logwrapper, verboseFlag, "config: '" << config_filename << "'.");

   std::string errorString;
   if (!btg::core::os::fileOperation::check(config_filename, errorString, false))
      {
         BTG_FATAL_ERROR(logwrapper,
                         GPD->sGUI_CLIENT(), "Could not open file '" << config_filename << "'");

         return BTG_ERROR_EXIT;
      }

   // Open the configuration file:
   clientConfiguration config(logwrapper, config_filename);

   bool const gotConfig = config.read();

   bool neverAskFlag = config.getNeverAskQuestions();

   clientDynConfig dynconfig(logwrapper, GPD->sCLI_DYNCONFIG());

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_RCONF_DONE);

   // Update init dialog.
   // iw->updateProgress(initWindow::IEV_CLA);

   if (!gotConfig)
      {
         BTG_FATAL_ERROR(logwrapper,
                         GPD->sGUI_CLIENT(), "Could not read the config file, '" << GPD->sGUI_CONFIG() << "'. Create one.");

         return BTG_ERROR_EXIT;
      }

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_CLA_DONE);

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_TRANSP);

   // Create a transport to the daemon:
   std::auto_ptr<btg::core::externalization::Externalization> apExternalization;
   std::auto_ptr<messageTransport> apTransport;

   INIT_TRANSPORT:
   {
      btg::core::externalization::Externalization* externalization = 0;
      messageTransport* transport                                  = 0;
      
      transportHelper transporthelper(logwrapper,
                                      GPD->sGUI_CLIENT(),
                                      config,
                                      cla);

      if (!transporthelper.initTransport(externalization, transport))
         {
            errorDialog::showAndDie(transporthelper.getMessages());

            delete transport;
            transport = 0;

            delete externalization;
            externalization = 0;

            return BTG_ERROR_EXIT;
         }
      
      apExternalization.reset(externalization);
      apTransport.reset(transport);
   } /* INIT_TRANSPORT */
   
   // Update init dialog.
   iw->updateProgress(initWindow::IEV_TRANSP_DONE);

   guiHandler guihandler(logwrapper,
                         *apExternalization,
                         *apTransport,
                         config,
                         dynconfig,
                         verboseFlag,
                         cla.automaticStart(),
                         0 /* initially null */);

   std::string initialStatusMessage;

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_SETUP);

   // Create a helper to do the initial setup of this client.
   std::auto_ptr<startupHelper> starthelper(new guiStartupHelper(logwrapper,
                                                                 config,
                                                                 cla,
                                                                 *apTransport,
                                                                 guihandler));

   if (!starthelper->init())
      {
         errorDialog::showAndDie(starthelper->getMessages());
         BTG_FATAL_ERROR(logwrapper,
                         GPD->sGUI_CLIENT(), "Internal error: start up helper not initialized.");

         return BTG_ERROR_EXIT;
      }
   
   // Initialize logging.
   if (starthelper->execute(startupHelper::op_log) != startupHelper::or_log_success)
      {
         BTG_FATAL_ERROR(logwrapper,
                         GPD->sCLI_CLIENT(), "Unable to initialize logging");

         return BTG_ERROR_EXIT;
      }

   if (config.authSet())
      {
         // Auth info is in the config.
         starthelper->setUser(config.getUserName());
         starthelper->setPasswordHash(config.getPasswordHash());
      }
   else
      {
         AUTH_RETRY:
         // Ask the user about which username and password to use.
         if (starthelper->execute(startupHelper::op_auth) != startupHelper::or_auth_success)
            {
               BTG_FATAL_ERROR(logwrapper,
                               GPD->sCLI_CLIENT(), "Unable to initialize logging");

               return BTG_ERROR_EXIT;
            }
      }
   
   /// Initialize the transport
   if (starthelper->execute(startupHelper::op_init) != startupHelper::or_init_success)
   {
      errorDialog::showAndDie("Authentication error.");
      goto AUTH_RETRY;
   }
   
   // Handle command line options:
   if (cla.doList())
      {
         if (starthelper->execute(startupHelper::op_list) == startupHelper::or_list_failture)
            {
               errorDialog::showAndDie(starthelper->getMessages());
            }

         return BTG_NORMAL_EXIT;
      }
   else if (cla.doAttachFirst())
      {
         // Attach to the first available session.

         if (starthelper->execute(startupHelper::op_attach_first) == startupHelper::or_attach_first_failture)
            {
               errorDialog::showAndDie(starthelper->getMessages());
               BTG_FATAL_ERROR(logwrapper,
                               GPD->sGUI_CLIENT(), "Unable to attach to session");

               return BTG_ERROR_EXIT;
            }

         initialStatusMessage = "Attached to session.";
      }
   else if (cla.doAttach())
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
               return BTG_ERROR_EXIT;
            }

         initialStatusMessage = "Attached to session.";

      } // Attach to as session.

   bool executeSetup        = false;
   bool attachToSession     = false;
   t_long sessionToAttachTo = Command::INVALID_SESSION;

   if ((!cla.doAttach()) && (!cla.doAttachFirst()))
      {
         // Not attaching to a session. Show a dialog that lets one
         // select which session to use or to create a new one.

         // Get a list of sessions.
         guihandler.reqGetActiveSessions();
         
         t_longList sessionlist = guihandler.getSessionList();
         t_strList sessionsIDs = guihandler.getSessionNames();

         sessionDialog sd(sessionlist, sessionsIDs);
         sd.run();

         switch (sd.getResult())
            {
            case sessionDialog::QUIT:
               {
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
                  sd.getSelectedSession(sessionToAttachTo);
                  break;
               }
            }
      }
   
   // Only execute setup if we are not attaching to an existing session.
   if (executeSetup)
      {
         if (starthelper->execute(startupHelper::op_setup) == startupHelper::or_setup_failture)
            {
               errorDialog::showAndDie(starthelper->getMessages());

               return BTG_ERROR_EXIT;
            }

         initialStatusMessage = "Established session.";
      }

   if (attachToSession)
      {
         std::string errorMessage;
         guihandler.reqSetupAttach(sessionToAttachTo);

         if (!guihandler.isAttachDone())
            {
               errorDialog::showAndDie("Unable to attach to session");
               BTG_FATAL_ERROR(logwrapper,
                               GPD->sGUI_CLIENT(), "Unable to attach to session");

               return BTG_ERROR_EXIT;
            }
      }

   // If the user requested to open any files, do it.
   if (cla.inputFilenamesPresent())
      {
         t_strList filelist = cla.getInputFilenames();
         t_strListCI iter;
         for (iter = filelist.begin(); iter != filelist.end(); iter++)
            {
               guihandler.reqCreate(*iter);
            }
      }

   if (cla.urlsPresent())
      {
         t_strList filelist = cla.getUrls();
         OpenUrls(guihandler, filelist);
      }

   // Done using the start up helper.
   starthelper.reset();

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_SETUP_DONE);

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_END);

   t_long session = guihandler.session();
   std::string str_session = btg::core::convertToString<t_long>(session);

   // Get some info about the current session, so it can be displayed
   // to the user.
   guihandler.reqSessionInfo();
   
   if (guihandler.dht())
      {
         str_session += " D";
      }

   if (guihandler.encryption())
      {
         str_session += " E";
      }

   // Start a thread that takes care of communicating with the daemon.
   handlerThread handlerthr(logwrapper,
                            verboseFlag,
                            guihandler);

   mainWindow mainWindow(logwrapper,
                         str_session, 
                         verboseFlag, 
                         neverAskFlag,
                         handlerthr,
                         dynconfig);

   BTG_NOTICE(logwrapper, initialStatusMessage);

   BTG_NOTICE(logwrapper, "Deleting the init window.");

   // Done using initWindow
   iw.reset();
   
   initMain.run(mainWindow);

   if (config.modified())
      {
         config.write();
      }

   return BTG_NORMAL_EXIT;
}

void OpenUrls(guiHandler & _handler, 
              t_strList const& _filelist)
{
   for (t_strListCI iter = _filelist.begin(); 
        iter != _filelist.end(); 
        iter++)
      {
         if (!btg::core::client::isUrlValid(*iter))
            {
               continue;
            }
         
         // Get a file name.
         std::string filename;

         if (!btg::core::client::getFilenameFromUrl(*iter, filename))
            {
               continue;
            }

         _handler.reqCreateFromUrl(filename, *iter);
         if (_handler.commandSuccess())
            {
               _handler.handleUrlProgress(_handler.UrlId());
            }
      }
}

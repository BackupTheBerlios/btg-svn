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
#include "guihelper.h"
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
   std::string appname("btgui");

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
                         appname, "Could not open file '" << config_filename << "'");

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
                         appname, "Could not read the config file, '" << GPD->sGUI_CONFIG() << "'. Create one.");

         return BTG_ERROR_EXIT;
      }

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_CLA_DONE);

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_TRANSP);

   // Create a transport to the daemon:
   std::auto_ptr<btg::core::externalization::Externalization> apExternalization;
   std::auto_ptr<messageTransport> apTransport;

   {
      btg::core::externalization::Externalization* externalization = 0;
      messageTransport* transport                                  = 0;
      
      transportHelper transporthelper(logwrapper,
                                      appname,
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
   }
   
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

   if (!starthelper->Init())
      {
         errorDialog::showAndDie(starthelper->getMessages());
         BTG_FATAL_ERROR(logwrapper,
                         appname, "Internal error: start up helper not initialized.");

         return BTG_ERROR_EXIT;
      }

   setDefaultLogLevel(logwrapper, cla.doDebug(), verboseFlag);

   // Initialize logging.
   if (!starthelper->SetupLogging())
      {
         BTG_FATAL_ERROR(logwrapper, appname, "Unable to initialize logging");

         return BTG_ERROR_EXIT;
      }

   bool attach      = cla.doAttach();
   bool attachFirst = cla.doAttachFirst();

   // Handle command line options:
   if (cla.doList())
      {
         starthelper->ListSessions();
         return BTG_NORMAL_EXIT;
      }
   else if (cla.doAttachFirst())
      {
         t_long sessionId;
         // Attach to the first available session.
         if (!starthelper->AttachFirstSession(sessionId))
            {
               errorDialog::showAndDie(starthelper->getMessages());
               BTG_FATAL_ERROR(logwrapper,
                               appname, "Unable to attach to session");

               return BTG_ERROR_EXIT;
            }

         initialStatusMessage = "Attached to session.";
      }
   else if (cla.doAttach())
      {
         // Attach to a certain session, either specified on the
         // command line or chosen by the user from a list.

         t_long sessionId;

         if (!starthelper->AttachSession(sessionId))
            {
               errorDialog::showAndDie(starthelper->getMessages());
               BTG_FATAL_ERROR(logwrapper,
                               appname, "Unable to attach to session");

               return BTG_ERROR_EXIT;
            }

         initialStatusMessage = "Attached to session.";
      }
   else 
      {
         // No options, show list of sessions or allow to create a new one.

         bool action_attach = false;
         t_long session;

         if (!starthelper->DefaultAction(action_attach, session))
            {
               BTG_FATAL_ERROR(logwrapper, appname, "Cancelled.");

               return BTG_ERROR_EXIT;
            }

         if (action_attach)
            {
               attach = action_attach;

               if (!starthelper->AttachToSession(session))
                  {
                     BTG_FATAL_ERROR(logwrapper, appname, "Unable to attach to session.");

                     return BTG_ERROR_EXIT;
                  }
               initialStatusMessage = "Attached to session.";
            }
      }

   // Only execute setup if we are not attaching to an existing session.
   if ((!attach) && (!attachFirst))
      {
         t_long sessionId;
         if (!starthelper->NewSession(sessionId))
            {
               BTG_FATAL_ERROR(logwrapper, appname, "Unable to create new session.");

               return BTG_ERROR_EXIT;
            }
         
         t_long session         = guihandler.session();
         std::string strSession = btg::core::convertToString<t_long>(session);
         initialStatusMessage   = "Established session #" + strSession + ".";
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

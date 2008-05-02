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

#include <bcore/project.h>
#include <bcore/util.h>
#include <bcore/client/carg.h>
#include <bcore/client/configuration.h>

#include <bcore/os/id.h>
#include <bcore/os/fileop.h>

#include <vector>
#include <bcore/status.h>
#include <bcore/trackerstatus.h>
#include <bcore/trackerstatus.h>
#include <bcore/t_string.h>

#include <bcore/crashlog.h>
#include <bcore/client/carg.h>

#include <bcore/client/handlerthr.h>
#include <bcore/client/loglevel.h>
#include <bcore/logable.h>

#include "nconfig.h"
#include "colors.h"
#include "keys.h"
#include "handler.h"
#include "initwindow.h"
#include "cli_helper.h"

extern "C"
{
#include <bcore/sigh.h>
}

using namespace btg::core;
using namespace btg::core::logger;
using namespace btg::core::client;
using namespace btg::UI::cli;

// Signal handler.
void global_signal_handler(int _signal_no);

// Main file for the client.
int main(int argc, char* argv[])
{
   btg::core::crashLog::init();
   projectDefaultsKiller PDK__;
   LogWrapperType logwrapper(new btg::core::logger::logWrapper);

   // UI stuff.
   keyMapping kmap;
   Colors     colors;

   // Parse command line arguments.
   commandLineArgumentHandler cla(GPD->sCLI_CONFIG(),
                                  false /* Extra option used to pass commands. */);
   cla.setup();

   if (!cla.parse(argc, argv))
      {
         return BTG_ERROR_EXIT;
      }

   // Before doing anything else, check if the user wants to get the
   // syntax of the configuration file.
   if (cla.listConfigFileSyntax())
      {
         ncliConfiguration non_existing(logwrapper, "non_existing");

         std::cout << non_existing.getSyntax() << std::endl;

         return BTG_NORMAL_EXIT;
      }

   bool verboseFlag  = cla.beVerbose();

   std::auto_ptr<initWindow> iw(new initWindow(kmap));
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

   std::string errorString;
   if (!btg::core::os::fileOperation::check(config_filename, errorString, false))
      {
         // BTG_FATAL_ERROR(GPD->sCLI_CLIENT(), "Could not open file '" << config_filename << "'.");

         iw->showError("Could not open file '" + config_filename + "'.");

         return BTG_ERROR_EXIT;
      }

   ncliConfiguration config(logwrapper, config_filename);
   bool const gotConfig = config.read();

   clientDynConfig dynconfig(logwrapper, GPD->sCLI_DYNCONFIG());

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_RCONF_DONE);

   if (!gotConfig)
      {
         // BTG_FATAL_ERROR(GPD->sCLI_CLIENT(), "Could not read the config file, '" << GPD->sCLI_CONFIG() << "'. Create one.");

         iw->showError("Could not read config file, '" + GPD->sCLI_CONFIG() + "'.");

         return BTG_ERROR_EXIT;
      }

   // Handle loading keys.
   iw->updateProgress(initWindow::IEV_RCONF_KEYS);

   for (t_uint keylabel = keyMapping::K_HELP;
        keylabel < keyMapping::K_RESIZE;
        keylabel++)
      {
         keyMapping::KEYLABEL kl = static_cast<keyMapping::KEYLABEL>(keylabel);
         t_int value             = keyMapping::K_UNDEF;

         if (config.getKey(kl, value))
            {
               kmap.add(kl, value);
            }
      }

   // Handle loading colors.
   std::vector<colorDef> colorLst;
   config.getColors(colorLst);

   colors.set(colorLst);

   // Set the default key to use.
   // This does not set keys previously set.
   kmap.setDefaults();

   std::string keyError;
   if (!kmap.check(keyError))
      {
         iw->showError(keyError);
       
         return BTG_ERROR_EXIT;
      }

   bool neverAskFlag = config.getNeverAskQuestions();

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_CLA_DONE);

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_TRANSP);

   std::auto_ptr<btg::core::externalization::Externalization> apExternalization;
   std::auto_ptr<messageTransport> apTransport;
   // Create a transport to the daemon:
   {
      btg::core::externalization::Externalization* externalization = 0;
      messageTransport* transport                                  = 0;

      transportHelper transporthelper(logwrapper,
                                      GPD->sCLI_CLIENT(),
                                      config,
                                      cla);

      if (!transporthelper.initTransport(externalization, transport))
         {
            delete transport;
            transport = 0;

            delete externalization;
            externalization = 0;

            iw->showError("Failed to initialize transport.");

            return BTG_ERROR_EXIT;
         }

      apExternalization.reset(externalization);
      apTransport.reset(transport);
   }

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_TRANSP_DONE);

   Handler handler(logwrapper,
                   *apExternalization, 
                   *apTransport, 
                   config, 
                   dynconfig, 
                   verboseFlag,
                   cla.automaticStart());

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_SETUP);

   // Create a helper to do the initial setup of this client.
   std::auto_ptr<startupHelper> starthelper(new cliStartupHelper(logwrapper,
                                                                 config,
                                                                 cla,
                                                                 *apTransport,
                                                                 handler));

   if (!starthelper->Init())
      {
         iw->showError("Internal error: startup helper not initialized.");

         return BTG_ERROR_EXIT;
      }

   setDefaultLogLevel(logwrapper, cla.doDebug(), verboseFlag);

   iw->show();
   iw->updateProgress(initWindow::IEV_SETUP);

   // Initialize logging.
   if (!starthelper->SetupLogging())
      {
         iw->showError("Unable to initialize logging.");

         return BTG_ERROR_EXIT;
      }


   bool attach      = cla.doAttach();
   bool attachFirst = cla.doAttachFirst();

   // Handle command line options:
   if (cla.doList())
      {
         iw->hide();
         starthelper->ListSessions();
         return BTG_NORMAL_EXIT;
      }
   else if (cla.doAttachFirst())
      {
         iw->hide();
         t_long sessionId;
         // Attach to the first available session.
         if (!starthelper->AttachFirstSession(sessionId))
            {
               iw->showError("Unable to attach to session.");

               return BTG_ERROR_EXIT;
            }
      }
   else if (cla.doAttach())
      {
         // Attach to a certain session, either specified on the
         // command line or chosen by the user from a list.

         iw->hide();

         t_long sessionId;

         if (!starthelper->AttachSession(sessionId))
            {
               iw->showError("Unable to attach to session.");
	       
               return BTG_ERROR_EXIT;
            }

         iw->show();
         iw->updateProgress(initWindow::IEV_SETUP);
      }
   else 
      {
         // No options, show list of sessions or allow to create a new one.
         iw->hide();

         bool action_attach = false;
         t_long session;

         if (!starthelper->DefaultAction(action_attach, session))
            {
               iw->showError("Cancelled.");

               return BTG_ERROR_EXIT;
            }

         if (action_attach)
            {
               attach = action_attach;

               if (!starthelper->AttachToSession(session))
                  {
                     iw->showError("Unable to attach to session.");
                     
                     return BTG_ERROR_EXIT;
                  }
            }
      }

   // Only execute setup if we are not attaching to an existing session.
   if ((!attach) && (!attachFirst))
      {
         t_long sessionId;
         if (!starthelper->NewSession(sessionId))
            {
               iw->showError("Unable to create new session.");

               return BTG_ERROR_EXIT;
            }
      }

#if defined(__APPLE__) && defined(__MACH__)

#else

   sigh_add(SIGINT, (void*)&global_signal_handler);
   sigh_add(SIGTERM, (void*)&global_signal_handler);
#endif

   // Done using the start up helper.
   starthelper.reset();

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_SETUP_DONE);

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_END);

   iw->hide();

   t_long session = handler.session();
   std::string strSession = btg::core::convertToString<t_long>(session);

   // Get some info about the current session, so it can be displayed
   // to the user.
   handler.reqSessionInfo();
   
   if (handler.dht())
      {
         strSession += " D";
      }

   if (handler.encryption())
      {
         strSession += " E";
      }

   handler.reqVersion();
   const btg::core::OptionBase & o = handler.getOption();

   // Start a thread that takes care of communicating with the daemon.
   handlerThread handlerthr(logwrapper, verboseFlag, handler);

   UI ui(logwrapper,
         strSession, 
         neverAskFlag, 
         o.getOption(btg::core::OptionBase::URL), 
         kmap, 
         colors, 
         handlerthr);

   // Init the ui.
   ui.init();

   // If the user requested to open any files, do it.
   if (cla.inputFilenamesPresent())
      {
         t_strList filelist = cla.getInputFilenames();
         ui.handleLoad(filelist);
      }

   // Open URLs.
   if (cla.urlsPresent())
      {
         t_strList filelist = cla.getUrls();
         ui.handleUrl(filelist);
      }


   ui.refresh();

   // Main event loop.
   while (ui.handleKeyboard());

   ui.destroy();

   if (config.modified())
      {
         config.write();
      }

   return BTG_NORMAL_EXIT;
}

void global_signal_handler(int _signal_no)
{
   switch (_signal_no)
      {
      case SIGINT:
      case SIGTERM:
         {
            break;
         }
      }
}

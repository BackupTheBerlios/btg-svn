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
#include <bcore/client/lastfiles.h>

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
   LogWrapperType logwrapper(new btg::core::logger::logWrapper);

   btg::core::crashLog::init();

   // UI stuff.
   keyMapping kmap;
   Colors     colors;

   // Parse command line arguments.
   commandLineArgumentHandler* cla = new commandLineArgumentHandler(GPD->sCLI_CONFIG(),
                                                                    false /* Extra option used to pass commands. */);
   cla->setup();

   if (!cla->parse(argc, argv))
      {
         delete cla;
         cla = 0;

         projectDefaults::killInstance();

         return BTG_ERROR_EXIT;
      }

   // Before doing anything else, check if the user wants to get the
   // syntax of the configuration file.
   if (cla->listConfigFileSyntax())
      {
         ncliConfiguration non_existing(logwrapper, "non_existing");

         std::cout << non_existing.getSyntax() << std::endl;

         delete cla;
         cla = 0;

         projectDefaults::killInstance();

         return BTG_NORMAL_EXIT;
      }

   bool verboseFlag  = cla->beVerbose();

   std::auto_ptr<initWindow> iw(new initWindow(kmap));
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
         // BTG_FATAL_ERROR(GPD->sCLI_CLIENT(), "Could not open file '" << config_filename << "'.");

         delete cla;
         cla = 0;

         iw->showError("Could not open file '" + config_filename + "'.");

         projectDefaults::killInstance();

         return BTG_ERROR_EXIT;
      }

   ncliConfiguration* config = new ncliConfiguration(logwrapper, config_filename);
   bool const gotConfig = config->read();

   clientDynConfig cliDynConf(logwrapper, GPD->sCLI_DYNCONFIG());
   lastFiles* lastfiles = new lastFiles(logwrapper, cliDynConf);

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_RCONF_DONE);

   if (!gotConfig)
      {
         // BTG_FATAL_ERROR(GPD->sCLI_CLIENT(), "Could not read the config file, '" << GPD->sCLI_CONFIG() << "'. Create one.");

         delete config;
         config = 0;

         delete lastfiles;
         lastfiles = 0;

         delete cla;
         cla    = 0;

         iw->showError("Could not read config file, '" + GPD->sCLI_CONFIG() + "'.");

         projectDefaults::killInstance();

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

         if (config->getKey(kl, value))
            {
               kmap.add(kl, value);
            }
      }

   // Handle loading colors.
   std::vector<colorDef> colorLst;
   config->getColors(colorLst);

   colors.set(colorLst);

   // Set the default key to use.
   // This does not set keys previously set.
   kmap.setDefaults();

   std::string keyError;
   if (!kmap.check(keyError))
      {
         delete config;
         config = 0;
       
         delete lastfiles;
         lastfiles = 0;
       
         delete cla;
         cla    = 0;
       
         iw->showError(keyError);
       
         projectDefaults::killInstance();
       
         return BTG_ERROR_EXIT;
      }

   bool neverAskFlag = config->getNeverAskQuestions();

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_CLA_DONE);

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_TRANSP);

   // Create a transport to the daemon:
   btg::core::externalization::Externalization* externalization = 0;
   messageTransport* transport                                  = 0;

   transportHelper* transporthelper = new transportHelper(logwrapper,
                                                          GPD->sCLI_CLIENT(),
                                                          config,
                                                          cla);

   if (!transporthelper->initTransport(externalization, transport))
      {
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

         iw->showError("Failed to initialize transport.");

         projectDefaults::killInstance();

         return BTG_ERROR_EXIT;
      }

   delete transporthelper;
   transporthelper = 0;

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_TRANSP_DONE);

   Handler* handler = new Handler(logwrapper,
                                  externalization, 
                                  transport, 
                                  config, 
                                  lastfiles, 
                                  verboseFlag,
                                  cla->automaticStart()
                                  );

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_SETUP);

   // Create a helper to do the initial setup of this client.
   startupHelper* starthelper = new ncliStartupHelper(logwrapper,
                                                      config,
                                                      cla,
                                                      transport,
                                                      handler);

   if (!starthelper->init())
      {
         // BTG_FATAL_ERROR(GPD->sCLI_CLIENT(), "Internal error: start up helper not initialized.");

         delete starthelper;
         starthelper = 0;

         delete handler;
         handler = 0;

         delete externalization;
         externalization = 0;

         delete cla;
         cla = 0;

         delete config;
         config = 0;

         delete lastfiles;
         lastfiles = 0;

         iw->showError("Internal error: startup helper not initialized.");

         projectDefaults::killInstance();

         return BTG_ERROR_EXIT;
      }

   setDefaultLogLevel(logwrapper, cla->doDebug(), verboseFlag);

   // Initialize logging.
   if (starthelper->execute(startupHelper::op_log) != startupHelper::or_log_success)
      {
         // BTG_FATAL_ERROR(GPD->sCLI_CLIENT(), "Unable to initialize logging");
         delete starthelper;
         starthelper = 0;

         delete handler;
         handler = 0;

         delete externalization;
         externalization = 0;

         delete cla;
         cla = 0;

         delete config;
         config = 0;

         delete lastfiles;
         lastfiles = 0;

         iw->showError("Unable to initialize logging.");

         projectDefaults::killInstance();

         return BTG_ERROR_EXIT;
      }

   // In case authorization is required from the user, hide window
   iw->hide();

   if (config->authSet())
      {
         // Auth info is in the config.
         starthelper->setUser(config->getUserName());
         starthelper->setPasswordHash(config->getPasswordHash());
      }
   else
      {
         AUTH_RETRY:
         // Ask the user about which username and password to use.
         if (starthelper->execute(startupHelper::op_auth) != startupHelper::or_auth_success)
            {
               // BTG_FATAL_ERROR(GPD->sCLI_CLIENT(), "Unable to initialize auth");

               delete starthelper;
               starthelper = 0;

               delete handler;
               handler = 0;

               delete externalization;
               externalization = 0;

               delete cla;
               cla = 0;

               delete config;
               config = 0;

               delete lastfiles;
               lastfiles = 0;

               // Show the init window to display the error.
               iw->show();
               iw->showError("Authorization failed.");

               projectDefaults::killInstance();

               return BTG_ERROR_EXIT;
            }
      }

   iw->show();
   iw->updateProgress(initWindow::IEV_SETUP);

   /// Initialize the transport.
   if (starthelper->execute(startupHelper::op_init) != startupHelper::or_init_success)
   {
      iw->hide();
      std::cout << "Authentication error." << std::endl;
      goto AUTH_RETRY;
   }

   // Handle command line options:
   if (cla->doList())
      {
         iw->showError("Unable to list sessions.");

         starthelper->execute(startupHelper::op_list);

         // Clean up, before quitting.
         delete starthelper;
         starthelper = 0;

         delete handler;
         handler = 0;

         delete externalization;
         externalization = 0;

         delete cla;
         cla = 0;

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
               // BTG_FATAL_ERROR(GPD->sCLI_CLIENT(), "Unable to attach to session");

               // Clean up, before quitting.
               delete starthelper;
               starthelper = 0;

               delete handler;
               handler = 0;

               delete externalization;
               externalization = 0;

               delete cla;
               cla = 0;

               delete config;
               config = 0;

               delete lastfiles;
               lastfiles = 0;

               iw->showError("Unable to attach to session.");

               projectDefaults::killInstance();

               return BTG_ERROR_EXIT;
            }
      }
   else if (cla->doAttach())
      {
         // Attach to a certain session, either specified on the
         // command line or chosen by the user from a list.

         iw->hide();

         if (starthelper->execute(startupHelper::op_attach) == startupHelper::or_attach_failture)
            {
               // BTG_FATAL_ERROR(GPD->sCLI_CLIENT(), "Unable to attach to session");

               // Clean up, before quitting.
               delete starthelper;
               starthelper = 0;

               delete handler;
               handler = 0;

               delete externalization;
               externalization = 0;

               delete cla;
               cla = 0;

               delete config;
               config = 0;

               delete lastfiles;
               lastfiles = 0;

               iw->show();
               iw->showError("Unable to attach to session.");
	       
               projectDefaults::killInstance();

               return BTG_ERROR_EXIT;
            }

         iw->show();
         iw->updateProgress(initWindow::IEV_SETUP);
      }

   // Only execute setup if we are not attaching to an existing session.
   if ((!cla->doAttach()) && (!cla->doAttachFirst()))
      {
         if (starthelper->execute(startupHelper::op_setup) == startupHelper::or_setup_failture)
            {
               // BTG_FATAL_ERROR(GPD->sCLI_CLIENT(), "Unable to connect to daemon.");

               // Clean up, before quitting.
               delete starthelper;
               starthelper = 0;

               delete handler;
               handler = 0;

               delete externalization;
               externalization = 0;

               delete cla;
               cla = 0;

               delete config;
               config = 0;

               delete lastfiles;
               lastfiles = 0;

               iw->showError("Unable to connect to daemon.");

               projectDefaults::killInstance();
               
               return BTG_ERROR_EXIT;
            }
      }

#if defined(__APPLE__) && defined(__MACH__)

#else

   sigh_add(SIGINT, (void*)&global_signal_handler);
   sigh_add(SIGTERM, (void*)&global_signal_handler);
#endif

   // Done using the start up helper.
   delete starthelper;
   starthelper = 0;

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_SETUP_DONE);

   // Update init dialog.
   iw->updateProgress(initWindow::IEV_END);

   iw->hide();

   t_long session = handler->session();
   std::string strSession = btg::core::convertToString<t_long>(session);

   // Get some info about the current session, so it can be displayed
   // to the user.
   handler->reqSessionInfo();
   
   if (handler->dht())
      {
         strSession += " D";
      }

   if (handler->encryption())
      {
         strSession += " E";
      }

   handler->reqVersion();
   const btg::core::OptionBase & o = handler->getOption();

   // Start a thread that takes care of communicating with the daemon.
   handlerThread* handlerthr = new handlerThread(logwrapper, verboseFlag, handler);

   UI ui(strSession, neverAskFlag, kmap, colors, handlerthr);

   // Init the ui.
   ui.init();

   // If the user requested to open any files, do it.
   if (cla->inputFilenamesPresent())
      {
         t_strList filelist = cla->getInputFilenames();
         ui.handleLoad(filelist);
      }

   // Open URLs.
   if (cla->urlsPresent())
      {
         t_strList filelist = cla->getUrls();
         ui.handleUrl(filelist);
      }


   ui.refresh();

   // Main event loop.
   while (ui.handleKeyboard());

   ui.destroy();

   // UI finished.
   delete handlerthr;
   handlerthr = 0;

   if (config->modified())
      {
         config->write();
      }

   delete config;
   config = 0;

   delete lastfiles;
   lastfiles = 0;

   delete handler;
   handler = 0;

   delete externalization;
   externalization = 0;

   delete cla;
   cla = 0;

   projectDefaults::killInstance();

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

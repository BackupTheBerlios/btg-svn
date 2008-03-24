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

#include "btgvs.h"
#include "ui.h"
#include "arg.h"

#include <bcore/project.h>
#include <bcore/type.h>
#include <bcore/util.h>
#include <bcore/logable.h>
#include <bcore/externalization/xmlrpc.h>
#include <bcore/os/id.h>
#include <bcore/os/fileop.h>
#include <bcore/logger/logger.h>
#include <bcore/logger/file_log.h>
#include <bcore/t_string.h>
#include <bcore/crashlog.h>
#include <bcore/client/handlerthr.h>
#include <bcore/client/loglevel.h>
#include <bcore/verbose.h>

#include <iostream>

using namespace btg::core;
using namespace btg::core::logger;
using namespace btg::core::client;
using namespace btg::UI::gui::viewer;

clientData clientdata;

void quitapp()
{
   if (!clientdata.handler)
      {
         return;
      }

   // Detach from session.
   clientdata.handler->reqDetach();

   // destroyGui(gui);

   delete clientdata.handlerthr;
   clientdata.handlerthr = 0;

   // By deleting the gui handler, the transport used is also deleted.
   delete clientdata.handler;
   clientdata.handler = 0;

   delete clientdata.externalization;
   clientdata.externalization = 0;

   delete clientdata.config;
   clientdata.config = 0;

   delete clientdata.lastfiles;
   clientdata.lastfiles = 0;

   projectDefaults::killInstance();

   AG_Quit();
}

int main(int argc, char **argv)
{
   LogWrapperType logwrapper(new btg::core::logger::logWrapper);

   btg::core::crashLog::init();

   std::string clientName("btgvs");

   vsCommandLineArgumentHandler* cla = new vsCommandLineArgumentHandler(GPD->sGUI_CONFIG());

   cla->setup();

   // Parse command line arguments.
   if (!cla->parse(argc, argv))
      {
         delete cla;
         cla    = 0;

         projectDefaults::killInstance();

         return BTG_ERROR_EXIT;
      }

   bool fullscreen   = cla->fullscreen();
   bool res1440x900  = cla->res1440x900();
   bool res1024x768  = cla->res1024x768();
   
   t_uint updateFreq = cla->getUpdateFreq();

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

   // Open the configuration file:
   std::string config_filename = GPD->sCLI_CONFIG();

   if (cla->configFileSet())
      {
         config_filename = cla->configFile();
      }

   VERBOSE_LOG(logwrapper, verboseFlag, "config: '" << config_filename << "'.");
   
   std::string errorString;
   if (!btg::core::os::fileOperation::check(config_filename, errorString, false))
      {
         BTG_FATAL_ERROR(logwrapper, 
                         clientName, "Could not open file '" << config_filename << "'");

         return BTG_ERROR_EXIT;
      }

   // Open the configuration file:
   clientdata.config = new clientConfiguration(logwrapper, config_filename);

   bool const gotConfig = clientdata.config->read();

   clientDynConfig cliDynConf(logwrapper, GPD->sCLI_DYNCONFIG());
   clientdata.lastfiles = new lastFiles(logwrapper, cliDynConf);

   if (!gotConfig)
      {
         BTG_FATAL_ERROR(logwrapper, 
                         clientName, "Could not read the config file, '" << GPD->sGUI_CONFIG() << "'. Create one.");
         delete clientdata.config;
         clientdata.config = 0;

         delete clientdata.lastfiles;
         clientdata.lastfiles = 0;

         delete cla;
         cla    = 0;

         projectDefaults::killInstance();

         return BTG_ERROR_EXIT;
      }

   // Create a transport to the daemon:
   clientdata.externalization  = 0;
   messageTransport* transport = 0;

   transportHelper* transporthelper = new transportHelper(logwrapper,
                                                          "btgvs",
                                                          clientdata.config,
                                                          cla);

   if (!transporthelper->initTransport(clientdata.externalization, transport))
      {
         BTG_FATAL_ERROR(logwrapper,
                         clientName, transporthelper->getMessages());

         delete clientdata.config;
         clientdata.config = 0;

         delete clientdata.lastfiles;
         clientdata.lastfiles = 0;

         delete cla;
         cla    = 0;

         delete transport;
         transport = 0;

         delete clientdata.externalization;
         clientdata.externalization = 0;

         delete transporthelper;
         transporthelper = 0;

         projectDefaults::killInstance();

         return BTG_ERROR_EXIT;
      }

   delete transporthelper;
   transporthelper = 0;

   btgvsGui gui;

   clientdata.handler = new viewerHandler(logwrapper,
                                          clientdata.externalization,
                                          transport,
                                          clientdata.config,
                                          clientdata.lastfiles,
                                          verboseFlag,
                                          cla->automaticStart(),
                                          &gui);

   std::string initialStatusMessage("");

   // Create a helper to do the initial setup of this client.
   startupHelper* starthelper = new viewerStartupHelper(logwrapper,
                                                        clientdata.config,
                                                        cla,
                                                        transport,
                                                        clientdata.handler);

   if (!starthelper->init())
      {

         BTG_FATAL_ERROR(logwrapper,
                         clientName, "Internal error: start up helper not initialized.");

         delete starthelper;
         starthelper = 0;

         delete clientdata.handler;
         clientdata.handler = 0;

         delete clientdata.externalization;
         clientdata.externalization = 0;

         delete cla;
         cla = 0;

         delete clientdata.config;
         clientdata.config = 0;

         delete clientdata.lastfiles;
         clientdata.lastfiles = 0;

         projectDefaults::killInstance();

         return BTG_ERROR_EXIT;
      }

   setDefaultLogLevel(logwrapper, cla->doDebug(), verboseFlag);

   // Initialize logging.
   if (starthelper->execute(startupHelper::op_log) != startupHelper::or_log_success)
      {
         BTG_FATAL_ERROR(logwrapper,
                         clientName, "Unable to initialize logging");
         delete starthelper;
         starthelper = 0;

         delete clientdata.handler;
         clientdata.handler = 0;

         delete clientdata.externalization;
         clientdata.externalization = 0;

         delete cla;
         cla = 0;

         delete clientdata.config;
         clientdata.config = 0;

         delete clientdata.lastfiles;
         clientdata.lastfiles = 0;

         projectDefaults::killInstance();

         return BTG_ERROR_EXIT;
      }

   if (!clientdata.config->authSet())
      {
         BTG_FATAL_ERROR(logwrapper,
                         clientName, "No auth info in client config file.");

         delete starthelper;
         starthelper = 0;
         
         delete clientdata.handler;
         clientdata.handler = 0;
         
         delete clientdata.externalization;
         clientdata.externalization = 0;
         
         delete cla;
         cla = 0;
         
         delete clientdata.config;
         clientdata.config = 0;

         delete clientdata.lastfiles;
         clientdata.lastfiles = 0;
         
         projectDefaults::killInstance();
         
         return BTG_ERROR_EXIT;
      }

   // Auth info is in the config.
   starthelper->setUser(clientdata.config->getUserName());
   starthelper->setPasswordHash(clientdata.config->getPasswordHash());

   /// Initialize the transport
   starthelper->execute(startupHelper::op_init);

   bool attached = false;

   // Handle command line options:
   if (cla->doAttachFirst())
      {
         // Attach to the first available session.

         if (starthelper->execute(startupHelper::op_attach_first) == 
             startupHelper::or_attach_first_failture)
            {
               BTG_FATAL_ERROR(logwrapper,
                               clientName, "Unable to attach to session");

               // Clean up, before quitting.
               delete starthelper;
               starthelper = 0;

               delete clientdata.handler;
               clientdata.handler = 0;

               delete cla;
               cla = 0;

               delete clientdata.externalization;
               clientdata.externalization = 0;

               delete clientdata.config;
               clientdata.config = 0;

               delete clientdata.lastfiles;
               clientdata.lastfiles = 0;

               projectDefaults::killInstance();

               return BTG_ERROR_EXIT;
            }

         initialStatusMessage = "Attached to session.";
         attached = true;
      }
   else if (cla->doAttach())
      {
         // Attach to a certain session, either specified on the
         // command line or chosen by the user from a list.

         startupHelper::operationResult result = starthelper->execute(startupHelper::op_attach);

         if (result == startupHelper::or_attach_failture)
            {
               BTG_FATAL_ERROR(logwrapper,
                               clientName, "Unable to attach to session");
            }
         
         if ((result == startupHelper::or_attach_failture) || 
             (result == startupHelper::or_attach_cancelled))
            {
               // Clean up, before quitting.
               delete starthelper;
               starthelper = 0;
	     
               delete clientdata.handler;
               clientdata.handler = 0;
	     
               delete clientdata.externalization;
               clientdata.externalization = 0;
	     
               delete cla;
               cla = 0;
	     
               delete clientdata.config;
               clientdata.config = 0;
	     
               delete clientdata.lastfiles;
               clientdata.lastfiles = 0;
	     
               projectDefaults::killInstance();

               return BTG_ERROR_EXIT;
            }

         initialStatusMessage = "Attached to session.";
         attached = true;
      }

   // Attempt to the first session even if no arguments were given.
   if (!attached)
      {
         // Attach to the first available session.
         if (starthelper->execute(startupHelper::op_attach_first) == 
             startupHelper::or_attach_first_failture)
            {
               BTG_FATAL_ERROR(logwrapper,
                               clientName, "Unable to attach to session");

               // Clean up, before quitting.
               delete starthelper;
               starthelper = 0;

               delete clientdata.handler;
               clientdata.handler = 0;

               delete cla;
               cla = 0;

               delete clientdata.externalization;
               clientdata.externalization = 0;

               delete clientdata.config;
               clientdata.config = 0;

               delete clientdata.lastfiles;
               clientdata.lastfiles = 0;

               projectDefaults::killInstance();

               return BTG_ERROR_EXIT;
            }

         initialStatusMessage = "Attached to session.";
      }

   // Done using arguments.
   delete cla;
   cla = 0;

   // Done using the start up helper.
   delete starthelper;
   starthelper = 0;

   t_long session = clientdata.handler->session();

   std::string str_session = "BTGVS";

   str_session += " (session #";
   str_session += btg::core::convertToString<t_long>(session);

   // Get some info about the current session, so it can be displayed
   // to the user.
   clientdata.handler->reqSessionInfo();
   
   if (clientdata.handler->dht())
      {
         str_session += " D";
      }

   if (clientdata.handler->encryption())
      {
         str_session += " E";
      }

   str_session += ")";

   // Start a thread that takes care of communicating with the daemon.
   // clientdata.handlerthr = new handlerThread(verboseFlag, clientdata.handler);
   clientdata.handlerthr = 0;

   BTG_NOTICE(logwrapper, initialStatusMessage);

   //
   // 
   // BTG initialized, start UI.
   // 
   //  

	// Initialize Agar.
	if (AG_InitCore(str_session.c_str(), 0) == -1)
      {
         std::cerr << "Unable to initialize agar subsystem." << std::endl;
         return -1;
      }

   t_uint af_flags = AG_VIDEO_DOUBLEBUF | AG_VIDEO_RESIZABLE;
   if (fullscreen)
      {
         af_flags |= AG_VIDEO_FULLSCREEN;
      }

   t_uint x = 800;
   t_uint y = 600;

   if (res1440x900)
      {
         x = 1440;
         y = 900;
      }

   if (res1024x768)
      {
         x = 1024;
         y = 768;
      }

   // AG_VIDEO_FULLSCREEN.
   if (AG_InitVideo(x, y, 32, af_flags) == -1) 
      {
         std::cerr << "Unable to initialize agar gfx subsystem." << std::endl;
         return -1;
      }

   // btgvsGui gui;
   createGui(gui);

   // Create a timer which will refresh the UI.
   timerData timerdata;
   timerdata.count      = 0;
   timerdata.gui        = &gui;
   timerdata.handlerthr = clientdata.handlerthr;
   timerdata.handler    = clientdata.handler;

   gui.updateDelay = updateFreq;

   // Run an update the first time.
   update_ui(&timerdata);

   // Create a timer for future updates.
   createTimer(gui, &timerdata);

   // Bind ESC to a quit function which will tell the daemon that this
   // client is detaching and clean up.
   AG_BindGlobalKey(SDLK_ESCAPE, KMOD_NONE, quitapp);

   // This will block until user presses ESC.
   run();

   quitapp();

   return BTG_NORMAL_EXIT;
}



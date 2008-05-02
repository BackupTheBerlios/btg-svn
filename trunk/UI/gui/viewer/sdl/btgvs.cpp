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
#include "arg.h"
#include "handler.h"

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

int main(int argc, char **argv)
{
   btg::core::crashLog::init();
   projectDefaultsKiller PDK__;
   LogWrapperType logwrapper(new btg::core::logger::logWrapper);

   std::string clientName("btgvs");

   vsCommandLineArgumentHandler cla(GPD->sGUI_CONFIG());

   cla.setup();

   // Parse command line arguments.
   if (!cla.parse(argc, argv))
      {
         return BTG_ERROR_EXIT;
      }

   bool fullscreen   = cla.fullscreen();
   bool res1440x900  = cla.res1440x900();
   bool res1024x768  = cla.res1024x768();
   
   t_uint updateFreq = cla.getUpdateFreq();

   // Before doing anything else, check if the user wants to get the
   // syntax of the configuration file.
   if (cla.listConfigFileSyntax())
      {
         clientConfiguration non_existing(logwrapper, "non_existing");
         
         std::cout << non_existing.getSyntax() << std::endl;

         return BTG_NORMAL_EXIT;
      }

   bool verboseFlag = cla.beVerbose();

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
                         clientName, "Could not open file '" << config_filename << "'");
         return BTG_ERROR_EXIT;
      }

   clientConfiguration config(logwrapper, config_filename);
   clientDynConfig dynconfig(logwrapper, GPD->sCLI_DYNCONFIG());

   if (!config.read())
      {
         BTG_FATAL_ERROR(logwrapper, 
                         clientName, "Could not read the config file, '" << GPD->sGUI_CONFIG() << "'. Create one.");
         return BTG_ERROR_EXIT;
      }

   // Create a transport to the daemon:
   std::auto_ptr<btg::core::externalization::Externalization> apExternalization;
   std::auto_ptr<messageTransport> apTransport;
   {
      btg::core::externalization::Externalization*  externalization = 0;
      messageTransport* transport                                   = 0;

      transportHelper transporthelper(logwrapper,
                                      "btgvs",
                                      config,
                                      cla);

      if (!transporthelper.initTransport(externalization, transport))
         {
            BTG_FATAL_ERROR(logwrapper, clientName, transporthelper.getMessages());

            delete transport;
            transport = 0;

            delete externalization;
            externalization = 0;

            return BTG_ERROR_EXIT;
         }
      
      apExternalization.reset(externalization);
      apTransport.reset(transport);
   }

   btgvsGui gui;

   viewerHandler handler(logwrapper,
                         *apExternalization,
                         *apTransport,
                         config,
                         dynconfig,
                         verboseFlag,
                         cla.automaticStart(),
                         gui);

   // Create a helper to do the initial setup of this client.
   std::auto_ptr<startupHelper> starthelper(new viewerStartupHelper(logwrapper,
                                                                    config,
                                                                    cla,
                                                                    *apTransport,
                                                                    handler));
   if (!starthelper->Init())
      {
         BTG_FATAL_ERROR(logwrapper, clientName, "Internal error: start up helper not initialized.");

         return BTG_ERROR_EXIT;
      }

   setDefaultLogLevel(logwrapper, cla.doDebug(), verboseFlag);

   // Initialize logging.
   if (!starthelper->SetupLogging())
      {
         BTG_FATAL_ERROR(logwrapper, clientName, "Unable to initialize logging");

         return BTG_ERROR_EXIT;
      }

   // Handle command line options:
   if (cla.doAttachFirst())
      {
         t_long sessionId;
         // Attach to the first available session.
         if (!starthelper->AttachFirstSession(sessionId))
            {
               BTG_FATAL_ERROR(logwrapper, clientName, "Unable to attach to session");

               return BTG_ERROR_EXIT;
            }
      }
   else if (cla.doAttach())
      {
         // Attach to a certain session, either specified on the
         // command line or chosen by the user from a list.

         t_long sessionId;

         if (!starthelper->AttachSession(sessionId))
            {
               BTG_FATAL_ERROR(logwrapper, clientName, "Unable to attach to session");

               return BTG_ERROR_EXIT;
            }
      }
   else 
      {
         // No options, show list of sessions or allow to create a new one.

         t_long sessionId;
         // Attach to the first available session.
         if (!starthelper->AttachFirstSession(sessionId))
            {
               BTG_FATAL_ERROR(logwrapper, clientName, "Unable to attach to session");

               return BTG_ERROR_EXIT;
            }
      }

   // Done using the start up helper.
   starthelper.reset();

   t_long session = handler.session();

   std::string str_session = "BTGVS";

   str_session += " (session #";
   str_session += btg::core::convertToString<t_long>(session);

   // Get some info about the current session, so it can be displayed
   // to the user.
   handler.reqSessionInfo();
   
   if (handler.dht())
      {
         str_session += " D";
      }

   if (handler.encryption())
      {
         str_session += " E";
      }

   str_session += ")";

   // Start a thread that takes care of communicating with the daemon.
   //handlerThread handlerthr(verboseFlag, clientdata.handler);

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
   timerdata.handlerthr = 0; //&handlerthr;
   timerdata.handler    = &handler;

   gui.updateDelay = updateFreq;

   // Run an update the first time.
   update_ui(&timerdata);

   // Create a timer for future updates.
   createTimer(gui, &timerdata);

   // Bind ESC to a quit function which will tell the daemon that this
   // client is detaching and clean up.
   AG_BindGlobalKey(SDLK_ESCAPE, KMOD_NONE, AG_Quit);

   // This will block until user presses ESC.
   run();

   handler.reqDetach();
   AG_Quit();

   return BTG_NORMAL_EXIT;
}

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
#include <bcore/util.h>
#include <bcore/client/carg.h>
#include <bcore/client/configuration.h>
#include <bcore/client/lastfiles.h>
#include <bcore/client/loglevel.h>

#include <bcore/os/id.h>
#include <bcore/os/fileop.h>

#include <bcore/t_string.h>

#include <bcore/verbose.h>

#include "nscreen.h"
#include "nscreen_log.h"

extern "C"
{
#include <bcore/sigh.h>

#include "runstate.h"

t_int global_btg_run = GR_RUN;

/* React on the signals from the outside world. */
void global_signal_handler(int _signal_no)
   {
      if (_signal_no == SIGINT || _signal_no == SIGTERM)
         {
            global_btg_run = GR_QUIT;
         }
   }
}

#include <iostream>

#include "cli.h"

#include <bcore/client/helper.h>
#include <bcore/client/urlhelper.h>
#include <bcore/logger/file_log.h>

#include <bcore/logable.h>
#include <bcore/crashlog.h>
#include <UI/cli/ncurses/cli_helper.h>

using namespace btg::core;
using namespace btg::core::logger;
using namespace btg::core::client;
using namespace btg::UI::cli;

void handleInput(std::string const& _line, 
                 cliHandler& _clihandler, 
                 Screen& _nscr, 
                 bool const _neverAskFlag);

void OpenUrls(Screen& _nscr, 
              cliHandler& _clihandler, 
              t_strList const& _filelist);

void OpenFiles(Screen& _nscr, 
               cliHandler& _clihandler, 
               lastFiles& _lastfiles,
               t_strList const& _filelist);

// Main file for the client.
int main(int argc, char* argv[])
{
   btg::core::crashLog::init();
   LogWrapperType logwrapper(new btg::core::logger::logWrapper);

   // Parse command line arguments.
   commandLineArgumentHandler cla(btg::core::projectDefaults::sCLI_CONFIG(),
                                  true /* Extra option used to pass commands. */);
   cla.setup();

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

   bool verboseFlag  = cla.beVerbose();

   // Open the configuration file:
   std::string config_filename = btg::core::projectDefaults::sCLI_CONFIG();

   if (cla.configFileSet())
      {
         config_filename = cla.configFile();
      }

   VERBOSE_LOG(logwrapper, verboseFlag, "config: '" << config_filename << "'.");

   std::string errorString;
   if (!btg::core::os::fileOperation::check(config_filename, errorString, false))
      {
         BTG_FATAL_ERROR(logwrapper, btg::core::projectDefaults::sCLI_CLIENT(), "Could not open file '" << config_filename << "'.");

         return BTG_ERROR_EXIT;
      }

   clientConfiguration config(logwrapper, config_filename);
   clientDynConfig dynconfig(logwrapper, btg::core::projectDefaults::sCLI_DYNCONFIG());

   if (!config.read())
      {
         BTG_FATAL_ERROR(logwrapper, btg::core::projectDefaults::sCLI_CLIENT(), "Could not read the config file, '" << btg::core::projectDefaults::sCLI_CONFIG() << "'. Create one.");

         return BTG_ERROR_EXIT;
      }

   bool neverAskFlag = config.getNeverAskQuestions();

   std::auto_ptr<btg::core::externalization::Externalization> apExternalization;
   std::auto_ptr<messageTransport> apTransport;
   // Create a transport to the daemon:
   {
      btg::core::externalization::Externalization* externalization = 0;
      messageTransport* transport                                  = 0;

      transportHelper transporthelper(logwrapper,
                                      btg::core::projectDefaults::sCLI_CLIENT(),
                                      config,
                                      cla);

      if (!transporthelper.initTransport(externalization, transport))
         {
            delete transport;
            transport = 0;

            delete externalization;
            externalization = 0;

            return BTG_ERROR_EXIT;
         }
      
      apExternalization.reset(externalization);
      apTransport.reset(transport);
   }

   cliHandler clihandler(logwrapper,
                         *apExternalization,
                         *apTransport, 
                         config, 
                         dynconfig, 
                         verboseFlag, 
                         cla.automaticStart());

   std::string initialStatusMessage("");

   // Create a helper to do the initial setup of this client.
   std::auto_ptr<startupHelper> starthelper(new cliStartupHelper(logwrapper,
                                                                 config,
                                                                 cla,
                                                                 *apTransport,
                                                                 clihandler));

   if (!starthelper->Init())
      {
         BTG_FATAL_ERROR(logwrapper, btg::core::projectDefaults::sCLI_CLIENT(), "Internal error: start up helper not initialized.");

         return BTG_ERROR_EXIT;
      }

   setDefaultLogLevel(logwrapper, cla.doDebug(), verboseFlag);

   // Initialize logging.
   if (!starthelper->SetupLogging())
      {
         BTG_FATAL_ERROR(logwrapper, btg::core::projectDefaults::sCLI_CLIENT(), "Unable to initialize logging");

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
               BTG_FATAL_ERROR(logwrapper, btg::core::projectDefaults::sCLI_CLIENT(), "Unable to attach to session");

               return BTG_ERROR_EXIT;
            }

         t_long session         = clihandler.session();
         std::string strSession = btg::core::convertToString<t_long>(session);
         initialStatusMessage   = "Attached to session #" + strSession + ".";
      }
   else if (cla.doAttach())
      {
         // Attach to a certain session, either specified on the
         // command line or chosen by the user from a list.

         t_long sessionId;

         if (!starthelper->AttachSession(sessionId))
            {
               BTG_FATAL_ERROR(logwrapper, btg::core::projectDefaults::sCLI_CLIENT(), "Unable to attach to session");

               return BTG_ERROR_EXIT;
            }

         t_long session         = clihandler.session();
         std::string strSession = btg::core::convertToString<t_long>(session);
         initialStatusMessage   = "Attached to session #" + strSession + ".";
      }
   else 
      {
         // No options, show list of sessions or allow to create a new one.

         bool action_attach = false;
         t_long session;

         if (!starthelper->DefaultAction(action_attach, session))
            {
               BTG_FATAL_ERROR(logwrapper, btg::core::projectDefaults::sCLI_CLIENT(), "Cancelled.");

               return BTG_ERROR_EXIT;
            }

         if (action_attach)
            {
               attach = action_attach;

               if (!starthelper->AttachToSession(session))
                  {
                     BTG_FATAL_ERROR(logwrapper, btg::core::projectDefaults::sCLI_CLIENT(), "Unable to attach to session.");

                     return BTG_ERROR_EXIT;
                  }
               t_long session         = clihandler.session();
               std::string strSession = btg::core::convertToString<t_long>(session);
               initialStatusMessage   = "Attached to session #" + strSession + ".";
            }
      }

   // Only execute setup if we are not attaching to an existing session.
   if ((!attach) && (!attachFirst))
      {
         t_long sessionId;
         if (!starthelper->NewSession(sessionId))
            {
               BTG_FATAL_ERROR(logwrapper, btg::core::projectDefaults::sCLI_CLIENT(), "Unable to create new session.");

               return BTG_ERROR_EXIT;
            }
         
         t_long session         = clihandler.session();
         std::string strSession = btg::core::convertToString<t_long>(session);
         initialStatusMessage   = "Established session #" + strSession + ".";
      }

#if defined(__APPLE__) && defined(__MACH__)

#else

   sigh_add(SIGINT, (void*)&global_signal_handler);
   sigh_add(SIGTERM, (void*)&global_signal_handler);
   /*
     Not working...
     sigh_add(SIGINT,   reinterpret_cast<void*>(&global_signal_handler));
     sigh_add(SIGTERM,  reinterpret_cast<void*>(&global_signal_handler));
   */
   //sigh_add(SIGWINCH, (void*)&global_signal_handler);
#endif

   // Done using the start up helper.
   starthelper.reset();

   std::auto_ptr<Screen> apnscr;

   if ((cla.CommandPresent()) && (cla.NoOutputPresent()))
      {
         apnscr.reset(new Screen(true /* no output to screen */));
      }
   else
      {
         apnscr.reset(new Screen(false));
      }

   // Generated with http://www.network-science.de/ascii/.
   // Font used : sblood.
   // Reflection: no.
   // Adjustment: left.
   // Stretch   : no.

   apnscr->setOutput(" @@@@@@@  @@@@@@@  @@@@@@@   @@@@@@@ @@@      @@@ ");
   apnscr->setOutput(" @@!  @@@   @@!   !@@       !@@      @@!      @@! ");
   apnscr->setOutput(" @!@!@!@    @!!   !@! @!@!@ !@!      @!!      !!@ ");
   apnscr->setOutput(" !!:  !!!   !!:   :!!   !!: :!!      !!:      !!: ");
   apnscr->setOutput(" :: : ::     :     :: :: :   :: :: : : ::.: : :   ");
   apnscr->setOutput("                                                  ");

   apnscr->setOutput(btg::core::projectDefaults::sCLI_CLIENT() +" version " + btg::core::projectDefaults::sFULLVERSION() + ", build " + btg::core::projectDefaults::sBUILD() + " initializing ..");
   apnscr->setOutput(initialStatusMessage);

   clihandler.reqVersion();
   if (clihandler.commandSuccess())
      {
         const btg::core::OptionBase & o = clihandler.getOption();
         apnscr->setOutput(o.toString());

         if (!o.getOption(btg::core::OptionBase::URL))
            {
               clihandler.DisableUrlDownload();
            }
      }

   // Get some info about the current session, so it can be displayed
   // to the user.
   clihandler.reqSessionInfo();
   
   initialStatusMessage = "Session settings: ";

   if (clihandler.dht())
      {
         initialStatusMessage += "DHT: on.";
      }
   else
      {
         initialStatusMessage += "DHT: off.";
      }

   if (clihandler.encryption())
      {
         initialStatusMessage += " Encryption: on.";
      }
   else
      {
         initialStatusMessage += " Encryption: off.";
      }

   apnscr->setOutput(initialStatusMessage);

   // If the user requested to open any files, do it.
   if (cla.inputFilenamesPresent())
      {
         t_strList filelist = cla.getInputFilenames();
         lastFiles lastfiles(logwrapper, dynconfig);
         OpenFiles(*apnscr, clihandler, lastfiles, filelist);
      }

   // Open URLs.
   if (cla.urlsPresent())
      {
         t_strList filelist = cla.getUrls();
         OpenUrls(*apnscr, clihandler, filelist);
      }

   // One or more commands to execute are present.
   if (cla.CommandPresent())
      {
         t_strList cmdlist = cla.getCommand();
         t_strListCI iter;
         for (iter = cmdlist.begin(); iter != cmdlist.end(); iter++)
            {
               handleInput(*iter, clihandler, *apnscr, neverAskFlag);
            }
      }

   std::cin.clear();

   std::string line = "";

   while (global_btg_run > 0)
      {
         apnscr->setOutput("# ", false);

         if (apnscr->getLine())
            {
               line = apnscr->getInput();
               BTG_NOTICE(logwrapper, "Got input: '" << line << "'");
               handleInput(line, clihandler, *apnscr, neverAskFlag);
            }
      } // while

   if (config.modified())
      {
         config.write();
      }

   return BTG_NORMAL_EXIT;
}

void handleInput(std::string const& _line, cliHandler& _clihandler, Screen& _nscr, bool const _neverAskFlag)
{
   switch (_clihandler.handleInput(_line))
      {
      case cliHandler::INPUT_OK:
         {
            // Check if the remote command produced a response.
            if (_clihandler.outputAvailable())
               {
                  _nscr.setOutput("Response: " + _clihandler.getOutput());
               }
            else if (_clihandler.errorAvailable())
               {
                  _nscr.setOutput("Response: " + _clihandler.getError());

                  if ((global_btg_run == GR_QUIT) || (global_btg_run == GR_FATAL))
                     {
                        sleep(3);
                     }
               }

            break;
         }
      case cliHandler::INPUT_IGNORE:
         {
            // There is nothing to output, as nothing was sent to the
            // daemon.
            break;
         }
      case cliHandler::INPUT_LOCAL:
         {
            if (_clihandler.outputAvailable())
               {
                  _nscr.setOutput("Response: " + _clihandler.getOutput());
                  if (global_btg_run == GR_QUIT)
                     {
                        sleep(1);
                     }
               }
            break;
         }
      case cliHandler::INPUT_SURE_QUERY:
         {
            if (!_neverAskFlag)
               {
                  // Ask the user if he is sure.
                  if (_nscr.isUserSure())
                     {
                        // The user is sure, use handle a saved command.
                        // This is used for handling quit and kill at this
                        // point.
                        _clihandler.handleInput(_line, true /* Handle saved command. */);
                     }
               }
            else
               {
                  // Not asking questions.
                  _clihandler.handleInput(_line, true);
               }
            break;
         }
      case cliHandler::INPUT_ERROR:
         {
            // The entered text was not a command.
            if (_clihandler.errorAvailable())
               {
                  _nscr.setOutput("Error: " + _clihandler.getError());
               }
            break;
         }
      case cliHandler::INPUT_NOT_A_COMMAND:
         {
            // No error was specified, so write a default error.
            _nscr.setOutput("Error: '" + _line + "', wrong syntax or command.");
            break;
         }
      case cliHandler::UNDEFINED_RESPONSE:
         {
            _nscr.setOutput("Undefined message. Something is really wrong.");
            break;
         }
      }
}

void OpenFiles(Screen& _nscr, 
               cliHandler& _clihandler, 
               lastFiles& _lastfiles,
               t_strList const& _filelist)
{
   for (t_strListCI iter = _filelist.begin(); 
        iter != _filelist.end(); 
        iter++)
      {
         _nscr.setOutput("Creating '" + *iter + "'");
         _clihandler.reqCreate(*iter);
         // Check if the remote command produced a response.
         if (_clihandler.outputAvailable())
            {
               _nscr.setOutput("Response: " + _clihandler.getOutput());
               // Update the list of last opened files.
               _lastfiles.add(*iter);
            }
         else if (_clihandler.errorAvailable())
            {
               _nscr.setOutput("Response: " + _clihandler.getError());
            }
      }
}

void OpenUrls(Screen& _nscr, 
              cliHandler& _clihandler, 
              t_strList const& _filelist)
{
   for (t_strListCI iter = _filelist.begin(); 
        iter != _filelist.end(); 
        iter++)
      {
         if (!btg::core::client::isUrlValid(*iter))
            {
               _nscr.setOutput("Response: Invalid URL.");
               continue;
            }
         
         // Get a file name.
         std::string filename;

         if (!btg::core::client::getFilenameFromUrl(*iter, filename))
            {
               _nscr.setOutput("Response: Unable to find a file name in URL.");
               continue;
            }

         _nscr.setOutput("Creating '" + filename + "' from URL '" + *iter + "'.");

         _clihandler.reqCreateFromUrl(filename, *iter);
         if (_clihandler.commandSuccess())
            {
               if (_clihandler.handleUrlProgress(_clihandler.UrlId()))
                  {
                     _nscr.setOutput("Response: created " + filename + ".");
                  }
               else
                  {
                     _nscr.setOutput("Response: unable to create " + filename + ".");
                  }
            }
         else
            {
               _nscr.setOutput("Response: unable to create " + filename + ".");
            }
      }
}

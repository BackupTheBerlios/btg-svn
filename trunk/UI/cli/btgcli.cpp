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

using namespace btg::core;
using namespace btg::core::logger;
using namespace btg::core::client;
using namespace btg::UI::cli;

void handleInput(std::string const& _line, 
                 cliHandler* _clihandler, 
                 ncursesScreen* _nscr, 
                 bool const _neverAskFlag);

void OpenUrls(ncursesScreen* _nscr, 
              cliHandler* _clihandler, 
              t_strList const& _filelist);

void OpenFiles(ncursesScreen* _nscr, 
               cliHandler* _clihandler, 
               lastFiles* _lastfiles,
               t_strList const& _filelist);

// Main file for the client.
int main(int argc, char* argv[])
{
   LogWrapperType logwrapper(new btg::core::logger::logWrapper);

   btg::core::crashLog::init();

   // Parse command line arguments.
   commandLineArgumentHandler* cla = new commandLineArgumentHandler(GPD->sCLI_CONFIG(),
                                                                    true /* Extra option used to pass commands. */);
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
         clientConfiguration non_existing(logwrapper, "non_existing");

         std::cout << non_existing.getSyntax() << std::endl;

         delete cla;
         cla = 0;

         projectDefaults::killInstance();

         return BTG_NORMAL_EXIT;
      }

   bool verboseFlag  = cla->beVerbose();

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
         BTG_FATAL_ERROR(logwrapper, GPD->sCLI_CLIENT(), "Could not open file '" << config_filename << "'.");

         delete cla;
         cla = 0;

         projectDefaults::killInstance();

         return BTG_ERROR_EXIT;
      }

   clientConfiguration* config = new clientConfiguration(logwrapper, config_filename);
   bool const gotConfig = config->read();

   clientDynConfig cliDynConf(logwrapper, GPD->sCLI_DYNCONFIG());
   lastFiles* lastfiles = new lastFiles(logwrapper, cliDynConf);

   if (!gotConfig)
      {
         BTG_FATAL_ERROR(logwrapper, GPD->sCLI_CLIENT(), "Could not read the config file, '" << GPD->sCLI_CONFIG() << "'. Create one.");

         delete config;
         config = 0;

         delete lastfiles;
         lastfiles = 0;

         delete cla;
         cla    = 0;

         projectDefaults::killInstance();

         return BTG_ERROR_EXIT;
      }

   bool neverAskFlag = config->getNeverAskQuestions();

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

         projectDefaults::killInstance();

         return BTG_ERROR_EXIT;
      }

   delete transporthelper;
   transporthelper = 0;

   cliHandler* clihandler = new cliHandler(logwrapper,
                                           externalization, 
                                           transport, 
                                           config, 
                                           lastfiles, 
                                           verboseFlag, 
                                           cla->automaticStart());

   std::string initialStatusMessage("");

   // Create a helper to do the initial setup of this client.
   startupHelper* starthelper = new cliStartupHelper(logwrapper,
                                                     config,
                                                     cla,
                                                     transport,
                                                     clihandler);

   if (!starthelper->init())
      {
         BTG_FATAL_ERROR(logwrapper, GPD->sCLI_CLIENT(), "Internal error: start up helper not initialized.");

         delete starthelper;
         starthelper = 0;

         delete clihandler;
         clihandler = 0;

         delete externalization;
         externalization = 0;

         delete cla;
         cla = 0;

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
         BTG_FATAL_ERROR(logwrapper, GPD->sCLI_CLIENT(), "Unable to initialize logging");
         delete starthelper;
         starthelper = 0;

         delete clihandler;
         clihandler = 0;

         delete externalization;
         externalization = 0;

         delete cla;
         cla = 0;

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
         AUTH_RETRY:
         // Ask the user about which username and password to use.
         if (starthelper->execute(startupHelper::op_auth) != startupHelper::or_auth_success)
            {
               BTG_FATAL_ERROR(logwrapper, GPD->sCLI_CLIENT(), "Unable to initialize auth");

               delete starthelper;
               starthelper = 0;

               delete clihandler;
               clihandler = 0;

               delete externalization;
               externalization = 0;

               delete cla;
               cla = 0;

               delete config;
               config = 0;

               delete lastfiles;
               lastfiles = 0;

               projectDefaults::killInstance();

               return BTG_ERROR_EXIT;
            }
      }

   /// Initialize the transport.
   if (starthelper->execute(startupHelper::op_init) != startupHelper::or_init_success)
   {
      std::cout << "Authentication error." << std::endl;
      return BTG_ERROR_EXIT;
   }

   // Handle command line options:
   if (cla->doList())
      {
         starthelper->execute(startupHelper::op_list);

         // Clean up, before quitting.
         delete starthelper;
         starthelper = 0;

         delete clihandler;
         clihandler = 0;

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
               BTG_FATAL_ERROR(logwrapper, GPD->sCLI_CLIENT(), "Unable to attach to session");

               // Clean up, before quitting.
               delete starthelper;
               starthelper = 0;

               delete clihandler;
               clihandler = 0;

               delete externalization;
               externalization = 0;

               delete cla;
               cla = 0;

               delete config;
               config = 0;

               delete lastfiles;
               lastfiles = 0;

               projectDefaults::killInstance();

               return BTG_ERROR_EXIT;
            }

         t_long session = clihandler->session();
         std::string strSession = btg::core::convertToString<t_long>(session);
         initialStatusMessage = "Attached to session #" + strSession + ".";
      }
   else if (cla->doAttach())
      {
         // Attach to a certain session, either specified on the
         // command line or chosen by the user from a list.

         if (starthelper->execute(startupHelper::op_attach) == startupHelper::or_attach_failture)
            {
               BTG_FATAL_ERROR(logwrapper, GPD->sCLI_CLIENT(), "Unable to attach to session");

               // Clean up, before quitting.
               delete starthelper;
               starthelper = 0;

               delete clihandler;
               clihandler = 0;

               delete externalization;
               externalization = 0;

               delete cla;
               cla = 0;

               delete config;
               config = 0;

               delete lastfiles;
               lastfiles = 0;

               projectDefaults::killInstance();

               return BTG_ERROR_EXIT;
            }

         t_long session = clihandler->session();
         std::string strSession = btg::core::convertToString<t_long>(session);
         initialStatusMessage = "Attached to session #" + strSession + ".";
      }

   // Only execute setup if we are not attaching to an existing session.
   if ((!cla->doAttach()) && (!cla->doAttachFirst()))
      {
         if (starthelper->execute(startupHelper::op_setup) == startupHelper::or_setup_failture)
            {
               BTG_FATAL_ERROR(logwrapper, GPD->sCLI_CLIENT(), "Unable to connect to daemon.");

               // Clean up, before quitting.
               delete starthelper;
               starthelper = 0;

               delete clihandler;
               clihandler = 0;

               delete externalization;
               externalization = 0;

               delete cla;
               cla = 0;

               delete config;
               config = 0;

               delete lastfiles;
               lastfiles = 0;

               return BTG_ERROR_EXIT;
            }

         t_long session = clihandler->session();
         std::string strSession = btg::core::convertToString<t_long>(session);
         initialStatusMessage = "Established session #" + strSession + ".";
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
   delete starthelper;
   starthelper = 0;

   ncursesScreen* nscr = 0;

   if ((cla->CommandPresent()) && (cla->NoOutputPresent()))
      {
         nscr = new ncursesScreen(true /* no output to screen */);
      }
   else
      {
         nscr = new ncursesScreen(false);
      }

   // Generated with http://www.network-science.de/ascii/.
   // Font used : sblood.
   // Reflection: no.
   // Adjustment: left.
   // Stretch   : no.

   nscr->setOutput(" @@@@@@@  @@@@@@@  @@@@@@@   @@@@@@@ @@@      @@@ ");
   nscr->setOutput(" @@!  @@@   @@!   !@@       !@@      @@!      @@! ");
   nscr->setOutput(" @!@!@!@    @!!   !@! @!@!@ !@!      @!!      !!@ ");
   nscr->setOutput(" !!:  !!!   !!:   :!!   !!: :!!      !!:      !!: ");
   nscr->setOutput(" :: : ::     :     :: :: :   :: :: : : ::.: : :   ");
   nscr->setOutput("                                                  ");

   nscr->setOutput(GPD->sCLI_CLIENT() +" version " + GPD->sFULLVERSION() + ", build " + GPD->sBUILD() + " initializing ..");
   nscr->setOutput(initialStatusMessage);

   clihandler->reqVersion();
   if (clihandler->commandSuccess())
      {
         const btg::core::OptionBase & o = clihandler->getOption();
         nscr->setOutput(o.toString());

         if (!o.getOption(btg::core::OptionBase::URL))
            {
               clihandler->DisableUrlDownload();
            }
      }

   // Get some info about the current session, so it can be displayed
   // to the user.
   clihandler->reqSessionInfo();
   
   initialStatusMessage = "Session settings: ";

   if (clihandler->dht())
      {
         initialStatusMessage += "DHT: on.";
      }
   else
      {
         initialStatusMessage += "DHT: off.";
      }

   if (clihandler->encryption())
      {
         initialStatusMessage += " Encryption: on.";
      }
   else
      {
         initialStatusMessage += " Encryption: off.";
      }

   nscr->setOutput(initialStatusMessage);

   // If the user requested to open any files, do it.
   if (cla->inputFilenamesPresent())
      {
         t_strList filelist = cla->getInputFilenames();
         OpenFiles(nscr, clihandler, lastfiles, filelist);
      }

   // Open URLs.
   if (cla->urlsPresent())
      {
         t_strList filelist = cla->getUrls();
         OpenUrls(nscr, clihandler, filelist);
      }

   // One or more commands to execute are present.
   if (cla->CommandPresent())
      {
         t_strList cmdlist = cla->getCommand();
         t_strListCI iter;
         for (iter = cmdlist.begin(); iter != cmdlist.end(); iter++)
            {
               handleInput(*iter, clihandler, nscr, neverAskFlag);
            }
      }

   std::string line = "";

   while (global_btg_run > 0)
      {
         nscr->setInput("# ");

         switch (nscr->getKeys())
            {
            case ncursesScreen::EVENT_RESIZE:
               {
                  BTG_NOTICE(logwrapper, "Resize event received (2).");
                  nscr->resize();
                  nscr->clear();
                  nscr->refresh();
                  break;
               }
            case ncursesScreen::EVENT_KEY:
               {
                  line = nscr->getInput();
                  BTG_NOTICE(logwrapper, "Got input: '" << line << "'");
                  handleInput(line, clihandler, nscr, neverAskFlag);
                  break;
               }
            case ncursesScreen::EVENT_NO_KEY:
               {
                  break;
               }
            }
      } // while

   if (config->modified())
      {
         config->write();
      }

   delete config;
   config = 0;

   delete lastfiles;
   lastfiles = 0;

   delete clihandler;
   clihandler = 0;

   delete externalization;
   externalization = 0;

   delete cla;
   cla = 0;

   delete nscr;
   nscr = 0;

   projectDefaults::killInstance();

   return BTG_NORMAL_EXIT;
}

void handleInput(std::string const& _line, cliHandler* _clihandler, ncursesScreen* _nscr, bool const _neverAskFlag)
{
   switch (_clihandler->handleInput(_line))
      {
      case cliHandler::INPUT_OK:
         {
            // Check if the remote command produced a response.
            if (_clihandler->outputAvailable())
               {
                  _nscr->setOutput("Response: " + _clihandler->getOutput());
               }
            else if (_clihandler->errorAvailable())
               {
                  _nscr->setOutput("Response: " + _clihandler->getError());

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
            if (_clihandler->outputAvailable())
               {
                  _nscr->setOutput("Response: " + _clihandler->getOutput());
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
                  if (_nscr->isUserSure())
                     {
                        // The user is sure, use handle a saved command.
                        // This is used for handling quit and kill at this
                        // point.
                        _clihandler->handleInput(_line, true /* Handle saved command. */);
                     }
               }
            else
               {
                  // Not asking questions.
                  _clihandler->handleInput(_line, true);
               }
            break;
         }
      case cliHandler::INPUT_ERROR:
         {
            // The entered text was not a command.
            if (_clihandler->errorAvailable())
               {
                  _nscr->setOutput("Error: " + _clihandler->getError());
               }
            break;
         }
      case cliHandler::INPUT_NOT_A_COMMAND:
         {
            // No error was specified, so write a default error.
            _nscr->setOutput("Error: '" + _line + "', wrong syntax or command.");
            break;
         }
      case cliHandler::UNDEFINED_RESPONSE:
         {
            _nscr->setOutput("Undefined message. Something is really wrong.");
            break;
         }
      }
}

void OpenFiles(ncursesScreen* _nscr, 
               cliHandler* _clihandler, 
               lastFiles* _lastfiles,
               t_strList const& _filelist)
{
   for (t_strListCI iter = _filelist.begin(); 
        iter != _filelist.end(); 
        iter++)
      {
         _nscr->setOutput("Creating '" + *iter + "'");
         _clihandler->reqCreate(*iter);
         // Check if the remote command produced a response.
         if (_clihandler->outputAvailable())
            {
               _nscr->setOutput("Response: " + _clihandler->getOutput());
               // Update the list of last opened files.
               _lastfiles->addLastFile(*iter);
            }
         else if (_clihandler->errorAvailable())
            {
               _nscr->setOutput("Response: " + _clihandler->getError());
            }
      }
}

void OpenUrls(ncursesScreen* _nscr, 
              cliHandler* _clihandler, 
              t_strList const& _filelist)
{
   for (t_strListCI iter = _filelist.begin(); 
        iter != _filelist.end(); 
        iter++)
      {
         if (!btg::core::client::isUrlValid(*iter))
            {
               _nscr->setOutput("Response: Invalid URL.");
               continue;
            }
         
         // Get a file name.
         std::string filename;

         if (!btg::core::client::getFilenameFromUrl(*iter, filename))
            {
               _nscr->setOutput("Response: Unable to find a file name in URL.");
               continue;
            }

         _nscr->setOutput("Creating '" + filename + "' from URL '" + *iter + "'.");

         _clihandler->reqCreateFromUrl(filename, *iter);
         if (_clihandler->commandSuccess())
            {
               if (_clihandler->handleUrlProgress(_clihandler->UrlId()))
                  {
                     _nscr->setOutput("Response: created " + filename + ".");
                  }
               else
                  {
                     _nscr->setOutput("Response: unable to create " + filename + ".");
                  }
            }
         else
            {
               _nscr->setOutput("Response: unable to create " + filename + ".");
            }
      }
}

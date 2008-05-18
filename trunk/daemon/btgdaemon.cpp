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

#include <bcore/addrport.h>

#include <bcore/command_factory.h>
#include <bcore/util.h>
#include <bcore/dbuf.h>
#include <bcore/crashlog.h>

#include <bcore/os/sleep.h>
#include <bcore/os/fileop.h>
#include <bcore/os/id.h>
#include <bcore/os/pidfile.h>

#include <bcore/logger/logger.h>
// Loggers:
#include <bcore/logger/console_log.h>
#include <bcore/logger/file_log.h>
#include <bcore/logger/syslog_log.h>

#include <bcore/externalization/xmlrpc.h>

#include "daemondata.h"

#include "dconfig.h"
#include "darg.h"
#include "filetrack.h"

#include "ipfilter/ipfilterif.h"
#include "ipfilter/levelone.h"
#include "ipfilter/emule.h"

#include <map>

extern "C"
{
#include "daemon.h"
#include <bcore/sigh.h>

#include <unistd.h>
#include <time.h>
} // extern "C"

#include <daemon/auth/auth.h>
#include <daemon/auth/passwd_auth.h>

#include "btgdaemon.h"
#include "daemonhandler.h"
#include <bcore/verbose.h>
#include <bcore/logable.h>

#if BTG_OPTION_EVENTCALLBACK
#  include "callbackmgr.h"
#endif // BTG_OPTION_EVENTCALLBACK

#if BTG_OPTION_UPNP
#  include "upnp/upnpif.h"
#  if BTG_OPTION_USECYBERLINK
#    include "upnp/cyberlink.h"
#  endif // BTG_OPTION_USECYBERLINK

#if BTG_LT_0_14
#  include "upnp/ltupnpif.h"
#endif

#endif // BTG_OPTION_UPNP

#include "modulelog.h"

using namespace btg::core;
using namespace btg::core::logger;
using namespace btg::daemon;

bool checkFile(LogWrapperType _logwrapper,
               std::string const& _purpose, 
               std::string & _filename);

bool loadFilter(LogWrapperType _logwrapper, bool _verboseFlag, daemonData & _dd);

const std::string moduleName("dem");

int global_btg_run = 1;
int global_btg_hup = 0;

// Main file for the daemon.
int main(int argc, char* argv[])
{
   LogWrapperType logwrapper(new btg::core::logger::logWrapper);
#if BTG_DEBUG
   btg::core::crashLog::init();
#endif // BTG_DEBUG

   daemonData dd;

   time(&dd.daemonStartTime);

   // Parse command line arguments.
   dd.cla = new commandLineArgumentHandler(GPD->sDAEMON_CONFIG());
   dd.cla->setup();

   if (!dd.cla->parse(argc, argv))
      {
         return BTG_ERROR_EXIT;
      }

   // Before doing anything else, check if the user wants to get the
   // syntax of the configuration file.
   if (dd.cla->listConfigFileSyntax())
      {
         daemonConfiguration non_existing(logwrapper, "non_existing");

         std::cout << non_existing.getSyntax() << std::endl;

         return BTG_NORMAL_EXIT;
      }


   bool verboseFlag = dd.cla->beVerbose();
   
   VERBOSE_LOG(logwrapper, verboseFlag, "Verbose logging enabled.");

   /*
    * Set debug level
    */
   
#if BTG_DEBUG
   // Set debug logging, if requested.
   if (dd.cla->doDebug())
      {
         logwrapper->setMinMessagePriority(logWrapper::PRIO_DEBUG);
      }
   else if (verboseFlag)
      {
         logwrapper->setMinMessagePriority(logWrapper::PRIO_VERBOSE);
      }
   else
      {
         // Default: only report errors.
         logwrapper->setMinMessagePriority(logWrapper::PRIO_ERROR);
      }
#else
   // No debug enabled, check for verbose flag.
   if (verboseFlag)
      {
         logwrapper->setMinMessagePriority(logWrapper::PRIO_VERBOSE);
      }
   else
      {
         // Default: only report errors.
         logwrapper->setMinMessagePriority(logWrapper::PRIO_ERROR);
      }
#endif // BTG_DEBUG

   
   dd.configFile = GPD->sDAEMON_CONFIG();

   if (dd.cla->configFileSet())
      {
         dd.configFile = dd.cla->configFile();
      }

   VERBOSE_LOG(logwrapper, verboseFlag, "config: '" << dd.configFile << "'.");

   if (!checkFile(logwrapper, "configuration file", dd.configFile))
      {
         return BTG_ERROR_EXIT;
      }

   dd.config = new daemonConfiguration(logwrapper, 
                                       dd.configFile);

   if (!dd.config->read())
      {
         BTG_FATAL_ERROR(logwrapper, 
                         "btgdaemon", "Could not open configuration file '" << dd.configFile << "'");
         return BTG_ERROR_EXIT;
      }

   MVERBOSE_LOG(logwrapper, verboseFlag, "config read from '" << dd.configFile << "'.");

   if (dd.config->getUseTorrentName())
      {
         VERBOSE_LOG(logwrapper, verboseFlag, "Sending status using torrent names.");
      }
   else
      {
         VERBOSE_LOG(logwrapper, verboseFlag, "Sending status using file names.");
      }

   /* 
    * Creating/check PID-file
    */
   
   btg::core::os::PIDFile pidfile;
   
   { // create PID-file
      std::string pfname;
      
      // try cmdline first
      if ( dd.cla->PIDFile().size() > 0 )
         {
            pfname = dd.cla->PIDFile();
            VERBOSE_LOG(logwrapper, verboseFlag, "Using PID-file from cmdline: '"
                        << pfname << "'.");
         }
      // then config
      else if ( dd.config->getPIDFile().size() > 0 )
         {
            pfname = dd.config->getPIDFile();
            VERBOSE_LOG(logwrapper, verboseFlag, "Using PID-file from config: '"
                        << pfname << "'.");
         }

      if ( pfname.size() > 0 )
         {
            // passwd file should be absolute to be able to unlink it after current dir change
            btg::core::os::fileOperation::resolvePath(pfname);
            pidfile.create(pfname.c_str());

            if (pidfile.exists())
               {
                  BTG_FATAL_ERROR(logwrapper, 
                                  "btgdaemon",
                                  "Another instance of btgdaemon with pid "
                                  << pidfile.pid()
                                  << " already running.");
                  return BTG_ERROR_EXIT;
               }
         
            if (pidfile.error())
               {
                  BTG_FATAL_ERROR(logwrapper, 
                                  "btgdaemon",
                                  "Could not create PIDfile '" << dd.config->getPIDFile() << "'");
                  return BTG_ERROR_EXIT;
               }
         }
   } // end create PID-file
   
#if BTG_OPTION_SAVESESSIONS

   /*
    * Setup sessionsaving
    */
   
   { // sessionsaving
      std::string ss_fname;

      // try cmdline first
      if (dd.cla->saveSessionsFileSet())
         {
            dd.ss_enable = true;
            ss_fname     = dd.cla->saveSessionsFile();
            VERBOSE_LOG(logwrapper, verboseFlag, "Using session file name from cmdline: '"
                        << ss_fname << "'." );
         }
      // then config file
      else if (dd.config->getSSEnable())
         {
            dd.ss_enable  = dd.config->getSSEnable();
            dd.ss_timeout = dd.config->getSSTimeout();
            ss_fname      = dd.config->getSSFilename();
            dd.ss_file.open( ss_fname.c_str() );
            VERBOSE_LOG(logwrapper, verboseFlag, "Using session file from config: '"
                        << ss_fname << "'." );
         }
      
      if (dd.ss_enable)
         {
            btg::core::os::fileOperation::resolvePath(ss_fname);
            dd.ss_file.open(ss_fname.c_str());
            if (!dd.ss_file.is_open())
               {
                  // file not exists (or something else but don't care for now)
                  dd.ss_file.clear();
                  dd.ss_file.open(ss_fname.c_str(), std::ios_base::out); // try to create a new
                  if (!dd.ss_file.is_open())
                     {
                        BTG_FATAL_ERROR(logwrapper, 
                                        "btgdaemon", "Could not open/create session file '"
                                        << ss_fname << "'");
                        return BTG_ERROR_EXIT;
                     }
                  dd.ss_file.close();
                  dd.ss_file.open(ss_fname.c_str()); // reopen in read/write mode
               }
         }
   } // end sessionsaving
   
#endif // BTG_OPTION_SAVESESSIONS

   /*
    * Initialize authorization.
    */

   std::string auth_file = dd.config->getAuthFile();

   if (!checkFile(logwrapper, "passwd file", auth_file))
      {
         return BTG_ERROR_EXIT;
      }

   dd.auth = new btg::daemon::auth::passwordAuth(logwrapper, auth_file);

   if (!dd.auth->initialized())
      {
         BTG_FATAL_ERROR(logwrapper, 
                         "btgdaemon", "Unable to load passwd file '" << auth_file << "'.");
         return BTG_ERROR_EXIT;
      }

   /*
    * Determine which transport to use.
    */

   switch (dd.config->getTransport())
      {
      case messageTransport::TCP:
         {
            addressPort ap = dd.config->getListenTo();
            if (!ap.valid())
               {
                  BTG_FATAL_ERROR(logwrapper, 
                                  "btgdaemon", "Invalid IPv4:port pair '" << ap.toString() << "'");
                  return BTG_ERROR_EXIT;
               }

            dd.externalization = new btg::core::externalization::XMLRPC(logwrapper);

            dd.transport = new tcpTransport(logwrapper, 
                                            dd.externalization,
                                            100 * 1024,
                                            FROM_SERVER,
                                            ap,
                                            1000 // 1s timeout
                                            );

            if (!dd.transport->isInitialized())
               {
                  BTG_FATAL_ERROR(logwrapper, 
                                  "btgdaemon", "Could not create a TCP/IP transport");
                  return BTG_ERROR_EXIT;
               }
            BTG_MNOTICE(logwrapper, 
                        "initialized a TCP/IP transport");
            break;
         }
      case messageTransport::STCP:
         {
            addressPort ap = dd.config->getListenTo();
            if (!ap.valid())
               {
                  BTG_FATAL_ERROR(logwrapper, 
                                  "btgdaemon", "Invalid IPv4:port pair '" << ap.toString() << "'");
                  return BTG_ERROR_EXIT;
               }

            std::string CAfile   = dd.config->getCACert();
            std::string CertFile = dd.config->getCert();
            std::string keyFile  = dd.config->getPrivKey();

            if (!checkFile(logwrapper, "certificate authority, PEM", CAfile))
               {
                  return BTG_ERROR_EXIT;
               }

            if (!checkFile(logwrapper, "certificate, PEM", CertFile))
               {
                  return BTG_ERROR_EXIT;
               }

            if (!checkFile(logwrapper, "private key, RSA", keyFile))
               {
                  return BTG_ERROR_EXIT;
               }

            btg::core::os::gtlsGeneric::init();

            dd.externalization     = new btg::core::externalization::XMLRPC(logwrapper);
            dd.secureTransportData = new btg::core::os::gtlsGlobalServerData(CAfile, CertFile, keyFile);

            dd.transport           = new btg::core::secureTcpTransport(logwrapper,
                                                                       dd.externalization,
                                                                       dd.secureTransportData,
                                                                       100 * 1024,
                                                                       FROM_SERVER,
                                                                       ap,
                                                                       1000 // 1s timeout
                                                                       );

            if (!dd.transport->isInitialized())
               {
                  BTG_FATAL_ERROR(logwrapper, 
                                  "btgdaemon", "Could not create a STCP/IP transport");
                  return BTG_ERROR_EXIT;
               }

            BTG_MNOTICE(logwrapper, 
                        "initialized a STCP/IP transport");
            break;
         }
      case messageTransport::XMLRPC:
         {
            addressPort ap = dd.config->getListenTo();
            if (!ap.valid())
               {
                  BTG_FATAL_ERROR(logwrapper, 
                                  "btgdaemon", "Invalid ipv4:port pair '" << ap.toString() << "'");
                  return BTG_ERROR_EXIT;
               }

            dd.externalization = new btg::core::externalization::XMLRPC(logwrapper);

            dd.transport = new httpTransport(logwrapper,
                                             dd.externalization,
                                             100 * 1024,
                                             FROM_SERVER,
                                             ap,
                                             1000 // 1s timeout
                                             );

            if (!dd.transport->isInitialized())
               {
                  BTG_FATAL_ERROR(logwrapper, 
                                  "btgdaemon", "Could not create a http transport");
                  return BTG_ERROR_EXIT;
               }
            BTG_MNOTICE(logwrapper, 
                        "initialized a XMLRPC transport");
            break;
         }
      case messageTransport::SXMLRPC:
         {
            addressPort ap = dd.config->getListenTo();
            if (!ap.valid())
               {
                  BTG_FATAL_ERROR(logwrapper, 
                                  "btgdaemon", "Invalid IPv4:port pair '" << ap.toString() << "'");
                  return BTG_ERROR_EXIT;
               }

            std::string CAfile   = dd.config->getCACert();
            std::string CertFile = dd.config->getCert();
            std::string keyFile  = dd.config->getPrivKey();

            if (!checkFile(logwrapper, "certificate authority, PEM", CAfile))
               {
                  return BTG_ERROR_EXIT;
               }

            if (!checkFile(logwrapper, "certificate, PEM", CertFile))
               {
                  return BTG_ERROR_EXIT;
               }

            if (!checkFile(logwrapper, "private key, RSA", keyFile))
               {
                  return BTG_ERROR_EXIT;
               }

            btg::core::os::gtlsGeneric::init();

            dd.externalization     = new btg::core::externalization::XMLRPC(logwrapper);
            dd.secureTransportData = new btg::core::os::gtlsGlobalServerData(CAfile, CertFile, keyFile);

            dd.transport           = new btg::core::secureHttpTransport(logwrapper,
                                                                        dd.externalization,
                                                                        dd.secureTransportData,
                                                                        100 * 1024,
                                                                        FROM_SERVER,
                                                                        ap,
                                                                        1000 // 1s timeout
                                                                        );

            if (!dd.transport->isInitialized())
               {
                  BTG_FATAL_ERROR(logwrapper, 
                                  "btgdaemon", "Could not create a SXMLRPC transport");
                  return BTG_ERROR_EXIT;
               }

            BTG_MNOTICE(logwrapper, 
                        "initialized a SXMLRPC transport");
            break;
         }
      case messageTransport::UNDEFINED:
         {
            BTG_FATAL_ERROR(logwrapper, 
                            "btgdaemon", "Unable to recognize transport name, cannot continue.");
         }
      }

   if (!dd.transport)
      {
         BTG_FATAL_ERROR(logwrapper, 
                         "btgdaemon", "No transport, cannot continue.");
         return BTG_ERROR_EXIT;
      }

   if (verboseFlag)
      {
         std::string transportName;

         switch (dd.config->getTransport())
            {
            case messageTransport::TCP:
               transportName = "TCP/IP";
               break;
            case messageTransport::STCP:
               transportName = "secure TCP/IP";
               break;
            case messageTransport::XMLRPC:
               transportName = "XML-RPC";
               break;
            case messageTransport::SXMLRPC:
               transportName = "secure XML-RPC";
               break;
            case messageTransport::UNDEFINED:
               transportName = "undefined";
               break;
            }

         VERBOSE_LOG(logwrapper, 
                     verboseFlag, "Transport, " << transportName << ", created and initialized.");
      }
   
   /*
    * Create an object to keep track of added torrents.
    */
   
   dd.filetrack = new fileTrack(logwrapper);

   /*
    * Create an object to keep track of connections
    */
   
   dd.connHandler = new connectionHandler();

   /*
    * Load filters.
    */
   if (!loadFilter(logwrapper, verboseFlag, dd))
      {
         return BTG_ERROR_EXIT;
      }

   /*
    * Make sure that the provided torrent range is valid and that the
    * daemon gets at least two ports.
    */

   /*
    * Get the port range used by libtorrent.
    */

   dd.portRange = dd.config->getLTPortRange();
   if ((((dd.portRange.first < daemonData::MIN_LISTEN_PORT) || (dd.portRange.second < daemonData::MIN_LISTEN_PORT)) || 
        (dd.portRange.first > dd.portRange.second)))
      {
         BTG_FATAL_ERROR(logwrapper, 
                         "btgdaemon", "Unable to use port range: " << dd.portRange.first << 
                         ":" << dd.portRange.second << ".");
         return BTG_ERROR_EXIT;
      }
   if ((dd.portRange.second - dd.portRange.first) < 1)
      {
         BTG_FATAL_ERROR(logwrapper, 
                         "btgdaemon", "Unable to use port range: " << dd.portRange.first << 
                         ":" << dd.portRange.second << ", at least two ports are required.");
         return BTG_ERROR_EXIT;
      }

#if BTG_OPTION_UPNP

   /*
    * init upnp.
    */
   
   bool const upnpEnabled = dd.config->getUseUPnP();
   std::auto_ptr<btg::daemon::upnp::upnpIf> upnpif;
   
   if (upnpEnabled)
      {
         VERBOSE_LOG(logwrapper, 
                     verboseFlag,
                     "UPnP using internal IP: " << dd.config->getUPNPInternalIP().getIp() << ".");
      }

#if BTG_OPTION_USELTUPNP
   // Use UPnP interface from libtorrent.

   if (upnpEnabled)
      {
         libtorrent::address upnpIpAddr;
         upnpIpAddr.from_string(dd.config->getUPNPInternalIP().getIp());
         
         upnpif.reset(
                      new btg::daemon::upnp::libtorrentUpnpIf(logwrapper,
                                                              verboseFlag,
                                                              upnpIpAddr)
                      );
      }
#elif BTG_OPTION_USECYBERLINK

   if (upnpEnabled)
      {
         upnpif.reset(
                      /* creates a thread */
                      new btg::daemon::upnp::cyberlinkUpnpIf(logwrapper, verboseFlag, dd.config->getUPNPInternalIP())
                      );
      }
#endif

   if (upnpif.get())
      {
         if (upnpEnabled)
            {
               VERBOSE_LOG(logwrapper, 
                           verboseFlag, "UPnP: attempting to forward ports.");

               if (!upnpif->open(dd.portRange))
                  {
                     BTG_FATAL_ERROR(logwrapper,
                                     "btgdaemon",
                                     "UPnP: port forwarding failed.");
                     return BTG_ERROR_EXIT;
                  }
               
               VERBOSE_LOG(logwrapper, 
                           verboseFlag, "UPnP: port forwarding succeded.");
            }
         else
            {
               VERBOSE_LOG(logwrapper, 
                           verboseFlag, "Not using UPnP.");
            }
      }

#endif // BTG_OPTION_UPNP
   
   
   /*
    * 
    * =================================================================
    * 
    * Begin of daemonization process
    * 
    * =================================================================
    * 
    */
   
   
   /*
    * Initializing logging
    */

   switch (dd.config->getLog())
      {
      case logBuffer::STDOUT:
         {
            boost::shared_ptr<btg::core::logger::logStream> l(new btg::core::logger::logStream(new btg::core::logger::consoleLogger()));
            logwrapper->setLogStream(l);
            break;
         }
      case logBuffer::LOGFILE:
         {
            std::string logfilename = dd.config->getLogFile();

            fileLogger* fl = new fileLogger(logfilename);

            if (fl->open())
               {
                  boost::shared_ptr<btg::core::logger::logStream> l(new btg::core::logger::logStream(fl));
                  logwrapper->setLogStream(l);

                  BTG_MNOTICE(logwrapper, 
                              "logfile opened");
               }
            else
               {
                  delete fl;
                  fl = 0;

                  if (dd.cla->doNotDetach())
                     {
                        // Log to stdout.
                        boost::shared_ptr<btg::core::logger::logStream> l(new btg::core::logger::logStream(new btg::core::logger::consoleLogger()));
                        logwrapper->setLogStream(l);

                        BTG_MNOTICE(logwrapper, 
                                    "opening logfile '" << logfilename << "' failed");
                     }
               }
            break;
         }
      case logBuffer::SYSLOG:
         {
            // Log to syslog.
            boost::shared_ptr<btg::core::logger::logStream> l(new logStream(new syslogLogger()));
            logwrapper->setLogStream(l);
            break;
         }
      case logBuffer::UNDEF:
         {
            // No logging.
            break;
         }
      }
   
   /*
    * change EUID
    */
   
   if ((dd.config->getRunAsUser().size() > 0) &&
       (dd.config->getRunAsGroup().size() > 0) )
      {
         if (btg::core::os::ID::changePossible())
            {
               bool idChangeResult = btg::core::os::ID::changeUserAndGroup(
                                                                           dd.config->getRunAsUser(),
                                                                           dd.config->getRunAsGroup()
                                                                           );
               if (idChangeResult)
                  {
                     VERBOSE_LOG(logwrapper, 
                                 verboseFlag, "Changed user/group to " <<
                                 dd.config->getRunAsUser() << "/" <<
                                 dd.config->getRunAsGroup() << ".");
                  }
            }
      }
   else
      {
         VERBOSE_LOG(logwrapper, verboseFlag, "Not changing user/group.");
      }

   if (btg::core::os::ID::changePossible())
      {
         // Make sure that the daemon will not run as root.
         VERBOSE_LOG(logwrapper, verboseFlag, "BTG will not run as root, sorry.");
         return BTG_ERROR_EXIT;
      }

   /*
    * Daemonize.
    */
   
   if (!dd.cla->doNotDetach())
      {
         // stop existing threads
#if BTG_OPTION_USECYBERLINK
         // cyberlinkif thread
         btg::daemon::upnp::cyberlinkUpnpIf * pCLUPnPIf = dynamic_cast<btg::daemon::upnp::cyberlinkUpnpIf*>(upnpif.get());
         if (pCLUPnPIf)
            {
               pCLUPnPIf->stop_thread();
            }
#endif // BTG_OPTION_USECYBERLINK        
         switch (do_daemonize())
            {
            case PID_OK:
               {
                  pidfile.clear(); // prevent file truncation in destructor
#if BTG_OPTION_UPNP
                  if (upnpif.get()) upnpif->clear(); // prevent port closing in destructor
#endif // BTG_OPTION_UPNP
                  BTG_MNOTICE(logwrapper, 
                              "parent exiting, child daemonizing");
                  return BTG_NORMAL_EXIT;
               }
               break;
            case PID_ERR:
               {
                  BTG_MNOTICE(logwrapper, 
                              "daemonized");
               }
               break;
            case SETID_ERR:
               {
                  BTG_MERROR(logwrapper, "setid failed. Abort");
                  return BTG_ERROR_EXIT;
               }
               break;
            case CHDIR_ERR:
               {
                  BTG_MERROR(logwrapper, "chdir failed. Abort");
                  return BTG_ERROR_EXIT;
               }
               break;
            }
         // re-start needed threads
#if BTG_OPTION_USECYBERLINK
         // cyberlinkif thread
         if (pCLUPnPIf)
            {
               pCLUPnPIf->start_thread();
            }
#endif // BTG_OPTION_USECYBERLINK
      }
   else
      {
         VERBOSE_LOG(logwrapper, verboseFlag, "Not detaching from TTY.");
      }
   
   /*
    * Write pidfile, if was set
    */

   pidfile.write();

   /*
    * creating callback manager after daemonizing,
    * because callbackManager creates a thread
    * threads aren't re-created in child process
    */
   
#if BTG_OPTION_EVENTCALLBACK
   dd.callbackmgr      = new callbackManager(logwrapper, verboseFlag);
#endif // BTG_OPTION_EVENTCALLBACK

   BTG_MNOTICE(logwrapper, 
               "init done");

   sigh_add(SIGINT, (void*)&global_signal_handler);
   sigh_add(SIGTERM, (void*)&global_signal_handler);
   sigh_add(SIGHUP, (void*)&global_signal_handler);

   daemonHandler* dh = new daemonHandler(logwrapper, &dd, verboseFlag);

   VERBOSE_LOG(logwrapper, 
               verboseFlag, GPD->sDAEMON() << " version " << GPD->sFULLVERSION() << ".");
   VERBOSE_LOG(logwrapper, 
               verboseFlag, "Ready to accept clients.");

   while (global_btg_run == 1)
      {
         dh->readFromTransport();
         dh->checkTimeout();

         if (global_btg_hup == 1)
            {
               // SIGHUP received, reload user file.
               VERBOSE_LOG(logwrapper, 
                           verboseFlag, "Reloaded users file.");
               dd.auth->reInit();

               if (!loadFilter(logwrapper, verboseFlag, dd))
                  {
                     // If something goes wrong with reloading the
                     // filter, quit.
                     global_btg_run = 0;
                  }
               dh->updateFilter();

               VERBOSE_LOG(logwrapper, 
                           verboseFlag, "Reloaded filter.");

               global_btg_hup = 0;
            }
      }

   dh->shutdown();

   delete dh;
   dh = 0;

#if BTG_OPTION_UPNP
   if (upnpif.get())
      {
         upnpif.get()->close();
      }
#endif

   BTG_MNOTICE(logwrapper, 
               "finished execution");
   VERBOSE_LOG(logwrapper, 
               verboseFlag, "Finished execution.");

   if (dd.config->modified())
      {
         BTG_MNOTICE(logwrapper, 
                     "saving config");
         dd.config->write();
      }

   projectDefaults::killInstance();

   return BTG_NORMAL_EXIT;
}

bool checkFile(LogWrapperType _logwrapper,
               std::string const& _purpose, 
               std::string & _filename)
{
   bool status = true;

   std::string errorString;
   if (!btg::core::os::fileOperation::check(_filename, errorString, false))
      {
         BTG_FATAL_ERROR(_logwrapper, 
                         "btgdaemon", "Filename (" << _purpose << ") '" << _filename << "', could not be used: " << errorString);
         status = false;
      }

   return status;
}

bool loadFilter(LogWrapperType _logwrapper, bool _verboseFlag, daemonData & _dd)
{
   if (_dd.filter)
      {
         delete _dd.filter;
         _dd.filter = 0;
      }

   _dd.config->getFilterFilename(_dd.filterFilename);
   _dd.filterType = _dd.config->getFilterType();

   if (_dd.filterType != IpFilterIf::IPF_UNDEF)
      {
         if (! btg::core::os::fileOperation::check(_dd.filterFilename))
            {
               BTG_FATAL_ERROR(_logwrapper,
                               "btgdaemon",
                               "Unable to load IP filter.");
               return false;
            }
      
         std::string filtername;

         switch (_dd.filterType)
            {
            case IpFilterIf::IPF_LEVELONE:
               {
                  VERBOSE_LOG(_logwrapper, 
                              _verboseFlag, "Loading IP filter, level one.");
                  BTG_MNOTICE(_logwrapper, 
                              "creating filter: IPF_LEVELONE");
                  filtername = "level one";
                  _dd.filter  = new levelOne(_logwrapper, _verboseFlag, _dd.filterFilename);
                  break;
               }
            case IpFilterIf::IPF_EMULE:
               {
                  VERBOSE_LOG(_logwrapper, 
                              _verboseFlag, "Loading IP filter, emule.");
                  BTG_MNOTICE(_logwrapper, 
                              "creating filter: IPF_EMULE");
                  filtername = "emule";
                  _dd.filter  = new Emule(_logwrapper, _verboseFlag, _dd.filterFilename);
                  break;
               }
            case IpFilterIf::IPF_UNDEF:
               {
                  VERBOSE_LOG(_logwrapper, 
                              _verboseFlag, "Loading IP filter disabled.");
                  _dd.filter = 0;
                  break;
               }
            }

         if (_dd.filter)
            {
               if ((!_dd.filter->initialized()) || (_dd.filter->numberOfEntries() == 0))
                  {
                     BTG_MNOTICE(_logwrapper, 
                                 "filter not initialized or empty. Deleted.");

                     delete _dd.filter;
                     _dd.filter = 0;
                  }
               else
                  {
                     BTG_MNOTICE(_logwrapper, 
                                 "IP filter '" << filtername << "' created, contains "
                                 << _dd.filter->numberOfEntries() << " entries.");
                     VERBOSE_LOG(_logwrapper, 
                                 _verboseFlag,
                                 "IP filter '" << filtername << "' created, contains "
                                 << _dd.filter->numberOfEntries() << " entries.");
                  }
            }
      }

   return true;
}

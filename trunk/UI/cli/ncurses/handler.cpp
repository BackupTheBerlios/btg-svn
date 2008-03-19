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

#include "handler.h"

#include <iostream>

#include <bcore/command_factory.h>
#include <bcore/util.h>
#include <bcore/t_string.h>

#include <bcore/hru.h>
#include <bcore/hrr.h>

#include <bcore/os/stdin.h>

#include <bcore/command/context.h>
#include <bcore/command/context_abort.h>
#include <bcore/command/context_clean.h>
#include <bcore/command/context_fi.h>
#include <bcore/command/context_fi_rsp.h>
#include <bcore/command/context_li_status.h>
#include <bcore/command/context_li_status_rsp.h>
#include <bcore/command/context_limit.h>
#include <bcore/command/context_peers.h>
#include <bcore/command/context_peers_rsp.h>
#include <bcore/command/context_start.h>
#include <bcore/command/context_status.h>
#include <bcore/command/context_status_rsp.h>
#include <bcore/command/context_stop.h>
#include <bcore/command/error.h>

#include <bcore/command/limit_base.h>

namespace btg
{
   namespace UI
   {
      namespace cli
      {

         using namespace btg::core;
         using namespace btg::core::client;

         Handler::Handler(btg::core::LogWrapperType _logwrapper,
                          btg::core::externalization::Externalization* _e,
                          messageTransport*                       _transport,
                          clientConfiguration*                    _config,
                          btg::core::client::lastFiles*           _lastfiles,
                          bool const _verboseFlag,
                          bool const _autoStartFlag)
            : handlerThreadIf(_logwrapper,
                              _e,
                              _transport,
                              _config,
                              _lastfiles,
                              _verboseFlag,
                              _autoStartFlag),
              lastSelected_files_()
         {
         }

         void Handler::onTransportInit()
         {

         }

         void Handler::onSetup(t_long const _session)
         {
            setSession(_session);
            this->setupDone = true;
         }

         void Handler::onSetupError(std::string const& _message)
         {
            setSession(ILLEGAL_ID);
            setupFailtureMessage = _message;
            setupDone            = false;
         }

         void Handler::onAttach()
         {
            this->attachDone = true;
         }

         void Handler::onAttachError(std::string const& _message)
         {
            setSession(ILLEGAL_ID);
            attachFailtureMessage = _message;
            attachDone            = false;
         }

         void Handler::onDetach()
         {
            //setOutput("Detaching current session");
            //global_btg_run = GR_QUIT;
         }

         void Handler::onList(t_intList const& _contextIDs, t_strList const& _filenames)
         {
            commandStatus = true;
         }

         void Handler::onListError()
         {
            commandStatus = false;
            this->cmd_failture++;
         }

         void Handler::onError(std::string const& _errorDescription)
         {
            commandStatus = false;

            this->cmd_failture++;

            BTG_NOTICE(logWrapper(), 
                       "On error: " << _errorDescription);
         }

         void Handler::onFatalError(std::string const& _errorDescription)
         {
            commandStatus = false;
         }

         void Handler::onQuit()
         {
            commandStatus = true;
         }

         void Handler::onKill()
         {
            commandStatus = true;
         }

         void Handler::onKillError(std::string _ErrorDescription)
         {
            commandStatus = false;
         }

         void Handler::onUptime(t_ulong const _uptime)
         {
            commandStatus = true;
         }

         void Handler::onSessionName(std::string const& _name)
         {
            commandStatus = true;
            currentSessionName = _name;
         }

         void Handler::onSetSessionName()
         {
            commandStatus = true;
         }

         void Handler::onCreateWithData()
         {
            commandStatus = true;

            lastfiles->addLastFile(last_filename);
            last_filename.clear();
         }

         void Handler::onAbort()
         {
            commandStatus = true;
         }

         void Handler::onStart()
         {
            commandStatus = true;
         }

         void Handler::onStop()
         {
            commandStatus = true;
         }

         void Handler::onStatus(btg::core::Status const& _status)
         {
            commandStatus = true;
            statusSize_         = 1;
         }

         void Handler::onStatusAll(t_statusList const& _vstatus)
         {
            statusList_        = _vstatus;
            statusSize_        = _vstatus.size();
            statusListUpdated_ = true;
         }

                  void Handler::onStatusError(std::string const& _errorDescription)
         {
            commandStatus = false;
            statusSize_         = 0;
            this->cmd_failture++;
         }

         void Handler::onFileInfo(t_fileInfoList const& _fileinfolist)
         {
            lastFileinfolist_   = _fileinfolist;
            commandStatus = true;
         }

         void Handler::onFileInfoError(std::string const& _errorDescription)
         {
            commandStatus = false;
         }

         void Handler::onPeers(t_peerList const& _peerlist)
         {
            commandStatus = true;
            lastPeerlist_.clear();
            lastPeerlist_       = _peerlist;
         }

         void Handler::onPeersError(std::string const& _errorDescription)
         {
            commandStatus = false;
            this->cmd_failture++;
         }

         void Handler::onListSessions(t_longList const& _sessions,
                                      t_strList const& _sessionNames)
         {
            commandStatus = true;
            sessionList         = _sessions;
            sessionNames        = _sessionNames;
         }

         void Handler::onSessionError()
         {
            commandStatus = false;
            this->cmd_failture++;
         }

         void Handler::onClean(t_strList const& _filenames, t_intList const& _contextIDs)
         {
            commandStatus = true;
         }

         void Handler::onListSessionsError(std::string const& _errorDescription)
         {
            commandStatus = false;
            this->cmd_failture++;

            // std::cout << _errorDescription << std::endl;
         }

         void Handler::getLastFileInfoList(t_fileInfoList & _fileInfoList) const
         {
            _fileInfoList = lastFileinfolist_;
         }

         void Handler::getLastPeers(t_peerList & _peerlist) const
         {
            _peerlist = lastPeerlist_;
         }

         void Handler::onSetFiles()
         {
            commandStatus = true;
         }

         void Handler::onMove()
         {
            commandStatus = true;
         }

         void Handler::onSetFilesError(std::string const& _errorDescription)
         {
            commandStatus = false;
            this->cmd_failture++;
         }
         
         void Handler::onSelectedFiles(btg::core::selectedFileEntryList const& _files)
         {
            commandStatus = true;
            lastSelected_files_ = _files;
         }
         
         void Handler::onSelectedFilesError(std::string const& _errorDescription)
         {
            commandStatus = false;
            this->cmd_failture++;
         }

         void Handler::getLastSelectedFiles(btg::core::selectedFileEntryList & _selected_files) const
         {
            _selected_files = lastSelected_files_;
         }

         void Handler::onSessionInfo(bool const _encryption, bool const _dht)
         {
            dht_enabled_        = _dht;
            encryption_enabled_ = _encryption;
         }

         void Handler::onCreateFromUrl(t_uint const _id)
         {

         }

         void Handler::onCreateFromUrlError(std::string const& _message)
         {

         }

         void Handler::onUrlStatus(t_uint const _id, 
                                   btg::core::urlStatus const _status)
         {

         }

         void Handler::onUrlStatusError(std::string const& _message)
         {

         }

         Handler::~Handler()
         {
         }

         /* */
         /* */
         /* */

         ncliStartupHelper::ncliStartupHelper(btg::core::LogWrapperType _logwrapper,
                                              btg::core::client::clientConfiguration*        _config,
                                              btg::core::client::commandLineArgumentHandler* _clah,
                                              btg::core::messageTransport*                   _messageTransport,
                                              btg::core::client::clientHandler*              _handler)
            : btg::core::client::startupHelper(_logwrapper,
                                               GPD->sCLI_CLIENT(),
                                               _config,
                                               _clah,
                                               _messageTransport,
                                               _handler)
         {
         }

         t_long ncliStartupHelper::queryUserAboutSession(t_longList const& _sessions,
                                                         t_strList const& _sessionsIDs) const
         {
            std::cout << "Session(s):" << std::endl;

            t_strListCI sessionIdIter = _sessionsIDs.begin();
            t_int session_no          = 0;
            for (t_longListCI vlci = _sessions.begin();
                 vlci != _sessions.end();
                 vlci++)
               {
                  std::cout << session_no << ": " << *vlci << " (" << *sessionIdIter << ")" << std::endl;
                  sessionIdIter++;
                  session_no++;
               }

            // Get user input.
            bool done = false;
            t_int input = -1;
            while (done != true)
               {
                  if ((session_no-1) > 0)
                     {
                        std::cout << "Enter a number (0-" << (session_no-1) << "): " << std::endl;
                     }
                  else
                     {
                        std::cout << "Enter a number: " << std::endl;
                     }

                  if(!(std::cin >> input))
                     {
                        std::cin.clear();
                        std::cin.ignore(254, '\n');
                     }
                  else
                     {
                        if (input >= 0 && input < session_no)
                           {
                              done = true;
                           }
                     }
               }

            return _sessions.at(input);
         }

         bool ncliStartupHelper::authUserQuery()
         {
            bool status = true;

            std::cout << "Username:" << std::endl;

            std::string username;

            std::cin >> username;

            std::cout << "Password:" << std::endl;

            std::string password;

            if (!btg::core::os::stdIn::getPassword(password))
               {
                  std::cout << "Unable to get password." << std::endl;

                  status = false;
                  return status;
               }

            setAuth(username, password);

            return status;
         }

         void ncliStartupHelper::showSessions(t_longList const& _sessions,
                                              t_strList const& _sessionNames) const
         {
            t_strListCI sessionNameIter = _sessionNames.begin();

            if (_sessions.size() > 0)
               {
                  std::cout << "Session(s):" << std::endl;

                  for (t_longListCI vlci = _sessions.begin();
                       vlci != _sessions.end();
                       vlci++)
                     {
                        std::cout << *vlci << " (" << *sessionNameIter << ")" << std::endl;
                        sessionNameIter++;
                     }
               }
            else
               {
                  std::cout << "No sessions to list." << std::endl;
               }
         }

         ncliStartupHelper::~ncliStartupHelper()
         {

         }

      } // namespace cli
   } // namespace UI
} // namespace btg

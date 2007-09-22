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

         Handler::Handler(btg::core::externalization::Externalization* _e,
                          messageTransport*                       _transport,
                          clientConfiguration*                    _config,
                          btg::core::client::lastFiles*           _lastfiles,
                          bool const _verboseFlag,
                          bool const _autoStartFlag)
            : handlerThreadIf(_e,
                              _transport,
                              _config,
                              _lastfiles,
                              _verboseFlag,
                              _autoStartFlag),
              lastCommandSuccess_(false),
              last_uploadRate(limitBase::LIMIT_DISABLED),
              last_downloadRate(limitBase::LIMIT_DISABLED),
              last_seedLimit(limitBase::LIMIT_DISABLED),
              last_seedTimeout(limitBase::LIMIT_DISABLED),
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
            lastCommandSuccess_ = true;
         }

         void Handler::onListError()
         {
            lastCommandSuccess_ = false;
            this->cmd_failture++;
         }

         void Handler::onError(std::string const& _errorDescription)
         {
            lastCommandSuccess_ = false;

            this->cmd_failture++;

            BTG_NOTICE("On error: " << _errorDescription);
         }

         void Handler::onFatalError(std::string const& _errorDescription)
         {
            lastCommandSuccess_ = false;
         }

         void Handler::onQuit()
         {
            lastCommandSuccess_ = true;
         }

         void Handler::onKill()
         {
            lastCommandSuccess_ = true;
         }

         void Handler::onKillError(std::string _ErrorDescription)
         {
            lastCommandSuccess_ = false;
         }

         void Handler::onUptime(t_ulong const _uptime)
         {
            lastCommandSuccess_ = true;
         }

         void Handler::onGlobalLimit()
         {
            lastCommandSuccess_ = true;
         }

         void Handler::onGlobalLimitError(std::string _ErrorDescription)
         {
            lastCommandSuccess_ = false;
         }

         void Handler::onGlobalLimitResponse(t_int const  _limitBytesUpld,
                                             t_int const  _limitBytesDwnld,
                                             t_int const  _maxUplds,
                                             t_long const _maxConnections)
         {
            last_gl_limitBytesUpld  = _limitBytesUpld;
            last_gl_limitBytesDwnld = _limitBytesDwnld;
            last_gl_maxUplds        = _maxUplds;
            last_gl_maxConnections  = _maxConnections;

            lastCommandSuccess_ = true;
         }

         void Handler::onGlobalLimitResponseError(std::string _ErrorDescription)
         {
            lastCommandSuccess_ = false;
         }


         void Handler::onCreateWithData()
         {
            lastCommandSuccess_ = true;

            lastfiles->addLastFile(last_filename);
            last_filename.clear();
         }

         void Handler::onAbort()
         {
            lastCommandSuccess_ = true;
         }

         void Handler::onStart()
         {
            lastCommandSuccess_ = true;
         }

         void Handler::onStop()
         {
            lastCommandSuccess_ = true;
         }

         void Handler::onStatus(btg::core::Status const& _status)
         {
            lastCommandSuccess_ = true;
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
            lastCommandSuccess_ = false;
            statusSize_         = 0;
            this->cmd_failture++;
         }

         void Handler::onFileInfo(t_fileInfoList const& _fileinfolist)
         {
            lastFileinfolist_   = _fileinfolist;
            lastCommandSuccess_ = true;
         }

         void Handler::onFileInfoError(std::string const& _errorDescription)
         {
            lastCommandSuccess_ = false;
         }

         void Handler::onPeers(t_peerList const& _peerlist)
         {
            lastCommandSuccess_ = true;
            lastPeerlist_.clear();
            lastPeerlist_       = _peerlist;
         }

         void Handler::onPeersError(std::string const& _errorDescription)
         {
            lastCommandSuccess_ = false;
            this->cmd_failture++;
         }

         void Handler::onLimit()
         {
            lastCommandSuccess_ = true;
         }

         void Handler::onListSessions(t_longList const& _sessions)
         {
            lastCommandSuccess_ = true;
            sessionList         = _sessions;
         }

         void Handler::onSessionError()
         {
            lastCommandSuccess_ = false;
            this->cmd_failture++;
         }

         void Handler::onClean(t_strList const& _filenames, t_intList const& _contextIDs)
         {
            lastCommandSuccess_ = true;
         }

         void Handler::onListSessionsError(std::string const& _errorDescription)
         {
            lastCommandSuccess_ = false;
            this->cmd_failture++;

            // std::cout << _errorDescription << std::endl;
         }

         void Handler::onLimitStatus(t_int const _uploadRate, t_int const _downloadRate, t_int const _seedLimit, t_long const _seedTimeout)
         {
            lastCommandSuccess_ = true;
            last_uploadRate     = _uploadRate;
            last_downloadRate   = _downloadRate;
            last_seedLimit      = _seedLimit;
            last_seedTimeout    = _seedTimeout;
         }

         void Handler::onLimitStatusError(std::string const& _errorDescription)
         {
            lastCommandSuccess_ = false;
            this->cmd_failture++;
         }

         bool Handler::lastCommandSuccess() const
         {
            return lastCommandSuccess_;
         }

         void Handler::getLastLimitStatus(t_int & _uploadRate, 
                                          t_int & _downloadRate, 
                                          t_int & _seedLimit, 
                                          t_int & _seedTimeout) const
         {
            _uploadRate   = last_uploadRate;
            _downloadRate = last_downloadRate;
            _seedLimit    = last_seedLimit;
            _seedTimeout  = last_seedTimeout;
         }

         void Handler::getLastGlobalLimitStatus(t_int & _gl_limitBytesUpld,
                                                t_int & _gl_limitBytesDwnld,
                                                t_int & _gl_maxUplds,
                                                t_int & _gl_maxConnections) const
         {
            _gl_limitBytesUpld  = last_gl_limitBytesUpld;
            _gl_limitBytesDwnld = last_gl_limitBytesDwnld;
            _gl_maxUplds        = last_gl_maxUplds;
            _gl_maxConnections  = last_gl_maxConnections;
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

         }

         void Handler::onSetFilesError(std::string const& _errorDescription)
         {
            lastCommandSuccess_ = false;
            this->cmd_failture++;
         }
         
         void Handler::onSelectedFiles(btg::core::selectedFileEntryList const& _files)
         {
            lastSelected_files_ = _files;
         }
         
         void Handler::onSelectedFilesError(std::string const& _errorDescription)
         {
            lastCommandSuccess_ = false;
            this->cmd_failture++;
         }

         void Handler::getLastSelectedFiles(btg::core::selectedFileEntryList & _selected_files) const
         {
            _selected_files = lastSelected_files_;
         }

         Handler::~Handler()
         {
         }

         /* */
         /* */
         /* */

         ncliStartupHelper::ncliStartupHelper(btg::core::client::clientConfiguration*        _config,
                                              btg::core::client::commandLineArgumentHandler* _clah,
                                              btg::core::messageTransport*                   _messageTransport,
                                              btg::core::client::clientHandler*              _handler)
            : btg::core::client::startupHelper(GPD->sCLI_CLIENT(),
                                               _config,
                                               _clah,
                                               _messageTransport,
                                               _handler)
         {
         }

         t_long ncliStartupHelper::queryUserAboutSession(t_longList const& _sessions) const
         {
            std::cout << "Session(s):" << std::endl;

            t_int session_no = 0;
            for (t_longListCI vlci = _sessions.begin();
                 vlci != _sessions.end();
                 vlci++)
               {
                  std::cout << session_no << ": " << *vlci << std::endl;
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

         void ncliStartupHelper::showSessions(t_longList const& _sessions) const
         {
            if (_sessions.size() > 0)
               {
                  std::cout << "Session(s):" << std::endl;

                  for (t_longListCI vlci = _sessions.begin();
                       vlci != _sessions.end();
                       vlci++)
                     {
                        std::cout << *vlci << std::endl;
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

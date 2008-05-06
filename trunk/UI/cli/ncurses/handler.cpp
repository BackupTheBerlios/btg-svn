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
                          btg::core::externalization::Externalization& _e,
                          messageTransport&                       _transport,
                          clientConfiguration&                    _config,
                          clientDynConfig&                        _dynconfig,
                          bool const _verboseFlag,
                          bool const _autoStartFlag)
            : handlerThreadIf(_logwrapper,
                              _e,
                              _transport,
                              _config,
                              _dynconfig,
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
            setupDone = true;
         }

         void Handler::onSetupError(std::string const& _message)
         {
            setSession(ILLEGAL_ID);
            setupFailtureMessage = _message;
            setupDone            = false;
         }

         void Handler::onAttach()
         {
            attachDone = true;
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
            cmd_failture++;
         }

         void Handler::onError(std::string const& _errorDescription)
         {
            commandStatus = false;

            cmd_failture++;

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
            lastfiles.add(last_filename);
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
            cmd_failture++;
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
            cmd_failture++;
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
            cmd_failture++;
         }

         void Handler::onClean(t_strList const& _filenames, t_intList const& _contextIDs)
         {
            commandStatus = true;
         }

         void Handler::onListSessionsError(std::string const& _errorDescription)
         {
            commandStatus = false;
            cmd_failture++;

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

         void Handler::onVersion(btg::core::OptionBase const& _ob)
         {
            setOption(_ob);
            commandStatus = true;
         }

         void Handler::onMove()
         {
            commandStatus = true;
         }

         void Handler::onSetFilesError(std::string const& _errorDescription)
         {
            commandStatus = false;
            cmd_failture++;
         }
         
         void Handler::onSelectedFiles(btg::core::selectedFileEntryList const& _files)
         {
            commandStatus = true;
            lastSelected_files_ = _files;
         }
         
         void Handler::onSelectedFilesError(std::string const& _errorDescription)
         {
            commandStatus = false;
            cmd_failture++;
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

         void Handler::onTrackerInfo(t_strList const& _trackerlist)
         {
            commandStatus = true;
            setTrackerList(_trackerlist);
         }

         void Handler::onCreateFromUrl(t_uint const _id)
         {
            commandStatus = true;
            setUrlId(_id);
         }

         void Handler::onCreateFromUrlError(std::string const& _message)
         {
            commandStatus = false;
         }

         void Handler::onFileStatus(t_uint const _id, 
                                    btg::core::fileStatus const _status)
         {
            commandStatus = true;
            setFileStatusResponse(_id, _status);
         }

         void Handler::onFileStatusError(std::string const& _errorDescription)
         {
            commandStatus = false;
         }

         void Handler::onUrlStatus(t_uint const _id, 
                                   btg::core::urlStatus const _status)
         {
            commandStatus = true;
            setUrlStatusResponse(_id, _status);
         }

         void Handler::onUrlStatusError(std::string const& _message)
         {
            commandStatus = false;
         }

         void Handler::onUrlDlProgress(t_uint const, t_float _dltotal, t_float _dlnow, t_float _dlspeed)
         {
            // TODO this
         }
         
         void Handler::onCreateFromFile(t_uint const _id)
         {
            commandStatus = true;
            setFileId(_id);
         }

         void Handler::onCreateFromFileError(std::string const& _errorDescription)
         {
            commandStatus = false;
         }

         void Handler::OnCreateFromFilePart()
         {
            commandStatus = true;
         }

         void Handler::OnCreateFromFilePartError(std::string const& _errorDescription)
         {
            commandStatus = false;
         }

         void Handler::onFileCancel()
         {
            
         } 

         void Handler::onFileCancelError(std::string const& _errorDescription)
         {

         }

         void Handler::onUrlCancel()
         {
            
         }

         void Handler::onUrlCancelError(std::string const& _errorDescription)
         {
            
         }

         Handler::~Handler()
         {
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

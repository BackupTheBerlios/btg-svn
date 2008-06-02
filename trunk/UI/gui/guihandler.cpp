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

#include "guihandler.h"

#include <bcore/util.h>
#include <bcore/t_string.h>

#include <gtkmm/main.h>

#include "sessionselection.h"
#include "mainstatusbar.h"

#include "authdialog.h"

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         using namespace btg::core;
         using namespace btg::core::client;

         guiHandler::guiHandler(btg::core::LogWrapperType _logwrapper,
                                btg::core::externalization::Externalization& _e,
                                messageTransport&             _transport,
                                clientConfiguration&          _config,
                                clientDynConfig&              _dynconfig,
                                bool const                    _verboseFlag,
                                bool const                    _autoStartFlag,
                                mainStatusbar*                _pstatus_bar
                                )
            : handlerThreadIf(_logwrapper,
                              _e,
                              _transport,
                              _config,
                              _dynconfig,
                              _verboseFlag,
                              _autoStartFlag),
              contextIDs(0),
              filenames(0),
              idsToRemove(0),
              idToFilenameMap(),
              fileinfolist(0),
              cleanedFilenames(0),
              pstatus_bar(_pstatus_bar),
              peerlist(0),
              UrlDlProgress(false)
         {
         }

         void guiHandler::onTransportInit()
         {
         }

         void guiHandler::onSetup(t_long const _session)
         {
            setSession(_session);
            setupDone = true;
         }

         void guiHandler::onSetupError(std::string const& _message)
         {
            setSession(ILLEGAL_ID);
            setupFailtureMessage = _message;
            setupDone            = false;
         }

         void guiHandler::onList(t_intList const& _contextIDs, t_strList const& _filenames)
         {
            contextIDs = _contextIDs;

            // Map context to filename.
            idToFilenameMap.clear();
            if (_contextIDs.size() == _filenames.size())
               {
                  t_strListCI j = _filenames.begin();
                  for (t_intListCI i=_contextIDs.begin();
                       i != _contextIDs.end(); i++)
                     {
                        idToFilenameMap[*i] = *j;
                        j++;
                     }
               }

            commandStatus = true;
         }

         t_intList guiHandler::getListResult()
         {
            commandStatus = false;
            return contextIDs;
         }

         void guiHandler::onListError()
         {
            commandStatus = false;
         }

         void guiHandler::onError(std::string const& _errorDescription)
         {
            commandStatus = false;
            BTG_NOTICE(logWrapper(), "Error: " << _errorDescription);
         }

         void guiHandler::onFatalError(std::string const& _errorDescription)
         {
            commandStatus = false;
            setFatalError();

            BTG_NOTICE(logWrapper(), "Fatal Error: " << _errorDescription);
         }

         void guiHandler::onCreateWithData()
         {
            commandStatus = true;
            lastfiles.add(last_filename);
            BTG_NOTICE(logWrapper(), 
                       "Added a new torrent with data, filename = " << last_filename << ".");
            last_filename.clear();
         }
         
         void guiHandler::onCreateFromUrl(t_uint const _id)
         {
            commandStatus = true;
            setUrlId(_id);
         }
         
         void guiHandler::onCreateFromUrlError(std::string const& _message)
         {
            commandStatus = false;
         }
         
         void guiHandler::onFileStatus(t_uint const _id, 
                                       btg::core::fileStatus const _status)
         {
            commandStatus = true;
            setFileStatusResponse(_id, _status);
         }
         
         void guiHandler::onFileStatusError(std::string const& _errorDescription)
         {
            commandStatus = false;
         }

         void guiHandler::onUrlStatus(t_uint const _id, 
                                      btg::core::urlStatus const _status)
         {
            commandStatus = true;
            UrlDlProgress = false;
            setUrlStatusResponse(_id, _status);
         }
         
         void guiHandler::onUrlStatusError(std::string const& _message)
         {
            commandStatus = false;
            UrlDlProgress = false;
         }

         void guiHandler::onUrlDlProgress(t_uint const, t_uint _dltotal, t_uint _dlnow, t_uint _dlspeed)
         {
            UrlDlProgress = true;
            UrlDlTotal = _dltotal;
            UrlDlNow = _dlnow;
            UrlDlSpeed = _dlspeed;
         }
         
         void guiHandler::onCreateFromFile(t_uint const _id)
         {
            commandStatus = true;
            setFileId(_id);
         }

         void guiHandler::onCreateFromFileError(std::string const& _errorDescription)
         {
            commandStatus = false;
            BTG_NOTICE(logWrapper(), 
                       "Error: " << _errorDescription);
         }

         void guiHandler::OnCreateFromFilePart()
         {
            commandStatus = true;
         }

         void guiHandler::OnCreateFromFilePartError(std::string const& _errorDescription)
         {
            commandStatus = false;
            BTG_NOTICE(logWrapper(), 
                       "Error: " << _errorDescription);
         }
         
         void guiHandler::onFileCancel()
         {
            commandStatus = true;
         }

         void guiHandler::onFileCancelError(std::string const& _errorDescription)
         {
            commandStatus = false;
         }

         void guiHandler::onUrlCancel()
         {
            commandStatus = true;
         }

         void guiHandler::onUrlCancelError(std::string const& _errorDescription)
         {
            commandStatus = false;
         }

         void guiHandler::onAbort()
         {
            BTG_NOTICE(logWrapper(), 
                       "onAbort, id=" << last_id << ".");
            commandStatus = true;
            idsToRemove.push_back(last_id);
            last_id = ILLEGAL_ID;
         }

         void guiHandler::onStart()
         {
            // A context was started.
            // Nothing to be done.
            commandStatus = true;
         }

         void guiHandler::onStop()
         {
            // A context was stopped.
            // Nothing to be done.
            commandStatus = true;
         }

         void guiHandler::onStatus(btg::core::Status const& _status)
         {
            commandStatus = true;
            last_status   = _status;
         }

         void guiHandler::onStatusAll(t_statusList const& _vstatus)
         {
            // Map context to filename.
            idToFilenameMap.clear();
            for (t_statusListCI iter = _vstatus.begin();
                 iter != _vstatus.end();
                 iter++)
               {
                  idToFilenameMap[iter->contextID()] = iter->filename();
               }

            statusList_        = _vstatus;
            statusSize_        = _vstatus.size();
            statusListUpdated_ = true;

            // commandStatus   = true;
         }

         void guiHandler::onStatusError(std::string const& _errorDescription)
         {
            BTG_NOTICE(logWrapper(), 
                       "Error: " << _errorDescription);

            statusSize_ = 0;
            commandStatus = false;
         }

         void guiHandler::onFileInfo(t_fileInfoList const& _fileinfolist)
         {
            commandStatus = true;
            fileinfolist  = _fileinfolist;
         }

         void guiHandler::onFileInfoError(std::string const& _errorDescription)
         {
            commandStatus = false;
            fileinfolist.clear();
            BTG_NOTICE(logWrapper(), 
                       "Error: " << _errorDescription);
         }

         void guiHandler::onPeers(t_peerList const& _peerlist)
         {
            commandStatus = true;
            peerlist      = _peerlist;
            peerEx = false;
         }

         void guiHandler::onPeersEx(t_uint _offset, t_peerExList const& _peerExList)
         {
            peerEx = true;
            peerExList = _peerExList;
            peerExOffset = _offset;
         }

         void guiHandler::onPeersError(std::string const& _errorDescription)
         {
            commandStatus = false;
            BTG_NOTICE(logWrapper(), 
                       "Error: " << _errorDescription);
         }

         void guiHandler::onClean(t_strList const& _filenames, t_intList const& _contextIDs)
         {
            commandStatus = true;
            for (t_intListCI iter=_contextIDs.begin();
                 iter != _contextIDs.end();
                 iter++)
               {
                  BTG_NOTICE(logWrapper(), 
                             "Removing context ID=" << *iter);
                  idsToRemove.push_back(*iter);
               }

            for (t_strListCI vsci = _filenames.begin();
                 vsci != _filenames.end();
                 vsci++)
               {
                  // Since the torrent finished downloading, remove it
                  // from the list of last opened files.
                  lastfiles.remove(*vsci);
               }

            cleanedFilenames = _filenames;
         }

         void guiHandler::onAttach()
         {
            attachDone = true;
         }

         void guiHandler::onAttachError(std::string const& _message)
         {
            BTG_NOTICE(logWrapper(), 
                       "guiHandler::onAttachError:" << _message);
            setSession(ILLEGAL_ID);
            attachFailtureMessage = _message;
            attachDone            = false;
         }

         void guiHandler::onListSessions(t_longList const& _sessions, 
                                         t_strList const& _sessionNames)
         {
            sessionList  = _sessions;
            sessionNames = _sessionNames;
         }

         void guiHandler::onListSessionsError(std::string const& _errorDescription)
         {
            BTG_FATAL_ERROR(logWrapper(),
                            btg::core::projectDefaults::sGUI_CLIENT(), _errorDescription);
         }

         void guiHandler::onSessionError()
         {
            BTG_FATAL_ERROR(logWrapper(), 
                            btg::core::projectDefaults::sGUI_CLIENT(), "Invalid session. Quitting.");
            setSessionError();
         }

         void guiHandler::onDetach()
         {
            commandStatus = true;
         }

         void guiHandler::onQuit()
         {
            commandStatus = true;
         }

         void guiHandler::onKill()
         {
            commandStatus = true;
         }

         void guiHandler::onKillError(std::string _ErrorDescription)
         {
            commandStatus = false;
         }

         void guiHandler::onUptime(t_ulong const _uptime)
         {
            commandStatus = true;
            if (pstatus_bar != 0)
               {
                  pstatus_bar->set("Daemon uptime: " + convertToString<t_ulong>(_uptime) + " sec.");
               }
         }

         void guiHandler::onSessionName(std::string const& _name)
         {
            commandStatus = true;
            currentSessionName = _name;
         }

         void guiHandler::onSetSessionName()
         {
            commandStatus = true;
         }

         t_intList guiHandler::getIdsToRemove()
         {
            t_intList ids = idsToRemove;
            idsToRemove.clear();
            return ids;
         }

         t_fileInfoList guiHandler::getFileInfoList() const
         {
            return fileinfolist;
         }

         bool guiHandler::idToFilename(t_int const _id, std::string & _filename)
         {
            bool status = false;
            t_intStrMapCI iter = idToFilenameMap.find(_id);

            if (iter != idToFilenameMap.end())
               {
                  _filename = iter->second;
                  status    = true;
               }

            return status;
         }

         t_strList const& guiHandler::getLastFiles() const
         {
            return lastfiles.get();
         }
         
         t_strList const& guiHandler::getLastURLs() const
         {
            return lasturls.getURLs();
         }
         
         t_strList const& guiHandler::getLastURLFiles() const
         {
            return lasturls.getFiles();
         }

         t_strList guiHandler::getCleanedFilenames()
         {
            t_strList temp_cfn = cleanedFilenames;
            cleanedFilenames.clear();
            return temp_cfn;
         }

         void guiHandler::setStatusBar(mainStatusbar* _pstatus_bar)
         {
            pstatus_bar = _pstatus_bar;
         }

         void guiHandler::onSetFiles()
         {
            commandStatus = true;
         }
         
         void guiHandler::onMove()
         {
            BTG_NOTICE(logWrapper(), 
                       "onMove, id=" << last_id << ".");
            commandStatus = true;
            idsToRemove.push_back(last_id);
            last_id = ILLEGAL_ID;
         }

         void guiHandler::onVersion(btg::core::OptionBase const& _ob)
         {
            setOption(_ob);
            commandStatus = true;
         }

         void guiHandler::onSetFilesError(std::string const& _errorDescription)
         {
            commandStatus = false;
         }

         void guiHandler::onSelectedFiles(btg::core::selectedFileEntryList const& _files)
         {
            commandStatus = true;
            selected_files = _files;
         }
         
         void guiHandler::onSelectedFilesError(std::string const& _errorDescription)
         {
            commandStatus = false;
         }

         btg::core::selectedFileEntryList guiHandler::getLastSelectedFiles() const
         {
            return selected_files;
         }
         
         bool guiHandler::getUrlDlProgress(t_uint &_total, t_uint &_now, t_uint &_speed)
         {
            _total = UrlDlTotal;
            _now = UrlDlNow;
            _speed = UrlDlSpeed;
            return UrlDlProgress;
         }

         void guiHandler::onSessionInfo(bool const _encryption, bool const _dht)
         {
            dht_enabled_        = _dht;
            encryption_enabled_ = _encryption;
         }
         
         void guiHandler::onTrackerInfo(t_strList const& _trackerlist)
         {
            commandStatus = true;
            setTrackerList(_trackerlist);
         }
         
         guiHandler::~guiHandler()
         {

         }

      } // namespace gui
   } // namespace UI
} // namespace btg

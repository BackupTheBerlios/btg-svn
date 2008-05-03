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

#include <bcore/util.h>
#include <bcore/t_string.h>

#include "ui.h"

namespace btg
{
   namespace UI
   {
      namespace gui
      {
         namespace viewer
         {

            using namespace btg::core;
            using namespace btg::core::client;

            viewerHandler::viewerHandler(btg::core::LogWrapperType _logwrapper,
                                         btg::core::externalization::Externalization& _e,
                                         messageTransport&             _transport,
                                         clientConfiguration&          _config,
                                         clientDynConfig&              _dynconfig,
                                         bool const                    _verboseFlag,
                                         bool const                    _autoStartFlag,
                                         btgvsGui&                     _gui
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
                 gui(_gui),
                 peerlist(0)
            {

            }

            void viewerHandler::onTransportInit()
            {

            }

            void viewerHandler::onSetup(t_long const _session)
            {
               setSession(_session);
               setupDone = true;
            }

            void viewerHandler::onSetupError(std::string const& _message)
            {
               setSession(ILLEGAL_ID);
               setupFailtureMessage = _message;
               setupDone            = false;
            }

            void viewerHandler::onList(t_intList const& _contextIDs, t_strList const& _filenames)
            {
               contextIDs = _contextIDs;

               idToFilenameMap.clear();

               // Map context to filenames.
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

            t_intList viewerHandler::getListResult()
            {
               commandStatus = false;
               return contextIDs;
            }

            void viewerHandler::onListError()
            {
               commandStatus = false;
            }

            void viewerHandler::onError(std::string const& _errorDescription)
            {
               commandStatus = false;
               BTG_NOTICE(logWrapper(), 
                          "Error: " << _errorDescription);
            }

            void viewerHandler::onFatalError(std::string const& _errorDescription)
            {
               commandStatus = false;
               setFatalError();

               BTG_NOTICE(logWrapper(), "Fatal Error: " << _errorDescription);
            }

            void viewerHandler::onCreateWithData()
            {
               commandStatus = true;
               lastfiles.add(last_filename);
               BTG_NOTICE(logWrapper(),
                          "Added a new torrent with data, filename = " << last_filename << ".");
               last_filename.clear();
            }

            void viewerHandler::onCreateFromUrl(t_uint const _id)
            {

            }
            
            void viewerHandler::onCreateFromUrlError(std::string const& _message)
            {
               
            }
            
            void viewerHandler::onFileStatus(t_uint const _id, 
                                             btg::core::fileStatus const _status)
            {
               // Not used.
            }

            void viewerHandler::onFileStatusError(std::string const& _errorDescription)
            {
               // Not used.
            }

            void viewerHandler::onUrlStatus(t_uint const _id, 
                                            btg::core::urlStatus const _status)
            {
               
            }
            
            void viewerHandler::onUrlStatusError(std::string const& _message)
            {
               
            }
            
            void viewerHandler::onUrlDlProgress(t_uint const _id,
                                                t_float _dltotal, 
                                                t_float _dlnow, 
                                                t_float _dlspeed)
            {
               
            }

            void viewerHandler::onCreateFromFile(t_uint const _id)
            {

            }

            void viewerHandler::onCreateFromFileError(std::string const& _errorDescription)
            {

            }

            void viewerHandler::OnCreateFromFilePart()
            {

            }

            void viewerHandler::OnCreateFromFilePartError(std::string const& _errorDescription)
            {

            }
            
            void viewerHandler::onFileCancel()
            {
               
            }
 
            void viewerHandler::onFileCancelError(std::string const& _errorDescription)
            {
               
            }

            void viewerHandler::onUrlCancel()
            {
               
            }

            void viewerHandler::onUrlCancelError(std::string const& _errorDescription)
            {
               
            }

            void viewerHandler::onAbort()
            {
               BTG_NOTICE(logWrapper(),
                          "onAbort, id=" << last_id << ".");
               commandStatus = true;
               idsToRemove.push_back(last_id);
               last_id = ILLEGAL_ID;
            }

            void viewerHandler::onStart()
            {
               // A context was started.
               // Nothing to be done.
               commandStatus = true;
            }

            void viewerHandler::onStop()
            {
               // A context was stopped.
               // Nothing to be done.
               commandStatus = true;
            }

            void viewerHandler::onStatus(btg::core::Status const& _status)
            {
               commandStatus = true;
               last_status   = _status;
            }

            void viewerHandler::onStatusAll(t_statusList const& _vstatus)
            {
               statusList_        = _vstatus;
               statusSize_        = _vstatus.size();
               statusListUpdated_ = true;

               // commandStatus   = true;
            }

            void viewerHandler::onStatusError(std::string const& _errorDescription)
            {
               BTG_NOTICE(logWrapper(),
                          "Error: " << _errorDescription);

               statusSize_ = 0;
               commandStatus = false;
            }

            void viewerHandler::onFileInfo(t_fileInfoList const& _fileinfolist)
            {
               commandStatus = true;
               fileinfolist  = _fileinfolist;
            }

            void viewerHandler::onFileInfoError(std::string const& _errorDescription)
            {
               commandStatus = false;
               fileinfolist.clear();
               BTG_NOTICE(logWrapper(),
                          "Error: " << _errorDescription);
            }

            void viewerHandler::onPeers(t_peerList const& _peerlist)
            {
               commandStatus = true;
               peerlist      = _peerlist;
            }

            void viewerHandler::onPeersError(std::string const& _errorDescription)
            {
               commandStatus = false;
               BTG_NOTICE(logWrapper(),
                          "Error: " << _errorDescription);
            }

            void viewerHandler::onClean(t_strList const& _filenames, t_intList const& _contextIDs)
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

            void viewerHandler::onAttach()
            {
               attachDone = true;
            }

            void viewerHandler::onAttachError(std::string const& _message)
            {
               BTG_NOTICE(logWrapper(),
                          "viewerHandler::onAttachError:" << _message);
               setSession(ILLEGAL_ID);
               attachFailtureMessage = _message;
               attachDone            = false;
            }

            void viewerHandler::onListSessions(t_longList const& _sessions,
                                               t_strList const& _sessionNames)
            {
               sessionList  = _sessions;
               sessionNames = _sessionNames;
            }

            void viewerHandler::onListSessionsError(std::string const& _errorDescription)
            {
               BTG_FATAL_ERROR(logWrapper(),
                               "btgvs", _errorDescription);
            }

            void viewerHandler::onSessionError()
            {
               BTG_FATAL_ERROR(logWrapper(),
                               "btgvs", "Invalid session. Quitting.");
               setSessionError();
            }

            void viewerHandler::onDetach()
            {
               commandStatus = true;
            }

            void viewerHandler::onQuit()
            {
               commandStatus = true;
            }

            void viewerHandler::onKill()
            {
               commandStatus = true;
            }

            void viewerHandler::onKillError(std::string _ErrorDescription)
            {
               commandStatus = false;
            }

            void viewerHandler::onUptime(t_ulong const _uptime)
            {
               commandStatus = true;
               /*
                 if (status_bar != 0)
                 {
                 status_bar->set("Daemon uptime: " + convertToString<t_ulong>(_uptime) + " sec.");
                 }
               */
            }

            void viewerHandler::onSessionName(std::string const& _name)
            {
               
            }

            void viewerHandler::onSetSessionName()
            {
               
            }
            
            t_intList viewerHandler::getIdsToRemove()
            {
               t_intList ids = idsToRemove;
               idsToRemove.clear();
               return ids;
            }

            t_fileInfoList viewerHandler::getFileInfoList() const
            {
               return fileinfolist;
            }

            bool viewerHandler::idToFilename(t_int const _id, std::string & _filename)
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

            t_strList viewerHandler::getLastFiles() const
            {
               return lastfiles.get();
            }

            t_strList viewerHandler::getCleanedFilenames()
            {
               t_strList temp_cfn = cleanedFilenames;
               cleanedFilenames.clear();
               return temp_cfn;
            }

            t_peerList viewerHandler::getPeers() const
            {
               return peerlist;
            }

            void viewerHandler::onSetFiles()
            {
               commandStatus = true;
            }
         
            void viewerHandler::onMove()
            {
               commandStatus = true;
            }

            void viewerHandler::onVersion(btg::core::OptionBase const& _ob)
            {
               setOption(_ob);
               commandStatus = true;
            }

            void viewerHandler::onSetFilesError(std::string const& _errorDescription)
            {
               commandStatus = false;
            }

            void viewerHandler::onSelectedFiles(btg::core::selectedFileEntryList const& _files)
            {
               commandStatus = true;
               selected_files = _files;
            }
         
            void viewerHandler::onSelectedFilesError(std::string const& _errorDescription)
            {
               commandStatus = false;
            }

            btg::core::selectedFileEntryList viewerHandler::getLastSelectedFiles() const
            {
               return selected_files;
            }
         
            void viewerHandler::onSessionInfo(bool const _encryption, bool const _dht)
            {
               dht_enabled_        = _encryption;
               encryption_enabled_ = _dht;
            }

            viewerHandler::~viewerHandler()
            {

            }

            viewerStartupHelper::viewerStartupHelper(btg::core::LogWrapperType _logwrapper,
                                                     btg::core::client::clientConfiguration& _config,
                                                     vsCommandLineArgumentHandler&           _clah,
                                                     btg::core::messageTransport&            _messageTransport,
                                                     btg::core::client::clientHandler&       _handler)
               : btg::core::client::startupHelper(_logwrapper,
                                                  "btgvs",
                                                  _config,
                                                  _clah,
                                                  _messageTransport,
                                                  _handler,
                                                  *this)
            {

            }

            bool viewerStartupHelper::AuthQuery()
            {
               // Not implemented, auth info is read from config file
               // only.
               return false;
            }

            bool viewerStartupHelper::AttachSessionQuery(t_longList const& _sessionsIDs,
                                                         t_strList const& _sessionNames,
                                                         t_long & _session)
            {
               // Not implemented.
               _session = Command::INVALID_SESSION;
            }
                  
            void viewerStartupHelper::ListSessions(t_longList const& _sessions,
                                                   t_strList const& _sessionNames)
            {
               // Not implemented.
            }
            
            bool viewerStartupHelper::DefaultAction(t_longList const& _sessions,
                                                    t_strList const& _sessionNames,
                                                    bool & _attach,
                                                    t_long & _sessionId)
            {
               // Not implemented.
               return false;
            }

            viewerStartupHelper::~viewerStartupHelper()
            {

            }

         } // namespace viewer
      } // namespace gui
   } // namespace UI
} // namespace btg

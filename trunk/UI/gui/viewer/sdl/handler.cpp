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
            using namespace std;

            viewerHandler::viewerHandler(btg::core::externalization::Externalization* _e,
                                         messageTransport*             _transport,
                                         clientConfiguration*          _config,
                                         btg::core::client::lastFiles* _lastfiles,
                                         bool const                    _verboseFlag,
                                         bool const                    _autoStartFlag,
                                         btgvsGui *                    _gui
                                         )
               : handlerThreadIf(_e,
                                 _transport,
                                 _config,
                                 _lastfiles,
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

            void viewerHandler::onError(string const& _errorDescription)
            {
               commandStatus = false;
               BTG_NOTICE("Error: " << _errorDescription);
            }

            void viewerHandler::onFatalError(std::string const& _errorDescription)
            {
               commandStatus = false;
               setFatalError();

               BTG_NOTICE("Fatal Error: " << _errorDescription);
            }

            void viewerHandler::onCreateWithData()
            {
               commandStatus = true;
               lastfiles->addLastFile(last_filename);
               BTG_NOTICE("Added a new torrent with data, filename = " << last_filename << ".");
               last_filename.clear();
            }

            void viewerHandler::onAbort()
            {
               BTG_NOTICE("onAbort, id=" << last_id << ".");
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
               BTG_NOTICE("Error: " << _errorDescription);

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
               BTG_NOTICE("Error: " << _errorDescription);
            }

            void viewerHandler::onPeers(t_peerList const& _peerlist)
            {
               commandStatus = true;
               peerlist      = _peerlist;
            }

            void viewerHandler::onPeersError(std::string const& _errorDescription)
            {
               commandStatus = false;
               BTG_NOTICE("Error: " << _errorDescription);
            }

            void viewerHandler::onLimit()
            {
               commandStatus = true;
            }

            void viewerHandler::onLimitStatus(t_int const _uploadRate, t_int const _downloadRate,
                                              t_int const _seedLimit, t_long const _seedTimeout)
            {
               commandStatus       = true;
               last_limit_upload   = _uploadRate;
               last_limit_download = _downloadRate;

               last_limit_seed_percent = _seedLimit;
               last_limit_seed_timeout = _seedTimeout;
            }

            void viewerHandler::onLimitStatusError(std::string const& _errorDescription)
            {
               commandStatus       = false;
               last_limit_upload   = 0;
               last_limit_download = 0;

               BTG_NOTICE("Error: " << _errorDescription);
            }

            void viewerHandler::onClean(t_strList const& _filenames, t_intList const& _contextIDs)
            {
               commandStatus = true;
               for (t_intListCI iter=_contextIDs.begin();
                    iter != _contextIDs.end();
                    iter++)
                  {
                     BTG_NOTICE("Removing context ID=" << *iter);
                     idsToRemove.push_back(*iter);
                  }

               for (t_strListCI vsci = _filenames.begin();
                    vsci != _filenames.end();
                    vsci++)
                  {
                     // Since the torrent finished downloading, remove it
                     // from the list of last opened files.
                     lastfiles->removeLastFile(*vsci);
                  }

               cleanedFilenames = _filenames;
            }

            void viewerHandler::onAttach()
            {
               attachDone = true;
            }

            void viewerHandler::onAttachError(string const& _message)
            {
               BTG_NOTICE("viewerHandler::onAttachError:" << _message);
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

            void viewerHandler::onListSessionsError(string const& _errorDescription)
            {
               BTG_FATAL_ERROR("btgvs", _errorDescription);
            }

            void viewerHandler::onSessionError()
            {
               BTG_FATAL_ERROR("btgvs", "Invalid session. Quitting.");
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
            
            void viewerHandler::onGlobalLimit()
            {

            }

            void viewerHandler::onGlobalLimitError(std::string _ErrorDescription)
            {

            }

            void viewerHandler::onGlobalLimitResponse(t_int const  _limitBytesUpld,
                                                      t_int const  _limitBytesDwnld,
                                                      t_int const  _maxUplds,
                                                      t_long const _maxConnections)
            {

            }

            void viewerHandler::onGlobalLimitResponseError(std::string _ErrorDescription)
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

            bool viewerHandler::idToFilename(t_int const _id, string & _filename)
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
               return lastfiles->getLastFiles();
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
         
            viewerHandler::~viewerHandler()
            {

            }

            viewerStartupHelper::viewerStartupHelper(btg::core::client::clientConfiguration*        _config,
                                                     vsCommandLineArgumentHandler* _clah,
                                                     btg::core::messageTransport*                   _messageTransport,
                                                     btg::core::client::clientHandler*              _handler)
               : btg::core::client::startupHelper("btgvs",
                                                  _config,
                                                  _clah,
                                                  _messageTransport,
                                                  _handler)
            {

            }

            t_long viewerStartupHelper::queryUserAboutSession(t_longList const& _sessions,
                                                              t_strList const& _sessionIds) const
            {
               return Command::INVALID_SESSION;
            }

            bool viewerStartupHelper::authUserQuery()
            {
               bool status = false;

               return status;
            }

            void viewerStartupHelper::showSessions(t_longList const& _sessions,
                                                   t_strList const& _sessionIds) const
            {
            }

            viewerStartupHelper::~viewerStartupHelper()
            {

            }

         } // namespace viewer
      } // namespace gui
   } // namespace UI
} // namespace btg

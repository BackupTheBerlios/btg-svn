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
 * $Id: handler.h,v 1.1.2.15 2007/07/22 11:53:16 wojci Exp $
 */

#ifndef CLI_H
#define CLI_H

#include <string>
#include <vector>

#include <bcore/project.h>
#include <bcore/type_btg.h>
#include <bcore/command/command.h>
#include <bcore/transport/transport.h>
#include <bcore/dbuf.h>
#include <bcore/client/statemachine.h>
#include <bcore/client/clientcallback.h>
#include <bcore/client/clienthandler.h>
#include <bcore/client/configuration.h>

#include <bcore/client/helper.h>
#include <bcore/client/handlerthr.h>

#include <bcore/file_info.h>

namespace btg
{
   namespace UI
      {
         namespace cli
            {
               /**
                * \addtogroup ncli
                */
               /** @{ */

               /// Client handler class.
               class Handler : public btg::core::client::handlerThreadIf
                  {
                  public:
                     /// Constructor.
                     Handler(btg::core::externalization::Externalization* _e,
                             btg::core::messageTransport*            _transport,
                             btg::core::client::clientConfiguration* _config,
                             btg::core::client::lastFiles*           _lastfiles,
                             bool const _verboseFlag,
                             bool const _autoStartFlag);

                     /// Get the last received limit status.
                     void getLastLimitStatus(t_int & _uploadRate,
                                             t_int & _downloadRate,
                                             t_int & _seedLimit,
                                             t_int & _seedTimeout) const;

                     /// Get the last received limit status.
                     void getLastGlobalLimitStatus(t_int & _gl_limitBytesUpld,
                                                   t_int & _gl_limitBytesDwnld,
                                                   t_int & _gl_maxUplds,
                                                   t_int & _gl_maxConnections) const;

                     /// Get the last saved file info.
                     void getLastFileInfoList(t_fileInfoList & _fileInfoList) const;

                     /// Get the last saved information about selected
                     /// files.
                     void getLastSelectedFiles(btg::core::selectedFileEntryList & _selected_files) const;

                     /// Get the last saved list of peers.
                     void getLastPeers(t_peerList & _peerlist) const;

                     /// Return true if the last command was
                     /// successful, false otherwise.
                     bool lastCommandSuccess() const;

                     /// Destructor.
                     virtual ~Handler();
                  private:
                     void onTransportInit();
                     void onSetup(t_long const _session);
                     void onSetupError(std::string const& _message);

                     void onAttach();
                     void onAttachError(std::string const& _message);

                     void onDetach();
                     //void onDetachError();

                     void onList(t_intList const& _contextIDs, t_strList const& _filenames);
                     void onListError();

                     void onCreateWithData();
                     void onAbort();
                     void onStart();
                     void onStop();

                     void onStatus(btg::core::Status const& _status);
                     void onStatusAll(t_statusList const& _vstatus);
                     void onStatusError(std::string const& _errorDescription);

                     void onFileInfo(t_fileInfoList const& _fileinfolist);
                     void onFileInfoError(std::string const& _errorDescription);

                     void onPeers(t_peerList const& _peerlist);
                     void onPeersError(std::string const& _errorDescription);

                     void onLimit();

                     void onListSessions(t_longList const& _sessions);
                     void onListSessionsError(std::string const& _errorDescription);

                     void onSessionError();

                     void onClean(t_strList const& _filenames, t_intList const& _contextIDs);

                     void onError(std::string const& _errorDescription);
                     void onFatalError(std::string const& _errorDescription);

                     void onLimitStatus(t_int const _uploadRate,
                                        t_int const _downloadRate,
                                        t_int const _seedLimit,
                                        t_long const _seedTimeout);

                     void onLimitStatusError(std::string const& _errorDescription);

                     void onSetFiles();

                     void onSetFilesError(std::string const& _errorDescription);

                     void onSelectedFiles(btg::core::selectedFileEntryList const& _files);

                     void onSelectedFilesError(std::string const& _errorDescription);

                     void onQuit();
                     void onKill();
                     void onKillError(std::string _ErrorDescription);

                     void onUptime(t_ulong const _uptime);

                     void onGlobalLimit();
                     void onGlobalLimitError(std::string _ErrorDescription);
                     void onGlobalLimitResponse(t_int const  _limitBytesUpld,
                                                t_int const  _limitBytesDwnld,
                                                t_int const  _maxUplds,
                                                t_long const _maxConnections);
                     void onGlobalLimitResponseError(std::string _ErrorDescription);

                     /// Indicates that the last performed command was
                     /// successful.
                     bool           lastCommandSuccess_;

                     /// Limit.
                     t_int          last_uploadRate;

                     /// Limit.
                     t_int          last_downloadRate;

                     /// Limit.
                     t_int          last_seedLimit;

                     /// Limit.
                     t_long         last_seedTimeout;

                     /// Global limit.
                     t_int          last_gl_limitBytesUpld;

                     /// Global limit.
                     t_int          last_gl_limitBytesDwnld;

                     /// Global limit.
                     t_int          last_gl_maxUplds;

                     /// Global limit.
                     t_long         last_gl_maxConnections;

                     /// Last received list of file info objects.
                     t_fileInfoList lastFileinfolist_;

                     /// List of selected files.
                     btg::core::selectedFileEntryList lastSelected_files_;

                     /// Last received list of peers.
                     t_peerList     lastPeerlist_;
                  };

               /** @} */

               /// Helper: executes a number of tasks when this client starts.
               class ncliStartupHelper: public btg::core::client::startupHelper
                  {
                  public:
                     /// Constructor.
                     ncliStartupHelper(btg::core::client::clientConfiguration*        _config,
                                       btg::core::client::commandLineArgumentHandler* _clah,
                                       btg::core::messageTransport*                   _messageTransport,
                                       btg::core::client::clientHandler*              _handler);

                     /// Query the user about which session to attach to.
                     virtual t_long queryUserAboutSession(t_longList const& _sessions) const;
                     virtual bool authUserQuery();
                     virtual void showSessions(t_longList const& _sessions) const;
                     /// Destructor.
                     virtual ~ncliStartupHelper();
                  };

               /** @} */

            } // namespace cli
      } // namespace UI
} // namespace btg

#endif

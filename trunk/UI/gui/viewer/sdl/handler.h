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

#ifndef VIEWERHANDLER_H
#define VIEWERHANDLER_H

#include <string>

#include <bcore/project.h>
#include <bcore/command/command.h>
#include <bcore/transport/transport.h>
#include <bcore/dbuf.h>
#include <bcore/client/clientcallback.h>
#include <bcore/client/clienthandler.h>

#include <bcore/type.h>
#include <bcore/type_btg.h>

#include <bcore/client/helper.h>
#include <bcore/client/configuration.h>
#include <bcore/client/handlerthr.h>

#include "arg.h"

#include "ui.h"

namespace btg
{
   namespace UI
   {
      namespace gui
      {
         namespace viewer
         {

            /**
             * \addtogroup viewer
             */
            /** @{ */

            /// Implements a handler for the viewer, which both
            /// used for sending commands to the daemon and
            /// contains callback which are called when the
            /// contained state machine has result to present to
            /// the user.
            class viewerHandler: public btg::core::client::handlerThreadIf
               {
               public:
                  /// Constructor.
                  ///
                  /// @param [in] _e         The externalization used.
                  ///
                  /// @param [in] _transport Message transport to
                  /// be used for communicating with the daemon.
                  ///
                  /// @param [in] _config Pointer to the class
                  /// holding the client configuration.
                  /// @param [in] _lastfiles Pointer to the class
                  /// holding the list of last accessed files.
                  /// @param [in] _verboseFlag Be verbose.
                  /// @param [in] _autoStartFlag Auto start loaded torrents.

                  viewerHandler(btg::core::externalization::Externalization* _e,
                                btg::core::messageTransport*            _transport,
                                btg::core::client::clientConfiguration* _config,
                                btg::core::client::lastFiles*           _lastfiles,
                                bool const                              _verboseFlag,
                                bool const                              _autoStartFlag,
                                btgvsGui &                              _gui
                                );

                  /// Returns the list of contexts from the daemon.
                  t_intList getListResult();

                  /// Get a list of IDs which became outdated and
                  /// can be removed. Used for handling removed or
                  /// aborted contexts.
                  t_intList getIdsToRemove();

                  /// Get the list of files a torrent contains.
                  t_fileInfoList getFileInfoList() const;

                  /// Convert an ID to the filename.
                  bool idToFilename(t_int const _id, std::string & _filename);

                  /// Get the list of files which were opened last.
                  t_strList getLastFiles() const;

                  /// Return the list of files cleaned by the last
                  /// clean operation.
                  t_strList getCleanedFilenames();

                  /// Get the last received list of peers.
                  t_peerList getPeers() const;

                  /// Get a list of selected files, updated by the
                  /// last command to the daemon.
                  btg::core::selectedFileEntryList getLastSelectedFiles() const;

                  /// Destructor.
                  virtual ~viewerHandler();
               private:

                  void onTransportInit();
                  void onSetup(t_long const _session);
                  void onSetupError(std::string const& _message);
                  void onList(t_intList const& _contextIDs, t_strList const& _filenames);
                  void onListError();
                  void onError(std::string const& _errorDescription);
                  void onFatalError(std::string const& _errorDescription);
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
                  void onLimitStatus(t_int const _uploadRate, t_int const _downloadRate,
                                     t_int const _seedLimit, t_long const _seedTimeout);
                  void onLimitStatusError(std::string const& _errorDescription);

                  void onSetFiles();

                  void onSetFilesError(std::string const& _errorDescription);

                  void onSelectedFiles(btg::core::selectedFileEntryList const& _files);

                  void onSelectedFilesError(std::string const& _errorDescription);

                  void onClean(t_strList const& _filenames, t_intList const& _contextIDs);
                  void onAttach();
                  void onAttachError(std::string const& _message);

                  void onListSessions(t_longList const& _sessions,
                                      t_strList const& _sessionNames);

                  void onListSessionsError(std::string const& _errorDescription);

                  void onSessionError();

                  void onDetach();
                  void onQuit();
                  void onKill();
                  void onKillError(std::string _ErrorDescription);

                  void onUptime(t_ulong const _uptime);

                  void onSessionName(std::string const& _name);
                  void onSetSessionName();

                  void onGlobalLimit();
                  void onGlobalLimitError(std::string _ErrorDescription);
                  void onGlobalLimitResponse(t_int const  _limitBytesUpld,
                                             t_int const  _limitBytesDwnld,
                                             t_int const  _maxUplds,
                                             t_long const _maxConnections);
                  void onGlobalLimitResponseError(std::string _ErrorDescription);

                  /// List of context IDs.
                  t_intList      contextIDs;

                  /// List of filenames.
                  t_strList      filenames;

                  /// Used for removing aborted contexts.
                  t_intList      idsToRemove;

                  /// Mapping of context IDs to filenames.
                  t_intStrMap    idToFilenameMap;

                  /// The last returned information about a
                  /// context's files.
                  t_fileInfoList fileinfolist;

                  /// List of files cleaned up by the last clean
                  /// operation.
                  t_strList      cleanedFilenames;

                  btgvsGui & gui;

                  /// List of peers, got from the last request to
                  /// get peers.
                  t_peerList     peerlist;

                  /// A list of selected files, updated by the
                  /// last command to the daemon.
                  btg::core::selectedFileEntryList selected_files;
               private:
                  /// Copy constructor.
                  viewerHandler(viewerHandler const& _gh);
                  /// Assignment operator.
                  viewerHandler& operator=(viewerHandler const& _gh);
               };

            /** @} */

            /// Helper: executes a number of tasks when this client
            /// starts.
            class viewerStartupHelper: public btg::core::client::startupHelper
               {
               public:
                  /// Constructor.
                  viewerStartupHelper(btg::core::client::clientConfiguration*        _config,
                                      vsCommandLineArgumentHandler* _clah,
                                      btg::core::messageTransport*                   _messageTransport,
                                      btg::core::client::clientHandler*              _handler);

                  /// Query the user about which session to attach to.
                  virtual t_long queryUserAboutSession(t_longList const& _sessions,
                                                       t_strList const& _sessionIds) const;
                  virtual bool authUserQuery();
                  virtual void showSessions(t_longList const& _sessions,
                                            t_strList const& _sessionIds) const;
                  /// Destructor.
                  virtual ~viewerStartupHelper();
               };

         } // namespace viewer
      } // namespace gui
   } // namespace UI
} // namespace btg

#endif

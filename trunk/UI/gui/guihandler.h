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

#ifndef GUIHANDLER_H
#define GUIHANDLER_H

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

namespace btg
{
   namespace UI
      {
         namespace gui
            {
               /**
                * \addtogroup gui
                */
               /** @{ */

               class mainStatusbar;

               /// Implements a handler for the GUI client, which both
               /// used for sending commands to the daemon and
               /// contains callback which are called when the
               /// contained state machine has result to present to
               /// the user.
               class guiHandler: public btg::core::client::handlerThreadIf
                  {
                  public:
                     /// Constructor.
                     ///
                     /// @param [in] _logwrapper Pointer used to send logs to.
                     /// @param [in] _e          The externalization used.
                     ///
                     /// @param [in] _transport Message transport to
                     /// be used for communicating with the daemon.
                     ///
                     /// @param [in] _config Pointer to the class
                     /// holding the client configuration.
                     /// @param [in] _dynconfig Pointer to the class
                     /// holding the dynamic client configuration.
                     /// @param [in] _verboseFlag Be verbose.
                     /// @param [in] _autoStartFlag Auto start loaded torrents.
                     /// @param [in] _status_bar Pointer to the class
                     /// representing a statusbar.

                     guiHandler(btg::core::LogWrapperType _logwrapper,
                                btg::core::externalization::Externalization& _e,
                                btg::core::messageTransport&            _transport,
                                btg::core::client::clientConfiguration& _config,
                                btg::core::client::clientDynConfig&     _dynconfig,
                                bool const                              _verboseFlag,
                                bool const                              _autoStartFlag,
                                mainStatusbar*                          _pstatus_bar
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

                     /// Set the member containing a pointer to a
                     /// statusbar.
                     void setStatusBar(mainStatusbar* _status_bar);

                     /// Get the last received list of peers.
                     t_peerList getPeers() const;

                     /// Get a list of selected files, updated by the
                     /// last command to the daemon.
                     btg::core::selectedFileEntryList getLastSelectedFiles() const;

                     /// Destructor.
                     virtual ~guiHandler();
                  private:

                     void onTransportInit();
                     void onSetup(t_long const _session);
                     void onSetupError(std::string const& _message);
                     void onList(t_intList const& _contextIDs, t_strList const& _filenames);
                     void onListError();
                     void onError(std::string const& _errorDescription);
                     void onFatalError(std::string const& _errorDescription);
                     void onCreateWithData();
                     void onCreateFromUrl(t_uint const _id);
                     void onCreateFromUrlError(std::string const& _message);
                     
                     void onCreateFromFile(t_uint const _id);
                     void onCreateFromFileError(std::string const& _errorDescription);
                     void OnCreateFromFilePart();
                     void OnCreateFromFilePartError(std::string const& _errorDescription);
                     
                     void onFileStatus(t_uint const _id, 
                                       btg::core::fileStatus const _status);
                     void onFileStatusError(std::string const& _errorDescription);

                     void onUrlStatus(t_uint const _id, 
                                      btg::core::urlStatus const _status);
                     void onUrlStatusError(std::string const& _message);
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

                     void onSetFiles();

                     void onMove();

                     void onVersion(btg::core::OptionBase const& _ob);

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

                     void onSessionInfo(bool const _encryption, bool const _dht);
                     
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

                     /// Pointer to the statusbar used for showing
                     /// short status messages.
                     mainStatusbar* pstatus_bar;

                     /// List of peers, got from the last request to
                     /// get peers.
                     t_peerList     peerlist;

                     /// A list of selected files, updated by the
                     /// last command to the daemon.
                     btg::core::selectedFileEntryList selected_files;
                  private:
                     /// Copy constructor.
                     guiHandler(guiHandler const& _gh);
                     /// Assignment operator.
                     guiHandler& operator=(guiHandler const& _gh);
                  };

               /** @} */

               /// Helper: executes a number of tasks when this client
               /// starts.
               class guiStartupHelper: public btg::core::client::startupHelper
                  {
                  public:
                     /// Constructor.
                     guiStartupHelper(btg::core::LogWrapperType _logwrapper,
                                      btg::core::client::clientConfiguration&        _config,
                                      btg::core::client::commandLineArgumentHandler& _clah,
                                      btg::core::messageTransport&                   _messageTransport,
                                      btg::core::client::clientHandler&              _handler);

                     /// Query the user about which session to attach to.
                     virtual t_long queryUserAboutSession(t_longList const& _sessions,
                                                          t_strList const& _sessionIds) const;
                     virtual bool authUserQuery();
                     virtual void showSessions(t_longList const& _sessions,
                                               t_strList const& _sessionNames) const;
                     /// Destructor.
                     virtual ~guiStartupHelper();
                  };

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif

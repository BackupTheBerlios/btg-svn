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
 * $Id: clientcallback.h,v 1.16.4.20 2007/06/06 22:04:31 wojci Exp $
 */

#ifndef CLIENTCALLBACK_H
#define CLIENTCALLBACK_H

#include <bcore/type.h>
#include <bcore/type_btg.h>

#include <bcore/trackerstatus.h>
#include <bcore/file_info.h>

#include <string>

namespace btg
{
   namespace core
      {
         namespace client
            {

               /**
                * \addtogroup gencli Generic client
                */
               /** @{ */

               /// Callbacks used by the generic client. The stateMachine
               /// class calls the functions defined in this class.
               class clientCallback
                  {
                  public:
                     /// Constructor.
                     clientCallback();

                     /// The client completed initialization of the transport which is used.
                     virtual void onTransportInit() = 0;

                     /// The client completed setup and is ready to
                     /// process commands.
                     /// @param [in] _session The session ID that was assigned to this new session
                     virtual void onSetup(t_long const _session) = 0;

                     /// Setup failed.
                     virtual void onSetupError(std::string const& _message) = 0;

                     /// The client got at list response.
                     virtual void onList(t_intList const& _contextIDs, t_strList const& _filenames) = 0;
                     /// The listing failed.
                     virtual void onListError() = 0;

                     /// Generic errors from the daemon.
                     /// @param [out] _errorDescription The error description.
                     virtual void onError(std::string const& _errorDescription) = 0;

                     /// Called when fatal errors occur, and the client should shut down.
                     virtual void onFatalError(std::string const& _errorDescription) = 0;

                     /// The client managed to create a new context with data.
                     virtual void onCreateWithData() = 0;

                     /// The client got the Id of the last context it created.
                     /// Note that implementing this callback is optional.
                     virtual void onLast(t_int _contextId);

                     /// The client managed to abort a context.
                     virtual void onAbort() = 0;

                     /// The client managed to start a context.
                     virtual void onStart() = 0;

                     /// The client managed to stop a context.
                     virtual void onStop() = 0;

                     /// The client got a status response.
                     /// @param [out] _status The status.
                     virtual void onStatus(btg::core::Status const& _status) = 0;

                     /// The status could not be obtained
                     /// @param [out] _errorDescription The error description.
                     virtual void onStatusError(std::string const& _errorDescription) = 0;

                     /// The client got a status response for all contexts.
                     /// @param [out] _vstatus List of status objects.
                     virtual void onStatusAll(t_statusList const& _vstatus) = 0;

                     /// The client got a file info response.
                     /// @param [out] _fileinfolist List of file info objects.
                     virtual void onFileInfo(t_fileInfoList const& _fileinfolist) = 0;

                     /// The client could not obtain a file info.
                     /// @param [out] _errorDescription The error description.
                     virtual void onFileInfoError(std::string const& _errorDescription) = 0;

                     /// The client got a list of peers.
                     virtual void onPeers(t_peerList const& _peerlist) = 0;

                     /// The client could not obtain a list of peers.
                     virtual void onPeersError(std::string const& _errorDescription) = 0;

                     /// The client limited a context or all contexts.
                     virtual void onLimit() = 0;

                     /// The client got a response to a limit status.
                     virtual void onLimitStatus(t_int const _uploadRate, t_int const _downloadRate,
                                                t_int const _seedLimit, t_long const _seedTimeout) = 0;
                     
                     /// The files requested by the client were set.
                     virtual void onSetFiles() = 0;

                     /// The files requested by the client could not
                     /// be set.
                     virtual void onSetFilesError(std::string const& _errorDescription) = 0;

                     /// The reply to the client requesting which
                     /// files are set.
                     virtual void onSelectedFiles(selectedFileEntryList const& _files) = 0;

                     /// Unable to get the list of set files.
                     virtual void onSelectedFilesError(std::string const& _errorDescription) = 0;

                     /// The client could not obtain a limit status.
                     virtual void onLimitStatusError(std::string const& _errorDescription) = 0;

                     /// The client got a list of contexts which were
                     /// downloaded 100% and got cleaned.
                     /// @param [out] _filenames  List of file names.
                     /// @param [out] _contextIDs List of context IDs (can be used for removing).
                     virtual void onClean(t_strList const& _filenames, t_intList const& _contextIDs) = 0;

                     /// The client attached to a session.
                     virtual void onAttach() = 0;

                     /// The client failed to attach to a session.
                     /// @param [out] _message The error description.
                     virtual void onAttachError(std::string const& _message) = 0;

                     /// The daemon sent a list of sessions.
                     /// @param [out] _sessions List of sessions.
                     virtual void onListSessions(t_longList const& _sessions) = 0;

                     /// The daemon informed the client that its
                     /// session is wrong. The client should shut
                     /// down.
                     virtual void onSessionError() = 0;

                     /// The list of sessions could not be obtained.
                     /// @param [out] _errorDescription The error description.
                     virtual void onListSessionsError(std::string const& _errorDescription) = 0;

                     /// The client managed to detach from its current session.
                     virtual void onDetach() = 0;

                     /// The client failed to detach from its current session.
                     /// This function is not used.
                     // virtual void onDetachError() = 0;

                     /// The client managed to quit the current session.
                     virtual void onQuit() = 0;

                     /// The client managed to kill the daemon.
                     virtual void onKill() = 0;

                     /// The client was unable to kill the daemon.
                     virtual void onKillError(std::string _ErrorDescription) = 0;

                     /// Global limit was set.
                     virtual void onGlobalLimit() = 0;

                     /// Unable to set global limit.
                     virtual void onGlobalLimitError(std::string _ErrorDescription) = 0;

                     /// Global limit status response.
                     virtual void onGlobalLimitResponse(t_int const  _limitBytesUpld,
                                                        t_int const  _limitBytesDwnld,
                                                        t_int const  _maxUplds,
                                                        t_long const _maxConnections) = 0;

                     /// Unable to get global limit status.
                     virtual void onGlobalLimitResponseError(std::string _ErrorDescription) = 0;

                     /// The client received the uptime from the daemon.
                     virtual void onUptime(t_ulong const _uptime) = 0;

                     /// Destructor.
                     virtual ~clientCallback();
                  };

            } // namespace client
      } // namespace core
} // namespace btg

#endif

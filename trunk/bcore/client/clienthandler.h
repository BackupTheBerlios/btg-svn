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

#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <bcore/transport/transport.h>
#include <bcore/type.h>
#include <bcore/status.h>
#include <bcore/addrport.h>
#include <bcore/file_info.h>

#include "clientcallback.h"
#include "statemachine.h"
#include "configuration.h"
#include "lastfiles.h"

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

         /// Handler interface implemented by clients.
         /// Uses a statemachine and a transport channel.
         /// The req* functions make the statemachine send commands.
         /// The _callback argument to the constructor is used
         /// for calling callbacks with the result from the statemachine.
         /// A number of members is avaible to hold information
         /// obtained as the result from executed commands.
         class clientHandler
         {
         public:
            /// Constructor.
            ///
            /// @param [in] _e         Pointer to the externalization which is used.
            /// @param [in] _callback  Pointer to an
            /// implementation of the client callback
            /// interface.
            ///
            /// @param [in] _transport Pointer to an
            /// implementation of the transport interface.
            ///
            /// @param [in] _config Pointer to the class
            /// holding the client configuration.
            ///
            /// \note This class owns this pointer and
            /// deletes it in the destructor.
            /// @param [in] _lastfiles Pointer to the class
            /// holding a list of last accessed files.
            /// @param [in] _verboseFlag Instructs this class to do verbose logging.
            /// @param [in] _autoStartFlag Indicates that the handler should start torrents automatically after loading them.
            clientHandler(btg::core::externalization::Externalization* _e,
                          btg::core::client::clientCallback* _callback,
                          btg::core::messageTransport*       _transport,
                          btg::core::client::clientConfiguration*  _config,
                          btg::core::client::lastFiles*            _lastfiles,
                          bool const _verboseFlag,
                          bool const _autoStartFlag);

            enum
            {
               NO_SEEDLIMIT   = -1, //!< Seed limit disabled.
               NO_SEEDTIMEOUT = -1 //!< Seed timeout disabled.
            };

            /// Initialize the connection.
            /// This sends a message to the daemon providing
            /// username and password for the user.
            virtual void reqInit(std::string const _username, 
                                 btg::core::Hash const _password);

            /// Setup the client and the connection to the daemon.
            virtual void reqSetup(t_int const _seedLimit,
                                  t_long const _seedTimeout,
                                  bool const _useDHT,
                                  bool const _useEncryption);

            /// Setup the client - connect to a running session.
            virtual void reqSetupAttach(t_long const _old_session);

            /// Get a list of active sessions, return true
            /// and modify the argument.
            virtual void reqGetActiveSessions();

            /// Do a list of contexts.
            virtual void reqList();

            /// Used to open a filename, when its given as a
            /// command line argument.
            virtual void reqCreate(std::string const& _filename);

            /// Used to open a number of filenames.
            virtual void reqCreate(t_strList const& _filenames);

            /// Get status from the daemon.
            virtual void reqStatus(t_int const _id, bool const _allContexts = false);

            /// Start a context.
            virtual void reqStart(t_int const _id, bool const _allContexts = false);

            /// Stop a context.
            virtual void reqStop(t_int const _id, bool const _allContexts = false);

            /// Abort a context.
            virtual void reqAbort(t_int const _id, bool const _eraseData, bool const _allContexts = false);

            /// Get file info about context.
            virtual void reqFileInfo(t_int const _id, bool const _allContexts = false);

            /// Request a list of peers.
            virtual void reqPeers(t_int const _id, bool const _allContexts = false);

            /// Limit a context.
            virtual void reqLimit(t_int const _id,
                                  t_int const _uploadRate,
                                  t_int const _downloadRate,
                                  t_int const _seedLimit,
                                  t_long const _seedTimeout,
                                  bool const _allContexts = false);

            /// Get status of limiting for a context.
            virtual void reqLimitStatus(t_int const _id, bool const _allContexts = false);

            /// Set the files to download.
            virtual void reqSetFiles(t_int const _id, 
                                     selectedFileEntryList const& _files);

            /// Get the files previously set to download.
            virtual void reqGetFiles(t_int const _id);

            /// Set global limit.
            virtual void reqGlobalLimit(t_int const  _limitBytesUpld,
                                        t_int const  _limitBytesDwnld,
                                        t_int const  _maxUplds,
                                        t_long const _maxConnections);

            /// Get status of global limit.
            virtual void reqGlobalLimitStatus();

            /// Clean contexts.
            virtual void reqClean(t_int const _id, bool const _allContexts = false);

            /// Detach from the session.
            virtual void reqDetach();

            /// Quit the session.
            virtual void reqQuit();

            /// Kill the daemon.
            virtual void reqKill();

            /// Request uptime from the daemon.
            virtual void reqUptime();

            virtual void reqSessionName();

            virtual void reqSetSessionName(std::string const& _name);

            /// Returns true if the statemachine thinks that
            /// it completed the setup of the client.
            virtual bool isSetupDone() const;

            /// Returns true if the statemachine thinks that
            /// its attached.
            virtual bool isAttachDone() const;

            /// Report status of the last command.
            /// @return True - the last operation was
            /// successful. False - failture.
            virtual bool commandSuccess() const;

            /// Returns the list of contexts from the daemon.
            virtual t_intList getListResult() const;

            /// Get the last status received.
            virtual btg::core::Status getStatus() const;

            /// Get the list of status objects - for all
            /// contexts.
            t_statusList getStatusList() const;

            /// Get the list of files a torrent contains.
            virtual t_fileInfoList getFileInfoList() const;

            /// Get the bandwidth limits.
            virtual void getLimitStatus(t_int & _limitBytesUpld, t_int & _limitBytesDwnld);

            /// Get the seed limits.
            virtual void getSeedLimitStatus(t_int & _limitSeedPercent,
                                            t_long & _limitSeedTime);

            /// Get a list of sessions, as returned by
            /// onListSessions.
            virtual t_longList getSessionList() const;

            /// Get a list of sessions, as returned by
            /// onListSessions.
            virtual t_strList getSessionNames() const;

            std::string getCurrentSessionName() const;

            /// Get a pointer to the contained client
            /// configuration object.
            virtual btg::core::client::clientConfiguration* getConfig() const;

            /// If setup fails, this function will return a
            /// textual description of why it failed.
            virtual std::string getSetupFailtureMessage();

            /// If setup fails, this function will return a
            /// textual description of why it failed.
            virtual std::string getAttachFailtureMessage();

            /// Returns true if a fatal error was discovered.
            virtual bool fatalError() const { return fatalerror; };

            /// Call to make the client discover a fatal error.
            virtual void setFatalError() { fatalerror = true; };

            /// Returns true if the daemon sent any session
            /// error messages.
            virtual bool sessionError() const { return sessionerror; };

            /// Call to make the client discover a session error.
            virtual void setSessionError() { sessionerror = true; };

            /// Get the session ID used by this handler.
            virtual t_long session() const;

            /// Set the session ID used by this handler.
            virtual void setSession(t_long const _session);

            /// Destructor.
            virtual ~clientHandler();
         protected:
            enum
            {
               ILLEGAL_ID = -1 //!< Ilegal context ID.
            };

            /// The transport used to communicate with the daemon.
            btg::core::messageTransport*      transport;

            /// The state machine used by this client.
            btg::core::client::stateMachine   statemachine;

            /// Indicates that setup was done.
            /// This should be set by the onSetup callback.
            bool                              setupDone;

            /// Indicates that attach is done.
            /// This should be set by the onAttach callback.
            bool                              attachDone;

            /// Indicates that a command was successful.
            /// This should be set by the on$Command callbacks.
            bool                              commandStatus;

            /// Flag set when erasing a context - as erasing
            /// a context is done by first creating an abort
            /// message with the erase flag set.
            bool                              eraseFlag;

            /// The id which was used by the last command
            /// sent, if the command used a context ID.
            t_int                             last_id;

            /// The last filename opened.
            std::string                       last_filename;

            /// The last status returned from the daemon.
            btg::core::Status                 last_status;

            /// List of status objects.
            t_statusList                      last_statuslist;

            /// The last list of contexts from the daemon.
            t_intList                         last_list;

            /// Map context IDs to filenames.
            std::map<t_int, std::string>      idToFilenameMap;

            /// The last file info list.
            t_fileInfoList                    last_fileinfolist;

            /// The last set upload limit.
            t_int                             last_limit_upload;
            /// The last set download limit.
            t_int                             last_limit_download;

            /// The last set seed limit.
            t_int                             last_limit_seed_percent;

            /// The last set seed timeout.
            t_long                            last_limit_seed_timeout;

            /// Counter: the number of failed requests.
            t_uint                            cmd_failture;

            /// List of sessions, as returned by onListSessions.
            t_longList                        sessionList;

            /// List of sessions, as returned by onListSessions.
            t_strList                         sessionNames;

            std::string                       currentSessionName;

            /// Pointer to the object holding the client configuration.
            btg::core::client::clientConfiguration* config;

            /// List of last accessed files.
            btg::core::client::lastFiles*     lastfiles;

            /// If setup fails, this member will contain a
            /// textual description of why it failed.
            std::string                       setupFailtureMessage;

            /// If attach fails, this member will contain a
            /// textual description of why it failed.
            std::string                       attachFailtureMessage;

            /// If true, a fatal error was discovered.
            bool                              fatalerror;

            /// True, if a session error was detected by the
            /// daemon.
            bool                              sessionerror;

            /// The session ID used by this handler.
            t_long                            session_;

            /// Indicates that the handler should start
            /// torrents automatically after loading them.
            bool const                        autoStartFlag_;
         private:
            /// Copy constructor.
            clientHandler(clientHandler const& _ch);
            /// Assignment operator.
            clientHandler& operator=(clientHandler const& _ch);
         };

      } // namespace client
   } // namespace core
} // namespace btg

#endif

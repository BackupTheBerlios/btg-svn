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

#include <iostream>

#include <bcore/project.h>
#include <bcore/util.h>

#include <bcore/os/fileop.h>
#include <bcore/os/id.h>
#include <bcore/auth/hash.h>
#include <bcore/client/urlhelper.h>
#include <bcore/os/sleep.h>

#include "clienthandler.h"

namespace btg
{
   namespace core
   {
      namespace client
      {

         using namespace btg::core;

         clientHandler::clientHandler(LogWrapperType _logwrapper,
                                      btg::core::externalization::Externalization* _e,
                                      btg::core::client::clientCallback* _callback,
                                      btg::core::messageTransport*       _transport,
                                      btg::core::client::clientConfiguration*  _config,
                                      btg::core::client::lastFiles*            _lastfiles,
                                      bool const _verboseFlag,
                                      bool const _autoStartFlag)
            : Logable(_logwrapper),
              transport(_transport),
              statemachine(_logwrapper, _e, transport, _callback, _verboseFlag),
              setupDone(false),
              attachDone(false),
              commandStatus(false),
              eraseFlag(false),
              last_id(clientHandler::ILLEGAL_ID),
              last_filename(),
              last_status(),
              last_statuslist(0),
              last_list(0),
              idToFilenameMap(),
              last_fileinfolist(0),
              last_limit_upload(0),
              last_limit_download(0),
              last_limit_seed_percent(0),
              last_limit_seed_timeout(0),
              last_limit_max_uploads(0),
              last_limit_max_connections(0),
              cmd_failture(0),
              sessionList(0),
              sessionNames(0),
              currentSessionName(),
              config(_config),
              lastfiles(_lastfiles),
              setupFailtureMessage(),
              attachFailtureMessage(),
              fatalerror(false),
              sessionerror(false),
              session_(clientHandler::ILLEGAL_ID),
              autoStartFlag_(_autoStartFlag),
              dht_enabled_(false),
              encryption_enabled_(false),
              last_url_id(URLS_INVALID_URLID),
              last_surl_id(URLS_INVALID_URLID), 
              last_surl_status(URLS_UNDEF),
              options()
         {
         }

         void clientHandler::reqInit(std::string const _username, btg::core::Hash const _password)
         {
            m_bTransinitwaitError = false;
            statemachine.doInit(_username, _password);
            statemachine.work();
         }

         void clientHandler::reqSetup(t_int const _seedLimit,
                                      t_long const _seedTimeout,
                                      bool const _useDHT,
                                      bool const _useEncryption)
         {
            requiredSetupData rsd(GPD->sBUILD(), _seedLimit, _seedTimeout, _useDHT, _useEncryption);

            statemachine.doSetup(new setupCommand(rsd));
            statemachine.work();
         }

         void clientHandler::reqSetupAttach(t_long const _old_session)
         {
            setSession(_old_session);
            statemachine.doAttach(new attachSessionCommand(GPD->sBUILD(), _old_session));
            statemachine.work();
         }

         void clientHandler::reqGetActiveSessions()
         {
            statemachine.doListSessions();
            statemachine.work();
         }

         void clientHandler::reqList()
         {
            commandStatus = false;

            statemachine.doList();
            statemachine.work();
         }

         void clientHandler::reqCreate(std::string const& _filename)
         {
            commandStatus = false;
            last_id       = clientHandler::ILLEGAL_ID;
            last_filename = _filename;

            // Before creating the file, resolve relative paths.
            std::string filename = _filename;
            btg::core::os::fileOperation::resolvePath(filename);

            statemachine.doCreate(filename, autoStartFlag_);
            statemachine.work();
         }

         void clientHandler::reqCreate(t_strList const& _filenames)
         {
            t_strListCI iter;
            for (iter = _filenames.begin(); 
                 iter != _filenames.end(); 
                 iter++)
               {
                  this->reqCreate(*iter);
               }
         }

         void clientHandler::reqCreateFromUrl(std::string const& _filename,
                                              std::string const& _url)
         {
            commandStatus = false;
            last_id       = clientHandler::ILLEGAL_ID;
            last_filename = _filename;

            statemachine.doCreateFromUrl(_filename,
                                         _url, 
                                         autoStartFlag_);
            statemachine.work();
         }

         void clientHandler::reqUrlStatus(t_uint _id)
         {
            commandStatus = false;
            statemachine.doUrlStatus(_id);
            statemachine.work();
         }

         void clientHandler::reqStatus(t_int const _id, bool const _allContexts)
         {
            commandStatus = false;
            last_id       = _id;

            statemachine.doStatus(_id, _allContexts);
            statemachine.work();
         }

         void clientHandler::reqStart(t_int const _id, bool const _allContexts)
         {
            commandStatus = false;
            last_id       = _id;

            statemachine.doStart(_id, _allContexts);
            statemachine.work();
         }

         void clientHandler::reqStop(t_int const _id, bool const _allContexts)
         {
            commandStatus = false;
            last_id       = _id;

            statemachine.doStop(_id, _allContexts);
            statemachine.work();
         }

         void clientHandler::reqAbort(t_int const _id, bool const _eraseData, bool const _allContexts)
         {
            commandStatus = false;
            last_id       = _id;

            if (_eraseData)
               {
                  // Abort and erase.
                  eraseFlag = true;
               }
            else
               {
                  eraseFlag = false;
               }

            statemachine.doAbort(_id, _eraseData, _allContexts);
            statemachine.work();
         }

         void clientHandler::reqFileInfo(t_int const _id, bool const _allContexts)
         {
            commandStatus = false;
            last_id       = _id;

            statemachine.doFileInfo(_id, _allContexts);
            statemachine.work();
         }

         void clientHandler::reqPeers(t_int const _id, bool const _allContexts)
         {
            commandStatus = false;
            last_id       = _id;

            statemachine.doPeers(_id, _allContexts);
            statemachine.work();
         }

         void clientHandler::reqLimit(t_int const _id,
                                      t_int const _uploadRate,
                                      t_int const _downloadRate,
                                      t_int const _seedLimit,
                                      t_long const _seedTimeout,
                                      bool const _allContexts)
         {
            commandStatus = false;
            last_id       = _id;

            statemachine.doLimit(_id, _uploadRate, _downloadRate, _seedLimit, _seedTimeout, _allContexts);
            statemachine.work();
         }

         void clientHandler::reqLimitStatus(t_int const _id, bool const _allContexts)
         {
            commandStatus = false;
            last_id       = _id;

            statemachine.doLimitStatus(_id, _allContexts);
            statemachine.work();
         }

         void clientHandler::reqSetFiles(t_int const _id, 
                                         selectedFileEntryList const& _files)
         {
            commandStatus = false;
            last_id       = _id;

            statemachine.doSetFiles(_id, _files);
            statemachine.work();
         }

         void clientHandler::reqGetFiles(t_int const _id)
         {
            commandStatus = false;
            last_id       = _id;

            statemachine.doGetFiles(_id);
            statemachine.work();
         }

         void clientHandler::reqGlobalLimit(t_int const  _limitBytesUpld,
                                            t_int const  _limitBytesDwnld,
                                            t_int const  _maxUplds,
                                            t_long const _maxConnections)
         {
            commandStatus = false;

            statemachine.doGlobalLimit(_limitBytesUpld,
                                       _limitBytesDwnld,
                                       _maxUplds,
                                       _maxConnections);
            statemachine.work();
         }

         void clientHandler::reqGlobalLimitStatus()
         {
            commandStatus = false;

            statemachine.doGlobalLimitStatus();
            statemachine.work();
         }

         void clientHandler::reqClean(t_int const _id, bool const _allContexts)
         {
            commandStatus = false;
            statemachine.doClean(_id, _allContexts);
            statemachine.work();
         }

         void clientHandler::reqDetach()
         {
            commandStatus = false;

            statemachine.doDetach();
            statemachine.work();
         }

         void clientHandler::reqQuit()
         {
            commandStatus = false;

            statemachine.doQuit();
            statemachine.work();
         }

         void clientHandler::reqKill()
         {
            commandStatus = false;

            statemachine.doKill();
            statemachine.work();
         }

         void clientHandler::reqUptime()
         {
            commandStatus = false;

            statemachine.doUptime();
            statemachine.work();
         }

         void clientHandler::reqSessionInfo()
         {
            commandStatus = false;

            statemachine.doSessionInfo();
            statemachine.work();
         }

         void clientHandler::reqSessionName()
         {
            commandStatus = false;

            statemachine.doSessionName();
            statemachine.work();
         }

         void clientHandler::reqSetSessionName(std::string const& _name)
         {
            commandStatus = false;

            statemachine.doSetSessionName(_name);
            statemachine.work();
         }

         void clientHandler::reqMoveContext(t_int const _id,
                                            t_long const _toSession)
         {
            commandStatus = false;
            last_id = _id;
            
            statemachine.doMoveContext(_id, _toSession);
            statemachine.work();
         }

         void clientHandler::reqVersion()
         {
            commandStatus = false;
            
            statemachine.doVersion();
            statemachine.work();
         }

         bool clientHandler::isSetupDone() const
         {
            return setupDone;
         }

         bool clientHandler::isAttachDone() const
         {
            return attachDone;
         }

         bool clientHandler::commandSuccess() const
         {
            return commandStatus;
         }

         t_intList clientHandler::getListResult() const
         {
            return last_list;
         }

         btg::core::Status clientHandler::getStatus() const
         {
            return last_status;
         }

         t_statusList clientHandler::getStatusList() const
         {
            return last_statuslist;
         }

         t_fileInfoList clientHandler::getFileInfoList() const
         {
            return last_fileinfolist;
         }

         void clientHandler::getLimitStatus(t_int & _limitBytesUpld, t_int & _limitBytesDwnld)
         {
            _limitBytesUpld  = last_limit_upload;
            _limitBytesDwnld = last_limit_download;
         }

         void clientHandler::getSeedLimitStatus(t_int  & _limitSeedPercent,
                                                t_long & _limitSeedTime)
         {
            _limitSeedPercent = last_limit_seed_percent;
            _limitSeedTime    = last_limit_seed_timeout;
         }

         void clientHandler::getDaemonLimitStatus(t_int & _limitMaxUploads,
                                                  t_long & _limitMaxConnections)
         {
            _limitMaxUploads = last_limit_max_uploads;
            _limitMaxConnections = last_limit_max_connections;
         }
         
         t_longList clientHandler::getSessionList() const
         {
            return sessionList;
         }

         t_strList clientHandler::getSessionNames() const
         {
            return sessionNames;
         }

         std::string clientHandler::getCurrentSessionName() const
         {
            return currentSessionName;
         }

         btg::core::client::clientConfiguration* clientHandler::getConfig() const
         {
            return config;
         }

         std::string clientHandler::getSetupFailtureMessage()
         {
            return setupFailtureMessage;
         }

         std::string clientHandler::getAttachFailtureMessage()
         {
            return attachFailtureMessage;
         }

         t_long clientHandler::session() const
         {
            return session_;
         }

         void clientHandler::setSession(t_long const _session)
         {
            session_ = _session;
         }

         bool clientHandler::dht()
         {
            return dht_enabled_;
         }

         bool clientHandler::encryption()
         {
            return encryption_enabled_;
         }

         t_uint clientHandler::UrlId() const
         {
            return last_url_id;
         }

         void clientHandler::setUrlId(t_uint const _id)
         {
            last_url_id = _id;
         }

         void clientHandler::UrlStatusResponse(t_uint & _id, 
                                               btg::core::urlStatus & _status) const
         {
            _id     = last_surl_id; 
            _status = last_surl_status;
         }

         void clientHandler::setUrlStatusResponse(t_uint const _id, 
                                                  btg::core::urlStatus const _status)
         {
            last_surl_id     = _id;
            last_surl_status = _status;
         }

         bool clientHandler::handleUrlProgress(t_uint _hid)
         {
            bool res  = false;
            bool cont = true;

            while (cont)
               {
                  reqUrlStatus(_hid);
                  if (!commandSuccess())
                     {
                        return res;
                     }
                  t_uint id = 0;
                  btg::core::urlStatus status;

                  UrlStatusResponse(id, status);
                  
                  switch (status)
                     {
                     case btg::core::URLS_UNDEF:
                        {
                           break;
                        }
                     case btg::core::URLS_WORKING:
                     case btg::core::URLS_FINISHED:
                        {
                           break;
                        }
                     case btg::core::URLS_ERROR:
                        {
                           cont = false;
                           break;
                        }
                     case btg::core::URLS_CREATE:
                        {
                           res  = true;
                           cont = false;
                           break;
                        }
                     case btg::core::URLS_CREATE_ERR:
                        {
                           res  = false;
                           cont = false;
                           break;
                        }
                     }
                  btg::core::os::Sleep::sleepMiliSeconds(500);
               }

            return res;
         }

         void clientHandler::setOption(btg::core::OptionBase const & _options)
         {
            options = _options; 
         }

         const btg::core::OptionBase& clientHandler::getOption() const
         {
            return options;
         }

         clientHandler::~clientHandler()
         {
            delete transport;
            transport = 0;
         }

      } // namespace client
   } // namespace core
} // namespace btg

/*
 * btg Copyright (C) 2005 Michael Wojciechowski.
 *
 * Session saving part written by Johan Ström.
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

#include "sessionsaver.h"
#include <bcore/project.h>
#include <bcore/addrport.h>
#include <bcore/command/setup.h>
#include <bcore/command/limit_base.h>
#include <bcore/util.h>
#include <bcore/dbuf.h>
#include <bcore/os/fileop.h>
#include <bcore/externalization/xmlrpc.h>
#include <daemon/modulelog.h>
#include "eventhandler.h"
#include "session_list.h"
#include "daemondata.h"
#include "eventhandler.h"
#include "dconfig.h"

#include <map>
#include <fstream>

using namespace btg::daemon;

// Version information.

// New file format. Incompatible with the old format..
// Increase with 1 for every change here or deeper down in the save struct.
const t_byte ss_version = 0x9a;

#define DESERIALIZE_CHECK(i, m, r) { \
  if((i)) \
  { \
    BTG_ERROR_LOG(logWrapper(), m); \
    return r; \
  } \
}

namespace btg
{
   namespace daemon
   {
      const std::string moduleName("ses");

      SessionSaver::SessionSaver(btg::core::LogWrapperType _logwrapper,
                                 bool const _verboseFlag,
                                 portManager & _portManager,
                                 limitManager & _limitManager,
                                 sessionList & _sessionlist,
                                 daemonData & _dd)
         : btg::core::Logable(_logwrapper),
           verboseFlag_(_verboseFlag),
           portManager_(_portManager),
           limitManager_(_limitManager),
           sessionlist_(_sessionlist),
           dd(_dd)
      {
      }

      t_int SessionSaver::loadSessions(btg::core::os::fstream & _file
#ifdef OLD_FORMAT
            , bool useBinaryFormat
#endif
            )
      {
         t_int handler_count = 0;

         BTG_MNOTICE(logWrapper(), "loadSessions(), Loading sessions from stream.");

         if (!_file.is_open())
            {
               BTG_ERROR_LOG(logWrapper(), "loadSessions(), Stream is closed. Failed to restore sessions.");
               return 0;
            }

         // Find out the size of the file.
         t_int size = 0;
#if HAVE_IOS_BASE
         _file.seekg (0, std::ios_base::end);
         size = _file.tellg();
         _file.seekg (0, std::ios_base::beg);
#else
         _file.seekg (0, std::ios::end);
         size = _file.tellg();
         _file.seekg (0, ios::beg);
#endif

         if (size <= 0)
            {
               BTG_ERROR_LOG(logWrapper(), "loadSessions(), stream is empty.");
               return 0;
            }

         t_byteP buffer = new t_byte[size];
         memset(buffer, size, 0);
         
         _file.read(reinterpret_cast<char*>(buffer), size);

         btg::core::externalization::Externalization* ext = 0;
         
#ifdef OLD_FORMAT
         if(useBinaryFormat)
            ext = new btg::core::externalization::Simple(logWrapper());
         else
#endif
            ext = new btg::core::externalization::XMLRPC(logWrapper());

         btg::core::dBuffer dbuf(buffer, size);
         ext->setBuffer(dbuf);

         delete [] buffer;
         buffer = 0;

         // We now have something loaded to the extif. Check signature
         // bytes.

         t_uint sig0=0;
         t_uint sig1=0;

         if ((!ext->bytesToUint(&sig0)) || (!ext->bytesToUint(&sig1)))
            {
               BTG_ERROR_LOG(logWrapper(), 
                             "SessionSaver::loadSessions(), "
                             "failed to parse stream, "
                             "unable to read signature byte(s).");
               delete ext;
               ext = 0;

#ifdef OLD_FORMAT
               if (!useBinaryFormat)
               {
                  BTG_ERROR_LOG(logWrapper(),
                                "SesssionSaver::loadSessions(), "
                                "trying old binary format.");
                  return loadSessions(_file, true);
               }
#endif
               return 0;
            }

         if ((sig0 != ~((t_uint)sig1)))
            {
               BTG_ERROR_LOG(logWrapper(),
                             "SesssionSaver::loadSessions(), "
                             "bad signature bytes");
               // Unknown signature.
               delete ext;
               ext = 0;
               
               return 0;
            }

         handler_count = handleSaved(ext, sig0);

         delete ext;
         ext = 0;

         return handler_count;
      }

      t_int SessionSaver::handleSaved(btg::core::externalization::Externalization* _e,
                                      t_uint _version)
      {
         // Global limits.
         if (_version >= 0x9a)
            {
               t_int upload_rate_limit   = btg::core::limitBase::LIMIT_DISABLED;
               t_int download_rate_limit = btg::core::limitBase::LIMIT_DISABLED;
               t_int max_uploads         = btg::core::limitBase::LIMIT_DISABLED;
               t_long max_connections    = btg::core::limitBase::LIMIT_DISABLED;
               
               bool r = _e->bytesToInt(&upload_rate_limit);
               DESERIALIZE_CHECK(!r, "Unable to load global limit, upload", 0);
               MVERBOSE_LOG(logWrapper(), verboseFlag_, "Upload limit: " << 
                            upload_rate_limit << ".");

               r = _e->bytesToInt(&download_rate_limit);
               DESERIALIZE_CHECK(!r, "Unable to load global limit, download", 0);
               MVERBOSE_LOG(logWrapper(), verboseFlag_, "Download limit: " << 
                            download_rate_limit << ".");

               r = _e->bytesToInt(&max_uploads);
               DESERIALIZE_CHECK(!r, "Unable to load global limit, max uploads", 0);
               MVERBOSE_LOG(logWrapper(), verboseFlag_, "Max uploads: " << 
                            max_uploads << ".");
               
               r = _e->bytesToLong(&max_connections);
               DESERIALIZE_CHECK(!r, "Unable to load global limit, max connections", 0);
               MVERBOSE_LOG(logWrapper(), verboseFlag_, "Max connections: " << 
                            max_connections << ".");
               
               limitManager_.set(upload_rate_limit, 
                                 download_rate_limit,
                                 max_uploads,
                                 max_connections);
            }
         
         t_int handler_count = 0;

         // get number of handlers
         t_int handler_count_tmp;

         DESERIALIZE_CHECK(!_e->bytesToInt(&handler_count_tmp),
                           "SessionSaver::loadSessions(), failed to parse stream. Improper format.",
                           0);

         for (t_int x=0;
              x < handler_count_tmp;
              x++)
            {
               t_long      session;
               std::string sessionName;
               std::string username;
               t_int       seedLimit;
               t_long      seedTimeout;

               DESERIALIZE_CHECK(!_e->bytesToLong(&session), "SessionSaver::loadSessions(), failed to parse stream. Missing session id for session no " << handler_count, handler_count);
               DESERIALIZE_CHECK(!_e->bytesToString(&sessionName), "SessionSaver::loadSessions(), failed to parse stream. Missing session name for session no " << handler_count, handler_count);
               DESERIALIZE_CHECK(!_e->bytesToString(&username), "SessionSaver::loadSessions(), failed to parse stream. Missing username for session no " << handler_count, handler_count);
               DESERIALIZE_CHECK(!_e->bytesToInt(&seedLimit), "SessionSaver::loadSessions(), failed to parse stream. Missing seed limit for session " << session, handler_count);
               DESERIALIZE_CHECK(!_e->bytesToLong(&seedTimeout), "SessionSaver::loadSessions(), failed to parse stream. Missing seed timeout for session " << session, handler_count);

               std::vector<btg::core::addressPort> nodes;

               eventHandler *eh = createSession(username, 
                                                session, 
                                                sessionName, 
                                                seedLimit, 
                                                seedTimeout);
               if (!eh)
                  {
                     BTG_ERROR_LOG(logWrapper(),
                                   "SessionSaver::loadSessions(), failed to create session with id " << session);
                     continue;
                  }

               if (!eh->deserialize(_e, _version))
                  {
                     BTG_ERROR_LOG(logWrapper(),
                                   "SessionSaver::loadSessions(), failed to deserialize session " << session);
                     delete eh;
                     eh = 0;
                     return handler_count;
                  }

               BTG_MNOTICE(logWrapper(),
                           "sessionSaver::loadSessions(), successfully reloaded session " << session);

               sessionlist_.add(session, eh, true);

               handler_count++;
            }

         return handler_count;
      }

      bool SessionSaver::saveSessions(btg::core::os::fstream & _file, bool const _dumpFastResume)
      {
         BTG_MNOTICE(logWrapper(),
                     "saveSessions(), Writing sessions to file");

         btg::core::externalization::Externalization* ext = 
            new btg::core::externalization::XMLRPC(logWrapper());

         // Add signature bytes.
         t_uint b = ss_version;
         ext->uintToBytes(&b);
         b = ~b;
         ext->uintToBytes(&b);

         // Global limits:
         t_int upload_rate_limit   = btg::core::limitBase::LIMIT_DISABLED;
         t_int download_rate_limit = btg::core::limitBase::LIMIT_DISABLED;
         t_int max_uploads         = btg::core::limitBase::LIMIT_DISABLED;
         t_long max_connections    = btg::core::limitBase::LIMIT_DISABLED;

         limitManager_.get(upload_rate_limit, 
                           download_rate_limit,
                           max_uploads,
                           max_connections);

         MVERBOSE_LOG(logWrapper(), verboseFlag_, "Upload limit: " << 
                      upload_rate_limit << ".");
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "Download limit: " << 
                      download_rate_limit << ".");
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "Max uploads: " << 
                      max_uploads << ".");
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "Max connections: " << 
                      max_connections << ".");

         ext->intToBytes(&upload_rate_limit);
         ext->intToBytes(&download_rate_limit);
         ext->intToBytes(&max_uploads);
         ext->longToBytes(&max_connections);

         // Moved the code that saves the sessions to the session list.
         sessionlist_.serialize(ext, _dumpFastResume);

         btg::core::dBuffer db;
         BTG_MNOTICE(logWrapper(),
                     "Calling getBuffer on " << ext << " with db " << &db);
         ext->getBuffer(db);

         delete ext;
         ext = 0;

         if (!_file.is_open())
            {
               BTG_ERROR_LOG(logWrapper(),
                             "saveSessions(), Stream is closed.");
               return false;
            }

#if HAVE_IOS_BASE
         _file.seekp (0, std::ios_base::beg);
#else
         _file.seekp (0, std::ios::beg);
#endif

         t_int size     = db.size();
         t_byteP buffer = new t_byte[size];
         db.getBytes(buffer, size);

         _file.write(reinterpret_cast<char*>(buffer), size);
         _file.rdbuf()->truncate(); // also flush

         delete [] buffer;
         buffer = 0;

         return true;
      }

      eventHandler* SessionSaver::createSession(std::string const & _username,
                                                t_long const _session_id,
                                                std::string const & _session_name,
                                                t_int const _seed_limit,
                                                t_int const _seed_timeout)
      {
         BTG_MNOTICE(logWrapper(),
                     "attempt to restore a session with id " << _session_id << " for user " << _username);

         std::string tempDir;
         std::string workDir;
         std::string seedDir;
         std::string destDir;
#if BTG_OPTION_EVENTCALLBACK
         std::string callback;
#endif // BTG_OPTION_EVENTCALLBACK

         bool tempDirPresent = dd.auth->getTempDir(_username, tempDir);
         bool workDirPresent = dd.auth->getWorkDir(_username, workDir);
         bool seedDirPresent = dd.auth->getSeedDir(_username, seedDir);
         bool destDirPresent = dd.auth->getDestDir(_username, destDir);
#if BTG_OPTION_EVENTCALLBACK
         bool callbackPresent = dd.auth->getCallbackFile(_username, callback);
#endif // BTG_OPTION_EVENTCALLBACK
         if (!tempDirPresent ||
             !workDirPresent ||
             !seedDirPresent ||
             !destDirPresent
#if BTG_OPTION_EVENTCALLBACK
             || !callbackPresent
#endif // BTG_OPTION_EVENTCALLBACK
             )
            {
               if (!tempDirPresent)
                  {
                     BTG_ERROR_LOG(logWrapper(),
                                   "tempDir missing for user " << _username);
                  }
               if (!workDirPresent)
                  {
                     BTG_ERROR_LOG(logWrapper(),
                                   "workDir missing for user " << _username);
                  }
               if (!seedDirPresent)
                  {
                     BTG_ERROR_LOG(logWrapper(),
                                   "seedDir missing for user " << _username);
                  }
               if (!destDirPresent)
                  {
                     BTG_ERROR_LOG(logWrapper(),
                                   "destDir missing for user " << _username);
                  }
#if BTG_OPTION_EVENTCALLBACK
               if (!callbackPresent)
                  {
                     BTG_ERROR_LOG(logWrapper(),
                                   "callback missing for user " << _username);
                  }
#endif // BTG_OPTION_EVENTCALLBACK
               return 0;
            }

         // Check that the directories gotten from the auth interface exist.
         if (!btg::core::os::fileOperation::check(tempDir, true) ||
             !btg::core::os::fileOperation::check(workDir, true) ||
             !btg::core::os::fileOperation::check(seedDir, true) ||
             !btg::core::os::fileOperation::check(destDir, true))
            {
               BTG_ERROR_LOG(logWrapper(),
                             "tempDir, workDir, seedDir or destDir does not exist for " << _username);
               return 0;
            }

         BTG_MNOTICE(logWrapper(),
                     "using the following paths:" << " tempdir = " << tempDir <<
                     ", work dir = " << workDir <<
                     ", seed dir = " << seedDir <<
                     ", dest dir = " << destDir);

         eventHandler* eh = new eventHandler(logWrapper(),
                                             dd.config,
                                             verboseFlag_,
                                             _username,
                                             tempDir,
                                             workDir,
                                             seedDir,
                                             destDir,
#if BTG_OPTION_EVENTCALLBACK
                                             callback,
#endif // BTG_OPTION_EVENTCALLBACK
                                             &portManager_,
                                             &limitManager_,
                                             dd.externalization,
                                             _session_id,
                                             dd.transport,
                                             dd.filetrack,
                                             dd.filter,
                                             dd.config->getUseTorrentName(),
                                             dd.connHandler,
                                             _session_name
#if BTG_OPTION_EVENTCALLBACK
                                             , dd.callbackmgr
#endif // BTG_OPTION_EVENTCALLBACK
                                             );

         // Disable DHT and encryption here. It will be enabled,
         // during the the deserialization.
         btg::core::requiredSetupData rsd(GPD->sBUILD(),
                                          _seed_limit,
                                          _seed_timeout,
                                          false,
                                          false);
         btg::core::setupCommand sc(rsd);

         if (!eh->setup(&sc))
            {
               delete eh;
               eh=0;
            }

         return eh;
      }

   } // namespace daemon
} // namespace btg

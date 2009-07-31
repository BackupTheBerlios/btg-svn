/*
 * btg Copyright (C) 2008 Michael Wojciechowski.
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

#include "daemonhandler.h"
#include <bcore/command/context_create.h>
#include <bcore/command/context_create_url.h>
#include <bcore/verbose.h>
#include <bcore/util.h>
#include <bcore/opstatus.h>

#include "modulelog.h"
#include "filetrack.h"

namespace btg
{
   namespace daemon
   {
      const std::string moduleName("hdlu");
#if BTG_OPTION_URL
      using namespace btg::core;

      void daemonHandler::handle_CN_CURLSTATUS(btg::core::Command* _command)
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");

         opStatusCommand* cosc = dynamic_cast<opStatusCommand*>(_command);
         t_uint id = cosc->id();

         t_uint urlstat = btg::core::OP_UNDEF;

         if (!urlmgr_.getStatus(id, urlstat))
            {
               sendError(_command->getType(), "Unknown URL id.");
               return;
            }
         
         opStatusResponseCommand* cosrc = new opStatusResponseCommand(id, 
                                                                                    btg::core::ST_URL, 
                                                                                    urlstat);

         if (urlstat == btg::core::OP_WORKING)
         {
            t_uint dltotal, dlnow, dlspeed;
            if (!urlmgr_.getDlProgress(id, dltotal, dlnow, dlspeed))
               {
                  sendError(_command->getType(), "Unknown URL id (by getDlProgress).");
                  return;
               }

            btg::core::setDlProgress(*cosrc, dltotal, dlnow, dlspeed);
         }
         
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << "Sending id=" << id << ", stat=" << urlstat << ".");

         sendCommand(dd_->externalization,
                     dd_->transport,
                     connectionID_,
                     cosrc);
      }

      void daemonHandler::handle_CN_CCREATEFROMURL(eventHandler* _eventhandler, 
                                                   btg::core::Command* _command)
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");

         contextCreateFromUrlCommand* ccful = dynamic_cast<contextCreateFromUrlCommand*>(_command);

         std::string userdir  = _eventhandler->getTempDir();
         std::string filename = ccful->getFilename();
         btg::core::Util::getFileFromPath(filename, filename);

         if (!dd_->filetrack->add(userdir,
                                  filename))
            {
               // File already exists.
               sendError(_command->getType(), "Torrent file already exists.");
               return;
            }

         bool unique = urlmgr_.unique(filename, userdir);

         if (!unique)
            {
               dd_->filetrack->remove(userdir,
                                      filename);

               sendError(_command->getType(), "Torrent file already exists.");
               return;
            }

         t_uint id = urlmgr_.addMapping(ccful->getUrl(),
                                       userdir,
                                       filename,
                                       _eventhandler->getSession(),
                                       ccful->getStart());
         
         sendCommand(dd_->externalization,
                     dd_->transport,
                     connectionID_,
                     new contextCreateFromUrlResponseCommand(id));
      }

      void daemonHandler::handle_CN_CCREATEURLABORT(btg::core::Command* _command)
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");

         opAbortCommand* coac = dynamic_cast<opAbortCommand*>(_command);

         t_uint id = coac->id();

         if (urlmgr_.abort(id))
            {
               sendAck(_command->getType());
            }
         else
            {
               sendError(_command->getType(), "Unable to abort.");
            }
      }

#endif // BTG_OPTION_URL

   } // namespace daemon
} // namespace btg

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

#include "modulelog.h"
#include "filetrack.h"

namespace btg
{
   namespace daemon
   {
      const std::string moduleName("hdl");
#if BTG_OPTION_URL
      using namespace btg::core;

      void daemonHandler::handle_CN_CURLSTATUS(eventHandler* _eventhandler, 
                                               btg::core::Command* _command)
      {
         contextUrlStatusCommand* cusc = dynamic_cast<contextUrlStatusCommand*>(_command);
         t_uint id = cusc->id();

         btg::core::urlStatus urlstat = URLS_UNDEF;

         if (!urlmgr.getStatus(id, urlstat))
            {
               sendError(_command->getType(), "Unknown URL id.");
               return;
            }

         sendCommand(dd_->externalization,
                     dd_->transport,
                     connectionID_,
                     new contextUrlStatusResponseCommand(id, urlstat));
      }

      void daemonHandler::handle_CN_CCREATEFROMURL(eventHandler* _eventhandler, 
                                                   btg::core::Command* _command)
      {
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

         bool unique = urlmgr.unique(filename, userdir);

         if (!unique)
            {
               dd_->filetrack->remove(userdir,
                                      filename);

               sendError(_command->getType(), "Torrent file already exists.");
               return;
            }

         t_uint id = urlmgr.addMapping(ccful->getUrl(),
                                       userdir,
                                       filename,
                                       _eventhandler->getSession(),
                                       ccful->getStart());
         
         sendCommand(dd_->externalization,
                     dd_->transport,
                     connectionID_,
                     new contextCreateFromUrlResponseCommand(id));
      }

#endif // BTG_OPTION_URL

   } // namespace daemon
} // namespace btg

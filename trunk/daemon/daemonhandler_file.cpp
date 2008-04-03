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
#include <bcore/command/context_create_file.h>
#include <bcore/verbose.h>

#include "modulelog.h"
#include "filetrack.h"

namespace btg
{
   namespace daemon
   {
      const std::string moduleName("hdl");
      const t_uint max_file_age = 30;

      using namespace btg::core;

      void daemonHandler::handle_CN_CCREATEFROMFILE(eventHandler* _eventhandler, 
                                                    btg::core::Command* _command)
      {
         contextCreateFromFileCommand* ccffc = dynamic_cast<contextCreateFromFileCommand*>(_command);
         std::string userdir  = _eventhandler->getTempDir();
         std::string filename = ccffc->getFilename();
         
         if (!dd_->filetrack->add(userdir,
                                  filename))
            {
               // File already exists.
               sendError(_command->getType(), "Torrent file already exists.");
               return;
            }

         t_uint id = filemgr.addFile(userdir + "/" + filename, ccffc->numberOfParts());

         sendCommand(dd_->externalization,
                     dd_->transport,
                     connectionID_,
                     new contextCreateFromFileResponseCommand(id));

         MVERBOSE_LOG(logWrapper(), 
                      verboseFlag_, "File ID: " << id << ", added '" << filename << 
                      "', waiting for file parts.");
      }

      void daemonHandler::handle_CN_CCREATEFROMFILEPART(eventHandler* _eventhandler, 
                                                        btg::core::Command* _command)
      {
         contextCreateFromFilePartCommand* ccffpc = dynamic_cast<contextCreateFromFilePartCommand*>(_command);
         if (!filemgr.addPiece(ccffpc->id(),
                               ccffpc->part(),
                               ccffpc->data()))
            {
               sendError(_command->getType(), "Upload of piece failed.");

               // TODO: remove the file from file tracker.
            }
         else
            {
               sendAck(_command->getType());
               MVERBOSE_LOG(logWrapper(), 
                            verboseFlag_, "File ID: " << ccffpc->id() << ", added part " << ccffpc->part() << ".");
            }

         // TODO: check if this was the last piece and create the context.
      }

      void daemonHandler::handle_CN_CCRFILESTATUS(eventHandler* _eventhandler, 
                                                  btg::core::Command* _command)
      {
         contextFileStatusCommand* cfsc = dynamic_cast<contextFileStatusCommand*>(_command);
      }

   } // namespace daemon
} // namespace btg

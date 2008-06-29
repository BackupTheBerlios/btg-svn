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
#include <bcore/util.h>
#include <bcore/opstatus.h>

#include "modulelog.h"
#include "filetrack.h"

namespace btg
{
   namespace daemon
   {
      const std::string moduleName("hdlf");

      using namespace btg::core;

      void daemonHandler::handle_CN_CCREATEFROMFILE(eventHandler* _eventhandler, 
                                                    btg::core::Command* _command)
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");

         contextCreateFromFileCommand* ccffc = dynamic_cast<contextCreateFromFileCommand*>(_command);
         std::string userdir  = _eventhandler->getTempDir();
         std::string filename=ccffc->getFilename();
         btg::core::Util::getFileFromPath(filename, filename);
         
         if (!dd_->filetrack->add(userdir,
                                  filename))
            {
               // File already exists.
               sendError(_command->getType(), "Torrent file already exists.");
               return;
            }

         t_uint id = filemgr.addFile(userdir, 
                                     filename, 
                                     ccffc->numberOfParts(), 
                                     ccffc->getStart());

         sendCommand(dd_->externalization,
                     dd_->transport,
                     connectionID_,
                     new contextCreateFromFileResponseCommand(id));

         BTG_MNOTICE(logWrapper(), 
                     "File ID: " << id << ", added '" << filename << 
                     "', waiting for file parts.");
      }

      void daemonHandler::handle_CN_CCREATEFROMFILEPART(eventHandler* _eventhandler, 
                                                        btg::core::Command* _command)
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");

         contextCreateFromFilePartCommand* ccffpc = dynamic_cast<contextCreateFromFilePartCommand*>(_command);
         const t_uint id = ccffpc->id();

         if (!filemgr.addPiece(id,
                               ccffpc->part(),
                               ccffpc->data()))
            {
               sendError(_command->getType(), "Upload of piece failed.");
               filemgr.setState(id, fileData::DLERROR);
               filemgr.removeData(id);
            }
         else
            {
               sendAck(_command->getType());
               BTG_MNOTICE(logWrapper(), 
                           "File ID: " << id << ", added part " << ccffpc->part() << ".");
            }
         
         if (filemgr.getStatus(id) == fileData::DONE)
            {
               std::string dir;
               std::string filename;
               bool start;
               filemgr.getFileInfo(id, dir, filename, start);

               // Got all pieces.
               // Remove from file tracker, will be added when adding the torrent.
               dd_->filetrack->remove(dir, filename);

               btg::core::sBuffer sbuf;
               if (!filemgr.getResult(id, sbuf))
                  {
                     filemgr.setState(id, fileData::DLERROR);
                     filemgr.removeData(id);
                     return;  
                  }

               // Add the torrent.
               btg::core::contextCreateWithDataCommand* ccwdc = 
                  new btg::core::contextCreateWithDataCommand(filename, sbuf, start);

               // Do not use a connection id.
               if (!_eventhandler->createWithData(ccwdc))
                  {
                     BTG_MERROR(logWrapper(), 
                                "Adding '" << filename << "' from file failed.");
                     filemgr.setState(id, fileData::CREATE_ERROR);
                  }
               else
                  {
                     BTG_MNOTICE(logWrapper(), 
                                 "Added '" << filename << "' from file.");
                     filemgr.setState(id, fileData::CREATED);
                  }
         
               filemgr.removeData(id);

               delete ccwdc;
               ccwdc = 0;
            }
      }

      void daemonHandler::handle_CN_CCRFILESTATUS(btg::core::Command* _command)
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");

         opStatusCommand* cosc = dynamic_cast<opStatusCommand*>(_command);

         t_uint id = cosc->id();
         fileData::Status s = filemgr.getStatus(id);

         t_uint status = btg::core::OP_UNDEF;

         switch (s)
            {
            case fileData::UNDEF:
               {
                  // Wrong ID.
                  status = btg::core::OP_UNDEF;
                  break;
               }
            case fileData::INIT:
            case fileData::WORK:
               {
                  status = btg::core::OP_WORKING;
                  break;
               }
            case fileData::DONE:
               {
                  status = btg::core::OP_FINISHED;
                  break;
               }
            case fileData::DLERROR:
               {
                  status = btg::core::OP_ERROR;
                  break;
               }
            case fileData::CREATED:
               {
                  status = btg::core::OP_CREATE;
                  break;
               }
            case fileData::CREATE_ERROR:
               {
                  status = btg::core::OP_CREATE_ERR;
                  break;
               }
            case fileData::ABORTED:
               {
                  status = btg::core::OP_ABORTED;
                  break;
               }
            }
         
         if (status == btg::core::OP_UNDEF)
            {
               sendError(_command->getType(), "Unknown file id.");
            }
         else
            {
               sendCommand(dd_->externalization,
                           dd_->transport,
                           connectionID_,

                           new opStatusResponseCommand(id, 
                                                              btg::core::ST_FILE, 
                                                              status));
            }
      }

      void daemonHandler::handle_CN_CCREATEFFABORT(btg::core::Command* _command)
      {
         MVERBOSE_LOG(logWrapper(), verboseFlag_, "client (" << connectionID_ << "): " << _command->getName() << ".");

         opAbortCommand* coac = static_cast<opAbortCommand*>(_command);

         t_uint id = coac->id();

         if (filemgr.abort(id))
            {
               sendAck(_command->getType());
            }
         else
            {
               sendError(_command->getType(), "Unknown file id.");
            }
      }

      void daemonHandler::handleFileDownloads()
      {
         if (filemgr.size() > 0)
            {
               MVERBOSE_LOG(logWrapper(), verboseFlag_, "Checking file downloads (" << filemgr.size() << ").");
            }
         filemgr.updateAge();
         filemgr.removeDead();
      }

   } // namespace daemon
} // namespace btg

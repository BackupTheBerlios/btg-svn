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

#include "filephelper.h"
#include "clienthandler.h"
#include "clientcallback.h"
#include <bcore/util.h>
#include <bcore/filestatus.h>
#include <bcore/os/fileop.h>
#include <bcore/os/sleep.h>

#include <iostream>

namespace btg
{
   namespace core
   {
      namespace client
      {
         createPartsReportInterface::createPartsReportInterface()
            : continue_(true)
         {
         }

         void createPartsReportInterface::CPRI_reset()
         {
            continue_ = true;
         }

         void createPartsReportInterface::CPRI_cancel()
         {
            continue_ = false;
         }

         bool createPartsReportInterface::CPRI_continue() const
         {
            return continue_;
         }

         createPartsReportInterface::~createPartsReportInterface()
         {
         }

         /* */

         bool createParts(btg::core::LogWrapperType _logwrapper,
                          clientHandler* _ch,
                          createPartsReportInterface* _cpri,
                          std::string const& _filename,
                          t_uint const _partSize)
         {
            std::string fullfilename = _filename;
            btg::core::os::fileOperation::resolvePath(fullfilename);

            std::string filename;
            Util::getFileFromPath(_filename, filename);

            _cpri->CPRI_init(filename);

            if (!_cpri->CPRI_continue())
               {
                  _cpri->CPRI_error("Cancelled.");
                  return false;
               }

            // Split file into parts.
            sBuffer tf;
            if (!tf.read(fullfilename))
               {
                  _cpri->CPRI_error("Unable to read input file.");
                  BTG_NOTICE(_logwrapper, "Error: Unable to read file '" << _filename << "'.");
                  return false;
               }

            BTG_NOTICE(_logwrapper, "Size = " << tf.size() << ".");

            std::vector<sBuffer> buffers;
            if (!tf.split(_partSize, buffers))
               {
                  _cpri->CPRI_error("Unable to split buffer.");
                  BTG_NOTICE(_logwrapper, "Error: Unable to split buffer.");
                  return false;
               }

            if (!_cpri->CPRI_continue())
               {
                  _cpri->CPRI_error("Cancelled.");
                  return false;
               }
            
            t_uint part = 0;
            t_uint nop  = buffers.size();

            _ch->reqCreateFromFile(filename, nop);

            if (!_ch->commandSuccess())
               {
                  _cpri->CPRI_error("(daemon) Unable to create file.");
                  BTG_NOTICE(_logwrapper, "Error: Creating file failed.");
                  return false;
               }

            t_uint id = _ch->getFileId();

            for (std::vector<sBuffer>::const_iterator iter = buffers.begin();
                 iter != buffers.end();
                 iter++)
               {
                  BTG_NOTICE(_logwrapper, "Sending part " << part << ".");

                  _ch->reqTransmitFilePart(id, 
                                           part,
                                           *iter);

                  if (!_ch->commandSuccess())
                     {
                        _cpri->CPRI_error("Sending file parts failed.");
                        BTG_NOTICE(_logwrapper, "Sending part failed.");
                        return false;
                     }

                  if (!_cpri->CPRI_continue())
                     {
                        _ch->reqCancelFile(id);
                        _cpri->CPRI_error("Cancelled.");
                        return false;
                     }

                  _cpri->CPRI_pieceUploaded(part, nop);
                  part++;
               }

            BTG_NOTICE(_logwrapper, "All parts sent.");
            _cpri->CPRI_wait("File sent.");

            // Wait for the daemon to create the file.
            bool op_status = true;
            bool cont = true;
            while (cont)
               {
                  _ch->reqFileStatus(id);
                  if (!_ch->commandSuccess())
                     {
                        _cpri->CPRI_error("Comm error.");
                        return false;
                     }

                  t_uint id = 0;
                  btg::core::fileStatus status;

                  _ch->fileStatusResponse(id, status);

                  switch (status)
                     {
                     case btg::core::FILES_UNDEF:
                        {
                           break;
                        }
                     case btg::core::FILES_WORKING:
                        {
                           _cpri->CPRI_wait("Working..");
                           break;
                        }
                     case btg::core::FILES_FINISHED:
                        {
                           _cpri->CPRI_wait("Loading finished");
                           break;
                        }
                     case btg::core::FILES_ERROR:
                        {
                           op_status = false;
                           _cpri->CPRI_error("Upload failed.");
                           cont = false;
                           break;
                        }
                     case btg::core::FILES_CREATE:
                        {
                           _cpri->CPRI_success(filename);
                           cont = false;
                           break;
                        }
                     case btg::core::FILES_CREATE_ERR:
                        {
                           op_status = false;
                           _cpri->CPRI_error("Unable to create torrent.");
                           cont = false;
                           break;
                        }
                     case btg::core::FILES_ABORTED:
                        {
                           op_status = false;
                           _cpri->CPRI_error("Aborted.");
                           cont = false;
                           break;
                        }
                     }
                  btg::core::os::Sleep::sleepMiliSeconds(500);
               }
 
           return op_status;
         }

      } // client
   } // core
} // btg



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

         bool createPartsReportInterface::CPRI_cancel()
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
                  _cpri->CPRI_error("Unable to read input.");
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
                        BTG_NOTICE(_logwrapper, "Sending part failed.");
                        return false;
                     }

                  if (!_cpri->CPRI_continue())
                     {
                        _cpri->CPRI_error("Cancelled.");
                        return false;
                     }

                  _cpri->CPRI_pieceUploaded(part, nop);
                  part++;
               }

            _cpri->CPRI_success(filename);
            BTG_NOTICE(_logwrapper, "All parts sent.");
            return true;
         }

      } // client
   } // core
} // btg



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

#include "loadhelper.h"
#include "clienthandler.h"
#include "clientcallback.h"
#include <bcore/util.h>
#include <bcore/opstatus.h>
#include <bcore/os/fileop.h>
#include <bcore/os/sleep.h>

#include <iostream>

namespace btg
{
   namespace core
   {
      namespace client
      {
         bool createParts(btg::core::LogWrapperType _logwrapper,
                          clientHandler & _ch,
                          createProgressIf & _cpif,
                          std::string const& _filename,
                          t_uint const _partSize)
         {
            std::string fullfilename = _filename;
            btg::core::os::fileOperation::resolvePath(fullfilename);

            std::string filename;
            Util::getFileFromPath(_filename, filename);

            _cpif.CPIF_init(false /* file */);
            _cpif.CPIF_begin(filename);
            

            if (!_cpif.CPIF_continue())
               {
                  _cpif.CPIF_error("Cancelled.");
                  return false;
               }

            // Split file into parts.
            sBuffer tf;
            if (!tf.read(fullfilename))
               {
                  _cpif.CPIF_error("Unable to read input file.");
                  BTG_NOTICE(_logwrapper, "Error: Unable to read file '" << _filename << "'.");
                  return false;
               }

            BTG_NOTICE(_logwrapper, "Size = " << tf.size() << ".");

            std::vector<sBuffer> buffers;
            if (!tf.split(_partSize, buffers))
               {
                  _cpif.CPIF_error("Unable to split buffer.");
                  BTG_NOTICE(_logwrapper, "Error: Unable to split buffer.");
                  return false;
               }

            if (!_cpif.CPIF_continue())
               {
                  _cpif.CPIF_error("Cancelled.");
                  return false;
               }
            
            t_uint part = 0;
            t_uint nop  = buffers.size();

            _ch.reqCreateFromFile(filename, nop);

            if (!_ch.commandSuccess())
               {
                  _cpif.CPIF_error("(daemon) Unable to create file.");
                  BTG_NOTICE(_logwrapper, "Error: Creating file failed.");
                  return false;
               }

            t_uint id = _ch.getFileId();

            for (std::vector<sBuffer>::const_iterator iter = buffers.begin();
                 iter != buffers.end();
                 iter++)
               {
                  BTG_NOTICE(_logwrapper, "Sending part " << part << ".");

                  _ch.reqTransmitFilePart(id, 
                                          part,
                                          *iter);

                  if (!_ch.commandSuccess())
                     {
                        _cpif.CPIF_error("Sending file parts failed.");
                        BTG_NOTICE(_logwrapper, "Sending part failed.");
                        return false;
                     }

                  if (!_cpif.CPIF_continue())
                     {
                        _ch.reqCancelFile(id);
                        _cpif.CPIF_error("Cancelled.");
                        return false;
                     }

                  _cpif.CPIF_filePiece(part, nop);
                  part++;
               }

            BTG_NOTICE(_logwrapper, "All parts sent.");
            _cpif.CPIF_wait("File sent.");

            // Wait for the daemon to create the file.
            bool op_status = true;
            bool cont = true;
            while (cont)
               {
                  _ch.reqFileStatus(id);
                  if (!_ch.commandSuccess())
                     {
                        _cpif.CPIF_error("Comm error.");
                        return false;
                     }

                  t_uint id = 0;
                  t_uint status;

                  _ch.fileStatusResponse(id, status);

                  switch (status)
                     {
                     case btg::core::OP_UNDEF:
                        {
                           break;
                        }
                     case btg::core::OP_WORKING:
                        {
                           _cpif.CPIF_wait("Working..");
                           break;
                        }
                     case btg::core::OP_FINISHED:
                        {
                           _cpif.CPIF_wait("Loading finished");
                           break;
                        }
                     case btg::core::OP_ERROR:
                        {
                           op_status = false;
                           _cpif.CPIF_error("Upload failed.");
                           cont = false;
                           break;
                        }
                     case btg::core::OP_CREATE:
                        {
                           _cpif.CPIF_success(filename);
                           cont = false;
                           break;
                        }
                     case btg::core::OP_CREATE_ERR:
                        {
                           op_status = false;
                           _cpif.CPIF_error("Unable to create torrent.");
                           cont = false;
                           break;
                        }
                     case btg::core::OP_ABORTED:
                        {
                           op_status = false;
                           _cpif.CPIF_error("Aborted.");
                           cont = false;
                           break;
                        }
                     }
                  btg::core::os::Sleep::sleepMiliSeconds(500);
               }
 
           return op_status;
         }

         bool createUrl(btg::core::LogWrapperType _logwrapper,
                        class clientHandler & _ch,
                        createProgressIf & _cpif,
                        std::string const& _filename,
                        std::string const& _url)
         {
            bool res = false;

            _cpif.CPIF_init(true /* url */);
            _cpif.CPIF_begin(_filename, _url);

            _ch.reqCreateFromUrl(_filename, _url);
            if (!_ch.commandSuccess())
               {
                  BTG_NOTICE(_logwrapper, "Unable to load URL '" << _url << "', filename '" << _filename << "'");
                  _cpif.CPIF_error("Unable to load URL");
                  return res;
               }

            if (!_cpif.CPIF_continue())
               {
                  _cpif.CPIF_error("Cancelled.");
                  return false;
               }

            t_uint hid = _ch.UrlId();

            bool cont = true;
            while (cont)
               {
                  if (!_cpif.CPIF_continue())
                     {
                        _cpif.CPIF_error("Cancelled.");
                        return false;
                     }
                  

                  _ch.reqUrlStatus(hid);
                  if (!_ch.commandSuccess())
                     {
                        BTG_NOTICE(_logwrapper, "Getting status for id " << hid << " failed.");
                        _cpif.CPIF_error("Unable to load URL");
                        return res;
                     }

                  t_uint id     = 0;
                  t_uint status = 0;

                  _ch.UrlStatusResponse(id, status);
                  
                  switch (status)
                     {
                     case btg::core::OP_UNDEF:
                        {
                           break;
                        }
                     case btg::core::OP_WORKING:
                        {
                           t_uint total = 0;
                           t_uint now   = 0;
                           t_uint speed = 0;

                           if (_ch.getUrlDlProgress(total, now, speed))
                              {
                                 _cpif.CPIF_urlDlStatus(total, now, speed);
                              }
                           break;
                        }
                     case btg::core::OP_FINISHED:
                        {
                           _cpif.CPIF_wait("Created URL.");
                           break;
                        }
                     case btg::core::OP_ERROR:
                        {
                           _cpif.CPIF_error("Error loading URL.");
                           cont = false;
                           break;
                        }
                     case btg::core::OP_CREATE:
                        {
                           _cpif.CPIF_success("Torrent created.");
                           res  = true;
                           cont = false;
                           break;
                        }
                     case btg::core::OP_CREATE_ERR:
                        {
                           _cpif.CPIF_error("Error creating context.");
                           res  = false;
                           cont = false;
                           break;
                        }
                     }
                  btg::core::os::Sleep::sleepMiliSeconds(500);
               }
 
           return res;
         }

      } // client
   } // core
} // btg



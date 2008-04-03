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

namespace btg
{
   namespace core
   {
      namespace client
      {

         bool createParts(clientHandler* _ch,
                          clientCallback* _cc,
                          std::string const& _filename,
                          t_uint const _partSize)
         {
            std::string filename = _filename;
 
            Util::getFileFromPath(_filename, filename);

            // Split file into parts.
            sBuffer tf;
            if (!tf.read(_filename))
               {
                  return false;
               }

            std::vector<sBuffer> buffers;
            tf.split(_partSize, buffers);
            t_uint part = 0;
            t_uint nop  = buffers.size();

            _ch->reqCreateFromFile(filename, nop);

            if (!_ch->commandSuccess())
               {
                  return false;
               }

            t_uint id = _ch->getFileId();

            for (std::vector<sBuffer>::const_iterator iter = buffers.begin();
                 iter != buffers.end();
                 iter++)
               {
                  _ch->reqTransmitFilePart(id, 
                                           part,
                                           *iter);

                  if (!_ch->commandSuccess())
                     {
                        return false;
                     }
               }

            return true;
         }

      } // client
   } // core
} // btg



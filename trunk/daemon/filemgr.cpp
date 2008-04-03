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

#include "filemgr.h"
#include <boost/bind.hpp>
#include <daemon/modulelog.h>

namespace btg
{
   namespace daemon
   {
      fileData::fileData(std::string const& _filename, t_uint _parts)
         : filename(_filename),
           parts(_parts),
           current_part(0),
           buffers(),
           status(fileData::INIT),
           valid(true)
      {
      }

      /* */

      const std::string moduleName("fmgr");
      const t_uint min_id = 1;

      fileManager::fileManager(btg::core::LogWrapperType _logwrapper)
         : btg::core::Logable(_logwrapper),
           current_id(min_id),
           max_id(0xFFFFFF)
      {
         BTG_MNOTICE(logWrapper(), "constructed");
      }

      t_uint fileManager::getCurrentId()
      {
         t_uint id = current_id;
         current_id++;

         if (id > max_id)
            {
               current_id = min_id;
            }

         return id;
      }

      t_uint fileManager::addFile(std::string const& _filename, 
                                  t_uint const _parts)
      {
         t_uint id = getCurrentId();

         BTG_MNOTICE(logWrapper(), 
                     "add(" << id << "): " << _filename << " in " << _parts << " parts.");

         fileData fd(_filename, _parts);
         std::pair<t_uint, fileData> pair(id, fd);

         files.insert(pair);

         return id;
      }

      bool fileManager::addPiece(const t_uint _id, 
                                 const t_uint _num,
                                 btg::core::sBuffer const& _data)
      {
         std::map<t_uint, fileData>::iterator iter = files.find(_id);

         if (iter == files.end())
            {
               return false;
            }

         if (iter->second.current_part != _num)
            {
               // Received unexpected buffer.
               iter->second.status = fileData::DLERROR;
               iter->second.buffers.clear();
               iter->second.valid  = false;

               return false;
            }

         iter->second.current_part++;

         iter->second.status = fileData::WORK;
         iter->second.buffers.push_back(_data);

         if (iter->second.current_part == iter->second.parts)
            {
               // Finished download.
               iter->second.status = fileData::DONE;
            }
         return true;
      }

      bool fileManager::nextPiece(const t_uint _id, t_uint & _piece) const
      {
         std::map<t_uint, fileData>::const_iterator iter = files.find(_id);

         if (iter == files.end())
            {
               return false;
            }

         _piece = iter->second.current_part;

         return true;
      }

      fileData::Status fileManager::getStatus(const t_uint _id)
      {
         std::map<t_uint, fileData>::const_iterator iter = files.find(_id);

         if (iter == files.end())
            {
               return fileData::UNDEF;
            }

         return iter->second.status;
      }

      bool fileManager::getResult(const t_uint _id, 
                                  btg::core::sBuffer & _buffer)
      {
         std::map<t_uint, fileData>::iterator iter = files.find(_id);

         if (iter == files.end())
            {
               return false;
            }

         if (iter->second.status != fileData::DONE)
            {
               return false;
            }
         
         _buffer = btg::core::sBuffer(iter->second.buffers);

         iter->second.buffers.clear();

         return true;
      }

      void fileManager::invalidate(const t_uint _id)
      {
         std::map<t_uint, fileData>::iterator iter = files.find(_id);

         if (iter == files.end())
            {
               return;
            }

         iter->second.valid = false;
      }

      void fileManager::setState(const t_uint _id, 
                                 fileData::Status const _status)
      {
         std::map<t_uint, fileData>::iterator iter = files.find(_id);

         if (iter == files.end())
            {
               return;
            }

         iter->second.status = _status;
      }

      fileManager::~fileManager()
      {
         files.clear();
      }

   } // namespace daemon
} // namespace btg




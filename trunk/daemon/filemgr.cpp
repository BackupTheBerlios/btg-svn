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
#include "modulelog.h"
#include "filetrack.h"

namespace btg
{
   namespace daemon
   {
      const t_uint max_file_age = 1;
      const std::string moduleName("fmgr");

      fileData::fileData(std::string const& _dir,
                         std::string const& _filename, 
                         t_uint _parts,
                         bool _start)
         : dir(_dir),
           filename(_filename),
           parts(_parts),
           start(_start),
           current_part(0),
           buffers(),
           status(fileData::INIT),
           valid(true),
           age(0)
      {
      }

      /* */

      fileManager::fileManager(btg::core::LogWrapperType _logwrapper,
                               fileTrack* _filetrack,
                               btg::daemon::opId & _opid)
         : btg::core::Logable(_logwrapper),
           filetrack(_filetrack),
           opid(_opid)
      {
         BTG_MNOTICE(logWrapper(), "constructed");
      }

      t_uint fileManager::getCurrentId()
      {
         return opid.id();
      }

      t_uint fileManager::addFile(std::string const& _dir, 
                                  std::string const& _filename, 
                                  t_uint const _parts,
                                  bool const _start)
      {
         t_uint id = getCurrentId();

         BTG_MNOTICE(logWrapper(), 
                     "add(" << id << "): " << _filename << " in " << _parts << " parts.");

         fileData fd(_dir, _filename, _parts, _start);
         std::pair<t_uint, fileData> pair(id, fd);

         files.insert(pair);

         return id;
      }

      bool fileManager::addPiece(const t_uint _id, 
                                 const t_uint _num,
                                 btg::core::sBuffer const& _data)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

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

      bool fileManager::nextPiece(const t_uint _id, t_uint & _piece)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

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
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

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
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

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
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::map<t_uint, fileData>::iterator iter = files.find(_id);

         if (iter == files.end())
            {
               return;
            }

         iter->second.valid = false;
      }

      void fileManager::resetAge(const t_uint _id)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::map<t_uint, fileData>::iterator iter = files.find(_id);

         if (iter == files.end())
            {
               return;
            }

         iter->second.age = 0;
      }

      void fileManager::setState(const t_uint _id, 
                                 fileData::Status const _status)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::map<t_uint, fileData>::iterator iter = files.find(_id);

         if (iter == files.end())
            {
               return;
            }

         iter->second.status = _status;
      }

      bool fileManager::abort(const t_uint _id)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::map<t_uint, fileData>::iterator iter = files.find(_id);

         if (iter == files.end())
            {
               return false;
            }
         
         bool status = true;

         switch (iter->second.status)
            {
            case fileData::UNDEF:
            case fileData::INIT:
               {
                  iter->second.status = fileData::ABORTED;
                  invalidate(_id);
                  status = true;
                  break;
               }
            case fileData::WORK:
            case fileData::DONE:
               {
                  removeData(_id);
                  iter->second.status = fileData::ABORTED;
                  invalidate(_id);
                  status = true;
                  break;
               }
            case fileData::DLERROR:
            case fileData::CREATED:
            case fileData::CREATE_ERROR:
               {
                  status = false;
                  break;
               }
            case fileData::ABORTED:
               {
                  status = true;
                  break;
               }
            }
         return status;
      }

      void fileManager::updateAge()
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::map<t_uint, fileData>::iterator iter;
         for (iter = files.begin();
              iter != files.end();
              iter++)
            {
               BTG_MNOTICE(logWrapper(), "filename: " << iter->second.filename << ", age: " << iter->second.age);
               
               iter->second.age++;
               if (iter->second.age > max_file_age)
                  {
                     iter->second.valid = false;
                  }
            }
      }

      void fileManager::removeDead()
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::map<t_uint, fileData>::iterator iter = files.begin();
         while (iter != files.end())
            {
               if (!iter->second.valid)
                  {
                     filetrack->remove(iter->second.dir, iter->second.filename);
                     files.erase(iter++);
                  }
               else
                  ++iter;
            }
      }
      
      void fileManager::removeData(const t_uint _id)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::map<t_uint, fileData>::iterator iter = files.find(_id);

         if (iter == files.end())
            {
               return;
            }

         iter->second.buffers.clear();
      }

      void fileManager::getFileInfo(const t_uint _id,
                                    std::string & _dir,
                                    std::string & _filename,
                                    bool        & _start)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::map<t_uint, fileData>::iterator iter = files.find(_id);
         _dir      = iter->second.dir;
         _filename = iter->second.filename;
         _start    = iter->second.start;
      }

      t_uint fileManager::size()
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         return files.size();
      }

      fileManager::~fileManager()
      {
         files.clear();
      }

   } // namespace daemon
} // namespace btg




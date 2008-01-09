/*
 * btg Copyright (C) 2005 Michael Wojciechowski.
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

#include "filetrack.h"
#include <bcore/logmacro.h>

#include "modulelog.h"

namespace btg
{
   namespace daemon
   {
      const std::string moduleName("fit");

      fileTrack::fileTrack(btg::core::LogWrapperType _logwrapper)
         : btg::core::Logable(_logwrapper),
           files_()
      {
      }

      bool fileTrack::add(std::string const& _userdir,
                          std::string const& _filename)
      {
         BTG_MNOTICE(logWrapper(), "adding filename '" << _filename << "'.");

         bool status = false;

         std::map<std::string, std::vector<fileTrackData> >::iterator mapiter = 
            files_.find(_userdir);

         if (mapiter == files_.end())
            {
               std::vector<fileTrackData> v;
               std::pair<std::string, std::vector<fileTrackData> > p;
               p.first  = _userdir;
               p.second = v;
               files_.insert(p);

               mapiter = files_.find(_userdir);
            }

         std::vector<fileTrackData>::iterator iter;
         if (!check(_userdir, _filename, iter))
            {
               // Add.
               status = true;
               mapiter->second.push_back(_filename);
            }

         return status;
      }

      bool fileTrack::setFiles(std::string const& _userdir,
                               std::string const& _filename,
                               std::vector<std::string> const& _files)
      {
         std::vector<fileTrackData>::iterator iter;
         if (!check(_userdir, _filename, iter))
            {
               return false;
            }

         iter->setFiles(_files);

         std::map<std::string, std::vector<fileTrackData> >::iterator mapiter = 
            files_.find(_userdir);
         
         // Find out if the files which were set are unique.
	 
         std::vector<fileTrackData>::iterator uiter;

         t_uint matches = 0;

         for (uiter = mapiter->second.begin();
              uiter != mapiter->second.end();
              uiter++)
            {
               // If a single file matches another torrent, we got a match.
               if (!uiter->isUnique(iter->entries()))
                  {
                     matches++;
                  }
            }
	 
         if (matches > 1)
            {
               // More than one torrent contains one or more file
               // contained in the torrent which is used for compartion.
               return false;
            }

         // The files added are unique.
         return true;
      }

      void fileTrack::remove(std::string const& _userdir,
                             std::string const& _filename)
      {
         BTG_MNOTICE(logWrapper(), "removing filename '" << _filename << "'.");

         std::vector<fileTrackData>::iterator iter;
         if (!check(_userdir, _filename, iter))
            {
               BTG_MNOTICE(logWrapper(), "filename '" << _filename << "' was not found.");
               return;
            }

         // files_[_userdir].erase(iter);

         std::map<std::string, std::vector<fileTrackData> >::iterator mapiter = 
            files_.find(_userdir);

         mapiter->second.erase(iter);

         if (mapiter->second.size() == 0)
            {
               files_.erase(mapiter);
            }
      }

      bool fileTrack::exists(std::string const& _userdir,
                             std::string const& _filename)
      {
         std::vector<fileTrackData>::iterator iter;
         if (!check(_userdir, _filename, iter))
            {
               return false;
            }
         return true;
      }

      bool fileTrack::check(std::string const& _userdir,
                            std::string const& _filename,
                            std::vector<fileTrackData>::iterator & _iter)
      {
         std::map<std::string, std::vector<fileTrackData> >::iterator mapiter = 
            files_.find(_userdir);

         if (mapiter == files_.end())
            {
               return false;
            }
         
         std::vector<fileTrackData>::iterator iter;

         for (iter = mapiter->second.begin();
              iter != mapiter->second.end();
              iter++)
            {
               if (*iter == _filename)
                  {
                     _iter = iter;
                     return true;
                  }
            }

         return false;
      }

      fileTrack::~fileTrack()
      {
         files_.clear();
      }

   } // namespace daemon
} // namespace btg


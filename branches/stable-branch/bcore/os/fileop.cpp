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

#include "fileop.h"

#include <bcore/logmacro.h>

extern "C"
{
   // For getenv.
#  include <stdlib.h>

   // For realpath.
#  include <limits.h>
}

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/exception.hpp>

namespace btg
{
   namespace core
   {
      namespace os
      {

         /// Replace home, if needed.
         void resolveHome(std::string & _path);

         bool fileOperation::check(std::string & _path,
                                   std::string & _errorMessage,
                                   bool const _directory)
         {
            bool result = true;

            resolveHome(_path);

            try
               {

                  boost::filesystem::path p(_path, boost::filesystem::native);

                  if (!boost::filesystem::exists(p))
                     {
                        result = false;
                     }
            
                  if (result && _directory)
                     {
                        if (!boost::filesystem::is_directory(p))
                           {
                              result = false;
                           }
                     }
               }
            catch(boost::filesystem::filesystem_error & _fserr)
               {
                  _errorMessage = _fserr.what();
                  result = false;
               }

            return result;
         }

         bool fileOperation::check(std::string & _path,
                                   bool const _directory)
         {
            std::string errorstr;

            bool result = fileOperation::check(_path, errorstr, _directory);

            return result;
         }

         bool fileOperation::resolvePath(std::string & _path)
         {
            bool status = false;

            // Replace home, if needed.
            resolveHome(_path);

            char* rpath = reinterpret_cast<char*>(malloc(PATH_MAX));
            memset(rpath, 0, PATH_MAX);

            if (realpath(_path.c_str(), rpath) != 0)
               {
                  _path.clear();
                  _path.insert(0, rpath);
                  status = true;
               }

            free(rpath);

            return status;
         }

         bool fileOperation::remove(std::string const& _filename)
         {
            bool status = true;

            if (::remove(_filename.c_str()) == -1)
               {
                  status = false;
               }

            return status;
         }

         void resolveHome(std::string & _path)
         {
            // Paths starting with home.
            std::string begin = _path.substr(0, 1);

            if (begin == "~")
               {
                  char* home = getenv("HOME");
                  if (home != 0)
                     {
                        _path.erase(_path.begin());
                        std::string shome(home);
                        for (std::string::iterator i = shome.end(); i >= shome.begin(); i--)
                           {
                              if (*i != 0)
                                 {
                                    _path.insert(_path.begin(), *i);
                                 }
                           }
                     }
               } // home directory.
         }

         bool fileOperation::createDirectory(std::string const& _path)
         {
            bool status = true;

            std::string path(_path);

            std::string errorMessage;

            if (fileOperation::check(path, errorMessage, true /* directory */))
               {
                  // Already exists.
                  return status;
               }
                                   
            boost::filesystem::path p(path, boost::filesystem::native);

            try
               {
                  create_directory(p);
               }
            catch(boost::filesystem::filesystem_error & _fserr)
               {
                  status = false;
               }

            return status;
         }
      } // namespace os
   } // namespace core
} // namespace btg

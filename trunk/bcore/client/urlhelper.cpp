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

#include "urlhelper.h"
#include <bcore/util.h>

namespace btg
{
   namespace core
   {
      namespace client
      {

         bool isUrlValid(std::string const& _url)
         {
            bool r = false;
            if (_url.size() < 8)
               {
                  return r;
               }

            std::string h = _url.substr(0, 7);
            if (h == "http://")
               {
                  r = true;
               }

            h = _url.substr(0, 8);
            if (h == "https://")
               {
                  r = true;
               }

            if (r == true)
               {
                  return r;
               }

            if (_url.size() < 10)
               {
                  return r;
               }

            h = _url.substr(0, 10);
            if (h == "magnet:?xt")
               {
                  r = true;
               }

            return r;
         }

         bool getFilenameFromUrl(std::string const& _url, std::string & _filename)
         {
            bool r = false;

            if ((_url.size() < 1) || (_url.size() < 8))
               {
                  return r;
               }

            std::string fileend = _url.substr(_url.size()-8, _url.size());

            if (fileend != ".torrent")
               {
                  return r;
               }

            std::string::const_iterator i = _url.end()--;
            std::string::const_iterator j = i;

            while (j > _url.begin())
               {
                  if (*j == '/')
                     {
                        r = true;
                        break;
                     }
                  j--;
               }

            if (r)
               {
                  j++;
                  for (j = j; j < i; j++)
                     {
                        _filename += *j;
                     }
               }

            return r;
         }

      } // client
   } // core
} // btg



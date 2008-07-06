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

#include "cpif.h"

namespace btg
{
   namespace core
   {
      namespace client
      {

         createProgressIf::createProgressIf()
            : url_(false),
              continue_(true)
         {
         }

         void createProgressIf::CPIF_init(bool _url)
         {
            url_ = _url;
         }

         void createProgressIf::CPIF_reset()
         {
            continue_ = true;
         }

         void createProgressIf::CPIF_cancel()
         {
            continue_ = false;
         }

         bool createProgressIf::CPIF_continue() const
         {
            return continue_;
         }

         bool createProgressIf::CPIF_url() const
         {
            return url_;
         }
         
         bool createProgressIf::CPIF_file() const
         {
            return !url_;
         }

         createProgressIf::~createProgressIf()
         {
         }

      } // client
   } // core
} // btg




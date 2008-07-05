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

#ifndef FILEPHELPER_H
#define FILEPHELPER_H

#include <bcore/type.h>
#include <bcore/logable.h>
#include <string>
#include "cpif.h"

namespace btg
{
   namespace core
   {
      namespace client
      {
         /**
          * \addtogroup gencli
          */
         /** @{ */

         /// Create a context, sending parts of it one at a time.
         /// @param [in] _logwrapper Log wrapper object.
         /// @param [in] _ch         Pointer to client handler instance. 
         /// @param [in] _cpif       Callback interface.
         /// @param [in] _filename   The filename to read data from.
         /// @param [in] _partSize   The size of the chunks the upload consists of.
         /// @return True - torrent uploaded and created, false - otherwise.
         bool createParts(btg::core::LogWrapperType _logwrapper,
                          class clientHandler & _ch,
                          createProgressIf & _cpif,
                          std::string const& _filename,
                          t_uint const _partSize = (64*1024));

         /// Create a context, by downloading an URL.
         /// @param [in] _logwrapper Log wrapper object.
         /// @param [in] _ch         Pointer to client handler instance. 
         /// @param [in] _cpif       Callback interface.
         /// @param [in] _filename   The filename to save the URL to.
         /// @param [in] _url        The URL to download.
         /// @return True - torrent downloaded and created, false - otherwise.
         bool createUrl(btg::core::LogWrapperType _logwrapper,
                        class clientHandler & _ch,
                        createProgressIf & _cpif,
                        std::string const& _filename,
                        std::string const& _url);

         /** @} */

      } // client
   } // core
} // btg

#endif // FILEPHELPER_H


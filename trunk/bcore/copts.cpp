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

#include "copts.h"
#include <bcore/project.h>

namespace btg
{
   namespace core
   {

      void getCompileTimeOptions(std::string & _output)
      {
#if BTG_DEBUG
         _output += "debug";
         _output += GPD->sNEWLINE();
#endif // BTG_DEBUG
               
#if BTG_OPTION_SAVESESSIONS
         _output += "session saving";
         _output += GPD->sNEWLINE();
#endif // BTG_OPTION_SAVESESSIONS
               
#if BTG_OPTION_EVENTCALLBACK
         _output += "event callback";
         _output += GPD->sNEWLINE();
#endif // BTG_OPTION_EVENTCALLBACK
               
#if BTG_OPTION_UPNP
         _output += "UPnP";
         _output += GPD->sNEWLINE();
#endif // BTG_OPTION_UPNP
               
#if BTG_OPTION_USECYBERLINK
         _output += "UPnP CyberLink";
         _output += GPD->sNEWLINE();
#endif // BTG_OPTION_USECYBERLINK

      }

   } // namespace core
} // namespace btg

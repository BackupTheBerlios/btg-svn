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

#ifndef BTGVS_H
#define BTGVS_H

#include "handler.h"
#include <bcore/externalization/xmlrpc.h>
#include <bcore/client/configuration.h>
#include <bcore/client/lastfiles.h>

namespace btg
{
   namespace UI
   {
      namespace gui
      {
         namespace viewer
         {

            /// Struct holding all required data used by this client.
            struct clientData
            {
               /// Constructor.
            clientData()
            : handlerthr(0),
                  handler(0),
                  externalization(0),
                  config(0),
                  lastfiles(0)
               {
               }

               /// Handler thread resposible for communicating with
               /// the daemon.
               btg::core::client::handlerThread*             handlerthr;

               /// Handler.
               btg::UI::gui::viewer::viewerHandler*          handler;

               /// Externalization interface implementation.
               btg::core::externalization::Externalization*  externalization;

               /// Configuration.
               btg::core::client::clientConfiguration*       config;

               /// Last opened files. Unused.
               btg::core::client::lastFiles*                 lastfiles;
            };

         }
      }
   }
}

#endif


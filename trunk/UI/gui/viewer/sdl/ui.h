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

#ifndef UI_H
#define UI_H

// Define this so that NLS headers will not be included.
#define _LIBINTL_H

extern "C"
{
#include <agar/core/core.h>
#include <agar/gui/gui.h>
}

#include <string>
#include <vector>

#include <bcore/client/handlerthr.h>

namespace btg
{
   namespace UI
   {
      namespace gui
      {
         namespace viewer
         {
            struct tableData
            {
               std::string filename;
               std::string status;
               std::string progress;
               std::string dlul;
               std::string size;
               std::string peers;
            };

            struct btgvsGui
            {
               AG_Timeout      timer;
               AG_Window*      window;
               AG_VBox*        contents_box;
               AG_Notebook*    notebook;
               AG_Statusbar*   statusbar;
               AG_Label*       statusbarlabel;
               AG_NotebookTab* tabs[3];
               AG_Table*       table;
               AG_Graph*       plot;
               AG_GraphItem*   plotItem;
            };

            class viewerHandler;

            struct timerData
            {
            timerData(btgvsGui &                           _gui,
                      btg::core::client::handlerThread*    _handlerthr,
                      viewerHandler* _handler)
            : gui(_gui), handlerthr(_handlerthr), handler(_handler)
               {
               }

               btgvsGui &                           gui;
               btg::core::client::handlerThread*    handlerthr;
               viewerHandler* handler;
            };

            void createGui(btgvsGui & _gui);

            void createTimer(btgvsGui & _gui, timerData* _timerdata);

            void run();

            void destroyGui(btgvsGui & _gui);

            void updateTable(btgvsGui & _gui, std::vector<tableData> const& _data);

         } // namespace viewer
      } // namespace gui
   } // namespace UI
} // namespace btg

#endif


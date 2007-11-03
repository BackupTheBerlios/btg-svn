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
            /// Struct containing information about one table row.
            struct tableData
            {
               /// The filename.
               std::string filename;
               /// Current status.
               std::string status;
               /// Progress.
               std::string progress;

               /// Download rate in bytes per second.
               t_uint      dlRate;

               /// String describing download/upload rate in human
               /// readable format.
               std::string dlul;

               /// Number of bytes downloaded.
               t_uint      dlCount;
               /// Number of bytes uploaded.
               t_uint      ulCount;

               /// Size of the torrent.
               std::string size;

               /// Number of peers.
               std::string peers;

               /// Operator, used to sort by download rate.
               bool operator<(tableData const& _td) const
               {
                  return (dlRate > _td.dlRate);
               }

            };

            /// Struct containing the information used by the GUI.
            struct btgvsGui
            {
               /// Constructor.
               btgvsGui()
               : window(0),
                  contents_box(0),
                  notebook(0),
                  statusbar(0),
                  statusbarlabel(0),
                  table(0),
                  plot(0),
                  plotDlItem(0),
                  plotZeroItem(0),
                  plotUlItem(0),
                  ul_max(100),
                  ul_min(0),
                  dl_max(100),
                  dl_min(0),
                  updateDelay(1000)
               {
                  tabs[0] = 0;
                  tabs[1] = 0;
                  tabs[2] = 0;
               }

               /// Timer.
               AG_Timeout      timer;
               /// Main window.
               AG_Window*      window;
               /// Vertical box.
               AG_VBox*        contents_box;
               /// Notebook.
               AG_Notebook*    notebook;
               /// Statusbar.
               AG_Statusbar*   statusbar;
               /// Label used for showing text on the statusbar.
               AG_Label*       statusbarlabel;
               /// Notebook tabs.
               AG_NotebookTab* tabs[3];
               /// Torrent table.
               AG_Table*       table;
               /// UL/DL plot.
               AG_Graph*       plot;
               /// DL plot.
               AG_GraphItem*   plotDlItem;
               /// Zero plot.
               AG_GraphItem*   plotZeroItem;
               /// UL plot.
               AG_GraphItem*   plotUlItem;

               /// Observed max UL.
               t_uint          ul_max;
               /// Observed min UL.
               t_uint          ul_min;
               /// Observed max DL.
               t_uint          dl_max;
               /// Observed min DL.
               t_uint          dl_min;

               /// How often the UI shall be updated.
               t_uint          updateDelay;
            };

            class viewerHandler;

            /// Struct containing required data used for updating the
            /// table showing torrents and their properties.
            struct timerData
            {
               /// Constructor.
            timerData(btgvsGui &                           _gui,
                      btg::core::client::handlerThread*    _handlerthr,
                      viewerHandler* _handler)
            : gui(_gui), handlerthr(_handlerthr), handler(_handler)
               {
               }

               /// Reference to the gui.
               btgvsGui &                           gui;
               /// Thread used for updating torrents.
               btg::core::client::handlerThread*    handlerthr;
               /// Pointer to handler.
               viewerHandler*                       handler;
            };

            /// Create the GUI.
            void createGui(btgvsGui & _gui);

            /// Create the timer used for updating tables.
            void createTimer(btgvsGui & _gui, timerData* _timerdata);

            /// Run the GUI.
            void run();

            /// Close the GUI.
            void destroyGui(btgvsGui & _gui);

            /// Update the table showing torrents.
            void updateTable(btgvsGui & _gui, std::vector<tableData> const& _data);

            /// Update the dl/ul graph.
            void updateGraph(btgvsGui & _gui, std::vector<tableData> const& _data);
            
            /// Update the whole GUI.
            void update_ui(timerData* _timerdata);

         } // namespace viewer
      } // namespace gui
   } // namespace UI
} // namespace btg

#endif


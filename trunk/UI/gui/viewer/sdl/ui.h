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

#include <config.h>
#include <string>
#include <vector>

#include <bcore/client/handlerthr.h>

#include "agar-if.h"

#if HAVE_LIRC
#  include <lirc/lirc_client.h>
#endif

namespace btg
{
   namespace UI
   {
      namespace gui
      {
         namespace viewer
         {
            /// Struct containing information about one table row.
            struct torrentData
            {
               std::string id;
               /// The filename.
               std::string filename;
               /// Current status.
               std::string status;
               /// Percent done.
               t_int done;
               
               /// Progress.
               std::string progress;
               /// UL/DL ratio.
               std::string ratio;
               /// Upload rate in bytes per second.
               t_uint      ulRate;

               /// Download rate in bytes per second.
               t_uint      dlRate;

               /// String describing download/upload rate in human
               /// readable format.
               std::string dl;

               std::string ul;
               
               /// Number of bytes downloaded.
               t_uint      dlCount;
               /// Number of bytes uploaded.
               t_uint      ulCount;

               /// Number of peers.
               t_uint      peerCount;
               /// Number of seeds.
               t_uint      seedCount;

               /// Number of files.
               std::string nof;
               
               /// Size of the torrent.
               std::string size;

               /// Number of peers.
               std::string peers;

               /// Operator, used to sort by download rate.
               bool operator<(torrentData const& _td) const
               {
                  return (dlRate > _td.dlRate);
               }

            };

            /// Struct containing the information used by the GUI.
            class btgvsGui
            {
            public:
               enum
               {
                  STRING_MAX = 512
               };
               
               /// Constructor.
               btgvsGui(bool _auto, t_uint _autoUpdateFreq);

               /// Destructor.
               ~btgvsGui();

               /// UI style.
               AG_Style        style;
               /// Timer.
               AG_Timeout*     timer;
               /// Main window.
               AG_Window*      window;

               /// Used to divide the display in two.
               AG_Box* top_box;
               
               AG_Label* torrentNumberLabel;
               char titleLabelText[STRING_MAX];
               
               AG_Box* panes[4];
               
               AG_Pane* hp;
               AG_Pane* vp;
               AG_Pane* dp;

               /// Vertical box.
               AG_Box*        basics_box;
               AG_Box*        speed_box;
               
               int pb_value;
               int pb_min;
               int pb_max; 
               
               AG_Box*        done_box;
               AG_Box*        files_box;
               
               AG_Label* idLabel;
               char idLabelText[STRING_MAX];
               
               AG_Label* fileLabel;
               char fileLabelText[STRING_MAX];
               
               AG_Label* statusLabel;
               char statusLabelText[STRING_MAX];
               
               AG_Label* etaLabel;
               char etaLabelText[STRING_MAX];
               
               AG_Label* dlSpeedLabel;
               char dlSpeedLabelText[STRING_MAX];
               
               AG_Label* ulSpeedLabel;
               char ulSpeedLabelText[STRING_MAX];
               
               AG_Label* nofLabel;
               char nofLabelText[STRING_MAX];
               
               AG_Label* fileSizeLabel;
               char fileSizeLabelText[STRING_MAX];
               
               AG_Label* seedsPeersLabel;
               char seedsPeersLabelText[STRING_MAX];
               
               /// Statusbar.
               AG_Statusbar*   statusbar;
               /// Label used for showing text on the statusbar.
               AG_Label*       statusbarlabel;
               /// Torrent table.
               // AG_Table*       table;
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

               /// Bandwidth label.
               AG_Label*       bwLabel;
               /// Global upload.
               std::string     uploadStr;
               /// Global download.
               std::string     downloadStr;

               /// Peer count label.
               AG_Label*       peLabel;
               /// Global number of peers.
               std::string     peerSeedsStr;

               /// Change between torrents automatically.
               const bool autoChange;
               
               const t_uint autoUpdateFreq;
               
               /// Which torrent is being displayed.
               t_int position;
               /// Number of torrents.
               t_int size;
               
               enum Direction
               {
                  DIR_UNDEF = 0,
                  DIR_UP,
                  DIR_DOWN,
                  DIR_LEFT,
                  DIR_RIGHT
               };
               
               Direction direction;
#if HAVE_LIRC
               /// Pointer to LIRC config.
               struct lirc_config* lircConfig;
               /// Socket used for talking to LIRC. 
               int lirc_socket;
#endif
            };

            class viewerHandler;

            /// Struct containing required data used for updating the
            /// table showing torrents and their properties.
            struct timerData
            {
               /// Current count, used to determine if the function 
               /// used to update the UI shall be executed.
               t_uint                               count;
               /// Reference to the gui.
               btgvsGui*                            gui;
               /// Thread used for updating torrents.
               btg::core::client::handlerThread*    handlerthr;
               /// Pointer to handler.
               viewerHandler*                       handler;
               
               std::vector<torrentData>               data;
            };

            void copyString(std::string const& _in, char* _dest, int _maxlen = 0);
            
            /// Create the GUI.
            void createGui(btgvsGui & _gui, timerData & _timerData);
#if HAVE_LIRC
            bool initLIRC(btgvsGui & _gui);
#endif
            /// Create the timer used for updating tables.
            void createTimer(btgvsGui & _gui, timerData* _timerdata);

            /// Set a custom style.
            void setUIStyle(AG_Style & _style);

            /// Set colors used by the UI.
            void setColors();

            /// Run the GUI.
            void run(btgvsGui & _gui);
#if HAVE_LIRC
            bool pollLirc(btgvsGui & _gui);

            void pushEvent(SDLKey _k);
#endif
            /// Close the GUI.
            void destroyGui(btgvsGui & _gui);

            /// Update the table showing torrents.
            void updateTorrentList(btgvsGui & _gui, std::vector<torrentData> const& _data);

            void updateTorrentDetails(btgvsGui & _gui, torrentData const& _data);
            
            /// Update global dl/ul/peer/seed counters.
            void updateGlobalStats(btgvsGui & _gui, std::vector<torrentData> const& _data);

            /// Update the whole GUI - this gets data from the network.
            void update_ui(timerData* _timerdata);

            /// Update the display - using the data already present.
            void update_ui_display(timerData* _timerdata);
            
            /// Update the statusbar.
            void update_statusbar(timerData* _timerdata);

            /// Callback
            void KeyDown(AG_Event *event);

            /// Move display.
            void move(timerData* _td);
            
         } // namespace viewer
      } // namespace gui
   } // namespace UI
} // namespace btg

#endif


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

#include <string>
#include <vector>

#include <bcore/project.h>
#include <bcore/type_btg.h>
#include <bcore/status.h>
#include <bcore/file_info.h>
#include <bcore/logable.h>
#include <bcore/client/handlerthr.h>
#include <bcore/client/cpif.h>

#include "mainwindow.h"
#include "topwindow.h"
#include "statuswindow.h"
#include "progress.h"
#include "keys.h"

namespace btg
{
   namespace UI
      {
         namespace cli
            {
               /**
                * \addtogroup ncli
                */
               /** @{ */

               /// Main UI class.
               class UI: private btg::core::Logable, private btg::core::client::createProgressIf
                  {
                  public:
                     /// Constructor.
                     UI(btg::core::LogWrapperType _logwrapper,
                        std::string const& _session,
                        bool _neverAskQuestions,
                        bool _urlDlEnabled,
                        keyMapping const& _keymap,
                        Colors & _colors,
                        btg::core::client::handlerThread& _handlerthread);

                     /// Initialize the UI.
                     bool init();

                     /// Destroy the contained windows.
                     void destroy();

                     /// Read from keyboard until a sequence used to
                     /// terminate the application is pressed.
                     /// If that happends, return false;
                     /// Else return true.
                     bool handleKeyboard();

                     /// Show help for the details.
                     dialog::RESULT handleShowDetailsHelp();

                     /// Show a window showing the torrent.
                     void handleShowDetails();

                     /// Mark a context.
                     void handleMark();

                     /// Mark every context.
                     void handleMarkAll();

                     /// Show a menu.
                     void handleMenu();

                     /// Handle setting the name of the current session.
                     void handleSessionName();

                     /// Show help.
                     dialog::RESULT handleHelp();

                     /// Show a dialog used to choose torrent files.
                     void handleLoad();

                     /// Load a number of files, by using a list of filenames.
                     /// 
                     /// Used for handling loading torrents using
                     /// command line arguments.
                     void handleLoad(t_strList const& _filelist);

                     /// Instruct the daemon to download an URL.
                     void handleLoadUrl();

                     /// Instruct the daemon to download the provided URLs.
                     void handleUrl(t_strList const& _filelist);

                     /// URL downloading progress.
                     bool handleUrlProgress(t_uint _hid);



                     /// Show a dialog that allows one to set global
                     /// limits.
                     void handleGlobalLimit();

                     /// Handle sorting the list of contexts.
                     void handleSort();

                     /// Refresh all windows.
                     void refresh();

                     /// Resize all contained windows.
                     void resize();

                     /// Get the trackers used by a context.
                     void getTrackers(t_int _contextID, 
                                      t_strList & _trackers);

                     /// Destructor.
                     virtual ~UI();
                  private:
                     void CPIF_begin(std::string const& _filename);
                     void CPIF_begin(std::string const& _filename, 
                                     std::string const& _url);
                     void CPIF_filePiece(t_uint _number, t_uint _parts);
                     void CPIF_urlDlStatus(t_uint _total, t_uint _now, t_uint _speed);
                     void CPIF_error(std::string const& _error);
                     void CPIF_wait(std::string const& _msg);
                     void CPIF_success(std::string const& _filename);

                     /// Pointer to a progress window used.
                     progressWindow* progress_;
                  private:
                     /// Menu entries.
                     enum menuEntries
                        {
                           M_UNDEF = 0,   //!< Undefined entry.
                           M_START = 1,   //!< Start a torrent.
                           M_STOP,        //!< Stop a torrent.
                           M_ABORT,       //!< Abort downloading a torrent.
                           M_ERASE,       //!< Erase a torrent and its data.
                           M_LIMIT,       //!< Show limit dialog.
                           M_CLEAN,       //!< Clean a torrent.
                           M_SHOWFILES,   //!< Show files belonging to a torrent.
                           M_SELECTFILES, //!< Select files to download in a torrent.
                           M_SHOWPEERS,   //!< Show peers.
                           M_MOVE
                        };

                     /// Used to return information about the state
                     /// after showing a window used to select files inside a torrent.
                     enum selectState
                        {
                           sS_SELECT_SUCCESS = 0,
                           sS_SELECT_FAILURE,
                           sS_SELECT_RESIZE,
                           sS_SELECT_ABORT
                        };

                     /// Method used for sorting contexts.
                     enum sortBy
                     {
                        sB_Id = 0,   /// Sort by id.
                        sB_Name,     /// Sort by name.
                        sB_Size,     /// Sort by file size.
                        sB_UlSpeed,  /// Sort by upload speed.
                        sB_DlSpeed,  /// Sort by download speed.
                        sB_Peers,    /// Sort by number of peers.
                        sB_Done      /// Sort by percent done.
                     };

                     /// String representing the current session ID.
                     std::string    session_;

                     /// Flag: if true, no questions are asked. Ever.
                     bool           neverAskQuestions_;

                     /// Indicates if the daemon supports URL downloading.
                     bool           urlDlEnabled_;

                     /// The keys used by the UI.
                     keyMapping     keymap_;

                     /// The colors used by the UI.
                     Colors &       colors_;

                     /// Main window showing list of torrents.
                     mainWindow     mainwindow_;

                     /// Top window. Shows title and download/upload.
                     topWindow      topwindow_;

                     /// Windows showing status at the bottom of the
                     /// screen.
                     statusWindow   statuswindow_;

                     /// Position, top - x.
                     t_uint         mainwindow_topx;

                     /// Position, top - y.
                     t_uint         mainwindow_topy;

                     /// Width.
                     t_uint         mainwindow_width;

                     /// Height.
                     t_uint         mainwindow_height;

                     /// Last used directory, used by the load dialog.
                     std::string    load_directory_;

                     /// Thread handling updates.
                     btg::core::client::handlerThread& handlerthread_;

                     /// Method used for sorting contexts.
                     sortBy         sortby_;

                     /// Set statusbar up to communicate that some action went ok.
                     void actionSuccess(std::string const& _action, 
                                        std::string const& _filename);

                     /// Set statusbar up to communicate an error caused by some action.
                     void actionFailture(std::string const& _action, 
                                         std::string const& _filename);

                     /// Set statusbar up to communicate that some action was aborted.
                     void actionAborted(std::string const& _action, 
                                        std::string const& _filename);

                     /// Set statusbar up to communicate that some
                     /// action was aborted because the window was
                     /// resized.
                     void actionAbortedByResize(std::string const& _action, 
                                                std::string const& _filename);
                     /// Generate help text.
                     void genHelpText(std::string & _output);

                     /// Handle a menu entry.
                     void handleMenuEntry(menuEntries const _menuentry,
                                          t_int const _context_id,
                                          std::string const& _filename);

                     /// Show a dialog used to move a context to
                     /// another session.
                     void handleMoveToSession(t_int const _context_id,
                                              std::string const& _filename);

                     /// Handle setting limits when multiple torrents
                     /// are selected.
                     void handleManyLimits(std::vector<t_int> const& _context_ids);

                     /// Set limit on a certain torrent.
                     bool setLimit(t_int const _context_id,
                                   std::string const& _filename,
                                   t_int const _download,
                                   t_int const _upload,
                                   t_int const _seedTimeout,
                                   t_int const _seedPercent);

                     /// Show a dialog, where the user can select
                     /// limits for a certain torrent.
                     bool limitPopup(t_int const _context_id,
                                     t_int & _download,
                                     t_int & _upload,
                                     t_int & _seedTimeout,
                                     t_int & _seedPercent);

                     /// Update the contents.
                     void update(std::vector<btg::core::Status> const& _list);

                     /// Clear the contents.
                     void clear();

                     /// Update the displayed bandwidth.
                     void updateBandwidth(std::vector<btg::core::Status> const& _list);

                     /// Remove a number of IDs from the contents.
                     void remove(std::vector<t_int> const& _id_list);

                     /// Quit a session.
                     bool handleQuit();

                     /// Resize the contained windows.
                     void handleResizeMainWindow();
                     
                     /// Set the status bar to its default text.
                     void setDefaultStatusText();

                     /// Show the files belonging to a torrent.
                     void handleShowFiles(std::vector<btg::core::fileInformation> const& _fi);

                     /// Show the files belonging to a torrent.
                     /// @return True - files set, false otherwise.
                     UI::selectState handleSelectFiles(t_int const _context_id,
                                                       btg::core::selectedFileEntryList const& _files);

                     /// Show the peers belongning to a torrent.
                     void handleShowPeers(t_peerList & _peerlist);
                  };

               /** @} */

            } // namespace cli
      } // namespace UI
} // namespace btg

#endif // UI_H


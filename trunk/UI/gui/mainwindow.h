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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtkmm.h>

#include <bcore/type.h>
#include <bcore/status.h>

#include "sig_button_menu.h"

#include <bcore/type_btg.h>
#include <bcore/logable.h>

#include <map>

#include <bcore/client/handlerthr.h>

#include <bcore/client/clientdynconfig.h>
#include <bcore/client/filephelper.h>

#include "mainnotebook.h" // for enum CurrentSelection


namespace btg
{
   namespace UI
   {
      namespace gui
      {
         /**
          * \addtogroup gui
          */
         /** @{ */

         class mainTreeview;
         class mainFileTreeview;
         class aboutDialog;
         class limitDialog;
         class preferencesDialog;
         class sessionSelectionDialog;
         class progressDialog;

         /// The main window of the gui client.
         class mainWindow : public Gtk::Window, public btg::core::Logable, public btg::core::client::createPartsReportInterface
            {
            public:
               /// Constructor.
               mainWindow(btg::core::LogWrapperType logwrapper,
                          std::string const& _session,
                          bool const _verboseFlag,
                          bool const _neverAskFlag,
                          btg::core::client::handlerThread& _handlerthread,
                          btg::core::client::clientDynConfig& _CDC);

               /// Used when a menu item is selected.
               void on_menu_item_selected(buttonMenuIds::MENUID _which_item);

               /// Indicates if URL downloading is enabled.
               bool isUrlDlEnabled() const;

               /// Destructor.
               virtual ~mainWindow();
            private:
               /// Used for refreshing the list of contexts.
               bool on_refresh_timeout(int);

               /// Called when the user closes this window.
               bool onWindowClose(GdkEventAny *);

               /// Update the plot.
               void updatePlot();

               /// Update the files shown in the UI.
               void updateFiles(t_int const _id);

               /// Update the peers shown in the UI.
               void updatePeers(t_int const _id);

               /// Clear selected files.
               void clearSelection();

               /// Update information about the selected file.
               void updateSelectedFiles(t_int const _id);

               /// Update the status information shown about a
               /// selected file.
               void updateDetails(t_int const _id,
                                  t_statusList const& _statuslist);

               /// Tell the daemon that _filename should be
               /// created.
               void openFile(std::string const& _filename);

               /// Load URL
               void openURL(std::string const& _url, std::string const& _filename);
               
               /// Log a message to internal application log and
               /// the normal log.
               void logMessage(std::string const& _msg);

               /// Write a message if verbose mode of operation
               /// is enabled.
               void logVerboseMessage(std::string const& _msg);

               /// Return true if a tracker status should be
               /// shown to the user. Return false if the status
               /// was shown before.
               bool showTrackerStatus(t_int _contextId, btg::core::trackerStatus const& _ts);

               /// Called when its no longer needed to receive
               /// tracker status for a certain context id.
               void removeTrackerStatus(t_int const _contextId);

               /// When the client receives status of a torrent,
               /// the can contain a tracker status which should
               /// be shown to the user if its serial number is unique.
               void updateTrackerStatus(btg::core::Status const& _status);

               /// Get the list of selected files from UI, if
               /// any changed, tell the daemon.
               void checkSelectedFiles();
               
               /// Enable and disable toolbar buttons and menu items that controls torrents
               void setControlFunction(const bool bSensitive = true);

               /// Handle opening a last file.
               void handle_btn_lastfile(int _which_item);

               /// Handle opening all last files.
               void handle_btn_lastfile_all();

               /// Handle opening a last URL.
               void handle_btn_lasturl(int _which_item);

               /// Handle opening all last URLs.
               void handle_btn_lasturl_all();

               /// Create and show a dialog to open a torrent
               /// file.
               /// If the user selects a torrent file, tell the
               /// daemon that it should be created.
               void handle_btn_load();

               /// Download an URL.
               void handle_btn_load_url();

               /// Handle detach.
               void handle_btn_detach();

               /// Handle about.
               void handle_btn_about();

               /// Handle quit.
               void handle_btn_quit();

               /// Handle daemon kill.
               void handle_btn_kill();

               /// Handle uptime.
               void handle_btn_uptime();

               /// Handle setting session name.
               void handle_btn_sesname();

               /// Handle limit.
               void handle_btn_glimit();

               /// Handle start.
               void handle_btn_start(t_int const _id);

               /// Handle stop.
               void handle_btn_stop(t_int const _id);

               /// Handle erase.
               void handle_btn_erase(t_int const _id);

               /// Handle abort.
               void handle_btn_abort(t_int const _id);

               /// Handle clean.
               void handle_btn_clean(t_int const _id);

               /// Handle limit.
               void handle_btn_limit(t_int const _id);

               /// Handle moving context to session.
               void handle_btn_move(t_int const _id);

               /// Handle preferences.
               void handle_btn_prefs(t_int const _id);

               void CPRI_init(std::string const& _filename);
               void CPRI_pieceUploaded(t_uint _number, t_uint _parts);
               void CPRI_error(std::string const& _error);
               void CPRI_wait(std::string const& _msg);
               void CPRI_success(std::string const& _filename);
            private:
               /// Indicates that the client does verbose logging.
               bool                      verboseFlag;
               /// Indicates that the client should never ask
               /// user questions.
               bool                      neverAskFlag;

               /// Used for a one second timeout.
               sigc::connection          timeout_connection;
               /// Pointer to the table used to show contexts.
               class mainTreeview*       mtw;
               /// Pointer to the status bar.
               class mainStatusbar*      msb;
               /// Pointer to the menu bar.
               class mainMenubar*        mmb;
               /// Pointer to the notebook used to show file
               /// table, traffic graph and log output.
               mainNotebook*             mnb;
               /// Pointer to the class implementing the client
               /// callback interface and using a statemachine
               /// to talk to the daemon on the other side of a
               /// transport interface.
               btg::core::client::handlerThread& handlerthread;
                     
               /// Pointer to the about dialog.
               aboutDialog*              aboutdialog;
               /// Pointer to the preferences dialog.
               preferencesDialog*        preferencesdialog;
               /// Used to store the last directory used for
               /// opening files.
               std::string               lastOpenDirectory;
               /// Flag which indicates that updating of contexts is allowed.
               bool                      updateContexts;
               /// Counter, used to decide when progress should be updated.
               t_uint                    progressCounter;
               /// Max, decides when progress should be updated.
               t_uint const              progressMax;
               /// Map context id to serial.
               std::map<t_int, t_int>    trackerstatSerial;
               /// Client dynamic configuration store
               btg::core::client::clientDynConfig & m_clientDynConfig;
               /// Toolbar buttons related to torrent control
               std::list<Gtk::ToolButton*> torrentControlButtons;
               
               /// Flag used by operations witn multiple selected torrents
               bool                        m_bMultipleContinue;
               /// limitDialog used to set limits
               limitDialog*                m_limitDialog;
               /// sessionSelectionDialog used by handle_btn_move
               sessionSelectionDialog*     m_sessionSelectionDialog;

               /// Last downloaded URL.
               std::string                 last_url;
               /// Last downloaded URL - the filename used.
               std::string                 last_url_file;

               /// Indicates if URL downloading is enabled.
               bool                        urlDlEnabled;
               
               /// Dialog used to show progress when creating new
               /// contexts.
               progressDialog*             upload_progressdialog;
               
               /// Counter, used to decide when peers should be updated.
               t_uint                      peersCounter;
               /// Max, decides when peers should be updated.
               t_uint const                peersMax;
               
               /// Selection ID which was at the last update, used in peers update
               t_int                       last_selection_id;
               
               /// The last mnb tab selected
               mainNotebook::CurrentSelection last_mnb_selection;
            private:
               /// Copy constructor.
               mainWindow(mainWindow const& _mw);
               /// Assignment operator.
               mainWindow& operator=(mainWindow const& _mw);
                  
               // Other signal handlers (override)
            protected:
               /// Callback: window changes state. For example mini->max.
               virtual bool on_window_state_event (GdkEventWindowState* event);
               /// Callback: resize.
               virtual bool on_configure_event (GdkEventConfigure* event);
               /// Callback: peers vscroll value changed
               void on_peers_scrolled();
            };

         /** @} */

      } // namespace gui
   } // namespace UI
} // namespace btg

#endif

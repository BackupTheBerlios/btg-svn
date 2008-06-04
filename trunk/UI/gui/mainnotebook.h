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

#ifndef MAINNOTEBOOK_H
#define MAINNOTEBOOK_H

#include <gtkmm.h>

#include <bcore/type_btg.h>
#include <bcore/logger/logif.h>
#include <bcore/file_info.h>

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

               class logTextview;
               class traficPlot;
               class mainFileTreeview;
               class selectFileTreeview;
               class peerTreeview;
               class detailsTreeview;

               /// Implements a notebook, which shows:
               /// files, traffic graph and log.
               class mainNotebook : public Gtk::Notebook
                  {
                  public:
                     /// The currently selected item.
                     enum CurrentSelection
                        {
                           ALL_FILES = 0,
                           SELECTED_FILES,
                           DETAILS,
                           GRAPH,
                           PEERS,
                           LOG
                        };
                  public:
                     /// Constructor.
                     mainNotebook();

                     /// Get the contained details table.
                     detailsTreeview* getDetails() const;

                     /// Get the contained text view, which is used
                     /// for showing a log.
                     logTextview* getTextView() const;

                     /// Return the contained file tree view, used to
                     /// list files inside a torrent.
                     mainFileTreeview* getFileTreeview() const;

                     /// Return the contained combo box used to
                     /// display peers.
                     peerTreeview* getPeerTreeview() const;
                     
                     /// Return the contained selected file tree view.
                     selectFileTreeview* getSelectFileTreeview() const;

                     /// Get a pointer the object used for plotting
                     /// bandwidth usage.
                     traficPlot* getPlot() const;

                     /// Get the currently selected item.
                     CurrentSelection selected() const;

                     /// Destructor.
                     virtual ~mainNotebook();
                  private:
                     /// The currently selected item.
                     CurrentSelection    current_selection;

                     /// Log.
                     logTextview*        logtextview;

                     /// List of files.
                     mainFileTreeview*   mftv;

                     /// List of selected files.
                     selectFileTreeview* sftv;

                     /// Pointer to the combo used to display peers.
                     peerTreeview*       peerList;
                     /// Scrolled window that contains peerTreeView
                     Gtk::ScrolledWindow* peerScrolledwindow;

                     /// Pointer to the object used for plotting
                     /// bandwidth usage.
                     traficPlot*         plot;

                     /// Pointer to table used for showing details.
                     detailsTreeview*    details;

                     /// Callback: when a page is switched.
                     /// 
                     /// Used to keep track of which page is currently
                     /// show, so only it can be updated.
                     void onSwitchPage(GtkNotebookPage* _page, 
                                       guint _page_num);
                  private:
                     /// Copy constructor.
                     mainNotebook(mainNotebook const& _mn);
                     /// Assignment operator.
                     mainNotebook& operator=(mainNotebook const& _mn);
                  
                  public:
                     /// Get peerTreeView vertical scrollbar (to attach value-changed signal handler)
                     Gtk::VScrollbar * peersVScrollbar();
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif

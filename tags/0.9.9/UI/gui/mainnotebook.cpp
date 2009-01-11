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

#include "mainnotebook.h"
#include "mainfiletreeview.h"
#include "selectfiletreeview.h"
#include "traficplot.h"
#include "logtextview.h"
#include "peertreeview.h"
#include "detailstreeview.h"

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         mainNotebook::mainNotebook()
            : current_selection(mainNotebook::GRAPH),
              logtextview(0),
              mftv(0),
              sftv(0),
              peerList(0),
              peerScrolledwindow(0),
              details(0)
         {
            Gtk::Label* bw_label = Gtk::manage(new class Gtk::Label("Bandwidth (all)"));
            bw_label->set_alignment(0.5,0.5);
            bw_label->set_padding(0,0);
            bw_label->set_justify(Gtk::JUSTIFY_CENTER);
            bw_label->set_line_wrap(false);
            bw_label->set_use_markup(false);
            bw_label->set_selectable(false);
            plot = new traficPlot;

            Gtk::Label* details_label = Gtk::manage(new class Gtk::Label("Details"));
            details_label->set_alignment(0.5,0.5);
            details_label->set_padding(0,0);
            details_label->set_justify(Gtk::JUSTIFY_LEFT);
            details_label->set_line_wrap(false);
            details_label->set_use_markup(false);
            details_label->set_selectable(false);
            details = Gtk::manage(new class detailsTreeview);
            Gtk::ScrolledWindow* detailsScrolledwindow = Gtk::manage(new class Gtk::ScrolledWindow());
            detailsScrolledwindow->set_flags(Gtk::CAN_FOCUS);
            detailsScrolledwindow->set_shadow_type(Gtk::SHADOW_NONE);
            detailsScrolledwindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
            detailsScrolledwindow->property_window_placement().set_value(Gtk::CORNER_TOP_LEFT);
            detailsScrolledwindow->add(*details);

            Gtk::Label* fileTreeLabel = Gtk::manage(new class Gtk::Label("Files"));
            fileTreeLabel->set_alignment(0.5,0.5);
            fileTreeLabel->set_padding(0,0);
            fileTreeLabel->set_justify(Gtk::JUSTIFY_LEFT);
            fileTreeLabel->set_line_wrap(false);
            fileTreeLabel->set_use_markup(false);
            fileTreeLabel->set_selectable(false);
            mftv = Gtk::manage(new class mainFileTreeview());
            Gtk::ScrolledWindow* fileScrolledwindow = Gtk::manage(new class Gtk::ScrolledWindow());
            fileScrolledwindow->set_flags(Gtk::CAN_FOCUS);
            fileScrolledwindow->set_shadow_type(Gtk::SHADOW_NONE);
            fileScrolledwindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
            fileScrolledwindow->property_window_placement().set_value(Gtk::CORNER_TOP_LEFT);
            fileScrolledwindow->add(*mftv);
            
            Gtk::Label* selectedFileTreeLabel = Gtk::manage(new class Gtk::Label("Selected files"));
            selectedFileTreeLabel->set_alignment(0.5,0.5);
            selectedFileTreeLabel->set_padding(0,0);
            selectedFileTreeLabel->set_justify(Gtk::JUSTIFY_LEFT);
            selectedFileTreeLabel->set_line_wrap(false);
            selectedFileTreeLabel->set_use_markup(false);
            selectedFileTreeLabel->set_selectable(false);
            sftv = Gtk::manage(new class selectFileTreeview());
            Gtk::ScrolledWindow* selectedFileScrolledwindow = Gtk::manage(new class Gtk::ScrolledWindow());
            selectedFileScrolledwindow->set_flags(Gtk::CAN_FOCUS);
            selectedFileScrolledwindow->set_shadow_type(Gtk::SHADOW_NONE);
            selectedFileScrolledwindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
            selectedFileScrolledwindow->property_window_placement().set_value(Gtk::CORNER_TOP_LEFT);
            selectedFileScrolledwindow->add(*sftv);
            
            Gtk::Label* peersLabel = Gtk::manage(new class Gtk::Label("Peers"));
            peersLabel->set_alignment(0.5,0.5);
            peersLabel->set_padding(0,0);
            peersLabel->set_justify(Gtk::JUSTIFY_LEFT);
            peersLabel->set_line_wrap(false);
            peersLabel->set_use_markup(false);
            peersLabel->set_selectable(false);
            peerList = Gtk::manage(new peerTreeview());
            peerScrolledwindow = Gtk::manage(new class Gtk::ScrolledWindow());
            peerScrolledwindow->set_flags(Gtk::CAN_FOCUS);
            peerScrolledwindow->set_shadow_type(Gtk::SHADOW_NONE);
            peerScrolledwindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
            peerScrolledwindow->property_window_placement().set_value(Gtk::CORNER_TOP_LEFT);
            peerScrolledwindow->add(*peerList);

            /*
            Gtk::Label* traficLabel = Gtk::manage(new class Gtk::Label("Traffic"));
            traficLabel->set_alignment(0.5,0.5);
            traficLabel->set_padding(0,0);
            traficLabel->set_justify(Gtk::JUSTIFY_LEFT);
            traficLabel->set_line_wrap(false);
            traficLabel->set_use_markup(false);
            traficLabel->set_selectable(false);
            */
            
            Gtk::Label* logLabel = Gtk::manage(new class Gtk::Label("Log"));
            logLabel->set_alignment(0.5,0.5);
            logLabel->set_padding(0,0);
            logLabel->set_justify(Gtk::JUSTIFY_LEFT);
            logLabel->set_line_wrap(false);
            logLabel->set_use_markup(false);
            logLabel->set_selectable(false);
            logtextview = Gtk::manage(new class logTextview());
            Gtk::ScrolledWindow* logScrolledwindow = Gtk::manage(new class Gtk::ScrolledWindow());
            logScrolledwindow->set_flags(Gtk::CAN_FOCUS);
            logScrolledwindow->set_shadow_type(Gtk::SHADOW_NONE);
            logScrolledwindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
            logScrolledwindow->property_window_placement().set_value(Gtk::CORNER_TOP_LEFT);
            logScrolledwindow->add(*logtextview);

            this->set_flags(Gtk::CAN_FOCUS);
            this->set_show_tabs(true);
            this->set_show_border(true);
            //this->set_tab_pos(Gtk::POS_LEFT);
            //this->set_scrollable(false);

            this->append_page(*(plot->getWidget()), *bw_label);
            this->pages().back().set_tab_label_packing(false, true, Gtk::PACK_START);

            this->append_page(*detailsScrolledwindow, *details_label);
            this->pages().back().set_tab_label_packing(false, true, Gtk::PACK_START);
            
            this->append_page(*fileScrolledwindow, *fileTreeLabel);
            this->pages().back().set_tab_label_packing(false, true, Gtk::PACK_START);

            this->append_page(*selectedFileScrolledwindow, *selectedFileTreeLabel);
            this->pages().back().set_tab_label_packing(false, true, Gtk::PACK_START);

            this->append_page(*peerScrolledwindow, *peersLabel);
            this->pages().back().set_tab_label_packing(false, true, Gtk::PACK_START);

            this->append_page(*logScrolledwindow, *logLabel);
            this->pages().back().set_tab_label_packing(false, true, Gtk::PACK_START);

            signal_switch_page().connect(sigc::mem_fun(*this, &mainNotebook::onSwitchPage));
         }

         logTextview* mainNotebook::getTextView() const
         {
            return logtextview;
         }

         mainFileTreeview *mainNotebook::getFileTreeview() const
         {
            return mftv;
         }

         peerTreeview* mainNotebook::getPeerTreeview() const
         {
            return peerList;
         }

         traficPlot* mainNotebook::getPlot() const
         {
            return plot;
         }

         detailsTreeview* mainNotebook::getDetails() const
         {
            return details;
         }

         selectFileTreeview* mainNotebook::getSelectFileTreeview() const
         {
            return sftv;
         }

         mainNotebook::CurrentSelection mainNotebook::selected() const
         {
            return current_selection;
         }

         void mainNotebook::onSwitchPage(GtkNotebookPage* _page, 
                                         guint _page_num)
         {
            switch(_page_num)
               {
               case 0:
                  current_selection = mainNotebook::GRAPH;
                  break;
               case 1:
                  current_selection = mainNotebook::DETAILS;
                  break;
               case 2:
                  current_selection = mainNotebook::ALL_FILES;
                  break;
               case 3:
                  current_selection = mainNotebook::SELECTED_FILES;
                  break;
               case 4:
                  current_selection = mainNotebook::PEERS;
                  break;
               case 5:
                  current_selection = mainNotebook::LOG;
                  break;
               default:
                  current_selection = mainNotebook::GRAPH;
                  break;
               }
         }

         mainNotebook::~mainNotebook()
         {
            delete plot;
            plot = 0;
         }
         
         Gtk::VScrollbar * mainNotebook::peersVScrollbar()
         {
            return peerScrolledwindow->get_vscrollbar();
         }

      } // namespace gui
   } // namespace UI
} // namespace btg

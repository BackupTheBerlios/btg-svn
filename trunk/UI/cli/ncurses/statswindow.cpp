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

#include "statswindow.h"
#include "mainwindow.h"

#include <bcore/t_string.h>
#include <bcore/hru.h>
#include <bcore/hrr.h>
#include <bcore/hrt.h>

#include <bcore/client/ratio.h>

namespace btg
{
   namespace UI
   {
      namespace cli
      {
         statsWindow::statsWindow(keyMapping const& _kmap, mainWindow & _mainwindow)
            : baseWindow(_kmap),
              mainwindow_(_mainwindow),
              counter_(0)
         {
            // Keys used by this window.
            std::vector<keyMapping::KEYLABEL> labels;

            labels.push_back(keyMapping::K_SELECT);
            labels.push_back(keyMapping::K_BACK);
            labels.push_back(keyMapping::K_HELP);
            labels.push_back(keyMapping::K_RESIZE);

            kmap_.setUsed(labels);
         }

         void statsWindow::resize(windowSize const& _ws)
         {

         }

         bool statsWindow::destroy()
         {
            if (window_ != 0)
               {
                  delwin(window_);
                  window_ = 0;
               }

            return true;
         }

         void statsWindow::refresh()
         {
            if (window_ == 0)
               {
                  return;
               }

            clear();
            draw();
            wrefresh(window_);
         }

         void statsWindow::draw()
         {
            // Reset the counter.
            counter_ = 0;

            std::string numberOfTorrents;
            std::string seeds;
            std::string leeches;
            std::string uldl_ratio;
            std::string dl_size;
            std::string ul_size;

            mainwindow_.getTotals(numberOfTorrents,
                                  seeds, leeches,
                                  uldl_ratio,
                                  dl_size, ul_size);


            setColor(Colors::C_NORMAL);
            
            addTopic("Number of torrents:");
            addValue(numberOfTorrents);
            
            addTopic("Seeds:");
            addValue(seeds);

            addTopic("Leeches:");
            addValue(leeches);

            addTopic("UL/DL ratio:");
            addValue(uldl_ratio);

            addTopic("DL size:");
            addValue(dl_size);

            addTopic("UL size:");
            addValue(ul_size);
         }

         void statsWindow::setText(std::string const& _text)
         {
            //::wattron(window_, A_REVERSE);
            mvwprintw(window_, 0, 0, "%s", _text.c_str());
            //::wattroff(window_, A_REVERSE);

            refresh();
         }

         void statsWindow::addTopic(std::string const& _text)
         {
            ::wattron(window_, A_BOLD);
            mvwprintw(window_, counter_, 0, "%s", _text.c_str());
            ::wattroff(window_, A_BOLD);
            counter_++;
         }

         void statsWindow::addValue(std::string const& _text)
         {
            mvwprintw(window_, counter_, 0, "%s", _text.c_str());
            counter_++;
         }

         windowSize statsWindow::calculateDimenstions(windowSize const& _ws) const
         {
            // Use the same size as the parent.
            windowSize ws = _ws;

            return ws;
         }

         statsWindow::~statsWindow()
         {
            destroy();
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

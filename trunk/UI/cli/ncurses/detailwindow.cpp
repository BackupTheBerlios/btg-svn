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

#include "detailwindow.h"
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
         detailWindow::detailWindow(keyMapping const& _kmap, mainWindow const& _mainwindow)
            : baseWindow(_kmap),
              mainwindow_(_mainwindow),
              counter_(0)
         {
            // Keys used by this window.
            std::vector<keyMapping::KEYLABEL> labels;

            labels.push_back(keyMapping::K_QUIT);
            labels.push_back(keyMapping::K_HELP);
            labels.push_back(keyMapping::K_RESIZE);

            kmap_.setUsed(labels);
         }

         void detailWindow::resize(windowSize const& _ws)
         {

         }

         bool detailWindow::destroy()
         {
            if (window_ != 0)
               {
                  delwin(window_);
                  window_ = 0;
               }

            return true;
         }

         void detailWindow::refresh()
         {
            if (window_ == 0)
               {
                  return;
               }

            clear();
            draw();
            wrefresh(window_);
         }

         void detailWindow::draw()
         {
            // Reset the counter.
            counter_ = 0;

            // Draw.
            btg::core::Status currentStatus;

            if (mainwindow_.getSelection(currentStatus))
               {
                  // Print the status of a context.
                  setColor(Colors::C_NORMAL);

                  addTopic("ID:");
                  std::string st_id = btg::core::convertToString<t_int>(currentStatus.contextID());
                  addValue(st_id);


                  addTopic("Filename:");
                  std::string st_filename = currentStatus.filename();
                  addValue(st_filename);

                  addTopic("Status:");
                  std::string st_status;
                  switch (currentStatus.status())
                     {
                     case btg::core::Status::ts_undefined:
                        st_status = "Undefined";
                        break;
                     case btg::core::Status::ts_queued:
                        st_status = "Waiting";
                        break;
                     case btg::core::Status::ts_checking:
                        st_status = "Checking";
                        break;
                     case btg::core::Status::ts_connecting:
                        st_status = "Connecting";
                        break;
                     case btg::core::Status::ts_downloading:
                        st_status = "Downloading";
                        break;
                     case btg::core::Status::ts_seeding:
                        st_status = "Seeding (download done)";
                        break;
                     case btg::core::Status::ts_finished:
                        st_status = "Finished (download done)";
                        break;
                     case btg::core::Status::ts_stopped:
                        st_status = "Stopped";
                        break;
                     }

                  addValue(st_status);

                  addTopic("Up/down ratio:");
                  std::string st_ratio;
                  btg::core::client::CalculateUlDlRatio(currentStatus, st_ratio);
                  addValue(st_ratio);

                  if (currentStatus.status() == btg::core::Status::ts_downloading)
                     {
                        addTopic("Download time:");
                        btg::core::humanReadableTime hrt = btg::core::humanReadableTime::convert(
                                                                                                 60ull * currentStatus.activityCounter()
                                                                                                 );
                        std::string download_time = hrt.toString();

                        addValue(download_time);
                     }
                  else if (currentStatus.status() == btg::core::Status::ts_seeding)
                     {
                        addTopic("Seed time:");

                        btg::core::humanReadableTime hrt = btg::core::humanReadableTime::convert(
                                                                                                 60ull * currentStatus.activityCounter()
                                                                                                 );
                        std::string seed_time = hrt.toString();

                        addValue(seed_time);
                     }

                  // Progress:
                  if (currentStatus.validTimeLeft())
                     {
                        addTopic("Time left:");
                        std::string st_progress;

                        currentStatus.timeLeftToString(st_progress);
                        addValue(st_progress);
                     }

                  addTopic("Total download:");
                  btg::core::humanReadableUnit hru =
                     btg::core::humanReadableUnit::convert(currentStatus.downloadTotal());
                  std::string st_tdl = hru.toString();
                  addValue(st_tdl);

                  addTopic("Total upload:");
                  hru = btg::core::humanReadableUnit::convert(currentStatus.uploadTotal());
                  std::string st_tul = hru.toString();
                  addValue(st_tul);

                  addTopic("Download rate:");
                  btg::core::humanReadableRate hrr =
                     btg::core::humanReadableRate::convert(static_cast<t_uint>(currentStatus.downloadRate()));
                  std::string st_dlr = hrr.toString();
                  addValue(st_dlr);

                  addTopic("Upload rate:");
                  hrr = btg::core::humanReadableRate::convert(static_cast<t_uint>(currentStatus.uploadRate()));
                  std::string st_ulr = hrr.toString();
                  addValue(st_ulr);

                  addTopic("Done:");
                  std::string st_done = btg::core::convertToString<t_ulong>(currentStatus.done()) + " %";
                  addValue(st_done);

                  addTopic("Leeches:");
                  std::string st_l = btg::core::convertToString<t_int>(currentStatus.leechers());
                  addValue(st_l);

                  addTopic("Seeds:");
                  std::string st_s = btg::core::convertToString<t_int>(currentStatus.seeders());
                  addValue(st_s);

                  unSetColor(Colors::C_NORMAL);
               }
            else
               {
                  setText("Unable to obtain selection.\n");
               }
         }

         void detailWindow::setText(std::string const& _text)
         {
            //::wattron(window_, A_REVERSE);
            mvwprintw(window_, 0, 0, "%s", _text.c_str());
            //::wattroff(window_, A_REVERSE);

            refresh();
         }

         void detailWindow::addTopic(std::string const& _text)
         {
            ::wattron(window_, A_BOLD);
            mvwprintw(window_, counter_, 0, "%s", _text.c_str());
            ::wattroff(window_, A_BOLD);
            counter_++;
         }

         void detailWindow::addValue(std::string const& _text)
         {
            mvwprintw(window_, counter_, 0, "%s", _text.c_str());
            counter_++;
         }

         windowSize detailWindow::calculateDimenstions(windowSize const& _ws) const
         {
            // Use the same size as the parent.
            windowSize ws = _ws;

            return ws;
         }

         detailWindow::~detailWindow()
         {
            destroy();
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

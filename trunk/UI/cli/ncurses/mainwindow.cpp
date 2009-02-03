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

#include "mainwindow.h"
#include "ui.h"

#include <bcore/t_string.h>
#include <bcore/logmacro.h>
#include <algorithm>
#include <boost/bind.hpp>

namespace btg
{
   namespace UI
   {
      namespace cli
      {
         mainWindow::mainWindow(btg::core::LogWrapperType _logwrapper,
                                keyMapping const& _kmap, 
                                UI & _ui)
            : btg::core::Logable(_logwrapper),
              baseWindow(_kmap),
              numberOfLines_(0),
              dm_(1, this),
              lcounter(0),
              max_filename_size(0),
              max_progress_size(0),
              max_stat_size(6),
              max_perc_size(0),
              max_peers_size(0),
              extra_space(2),
              ui_(_ui)
         {
         }

         bool mainWindow::init(windowSize const& _ws)
         {
            bool status = baseWindow::init(_ws);

            if (status)
               {
                  numberOfLines_  = height_-1;
                  dm_.resize(numberOfLines_);
               }
            
            return status;
         }

         void mainWindow::resize(windowSize const& _ws)
         {
            destroy();
            init(_ws);
            refresh();
         }

         void mainWindow::refresh()
         {
            if (window_ == 0)
               {
                  return;
               }

            dm_.draw();
            wrefresh(window_);
         }

         bool mainWindow::statusToProgress(btg::core::Status const& _s, 
                                           std::string & _output) const
         {
            if (_s.status() == btg::core::Status::ts_downloading)
               {
                  if (_s.time_left_d() != 0)
                     {
                        // Only show days.
                        _output += btg::core::convertToString<t_ulong>(_s.time_left_d());
                        _output += " days";
                     }
                  else
                     {
                        if ((_s.time_left_h() != 0) ||
                            (_s.time_left_m() != 0) ||
                            (_s.time_left_s() != 0)
                            )
                           {
                              if (_s.time_left_h() < 10)
                                 {
                                    _output += "0";
                                 }
                              _output += btg::core::convertToString<t_ulong>(_s.time_left_h()) + ":";
                              
                              if (_s.time_left_m() < 10)
                                 {
                                    _output += "0";
                                 }
                              _output += btg::core::convertToString<t_int>(_s.time_left_m()) + ":";
                              
                              if (_s.time_left_s() < 10)
                                 {
                                    _output += "0";
                                 }
                              _output += btg::core::convertToString<t_int>(_s.time_left_s());
                           }
                     }
                  return true;
               }
         
            return false;
         }

         void mainWindow::update(std::vector<btg::core::Status> const& _list)
         {
            dm_.update(_list);
            refresh();
         }

         void mainWindow::remove(std::vector<t_int> const& _id_list)
         {
            dm_.remove(_id_list);
            refresh();
         }

         void mainWindow::clearContents()
         {
            clear();
         }

         void mainWindow::moveDown()
         {
            dm_.down();
            refresh();
         }

         void mainWindow::moveUp()
         {
            dm_.up();
            refresh();
         }

         void mainWindow::toStart()
         {
            dm_.begin();
         }

         void mainWindow::toEnd()
         {
            dm_.end();
         }

         bool mainWindow::get(t_int const _context_id, 
                              btg::core::Status & _status) const
         {
            return dm_.get(_context_id, _status);
         }

         bool mainWindow::getSelection(btg::core::Status & _status)
         {
            t_strList trackers;
            return getSelection(_status, trackers);
         }

         bool mainWindow::getSelection(btg::core::Status & _status, 
                                       t_strList & _trackers)
         {
            bool status = false;

            if (dm_.getCurrent(_status))
               {
                  if (!dm_.getCurrent(_trackers))
                     {
                        // No list of trackers.
                        t_strList trackers;
                        ui_.getTrackers(_status.contextID(), trackers);
                        dm_.setCurrent(trackers);
                        _trackers = trackers;
                     }

                  status = true;
               }

            return status;
         }

         void mainWindow::mark()
         {
            // BTG_NOTICE(logWrapper(), "mark.");
            dm_.mark();
         }

         void mainWindow::markAll()
         {
            // BTG_NOTICE(logWrapper(), "mark all.");
            dm_.markAll();
         }

         void mainWindow::getMarked(std::vector<t_int> & _id_list) const
         {
            dm_.getSelected(_id_list);
         }

         windowSize mainWindow::calculateDimenstions(windowSize const& _ws) const
         {
            windowSize ws;
            ws.topX   = 0;
            ws.topY   = 2;
            ws.width  = _ws.width;
            ws.height = _ws.height-3;

            return ws;
         }

         void mainWindow::setSortBy(DisplayModel::sortBy const _sortby)
         {
            dm_.setSortBy(_sortby);
            refresh();
         }

         void mainWindow::pstart()
         {
            lcounter = 0;
         }

         void mainWindow::findSizes(std::vector<statusEntry> const& _l)
         {
            // Find the max length of the contained file names;
            max_filename_size = 0;
            max_progress_size = 0;
            max_stat_size     = 5; // Constant.
            max_perc_size     = 0;
            max_peers_size    = 0;
            extra_space       = 2;

            std::vector<statusEntry>::const_iterator iter;
            for (iter = _l.begin();
                 iter != _l.end();
                 iter++)
               {
                  btg::core::Status const& s = iter->status;

                  t_uint size = s.filename().size();
                  if (size > max_filename_size)
                     {
                        max_filename_size = size;
                     }

                  std::string progress;
                  if (statusToProgress(s, progress))
                     {
                        if (progress.size() > max_progress_size)
                           {
                              max_progress_size = progress.size();
                           }
                     }

                  t_int done = s.done();

                  std::string st_done(" ");
                  if (done < 10)
                     {
                        st_done += " ";
                     }
                  if (done < 100)
                     {
                        st_done += " ";
                     }
                  st_done += btg::core::convertToString<t_int>(done);
                  st_done += "%";

                  if (st_done.size() > max_perc_size)
                     {
                        max_perc_size = st_done.size();
                     }

                  std::string st_peers = " ";
                  st_peers += btg::core::convertToString<t_int>(s.leechers());
                  st_peers += "/";
                  st_peers += btg::core::convertToString<t_int>(s.seeders());

                  if (st_peers.size() > max_peers_size)
                     {
                        max_peers_size = st_peers.size();
                     }
               }

            max_filename_size = width_ - 
               (max_progress_size + 
                max_stat_size + 
                max_perc_size + 
                max_peers_size + 
                extra_space);

            // BTG_NOTICE(logWrapper(), "findSizes: " << max_filename_size);
         }
         
         void mainWindow::paint(const t_uint _number, 
                                const bool _current,
                                const statusEntry & _se,
                                const t_uint _total)
         {
            setColor(Colors::C_NORMAL);
            
            // BTG_NOTICE(logWrapper(), "paint: " << _number);

            btg::core::Status const& s = _se.status;
            std::string filename = s.filename();
            
            if (filename.size() > max_filename_size)
               {
                  filename = filename.substr(0, max_filename_size);
               }
            else
               {
                  t_int diff = max_filename_size - filename.size();
                  if (diff > 0)
                     {
                        addSpace(diff, filename);
                     }
               }

            std::string st_status(" ");
            switch (s.status())
               {
               case btg::core::Status::ts_undefined:
                  {
                     st_status += "undf";
                     break;
                  }
               case btg::core::Status::ts_queued:
                  {
                     st_status += "wait";
                     break;
                  }
               case btg::core::Status::ts_checking:
                  {
                     st_status += "Chec";
                     break;
                  }
               case btg::core::Status::ts_connecting:
                  {
                     st_status += "conn";
                     break;
                  }
               case btg::core::Status::ts_downloading:
                  {
                     st_status += "down";
                     break;
                  }
               case btg::core::Status::ts_seeding:
                  {
                     st_status += "seed";
                     break;
                  }
               case btg::core::Status::ts_finished:
                  {
                     st_status += "fini";
                     break;
                  }
               case btg::core::Status::ts_stopped:
                  {
                     st_status += "stop";
                     break;
                  }
               }

            // Add percent:

            t_int done = s.done();

            std::string st_done(" ");
            if (done < 10)
               {
                  st_done += " ";
               }
            if (done < 100)
               {
                  st_done += " ";
               }

            st_done += btg::core::convertToString<t_int>(done);
            st_done += "%";

            st_done += " ";

            // Add progress:

            std::string st_progress("");

            statusToProgress(s, st_progress);

            t_int diff = max_progress_size - st_progress.size();
            if (diff > 0)
               {
                  addSpace(diff, st_progress);
               }

            std::string st_peers = " ";
            st_peers += btg::core::convertToString<t_int>(s.leechers());
            st_peers += "/";
            st_peers += btg::core::convertToString<t_int>(s.seeders());

            // Display marked torrents with different color.
            if (_se.marked)
               {
                  // BTG_NOTICE(logWrapper(), "Marked.");
                  setColor(Colors::C_MARK);
               }

            if (_current)
               {
                  // Highlight.

                  std::string spacestr;
                  t_int spaces = width_ - filename.size() - 
                     st_status.size() - st_done.size() - 
                     st_progress.size() - st_peers.size() - 1;
                  /* extra space is inserted at
                   * the end of the line.
                   */

                  if (spaces > 0)
                     {
                        for (t_int spacecounter = 0;
                             spacecounter < spaces;
                             spacecounter++)
                           {
                              spacestr += " ";
                           }
                     }

                  ::wattron(window_, A_REVERSE);
                  mvwprintw(window_, lcounter, 0, " %s%s%s%s%s%s", 
                            filename.c_str(), 
                            st_status.c_str(),
                            st_done.c_str(),
                            st_progress.c_str(),
                            st_peers.c_str(),
                            spacestr.c_str());
                  ::wattroff(window_, A_REVERSE);
               }
            else
               {
                  mvwprintw(window_, lcounter, 0, " %s%s%s%s%s", 
                            filename.c_str(),
                            st_status.c_str(),
                            st_done.c_str(),
                            st_progress.c_str(),
                            st_peers.c_str());

               }

            // Display marked torrents with different color.
            if (_se.marked)
               {
                  unSetColor(Colors::C_MARK);
               }

            lcounter++;
                  
            unSetColor(Colors::C_NORMAL);
         }

         void mainWindow::pend()
         {

         }

         void mainWindow::addSpace(t_uint const _number,
                                   std::string & _output) const
         {
            for (t_uint counter = 0;
                 counter < _number;
                 counter++)
               {
                  _output += " ";
               }
         }


         mainWindow::~mainWindow()
         {
            destroy();
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

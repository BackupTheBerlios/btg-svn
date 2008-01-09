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

#include <bcore/t_string.h>
#include <bcore/logmacro.h>

namespace btg
{
   namespace UI
   {
      namespace cli
      {
         statusEntry::statusEntry(btg::core::Status const& _s)
            : status(_s),
              marked(false),
              updated(true)
         {

         }

         statusEntry::statusEntry()
            : status(),
              marked(false),
              updated(false)
         {
         }

         /* */
         /* */
         /* */
	
         statusList::statusList()
            : changed_(false),
              statusList_()
         {

         }

         void statusList::get(std::vector<statusEntry> & _list) const
         {
            std::map<t_int, statusEntry>::const_iterator iter;
            for (iter = statusList_.begin();
                 iter != statusList_.end();
                 iter++)
               {
                  _list.push_back(iter->second);
               }
         }

         bool statusList::get(t_uint const _context_id,
                              btg::core::Status & _status) const
         {
            bool status = false;

            std::map<t_int, statusEntry>::const_iterator iter = statusList_.find(_context_id);

            if (iter != statusList_.end())
               {
                  _status = iter->second.status;
                  status  = true;
               }

            return status;
         }

         bool statusList::getAtPosition(t_uint const _position, btg::core::Status & _status) const
         {
            bool status = false;

            if (statusList_.size() == 0)
               {
                  return status;
               }

            std::map<t_int, statusEntry>::const_iterator iter;
            t_uint counter = 0;

            for (iter = statusList_.begin();
                 iter != statusList_.end();
                 iter++)
               {
                  if (counter == _position)
                     {
                        status = true;
                        break;
                     }
                  counter++;
               }

            if (status)
               {
                  _status = iter->second.status;
               }

            return status;
         }

         void statusList::mark(t_uint const _position)
         {
            if (statusList_.size() == 0)
               {
                  return;
               }

            std::map<t_int, statusEntry>::iterator iter;
            t_uint counter = 0;

            for (iter = statusList_.begin();
                 iter != statusList_.end();
                 iter++)
               {
                  if (counter == _position)
                     {
                        if (iter->second.marked)
                           {
                              iter->second.marked = false;
                           }
                        else
                           {
                              iter->second.marked = true;
                           }  
                        break;
                     }
                  counter++;
               }
         }
         void statusList::markAll()
         {
            std::map<t_int, statusEntry>::iterator iter;

            for (iter = statusList_.begin();
                 iter != statusList_.end();
                 iter++)
               {
                  if (iter->second.marked)
                     {
                        iter->second.marked = false;
                     }
                  else
                     {
                        iter->second.marked = true;
                     }  
               }
         }
	
         void statusList::getMarked(std::vector<t_int> & _id_list) const
         {
            std::map<t_int, statusEntry>::const_iterator iter;

            for (iter = statusList_.begin();
                 iter != statusList_.end();
                 iter++)
               {
                  if (iter->second.marked)
                     {
                        _id_list.push_back(iter->first);
                     }
               }
         }

         void statusList::clearMark()
         {
            std::map<t_int, statusEntry>::iterator iter;

            for (iter = statusList_.begin();
                 iter != statusList_.end();
                 iter++)
               {
                  iter->second.marked = false;
               }
         }

         void statusList::resetUpdated()
         {
            std::map<t_int, statusEntry>::iterator iter;

            for (iter = statusList_.begin();
                 iter != statusList_.end();
                 iter++)
               {
                  iter->second.updated = false;
               }
         }

         void statusList::removeDead()
         {
            std::map<t_int, statusEntry>::iterator iter = statusList_.begin();

            while (iter != statusList_.end())
               {
                  std::map<t_int, statusEntry>::iterator killIter = iter++;
	      
                  if (!killIter->second.updated)
                     {
                        statusList_.erase(killIter);
                     }
               }
         }
	
         void statusList::update(std::vector<btg::core::Status> const& _list)
         {
            std::vector<btg::core::Status>::const_iterator iter;

            std::vector<btg::core::Status> new_entries;

            // Find out if any IDs were removed.
            resetUpdated();

            for (iter = _list.begin();
                 iter != _list.end();
                 iter++)
               {
                  t_int id = iter->contextID();

                  std::map<t_int, statusEntry>::iterator dst_iter =
                     statusList_.find(id);

                  if (dst_iter != statusList_.end())
                     {
                        // Update context.
                        dst_iter->second.status  = *iter;
                        dst_iter->second.updated = true;
                     }
                  else
                     {
                        // New context.
                        // This also sets the updated flag.
                        new_entries.push_back(*iter);
                        // BTG_NOTICE("Added new context");
                     }
               }

            if (new_entries.size() > 0)
               {
                  // BTG_NOTICE("Context number changed - add");
                  changed_ = true;
               }

            std::vector<btg::core::Status>::iterator iiter;

            for (iiter = new_entries.begin();
                 iiter != new_entries.end();
                 iiter++)
               {
                  std::pair<t_int, statusEntry> p(iiter->contextID(), statusEntry(*iiter));
                  statusList_.insert(p);
               }

            // Remove dead entries.
            removeDead();
         }

         void statusList::remove(std::vector<t_int> const& _id_list)
         {
            std::vector<t_int>::const_iterator iter;
            for (iter = _id_list.begin();
                 iter != _id_list.end();
                 iter++)
               {
                  std::map<t_int, statusEntry>::iterator erase_iter =
                     statusList_.find(*iter);

                  if (erase_iter != statusList_.end())
                     {
                        // BTG_NOTICE("Removing " << erase_iter->second.status.contextID());
                        statusList_.erase(erase_iter);
                     }
               }
            if (_id_list.size() > 0)
               {
                  // BTG_NOTICE("Context number changed - remove");
                  changed_ = true;
               }
         }

         void statusList::clear()
         {
            statusList_.clear();
         }

         void statusList::resetChanged()
         {
            // BTG_NOTICE("Reseting context change flag");

            changed_ = false;
         }

         bool statusList::changed() const
         {
            return changed_;
         }

         t_uint statusList::size() const
         {
            return statusList_.size();
         }

         statusList::~statusList()
         {
            statusList_.clear();
         }

         /* */
         /* */
         /* */

         mainWindow::mainWindow(keyMapping const& _kmap)
            : baseWindow(_kmap),
              numberOfLines_(0),
              list_(),
              positionWindowStart_(0),
              positionWindowEnd_(0),
              currentPosition_(0)
         {
         }

         bool mainWindow::init(windowSize const& _ws)
         {
            bool status = baseWindow::init(_ws);

            if (status)
               {
                  numberOfLines_  = height_-1;
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

            drawList();
            wrefresh(window_);
         }

         void mainWindow::drawList()
         {
            std::vector<statusEntry> lst;

            list_.get(lst);

            if (lst.size() == 0)
               {
                  // No point in displaying an empty list.
                  return;
               }

            setColor(Colors::C_NORMAL);

            std::vector<statusEntry>::const_iterator iterBeg = lst.begin();
            std::vector<statusEntry>::const_iterator iterEnd = lst.begin();

            if (positionWindowEnd_ == 0)
               {
                  positionWindowEnd_ = numberOfLines_;
               }

            if (positionWindowEnd_ > static_cast<t_int>(lst.size()))
               {
                  positionWindowEnd_ = lst.size();
               }

            // Move the iterator the the start of the window to display.
            for (t_int counter = 0;
                 counter < positionWindowStart_;
                 counter++)
               {
                  iterBeg++;
               }

            // Move the iterator the the end of the window to display.
            for (t_int counter = 0;
                 counter < positionWindowEnd_;
                 counter++)
               {
                  iterEnd++;
               }

            // Find the max length of the contained file names;
            t_uint max_filename_size = 0;
            t_uint max_progress_size = 0;
            t_uint max_stat_size     = 5; // Constant.
            t_uint max_perc_size     = 0;
            t_uint max_peers_size    = 0;
            t_uint extra_space       = 2;

            std::vector<statusEntry>::const_iterator iter;
            for (iter = iterBeg;
                 iter != iterEnd;
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

            if ((max_filename_size + max_progress_size + max_stat_size + max_perc_size + max_peers_size + extra_space)
                > width_)
               {
                  max_filename_size = width_ - (max_progress_size + max_stat_size + max_perc_size + max_peers_size + extra_space);
               }

            // Display the window.
            t_int counter = 0;
            for (iter = iterBeg;
                 iter != iterEnd;
                 iter++)
               {
                  btg::core::Status const& s = iter->status;
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
                  if (iter->marked)
                     {
                        setColor(Colors::C_MARK);
                     }

                  if (counter == currentPosition_)
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
                        mvwprintw(window_, counter, 0, " %s%s%s%s%s%s", 
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
                        mvwprintw(window_, counter, 0, " %s%s%s%s%s", 
                                  filename.c_str(),
                                  st_status.c_str(),
                                  st_done.c_str(),
                                  st_progress.c_str(),
                                  st_peers.c_str());

                     }

                  // Display marked torrents with different color.
                  if (iter->marked)
                     {
                        unSetColor(Colors::C_MARK);
                     }

                  counter++;
               }
            unSetColor(Colors::C_NORMAL);
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
            list_.update(_list);

            if (list_.changed())
               {
                  list_.resetChanged();

                  // Fix the position window.
                  if (positionWindowEnd_ < static_cast<t_int>(list_.size()))
                     {
                        positionWindowEnd_ = list_.size();

                        if (positionWindowEnd_ > numberOfLines_)
                           {
                              positionWindowEnd_ = numberOfLines_;
                           }
                     }

                  drawList();
                  refresh();
               }
         }

         void mainWindow::remove(std::vector<t_int> const& _id_list)
         {
            list_.remove(_id_list);

            if (list_.changed())
               {
                  list_.resetChanged();

                  // Adjust:
                  const t_int list_size = list_.size();

                  if (positionWindowStart_ > list_size)
                     {
                        positionWindowStart_ = list_.size();
                     }

                  if (positionWindowEnd_ > list_size)
                     {
                        positionWindowEnd_ = list_.size();
                     }

                  if (currentPosition_ > list_size)
                     {
                        currentPosition_ = list_.size();
                     }

                  clear();
                  drawList();
                  refresh();
               }
         }

         void mainWindow::clearContents()
         {
            list_.clear();
         }

         void mainWindow::moveDown()
         {
            const t_int list_max = list_.size();

            if (currentPosition_ < (numberOfLines_-1))
               {
                  currentPosition_++;

                  if (list_.size() > 0)
                     {
                        if (currentPosition_ > (list_max - 1))
                           {
                              currentPosition_--;
                           }
                     }
               }
            else
               {
                  if (positionWindowEnd_ < list_max)
                     {
                        positionWindowStart_++;
                        positionWindowEnd_++;
                     }
               }

         }

         void mainWindow::moveUp()
         {
            if (currentPosition_ > 0)
               {
                  currentPosition_--;
               }
            else
               {
                  if ((positionWindowStart_-1) >= 0)
                     {
                        positionWindowStart_--;
                        positionWindowEnd_--;
                     }
               }
         }

         void mainWindow::toStart()
         {
            currentPosition_     = 0;
         }

         void mainWindow::toEnd()
         {
            currentPosition_ = (numberOfLines_-1);

            t_int max_position = list_.size();
            if (max_position > 0)
               {
                  max_position--;
               }

            if (currentPosition_ > max_position)
               {
                  currentPosition_ = max_position;
               }
         }

         bool mainWindow::get(t_int const _context_id, btg::core::Status & _status) const
         {
            return list_.get(_context_id, _status);
         }

         bool mainWindow::getSelection(btg::core::Status & _status) const
         {
            bool status = false;

            if (list_.size() == 0)
               {
                  return status;
               }

            t_uint current = positionWindowStart_ + currentPosition_;

            if (list_.getAtPosition(current, _status))
               {
                  status = true;
               }

            return status;
         }

         void mainWindow::mark()
         {
            if (list_.size() == 0)
               {
                  return;
               }

            t_uint current = positionWindowStart_ + currentPosition_;

            list_.mark(current);
         }

         void mainWindow::markAll()
         {
            list_.markAll();
         }

         void mainWindow::getMarked(std::vector<t_int> & _id_list) const
         {
            list_.getMarked(_id_list);
         }

         void mainWindow::clearMark()
         {
            list_.clearMark();
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

         mainWindow::~mainWindow()
         {
            destroy();
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

/*
 * btg Copyright (C) 2009 Michael Wojciechowski.
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

#include "dm.h"
#include <algorithm>
#include <boost/bind.hpp>

namespace btg
{
   namespace UI
   {
      namespace cli
      {
         DisplayModel::DisplayModel(t_uint _visible, 
                                    DisplayModelIf* _displayModelIf)
            : visible_(_visible),
              displayModelIf_(_displayModelIf),
              lposition_(0),
              wposition_(0),
              statusList_(),
              sortby_(DisplayModel::sB_Id),
              newEntries_(false),
              deletedEntries_(false)
         {

         }

         void DisplayModel::down()
         {
            t_int temp_wposition = 0;
            t_int temp_lposition = 0;
            
            if ((wposition_ + 1) >= visible_)
               {
                  temp_wposition = 0;
                  temp_lposition = lposition_;
                  temp_lposition += visible_;
               }
            else
               {
                  temp_wposition = wposition_;
                  temp_lposition = lposition_;
                  temp_wposition++;
               }
               
            if ((temp_lposition+temp_wposition) < statusList_.size())
            {
               lposition_ = temp_lposition;
               wposition_ = temp_wposition;
            }
         }

         void DisplayModel::up()
         {
            t_int temp_wposition = wposition_;
            t_int temp_lposition = lposition_;

            temp_wposition--;
            if (temp_wposition < 0)
               {
                  temp_wposition = 0;
                  temp_lposition--;
               }

            if (temp_lposition < 0)
               {
                  temp_lposition = 0;
               }

            wposition_ = temp_wposition;
            lposition_ = temp_lposition;
         }

         void DisplayModel::begin()
         {
            lposition_ = 0;
            wposition_ = 0;
         }
   
         void DisplayModel::end()
         {
            t_int move = statusList_.size() - (lposition_ + wposition_);

            if (move > 0)
               {
                  for (t_int i = 0; i < move; i++)
                     {
                        down();
                     }
               }
         }

         void DisplayModel::mark()
         {
            std::vector<statusEntry>::iterator iter = statusList_.begin();
            iter += lposition_;
            iter += wposition_;

            if (iter->marked)
               {
                  iter->marked = false;
               }
            else
               {
                  iter->marked = true;
               }
         }

         void DisplayModel::markAll()
         {
            std::vector<statusEntry>::iterator iter;
            for (iter = statusList_.begin();
                 iter != statusList_.end();
                 iter++)
               {
                  if (iter->marked)
                     {
                        iter->marked = false;
                     }
                  else
                     {
                        iter->marked = true;
                     }
               }
         }

         void DisplayModel::markNone()
         {
            std::vector<statusEntry>::iterator iter;
            for (iter = statusList_.begin();
                 iter != statusList_.end();
                 iter++)
               {
                  iter->marked = false;
               }
         }

         void DisplayModel::resize(t_uint _visible)
         {
            visible_ = _visible;
         }

         void DisplayModel::update(std::vector<btg::core::Status> const& _list)
         {
            std::vector<btg::core::Status>::const_iterator iter;

            std::vector<btg::core::Status> new_entries;

            // Find out if any IDs were removed.
            newEntries_ = false;

            resetUpdatedFlag();

            for (iter = _list.begin();
                 iter != _list.end();
                 iter++)
               {
                  t_int id = iter->contextID();

                  std::vector<statusEntry>::iterator dst_iter =
                     find(id);

                  if (dst_iter != statusList_.end())
                     {
                        // Update context.
                        dst_iter->status  = *iter;
                        dst_iter->updated = true;
                     }
                  else
                     {
                        // New context.
                        // This also sets the updated flag.
                        new_entries.push_back(*iter);
                        newEntries_ = true;
                     }
               }

            std::vector<btg::core::Status>::iterator iiter;

            for (iiter = new_entries.begin();
                 iiter != new_entries.end();
                 iiter++)
               {
                  statusList_.push_back(statusEntry(*iiter));
               }

            deletedEntries_ = false;

            // Remove dead entries.
            removeDead();

            if (newEntries_ || deletedEntries_)
               {
                  // Make sure that the visible entries are still
                  // valid.
                  if ((lposition_+wposition_) > statusList_.size())
                     {
                        begin();
                        end();
                     }
               }
         }

         bool DisplayModel::sizeChanged() const
         {
            bool res = false;
            if (newEntries_ || deletedEntries_)
               {
                  res = true;
               }

            return res;
         }

         void DisplayModel::setSortBy(DisplayModel::sortBy const _sortby)
         {
            sortby_ = _sortby;
            sort();
            markNone();
         }

         void DisplayModel::sort()
         {
            std::sort(statusList_.begin(), 
                      statusList_.end(), 
                      boost::bind(&DisplayModel::isStatusLess, this, _1, _2));
         }

         std::vector<statusEntry>::const_iterator DisplayModel::find(t_int const _id) const
         {
            std::vector<statusEntry>::const_iterator iter;

            for (iter = statusList_.begin();
                 iter != statusList_.end();
                 iter++)
               {
                  if (iter->id == _id)
                     {
                        return iter;
                     }
               }

            return statusList_.end();
         }

         std::vector<statusEntry>::iterator DisplayModel::find(t_int const _id)
         {
            std::vector<statusEntry>::iterator iter;

            for (iter = statusList_.begin();
                 iter != statusList_.end();
                 iter++)
               {
                  if (iter->id == _id)
                     {
                        return iter;
                     }
               }

            return statusList_.end();
         }

         void DisplayModel::resetUpdatedFlag()
         {
            std::vector<statusEntry>::iterator iter;
            for (iter = statusList_.begin();
                 iter != statusList_.end();
                 iter++)
               {
                  iter->updated = false;
               }
         }

         void DisplayModel::removeDead()
         {
            std::vector<t_int> delete_entries;
            std::vector<statusEntry>::iterator iter = statusList_.begin();
         
            while (iter != statusList_.end())
               {
                  if (!iter->updated)
                     {
                        deletedEntries_ = true;
                        delete_entries.push_back(iter->status.contextID());
                     }
                  iter++;
               }

            for (std::vector<t_int>::const_iterator i = delete_entries.begin();
                 i != delete_entries.end();
                 i++)
               {
                  std::vector<statusEntry>::iterator del_iter =
                     find(*i);

                  if (del_iter != statusList_.end())
                     {
                        statusList_.erase(del_iter);
                     }
               }
         }

         void DisplayModel::draw()
         {
            displayModelIf_->pstart();

            std::vector<statusEntry>::const_iterator i_beg = statusList_.begin();
            i_beg += lposition_;
            std::vector<statusEntry>::const_iterator i_end = statusList_.begin();
            i_end += lposition_;

            if ((lposition_+visible_) < statusList_.size())
               {
                  i_end += visible_;
               }
            else
               {
                  i_end = statusList_.end();
               }

            std::vector<statusEntry> l;
            std::vector<statusEntry>::const_iterator i;
            for (i = i_beg;
                 i != i_end;
                 i++)
               {
                  l.push_back(*i);
               }

            displayModelIf_->findSizes(l);

            t_uint n = 0;
            bool current = false;

            for (i = i_beg;
                 i != i_end;
                 i++)
               {
                  if (n == wposition_)
                     {
                        current = true;
                     }
                  else
                     {
                        current = false;
                     }

                  displayModelIf_->paint(n, current, *i, wposition_);
                  n++;
               }

            displayModelIf_->pend();
         }

         t_uint DisplayModel::size() const
         {
            return statusList_.size();
         }

         bool DisplayModel::isStatusLess(statusEntry const& _l, statusEntry const& _r) const
         {
            bool result = false;
            switch(sortby_)
               {
               case sB_Id:
                  result = _l.status.contextID() < _r.status.contextID();
                  break;
               case sB_Name:
                  result = _l.status.filename() < _r.status.filename();
                  break;
               case sB_Size:
                  result = _l.status.filesize() > _r.status.filesize();
                  break;
               case sB_UlSpeed:
                  result = _l.status.uploadRate() > _r.status.uploadRate();
                  break;
               case sB_DlSpeed:
                  result = _l.status.downloadRate() > _r.status.downloadRate();
                  break;
               case sB_Peers:
                  {
                     t_uint l = (_l.status.seeders() + _l.status.leechers());
                     t_uint r = (_r.status.seeders() + _r.status.leechers());
                     result = l > r;
                     break;
                  }
               case sB_Done:
                  result = _l.status.done() > _r.status.done();
                  break;
               }

            return result;
         }

         bool DisplayModel::getCurrent(btg::core::Status & _status) const
         {
            if (statusList_.size() == 0)
               {
                  return false;
               }
          
            std::vector<statusEntry>::const_iterator iter = statusList_.begin();
            iter += lposition_;
            iter += wposition_;

            _status = iter->status;

            return true;
         }

         bool DisplayModel::getCurrent(t_strList & _trackers) const
         {
            bool status = false;
            if (statusList_.size() == 0)
               {
                  return status;
               }

            std::vector<statusEntry>::const_iterator iter = statusList_.begin();
            iter += lposition_;
            iter += wposition_;

            if (iter->trackers_set)
               {
                  _trackers = iter->trackers;
                  status    = true;
                  return status;
               }
            return status;
         }

         bool DisplayModel::setCurrent(t_strList const& _trackers)
         {
            bool status = false;
            if (statusList_.size() == 0)
               {
                  return status;
               }

            std::vector<statusEntry>::iterator iter = statusList_.begin();
            iter += lposition_;
            iter += wposition_;

            iter->trackers_set = true;
            iter->trackers     = _trackers;
            status    = true;

            return status;
         }

         void DisplayModel::remove(std::vector<t_int> const& _id_list)
         {
            std::vector<t_int>::const_iterator iter;
            for (iter = _id_list.begin();
                 iter != _id_list.end();
                 iter++)
               {
                  std::vector<statusEntry>::iterator i = find(*iter);
                  if (i != statusList_.end())
                     {
                        i->updated = false;
                     }
               }
            
            std::vector<btg::core::Status> l;
            std::vector<statusEntry>::const_iterator liter;
            for (liter = statusList_.begin();
                 liter != statusList_.end();
                 liter++)
               {
                  l.push_back(liter->status);
               }

            update(l);
         }

         bool DisplayModel::get(t_int const _context_id, btg::core::Status & _status) const
         {
            std::vector<statusEntry>::const_iterator i = find(_context_id);

            if (i != statusList_.end())
               {
                  _status = i->status;
                  return true;
               }

            return false;
         }

         void DisplayModel::getSelected(std::vector<t_int> & _id_list) const
         {
            std::vector<statusEntry>::const_iterator iter;
            for (iter = statusList_.begin();
                 iter != statusList_.end();
                 iter++)
               {
                  if (iter->marked)
                     {
                        _id_list.push_back(iter->id);
                     }
               }
         }
         
         t_uint DisplayModel::selected() const
         {
            t_uint num = 0;
            std::vector<statusEntry>::const_iterator iter;
            for (iter = statusList_.begin();
                 iter != statusList_.end();
                 iter++)
               {
                  if (iter->marked)
                     {
                        num++;
                     }
               }
            return num;
         }

         DisplayModel::~DisplayModel()
         {
            statusList_.clear();
            displayModelIf_ = 0;
         }

      } // namespace cli
   } // namespace UI
} // namespace btg




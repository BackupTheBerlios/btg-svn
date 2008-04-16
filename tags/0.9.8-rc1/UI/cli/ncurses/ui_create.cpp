/*
 * btg Copyright (C) 2008 Michael Wojciechowski.
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

#include "ui.h"
#include <bcore/t_string.h>
#include "progress.h"

namespace btg
{
   namespace UI
   {
      namespace cli
      {
         void UI::CPRI_init(std::string const& _filename)
         {
            windowSize dimensions;
            mainwindow_.getSize(dimensions);

            progress_ = new progressWindow(dimensions, keymap_);
         }

         void UI::CPRI_pieceUploaded(t_uint _number, t_uint _parts)
         {
            if (progress_)
               {
                  t_uint p = ((_number * 100) / _parts) * 0.9;
                  // t_uint p = ((_number * 100) / _parts) / 2;

                  std::string number = btg::core::convertToString<t_uint>(_number);
                  std::string parts  = btg::core::convertToString<t_uint>(_parts);

                  progress_->updateProgress(p, "Uploading #" + number + "/" + parts + ".");
               }
         }

         void UI::CPRI_error(std::string const& _error)
         {
            if (progress_)
               {
                  progress_->updateProgress(100, _error);
               }

            delete progress_;
            progress_ = 0;
         }

         void UI::CPRI_wait(std::string const& _msg)
         {
            if (progress_)
               {
                  progress_->updateProgress(95, "Wait for daemon..");
               }
         }

         void UI::CPRI_success(std::string const& _filename)
         {
            if (progress_)
               {
                  progress_->updateProgress(100, "Torrent created.");
               }

            delete progress_;
            progress_ = 0;
         }

      } // namespace cli
   } // namespace UI
} // namespace btg


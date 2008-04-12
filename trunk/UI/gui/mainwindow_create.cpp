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

#include "mainwindow.h"

#include <bcore/t_string.h>
#include <bcore/os/sleep.h>

#include "progressdialog.h"

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         void mainWindow::CPRI_init(std::string const& _filename)
         {
            CPRI_reset();

            upload_progressdialog = new progressDialog("Uploading '" + _filename + "' to the daemon.", true);
            upload_progressdialog->updateProgress(0, "Initalizing..");
         }

         void mainWindow::CPRI_pieceUploaded(t_uint _number, t_uint _parts)
         {
            if (upload_progressdialog)
               {
                  if (upload_progressdialog->cancelPressed())
                     {
                        CPRI_cancel();
                     }

                  // Show as 0-98 % while uploading.
                  t_uint p = ((double)_number / _parts) * 98;

                  std::string number = btg::core::convertToString<t_uint>(_number);
                  std::string parts  = btg::core::convertToString<t_uint>(_parts);

                  upload_progressdialog->updateProgress(p, "Uploading #" + number + "/" + parts + ".");
               }
         }

         void mainWindow::CPRI_error(std::string const& _error)
         {
            if (upload_progressdialog)
               {
                  upload_progressdialog->updateProgress(100, "Upload done.");
                  btg::core::os::Sleep::sleepMiliSeconds(1000);
               }

            delete upload_progressdialog;
            upload_progressdialog = 0;
         }

         void mainWindow::CPRI_wait(std::string const& _msg)
         {
            if (upload_progressdialog)
               {
                  if (upload_progressdialog->cancelPressed())
                     {
                        CPRI_cancel();
                     }
                  // waiting at 98%
                  upload_progressdialog->updateProgress(98, _msg);
               }
         }

         void mainWindow::CPRI_success(std::string const& _filename)
         {
            if (upload_progressdialog)
               {
                  upload_progressdialog->updateProgress(100, "Upload done.");
                  btg::core::os::Sleep::sleepMiliSeconds(1000);
               }

            delete upload_progressdialog;
            upload_progressdialog = 0;
         }

      } // namespace gui
   } // namespace UI
} // namespace btg

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

#include "ui.h"

#include <bcore/logmacro.h>

#include "helpwindow.h"
#include "filelist.h"
#include "fileview.h"
#include "peerlist.h"

#include "basemenu.h"
#include "limitwindow.h"
#include "progress.h"

#include <bcore/client/handlerthr.h>
#include "handler.h"

#include <bcore/command/limit_base.h>
#include <bcore/hrr.h>

#define GET_HANDLER_INST \
   boost::shared_ptr<boost::mutex> ptr = handlerthread_.mutex(); \
   boost::mutex::scoped_lock interface_lock(*ptr); \
   Handler* handler = dynamic_cast<Handler*>(&handlerthread_.handler());

namespace btg
{
   namespace UI
   {
      namespace cli
      {
         using namespace btg::core;
         using namespace btg::core::client;

         UI::UI(btg::core::LogWrapperType _logwrapper,
                std::string const& _session,
                bool _neverAskQuestions,
                bool _urlDlEnabled,
                keyMapping const& _keymap,
                Colors & _colors,
                btg::core::client::handlerThread& _handlerthread)
            : btg::core::Logable(_logwrapper),
              btg::core::client::createPartsReportInterface(),
              progress_(0),
              session_(_session),
              neverAskQuestions_(_neverAskQuestions),
              urlDlEnabled_(_urlDlEnabled),
              keymap_(_keymap),
              colors_(_colors),
              mainwindow_(_keymap, *this),
              topwindow_(_keymap),
              statuswindow_(_keymap),
              mainwindow_topx(0),
              mainwindow_topy(0),
              mainwindow_width(0),
              mainwindow_height(0),
              load_directory_(),
              handlerthread_(_handlerthread)
         {
         }

         bool UI::init()
         {
            baseWindow::ncursesInit();

            if (!colors_.init())
               {
                  // No support for colors.
                  return false;
               }

            windowSize dimensions;
            dimensions.topX = 0;
            dimensions.topY = 0;

            baseWindow::getScreenSize(dimensions.width, dimensions.height);

            //
            // Attempt to use the following layout:
            //
            // +-----------------------+
            // |       status          |
            // +-----------------------+
            // |                       |
            // |       list            |
            // |                       |
            // |                       |
            // |                       |
            // |                       |
            // +-----------------------+
            // |       keys            |
            // +-----------------------+

            // Init the main window.

            windowSize mainwndSize = mainwindow_.calculateDimenstions(dimensions);
            bool init_success = mainwindow_.init(mainwndSize);

            if (!init_success)
               {
                  return false;
               }

            // Init the top window.
            windowSize topwndSize = topwindow_.calculateDimenstions(dimensions);
            init_success = topwindow_.init(topwndSize);

            if (!init_success)
               {
                  return false;
               }

            topwindow_.setTitle(GPD->sCLI_CLIENT() +" version " + GPD->sFULLVERSION() + 
                                " (session #" + session_ + ")");

            // Init the status window.
            windowSize statuswndSize = statuswindow_.calculateDimenstions(dimensions);
            init_success = statuswindow_.init(statuswndSize);
            if (!init_success)
               {
                  return false;
               }

            setDefaultStatusText();
	    
            refresh();

            return true;
         }

         void UI::destroy()
         {
            mainwindow_.destroy();

            baseWindow::ncursesDestroy();
         }


         void UI::update(std::vector<btg::core::Status> const& _list)
         {
            mainwindow_.update(_list);
         }

         void UI::clear()
         {
            mainwindow_.clearContents();
         }

         void UI::updateBandwidth(std::vector<btg::core::Status> const& _list)
         {
            t_ulong dwnlr = 0;
            t_ulong upldr = 0;

            std::vector<btg::core::Status>::const_iterator iter;
            for (iter = _list.begin();
                 iter != _list.end();
                 iter++)
               {
                  dwnlr += iter->downloadRate();
                  upldr += iter->uploadRate();
               }

            btg::core::humanReadableRate u_hrr = humanReadableRate::convert(upldr);
            btg::core::humanReadableRate d_hrr = humanReadableRate::convert(dwnlr);

            std::string status = "U: ";
            status += u_hrr.toString();
            status += " D: ";
            status += d_hrr.toString();

            topwindow_.setStatus(status);
         }

         void UI::remove(std::vector<t_int> const& _id_list)
         {
            mainwindow_.remove(_id_list);
         }

         void UI::setDefaultStatusText()
         {
            std::string value("KEY");
            keymap_.getValue(keyMapping::K_HELP, value);
            statuswindow_.setStatus("Press /" + value + "/ for help.");

            refresh();
         }

         bool UI::setLimit(t_int const _context_id,
                           std::string const& _filename,
                           t_int const _download,
                           t_int const _upload,
                           t_int const _seedTimeout,
                           t_int const _seedPercent)
         {
            bool status = false;

            {
               GET_HANDLER_INST;
               handler->reqLimit(_context_id,
                                 _upload,
                                 _download,
                                 _seedPercent,
                                 _seedTimeout);

               if (handler->commandSuccess())
                  {
                     status = true;
                     actionSuccess("Limit", _filename);
                  }
               else
                  {
                     actionFailture("Limit", _filename);
                  }
            }

            return status;
         }

         bool UI::limitPopup(t_int const _context_id,
                             t_int & _download,
                             t_int & _upload,
                             t_int & _seedTimeout,
                             t_int & _seedPercent)
         {
            bool status = false;

            statuswindow_.setStatus("Limiting torrent.");

            t_int download     = limitBase::LIMIT_DISABLED;
            t_int upload       = limitBase::LIMIT_DISABLED;
            t_int seedPercent = limitBase::LIMIT_DISABLED;
            t_long seedTimeout  = limitBase::LIMIT_DISABLED;

            // Get the current limit.
            {
               GET_HANDLER_INST;
               handler->reqLimitStatus(_context_id);

               if (handler->commandSuccess())
                  {
                     handler->getLimitStatus(upload, download);
                     handler->getSeedLimitStatus(seedPercent, seedTimeout);

                     if (upload != limitBase::LIMIT_DISABLED)
                        {
                           upload      /= limitBase::KiB_to_B;
                        }

                     if (download != limitBase::LIMIT_DISABLED)
                        {
                           download    /= limitBase::KiB_to_B;
                        }
                  }
            }

            windowSize limitdimensions;
            mainwindow_.getSize(limitdimensions);

            limitWindow limitwindow(keymap_,
                                    limitdimensions,
                                    upload,
                                    limitBase::LIMIT_DISABLED, /* min */
                                    65536,              /* max */
                                    "Upload (KiB/sec)", /* label */
                                    download,
                                    limitBase::LIMIT_DISABLED,
                                    65536,
                                    "Download (KiB/sec)",
                                    seedTimeout,
                                    limitBase::LIMIT_DISABLED,
                                    1440,
                                    "Seed timeout (minutes)",
                                    static_cast<t_int>(seedPercent),
                                    limitBase::LIMIT_DISABLED,
                                    10000,
                                    "Seed percent (%)");

            switch (limitwindow.run())
               {
               case dialog::R_RESIZE:
                  {
                     // the window was resized.
                     handleResizeMainWindow();
		  
                     // abort setting of limits, if the window was resized.
                     return false;
                     break;
                  }
               case dialog::R_NCREAT:
                  {
                     return false;
                     break;
                  }
               default:
                  {
                     break;
                  }
               }
            
            t_int tmp_seedTimeout;
            if (limitwindow.getLimits(upload,
                                      download,
                                      seedPercent,
                                      tmp_seedTimeout))
               {
                  seedTimeout = tmp_seedTimeout; // to convert int to long
                  status = true;

                  if (upload != limitBase::LIMIT_DISABLED)
                     {
                        upload      *= limitBase::KiB_to_B;
                     }

                  if (download != limitBase::LIMIT_DISABLED)
                     {
                        download    *= limitBase::KiB_to_B;
                     }

                  _download    = download;
                  _upload      = upload;
                  _seedTimeout = seedTimeout;
                  _seedPercent = seedPercent;
               }

            return status;
         }

         void UI::refresh()
         {
            topwindow_.clear();
            mainwindow_.clear();
            statuswindow_.clear();

            topwindow_.refresh();
            mainwindow_.refresh();
            statuswindow_.refresh();

            ::refresh();
         }

         void UI::resize()
         {
            // Find out the new sizes.
            windowSize dimensions;
            // TODO: save the new dimension here.

            // Resize the contained windows.
            topwindow_.resize(dimensions);
            mainwindow_.resize(dimensions);
            statuswindow_.resize(dimensions);
         }

         void UI::actionSuccess(std::string const& _action, 
                                std::string const& _filename)
         {
            statuswindow_.setStatus(_action + ", '" + _filename + "'.");
         }

         void UI::actionFailture(std::string const& _action, 
                                 std::string const& _filename)
         {
            statuswindow_.setError(_action + " failed, '" + _filename + "'.");
         }

         void UI::actionAborted(std::string const& _action, 
                                std::string const& _filename)
         {
            statuswindow_.setError(_action + " aborted, '" + _filename + "'.");
         }

         void UI::actionAbortedByResize(std::string const& _action, 
                                        std::string const& _filename)
         {
            statuswindow_.setError("Resize, " + _action + " aborted, '" + _filename + "'.");
         }

         void UI::genHelpText(std::string & _output)
         {
            std::string keyDescr;

            if (helpWindow::generateHelpForKey(keymap_,
                                               keyMapping::K_HELP,
                                               "help",
                                               keyDescr,
                                               true /* short */))
               {
                  _output += keyDescr;
               }
         }

         UI::~UI()
         {
            delete progress_;
            progress_ = 0;
         }

      } // namespace cli
   } // namespace UI
} // namespace btg


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

#include "fileview.h"

#include <algorithm>

#include "helpwindow.h"

#include <bcore/util.h>
#include <bcore/hru.h>
#include <bcore/t_string.h>

namespace btg
{
   namespace UI
   {
      namespace cli
      {
         // One file uses 3 lines.
         const t_uint linesPerFile = 4;

         fileView::fileView(keyMapping const& _kmap,
                            windowSize const& _ws,
                            std::vector<btg::core::fileInformation> const& _fi)
            : baseWindow(_kmap),
              size_(_ws),
              info_(_fi),
              numberOfLines_(0),
              positionWindowStart_(0),
              positionWindowEnd_(0),
              currentPosition_(0)
         {
            // Keys used by this window.
            std::vector<keyMapping::KEYLABEL> labels;
            labels.push_back(keyMapping::K_HELP);
            labels.push_back(keyMapping::K_BACK);
            labels.push_back(keyMapping::K_DOWN);
            labels.push_back(keyMapping::K_UP);
            labels.push_back(keyMapping::K_LIST_START);
            labels.push_back(keyMapping::K_LIST_END);
            labels.push_back(keyMapping::K_RESIZE);
            kmap_.setUsed(labels);
         }

         dialog::RESULT fileView::run()
         {
            if (!init(size_))
               {
                  return dialog::R_NCREAT;
               }

            refresh();

            // Handle keyboard.

            keyMapping::KEYLABEL label;

            bool cont    = true;

            while (cont)
               {
                  label = handleKeyboard();

                  switch (label)
                     {
                     case keyMapping::K_HELP:
                        {
                           if (showHelp() == dialog::R_RESIZE)
                              {
                                 // Resized window.
                                 return dialog::R_RESIZE;
                              }
                           break;
                        }
                     case keyMapping::K_BACK:
                        {
                           cont = false;
                           break;
                        }
                     case keyMapping::K_DOWN:
                        {
                           moveDown();
                           refresh();
                           break;
                        }
                     case keyMapping::K_UP:
                        {
                           moveUp();
                           refresh();
                           break;
                        }
                     case keyMapping::K_LIST_START:
                        {
                           toStart();
                           refresh();
                           break;
                        }
                     case keyMapping::K_LIST_END:
                        {
                           toEnd();
                           refresh();
                           break;
                        }
                     case keyMapping::K_RESIZE:
                        {
                           // Close this window, and make the parent
                           // resize its window.
                           return dialog::R_RESIZE;
                           break;
                        }
                     default:
                        {
                           refresh();
                           break;
                        }
                     }
               }

            return dialog::R_QUIT;
         }

         bool fileView::init(windowSize const& _ws)
         {
            bool status = baseWindow::init(_ws);

            if (!status)
               {
                  return status;
               }

            numberOfLines_  = (_ws.height-1) / linesPerFile;

            if (numberOfLines_ < 1)
               {
                  // Not enough space to show even one file.
                  // Abort.
                  return false;
               }

            return status;
         }

         void fileView::resize(windowSize const& _ws)
         {

         }

         bool fileView::destroy()
         {
            if (window_ != 0)
               {
                  delwin(window_);
                  window_ = 0;
               }

            return true;
         }

         void fileView::refresh()
         {
            if (window_ == 0)
               {
                  return;
               }

            clear();
            drawList();
            wrefresh(window_);
         }

         void fileView::drawList()
         {
            std::vector<btg::core::fileInformation>::const_iterator iterBeg = info_.begin();
            std::vector<btg::core::fileInformation>::const_iterator iterEnd = info_.begin();

            if (positionWindowEnd_ == 0)
               {
                  positionWindowEnd_ = numberOfLines_;
               }

            if (positionWindowEnd_ > static_cast<t_int>(info_.size()))
               {
                  positionWindowEnd_ = info_.size();
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

            // Display the window.
            t_int counter = 0;
            t_int scr_counter = 0;
            std::vector<btg::core::fileInformation>::const_iterator iter;
            for (iter = iterBeg;
                 iter != iterEnd;
                 iter++)
               {
                  btg::core::fileInformation const& fi = *iter;

                  std::string org_filename = fi.getFilename();

                  // Split the filename into path + file.
                  std::string path;
                  std::string filename;
                  splitPath(org_filename, path, filename);

                  // Convert the file size into human readable units.
                  t_ulong bytes                    = fi.getFileSize();
                  btg::core::humanReadableUnit hru = btg::core::humanReadableUnit::convert(bytes);
                  std::string fsize                = hru.toString();

                  // Display the pieces that we have and the percent of the file downloaded.
                  std::string pieces;
                  genPieces(fi, pieces);
                  
                  // Find out how much padding is required.
                  std::string path_spaces;
                  std::string filename_spaces;
                  std::string fsize_spaces;
                  std::string pieces_spaces;
                  genSpaces(path, path_spaces);
                  genSpaces(filename, filename_spaces);
                  genSpaces(fsize, fsize_spaces);
                  genSpaces(pieces, pieces);

                  if (counter == currentPosition_)
                     {
                        ::wattron(window_, A_REVERSE);

                        setColor(Colors::C_FILE);
                        mvwprintw(window_, scr_counter,   0, " %s%s", filename.c_str(), filename_spaces.c_str());
                        mvwprintw(window_, scr_counter+1, 0, " %s%s", path.c_str(),     path_spaces.c_str());
                        mvwprintw(window_, scr_counter+2, 0, " %s%s", fsize.c_str(),    fsize_spaces.c_str());
                        mvwprintw(window_, scr_counter+3, 0, " %s%s", pieces.c_str(),   pieces_spaces.c_str());
                        unSetColor(Colors::C_FILE);

                        ::wattroff(window_, A_REVERSE);
                     }
                  else
                     {
                        setColor(Colors::C_FILE);
                        mvwprintw(window_, scr_counter,   0, " %s%s", filename.c_str(), filename_spaces.c_str());
                        mvwprintw(window_, scr_counter+1, 0, " %s%s", path.c_str(),     path_spaces.c_str());
                        mvwprintw(window_, scr_counter+2, 0, " %s%s", fsize.c_str(),    fsize_spaces.c_str());
                        mvwprintw(window_, scr_counter+3, 0, " %s%s", pieces.c_str(),   pieces_spaces.c_str());
                        unSetColor(Colors::C_FILE);
                     }

                  counter++;
                  scr_counter += linesPerFile;
               }
         }

         void fileView::moveDown()
         {
            const t_int list_max = info_.size();

            if (currentPosition_ < (numberOfLines_-1))
               {
                  currentPosition_++;

                  if (info_.size() > 0)
                     {
                        if (currentPosition_ > (list_max-1))
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

         void fileView::moveUp()
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

         void fileView::toStart()
         {
            currentPosition_     = 0;
         }

         void fileView::toEnd()
         {
            currentPosition_ = (numberOfLines_-1);

            t_int max_position = info_.size();
            if (max_position > 0)
               {
                  max_position--;
               }

            if (currentPosition_ > max_position)
               {
                  currentPosition_ = max_position;
               }
         }

         windowSize fileView::calculateDimenstions(windowSize const& _ws) const
         {
            // Use the parents dimensions.
            windowSize ws = _ws;

            return ws;
         }

         dialog::RESULT fileView::showHelp()
         {
            // Show a help window on the middle of the screen.
            std::vector<std::string> helpText;

            helpText.push_back("Help");
            helpText.push_back("    ");

            std::string keyDescr;

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_DOWN,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to move down");
               }


            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_UP,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to move up");
               }

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_BACK,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to quit this display");
               }

            helpText.push_back(" ");

            helpWindow hw(kmap_, helpText);

            return hw.run();
         }

         void fileView::splitPath(std::string const& _input,
                                  std::string & _path,
                                  std::string & _filename) const
         {
            t_strList lst    = btg::core::Util::splitLine(_input, "/");
            t_strListCI iter = lst.begin();

            if (lst.size() <= 1)
               {
                  _path     = "./";
                  _filename = _input;
               }
            else
               {
                  t_strListCI iter_end = lst.end();
                  iter_end--;

                  // At least one "/".
                  for (iter = lst.begin();
                       iter != iter_end;
                       iter++)
                     {
                        _path += *iter;
                        _path += "/";
                     }

                  _filename = *iter;
               }
         }

         void fileView::genPieces(btg::core::fileInformation const& _fi, std::string & _output)
         {
            if (_fi.isFull())
               {
                  _output += "100% done";
               }
            else if (_fi.isEmpty())
               {
                  _output += "0% done";
               }
            else
               {
                  t_bitVector bl = _fi.getBits();

                  t_bitVectorCI iter;
                  t_uint bit_set = 0;
                  t_uint bit_unset = 0;

                  for (iter  = bl.begin();
                       iter != bl.end();
                       iter++)
                     {
                        if (*iter)
                           {
                              bit_set++;
                           }
                        else
                           {
                              bit_unset++;
                           }
                     }

                  t_int perc = bit_set * 100 / (bit_set + bit_unset);
                  _output += btg::core::convertToString<t_int>(perc);
                  _output += "% done";
               }
         }

         fileView::~fileView()
         {
            destroy();
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

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
 * $Id: fileselect.cpp,v 1.1.2.2 2007/06/07 22:10:16 wojci Exp $
 */

#include "fileselect.h"

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
         const t_uint linesPerFile = 2;

         fileSelect::fileSelect(keyMapping const& _kmap, 
                                windowSize const& _ws,
                                btg::core::selectedFileEntryList const& _list)
            : baseWindow(_kmap),
              size_(_ws),
              original_files_(_list.files()),
              selected_files_(_list.files()),
              numberOfLines_(0),
              positionWindowStart_(0),
              positionWindowEnd_(0),
              currentPosition_(0),
              currentId_(0),
              pressed_select_key_(false)       
         {

         }

         dialog::RESULT fileSelect::run()
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
                     case keyMapping::K_QUIT:
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
                     case keyMapping::K_MARK:
                        {
                           handleMark();
                           moveDown();
                           refresh();
                           break;
                        }
                     case keyMapping::K_SELECT:
                        {
                           pressed_select_key_ = true;
                           cont                = false;
                           break;
                        }
                     case keyMapping::K_MARK_ALL:
                        {
                           handleMarkAll();
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

         bool fileSelect::init(windowSize const& _ws)
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

         void fileSelect::resize(windowSize const& _ws)
         {

         }

         bool fileSelect::destroy()
         {
            if (window_ != 0)
               {
                  delwin(window_);
                  window_ = 0;
               }

            return true;
         }

         void fileSelect::refresh()
         {
            if (window_ == 0)
               {
                  return;
               }

            clear();
            drawList();
            wrefresh(window_);
         }

         void fileSelect::drawList()
         {
            std::vector<btg::core::selectedFileEntry>::iterator iterBeg = selected_files_.begin();
            std::vector<btg::core::selectedFileEntry>::iterator iterEnd = selected_files_.begin();

            if (positionWindowEnd_ == 0)
               {
                  positionWindowEnd_ = numberOfLines_;
               }

            if (positionWindowEnd_ > static_cast<t_int>(selected_files_.size()))
               {
                  positionWindowEnd_ = selected_files_.size();
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
            t_int counter     = 0;
            t_int scr_counter = 0;
            std::vector<btg::core::selectedFileEntry>::const_iterator iter;
            for (iter = iterBeg;
                 iter != iterEnd;
                 iter++)
               {
                  // Split the filename into path + file.
                  std::string path;
                  std::string filename;
                  splitPath(iter->filename(), path, filename);

                  // Find out how much padding is required.
                  std::string path_spaces;
                  std::string filename_spaces;
                  genSpaces(path, path_spaces);
                  genSpaces(filename, filename_spaces);

                  // Display marked torrents with different color.
                  if (iter->selected())
                     {
                        setColor(Colors::C_MARK);
                     }

                  if (counter == currentPosition_)
                     {
                        ::wattron(window_, A_REVERSE);
                        
                        if (!iter->selected())
                           {
                              setColor(Colors::C_FILE);
                           }
                        mvwprintw(window_, scr_counter,   0, " %s%s", filename.c_str(), filename_spaces.c_str());
                        mvwprintw(window_, scr_counter+1, 0, " %s%s", path.c_str(),     path_spaces.c_str());

                        if (!iter->selected())
                           {
                              unSetColor(Colors::C_FILE);
                           }

                        ::wattroff(window_, A_REVERSE);
                     }
                  else
                     {
                        if (!iter->selected())
                           {
                              setColor(Colors::C_FILE);
                           }
                        mvwprintw(window_, scr_counter,   0, " %s%s", filename.c_str(), filename_spaces.c_str());
                        mvwprintw(window_, scr_counter+1, 0, " %s%s", path.c_str(),     path_spaces.c_str());
                        if (!iter->selected())
                           {
                              unSetColor(Colors::C_FILE);
                           }
                     }

                  // Display marked torrents with different color.
                  if (iter->selected())
                     {
                        unSetColor(Colors::C_MARK);
                     }

                  counter++;
                  scr_counter += linesPerFile;
               }
         }

         void fileSelect::moveDown()
         {
            const t_int list_max = selected_files_.size();

            if (currentPosition_ < (numberOfLines_-1))
               {
                  currentPosition_++;

                  if (selected_files_.size() > 0)
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

         void fileSelect::moveUp()
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

         void fileSelect::handleMark()
         {
            if (selected_files_.size() == 0)
               {
                  return;
               }

            t_uint current = positionWindowStart_ + currentPosition_;

            std::vector<btg::core::selectedFileEntry>::iterator iter = selected_files_.begin();
            iter += current;
            
            if (iter != selected_files_.end())
               {
                  if (iter->selected())
                     {
                        iter->deSelect();
                     }
                  else
                     {
                        iter->select();
                     }
               }
         }

         void fileSelect::handleMarkAll()
         {
            std::vector<btg::core::selectedFileEntry>::iterator iter;
            for (iter = selected_files_.begin();
                 iter != selected_files_.end();
                 iter++)
               {
                  if (iter->selected())
                     {
                        iter->deSelect();
                     }
                  else
                     {
                        iter->select();
                     }  
               }
         }

         void fileSelect::toStart()
         {
            currentPosition_     = 0;
         }

         void fileSelect::toEnd()
         {
            currentPosition_ = (numberOfLines_-1);

            t_int max_position = selected_files_.size();
            if (max_position > 0)
               {
                  max_position--;
               }

            if (currentPosition_ > max_position)
               {
                  currentPosition_ = max_position;
               }
         }

         windowSize fileSelect::calculateDimenstions(windowSize const& _ws) const
         {
            // Use the parents dimensions.
            windowSize ws = _ws;

            return ws;
         }

         dialog::RESULT fileSelect::showHelp()
         {
            // Show a help window on the middle of the screen.
            std::vector<std::string> helpText;

            helpText.push_back("Help");
            helpText.push_back("----");
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
                                               keyMapping::K_MARK,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to choose a file");
               }

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_MARK_ALL,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to choose all files");
               }

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_SELECT,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to set files to download");
               }

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_QUIT,
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

         void fileSelect::splitPath(std::string const& _input,
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

         bool fileSelect::changed() const
         {
            if (original_files_ == selected_files_)
               {
                  return false;
               }

            return true;
         }

         bool fileSelect::pressed_select() const
         {
            return pressed_select_key_;
         }

         void fileSelect::getFiles(btg::core::selectedFileEntryList & _list)
         {
            _list.setFiles(selected_files_);
         }

         fileSelect::~fileSelect()
         {
            destroy();
            selected_files_.clear();
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

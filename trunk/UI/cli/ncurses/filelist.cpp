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

#include "filelist.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>

#include <algorithm>
#include <cctype>

#include "helpwindow.h"

namespace btg
{
   namespace UI
   {
      namespace cli
      {
         bool filename_compare(boost::filesystem::path const& _p1,
                               boost::filesystem::path const& _p2);

         namespace fs = boost::filesystem;

         fileList::fileList(keyMapping const& _kmap, 
                            windowSize const& _ws,
                            std::string const& _directory)
            : baseWindow(_kmap),
              size_(_ws),
              currentDirectory_(_directory),
              currentFile_(),
              numberOfLines_(0),
              positionWindowStart_(0),
              positionWindowEnd_(0),
              currentPosition_(0)
         {
            if (currentDirectory_.size() == 0)
               {
                  currentDirectory_ = fs::initial_path().string();
               }
         }

         dialog::RESULT fileList::run()
         {
            init(size_);

            getDirectory();

            refresh();

            // Handle keyboard.

            keyMapping::KEYLABEL label = keyMapping::K_UNDEF;
            t_uint               key   = 0;
            bool                 cont  = true;

            while (cont)
               {
                  key = readAnyKey();
                  if (!handleKeyboard(key, label))
                     {
                        // Got something that is not recognized as one of
                        // the keys that are used.
                        // 
                        // Use this information to move to the position
                        // in the list of files which begins with this
                        // character.

                        moveTo(key);
                        continue;
                     }

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
                     case keyMapping::K_QUITSCREEN:
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
                     case keyMapping::K_SELECT:
                        {
                           fs::path p;
                           if (getSelection(p))
                              {
                                 if (p.string() == "")
                                    {
                                       break;
                                    }

                                 if (fs::is_directory(p))
                                    {
                                       // check here, that the
                                       // selected directory is valid.
                                       //
                                       // It seems that when using
                                       // directories with + in them,
                                       // an exception is thrown.
                                       try
                                          {
                                             fs::path fpath(p.string());
                                          }
                                       catch (...)
                                          {
                                             break;
                                          }

                                       currentDirectory_ = p.string();

                                       getDirectory();

                                       currentPosition_     = 0;
                                       positionWindowEnd_   = 0;
                                       positionWindowStart_ = 0;

                                       refresh();
                                    }
                                 else
                                    {
                                       // A file was selected.
                                       currentFile_ = p.string();
                                       cont         = false;
                                    }
                              }

                           // A file or directory was selected.
                           // Find out which one.
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

         std::string fileList::getLastDirectory() const
         {
            return currentDirectory_;
         }

         bool fileList::getFile(std::string & _file) const
         {
            bool status = false;

            if (currentFile_.size() > 0)
               {
                  _file  = currentFile_;
                  status = true;
               }

            return status;
         }

         bool fileList::init(windowSize const& _ws)
         {
            bool status = baseWindow::init(_ws);

            if (status)
               {
                  numberOfLines_  = _ws.height-1;
               }

            return status;
         }

         void fileList::resize(windowSize const& _ws)
         {

         }

         bool fileList::destroy()
         {
            if (window_ != 0)
               {
                  delwin(window_);
                  window_ = 0;
               }

            return true;
         }

         void fileList::refresh()
         {
            if (window_ == 0)
               {
                  return;
               }

            clear();
            drawList();
            wrefresh(window_);
         }

         void fileList::fillIterator(std::vector<fs::path>::const_iterator & _startIter,
                                     std::vector<fs::path>::const_iterator & _endIter)
         {
            if (positionWindowEnd_ == 0)
               {
                  positionWindowEnd_ = numberOfLines_;
               }
	  
            if (positionWindowEnd_ > static_cast<t_int>(entries_.size()))
               {
                  positionWindowEnd_ = entries_.size();
               }
	  
            // Move the iterator the the start of the window to display.
            for (t_int counter = 0;
                 counter < positionWindowStart_;
                 counter++)
               {
                  _startIter++;
               }
	  
            // Move the iterator the the end of the window to display.
            for (t_int counter = 0;
                 counter < positionWindowEnd_;
                 counter++)
               {
                  _endIter++;
               }
         }

         void fileList::drawList()
         {
            std::vector<fs::path>::const_iterator iterBeg = entries_.begin();
            std::vector<fs::path>::const_iterator iterEnd = entries_.begin();

            fillIterator(iterBeg, iterEnd);

            // Display the window.
            t_int counter = 0;
            std::vector<fs::path>::const_iterator iter;
            for (iter = iterBeg;
                 iter != iterEnd;
                 iter++)
               {
                  fs::path p = *iter;

                  std::string filename = p.leaf();

                  bool isDirectory = false;

                  try
                     {
                        if (fs::is_directory(p))
                           {
                              isDirectory = true;
                           }
                     }
                  catch(...)
                     {
                        isDirectory = false;
                     }

                  if (counter == currentPosition_)
                     {
                        ::wattron(window_, A_REVERSE);

                        std::string spacestr;
                        t_int spaces = width_ - filename.size() - 1 /* extra space is inserted at
                                                                     * the beginning of the line.
                                                                     */;
                        if (spaces > 0)
                           {
                              for (t_int spacecounter = 0;
                                   spacecounter < spaces;
                                   spacecounter++)
                                 {
                                    spacestr += " ";
                                 }
                           }
                        if (isDirectory)
                           {
                              setColor(Colors::C_DIRECTORY);
                              mvwprintw(window_, counter, 0, " %s%s", filename.c_str(), spacestr.c_str());
                              unSetColor(Colors::C_DIRECTORY);
                           }
                        else
                           {
                              setColor(Colors::C_FILE);
                              mvwprintw(window_, counter, 0, " %s%s", filename.c_str(), spacestr.c_str());
                              unSetColor(Colors::C_FILE);
                           }

                        ::wattroff(window_, A_REVERSE);
                     }
                  else
                     {
                        if (isDirectory)
                           {
                              setColor(Colors::C_DIRECTORY);
                              mvwprintw(window_, counter, 0, " %s\n", filename.c_str());
                              unSetColor(Colors::C_DIRECTORY);
                           }
                        else
                           {
                              setColor(Colors::C_FILE);
                              mvwprintw(window_, counter, 0, " %s\n", filename.c_str());
                              unSetColor(Colors::C_FILE);
                           }
                     }

                  counter++;
               }
         }

         void fileList::getDirectory()
         {
            entries_.clear();

            fs::directory_iterator end_iter;

            // Fill the list, not caring if something cannot be
            // accessed.
            fs::path currentpath(currentDirectory_);

            std::vector<boost::filesystem::path> directories;
            std::vector<boost::filesystem::path> files;

            for (fs::directory_iterator iter(currentpath);
                 iter != end_iter;
                 iter++ )
               {
                  try
                     {
                        fs::path p = *iter;

                        std::string filename = p.leaf();
                        if (filename.size() > 0)
                           {
                              bool isdir = fs::is_directory(p);

                              // Do not show entries that start with a dot,
                              // as they are hidden.
                              if (filename[0] == '.')
                                 {
                                    continue;
                                 }

                              // Do not show files that are not
                              // torrent files.
                              if ((!isdir) && (filename.find(".torrent") == std::string::npos))
                                 {
                                    continue;
                                 }

                              if (isdir)
                                 {
                                    directories.push_back(p);
                                 }
                              else
                                 {
                                    files.push_back(p);
                                 }
                           }
                     }
                  catch (const std::exception & ex)
                     {
                     }
               }
            // Sort the result by name.
            std::sort(files.begin(), files.end(), filename_compare);
            std::sort(directories.begin(), directories.end(), filename_compare);

            entries_.push_back(fs::path(".."));
            std::copy(directories.begin(), directories.end(), back_inserter(entries_));
            std::copy(files.begin(),       files.end(),       back_inserter(entries_));
         }

         void fileList::moveDown()
         {
            const t_int list_max = entries_.size();

            if (currentPosition_ < (numberOfLines_-1))
               {
                  currentPosition_++;

                  if (entries_.size() > 0)
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

         void fileList::moveUp()
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

         void fileList::toStart()
         {
            currentPosition_     = 0;
         }

         void fileList::toEnd()
         {
            currentPosition_ = (numberOfLines_-1);

            t_int max_position = entries_.size();
            if (max_position > 0)
               {
                  max_position--;
               }

            if (currentPosition_ > max_position)
               {
                  currentPosition_ = max_position;
               }
         }

         bool fileList::getSelection(fs::path & _p) const
         {
            bool status = false;

            if (entries_.size() == 0)
               {
                  return status;
               }

            t_uint current = positionWindowStart_ + currentPosition_;

            if (current == 0)
               {
                  // Handle ".."
                  fs::path current(currentDirectory_);

                  try
                     {
                        _p = current.branch_path();
                        status = true;
                     }
                  catch (...)
                     {
                        _p     = fs::path(currentDirectory_);
                        status = false;
                     }
               }
            else
               {
                  std::vector<fs::path>::const_iterator iter = entries_.begin();

                  if ((iter + current) != entries_.end())
                     {
                        iter  += current;
                        _p     = *iter;
                        status = true;
                     }
               }

            return status;
         }

         windowSize fileList::calculateDimenstions(windowSize const& _ws) const
         {
            // Use the parents dimensions.
            windowSize ws = _ws;

            return ws;
         }

         dialog::RESULT fileList::showHelp()
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
                                               keyMapping::K_SELECT,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to select a torrent file to load or enter dir");
               }

            if (helpWindow::generateHelpForKey(kmap_,
                                               keyMapping::K_QUITSCREEN,
                                               "",
                                               keyDescr,
                                               false))
               {
                  helpText.push_back(keyDescr);
                  helpText.push_back("  to abort loading");
               }

            helpText.push_back(" ");

            helpText.push_back("Press shift + character to jump to the filename");
            helpText.push_back("or directory beginning with this chracter.");

            helpText.push_back(" ");

            helpWindow hw(kmap_, helpText);

            return hw.run();
         }

         void fileList::moveTo(t_int const _key)
         {
            std::vector<fs::path>::const_iterator iterBeg = entries_.begin();
            std::vector<fs::path>::const_iterator iterEnd = entries_.begin();

            fillIterator(iterBeg, iterEnd);

            // Display the window.
            t_int counter = 0;
            std::vector<fs::path>::const_iterator iter;
            for (iter = iterBeg;
                 iter != iterEnd;
                 iter++)
               {
                  fs::path p = *iter;
	      
                  std::string filename = p.leaf();

                  if (filename.size() >= 1)
                     {
                        t_int c = std::toupper(filename[0]);
                        if (c == _key)
                           {
                              currentPosition_ = counter;
                              refresh();
                              break;
                           }
                     }
                  counter++;
               }
         }

         fileList::~fileList()
         {
            destroy();
         }
         
         bool filename_compare(boost::filesystem::path const& _p1,
                               boost::filesystem::path const& _p2)
         {
            return (_p1.string() < _p2.string());
         }
         
      } // namespace cli
   } // namespace UI
} // namespace btg

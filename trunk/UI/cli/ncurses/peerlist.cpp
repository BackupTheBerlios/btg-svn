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

#include "peerlist.h"

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
         // One entry uses 2 lines.
         const t_uint linesPerFile = 2;

         peerList::peerList(keyMapping const& _kmap,
                            windowSize const& _ws,
			    t_peerList const& _peers)
            : baseWindow(_kmap),
              size_(_ws),
              peers_(_peers),
              numberOfLines_(0),
              positionWindowStart_(0),
              positionWindowEnd_(0),
              currentPosition_(0)
         {
         }

         dialog::RESULT peerList::run()
         {
            if (!init(size_))
               {
                  return dialog::R_NCREAT;
               }

            refresh();

            // Handle keyboard.

            keyMapping::KEYLABEL label;

            bool cont = true;

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

         bool peerList::init(windowSize const& _ws)
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

         void peerList::resize(windowSize const& _ws)
         {

         }

         bool peerList::destroy()
         {
            if (window_ != 0)
               {
                  delwin(window_);
                  window_ = 0;
               }

            return true;
         }

         void peerList::refresh()
         {
            if (window_ == 0)
               {
                  return;
               }

            clear();
            drawList();
            wrefresh(window_);
         }

         void peerList::drawList()
         {
	   t_peerListCI iterBeg = peers_.begin();
	   t_peerListCI iterEnd = peers_.begin();

            if (positionWindowEnd_ == 0)
               {
                  positionWindowEnd_ = numberOfLines_;
               }

            if (positionWindowEnd_ > static_cast<t_int>(peers_.size()))
               {
                  positionWindowEnd_ = peers_.size();
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
	    t_peerListCI iter;
            for (iter = iterBeg;
                 iter != iterEnd;
                 iter++)
               {
		 btg::core::Peer const& peer = *iter;

		 std::string address = peer.address();
		 
		 std::string info;

		 info += "Client: " + peer.identification() + ".";

		 if (peer.seeder())
		   {
		     info += " Seed.";
		   }

		 std::string address_spaces;
		 genSpaces(address, address_spaces);

		 std::string info_spaces;
		 genSpaces(info, info_spaces);

		 if (counter == currentPosition_)
                     {
                        ::wattron(window_, A_REVERSE);

                        setColor(Colors::C_FILE);
                        mvwprintw(window_, scr_counter,   0, " %s%s", address.c_str(), address_spaces.c_str());
                        mvwprintw(window_, scr_counter+1, 0, " %s%s", info.c_str(),    info_spaces.c_str());
			unSetColor(Colors::C_FILE);

                        ::wattroff(window_, A_REVERSE);
                     }
                  else
                     {
                        setColor(Colors::C_FILE);
                        mvwprintw(window_, scr_counter,   0, " %s%s", address.c_str(), address_spaces.c_str());
                        mvwprintw(window_, scr_counter+1, 0, " %s%s", info.c_str(),    info_spaces.c_str());
                        unSetColor(Colors::C_FILE);
                     }

                  counter++;
                  scr_counter += linesPerFile;
               }
         }

         void peerList::moveDown()
         {
            const t_int list_max = peers_.size();

            if (currentPosition_ < (numberOfLines_-1))
               {
                  currentPosition_++;

                  if (peers_.size() > 0)
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

         void peerList::moveUp()
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

         void peerList::toStart()
         {
            currentPosition_     = 0;
         }

         void peerList::toEnd()
         {
            currentPosition_ = (numberOfLines_-1);

            t_int max_position = peers_.size();
            if (max_position > 0)
               {
                  max_position--;
               }

            if (currentPosition_ > max_position)
               {
                  currentPosition_ = max_position;
               }
         }

         windowSize peerList::calculateDimenstions(windowSize const& _ws) const
         {
            // Use the parents dimensions.
            windowSize ws = _ws;

            return ws;
         }

         dialog::RESULT peerList::showHelp()
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
                                               keyMapping::K_QUITSCREEN,
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

         peerList::~peerList()
         {
            destroy();
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

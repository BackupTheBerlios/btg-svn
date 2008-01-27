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

#ifndef SIG_BUTTON_MENU_H
#define SIG_BUTTON_MENU_H

namespace btg
{
   namespace UI
      {
         namespace gui
            {
               /**
                * \addtogroup gui
                */
               /** @{ */

               /// IDs used for acting on tool button and menu items
               /// presses.
               class buttonMenuIds
                  {
                  public:
                     /// ID used to decide which menu option was selected.
                     enum MENUID
                        {
                           BTN_LOAD        = 100,  //!< Open a file.
                           BTN_START,              //!< Start a context.
                           BTN_STOP,               //!< Start a context.
                           BTN_ABORT,              //!< Abort a context.
                           BTN_ERASE,              //!< Abort a context.
                           BTN_CLEAN,              //!< Clean downloaded contexts.
                           BTN_LIMIT,              //!< Limit download/upload of contexts.
                           BTN_MOVE,               //!< Move context to another session.
                           BTN_PREFERENCES,        //!< Show preferences.
                           BTN_DETACH,             //!< Detach from daemon.
                           BTN_ABOUT,              //!< Show an about box.
                           BTN_QUIT,               //!< Quit the application.
                           BTN_KILL,               //!< Kill the daemon.
                           BTN_GLIMIT,             //!< Set globa limits.
                           BTN_UPTIME,             //!< Ask the daemon for uptime.
                           BTN_SESNAME,            //!< Change the name of the current session.

                           BTN_LASTFILE0 = 0,   //!< Last opened file #0.
                           BTN_LASTFILE1 = 1,   //!< Last opened file #1.
                           BTN_LASTFILE2 = 2,   //!< Last opened file #2.
                           BTN_LASTFILE3 = 3,   //!< Last opened file #3.
                           BTN_LASTFILE4 = 4,   //!< Last opened file #4.
                           BTN_LASTFILE5 = 5,   //!< Last opened file #5.
                           BTN_LASTFILE6 = 6,   //!< Last opened file #6.
                           BTN_LASTFILE7 = 7,   //!< Last opened file #7.
                           BTN_LASTFILE8 = 8,   //!< Last opened file #8.
                           BTN_LASTFILE9 = 9,   //!< Last opened file #9.
                           BTN_ALL_LAST  = 10   //!< Open all last files.
                        };
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif

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

#ifndef DAEMON_H
#define DAEMON_H

typedef enum
   {
      PID_OK    =  0, //<! Tell the parent to quit.
      PID_ERR   = -1, //<! Tell the child to continue.
      SETID_ERR = -2, //<! The function setid failed.
      CHDIR_ERR = -3  //<! The function chdir failed.
   } daemonize_status; /// Status as returned by the do_daemonize function.

/// Turn a process into a daemon.
/// Written with the help of the Linux Daemon Writing HOWTO by Devin Watson
/// (http://www.linuxprofilm.com/articles/linux-daemon-howto.html).
daemonize_status do_daemonize(void);

/* React on the signals from the outside world. */
void global_signal_handler(int signal_no);

#endif

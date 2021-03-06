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

#ifndef GTKMM_VER_H
#define GTKMM_VER_H

// Macro that returns true if Gtkmm 2.6 or better is available.
// Forced to use this, as some things like Gtk::CellRendererProgress
// are not available in gtkmm 2.2.

//#define GTKMM_2_6_OR_BETTER ((GTKMM_MAJOR_VERSION == 2) && (GTKMM_MINOR_VERSION >= 6) && (GTKMM_MICRO_VERSION >= 2))

// We are using gtkmm >= 2.6.2 now (checked in configure)
#define GTKMM_2_6_OR_BETTER 1

#endif // GTKMM_VER_H

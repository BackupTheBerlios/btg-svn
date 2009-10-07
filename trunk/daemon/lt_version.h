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

#ifndef LT_VERSION_H
#define LT_VERSION_H

#include <libtorrent/version.hpp>

#define BTG_LT_0_15 ((LIBTORRENT_VERSION_MAJOR == 0) && (LIBTORRENT_VERSION_MINOR == 15))
#define BTG_LT_0_14 ((LIBTORRENT_VERSION_MAJOR == 0) && (LIBTORRENT_VERSION_MINOR == 14))

#if BTG_LT_0_15
#elif BTG_LT_0_14
#else
#  error "Using unknown libtorrent version."
#endif // version check.

#endif // LT_VERSION_H

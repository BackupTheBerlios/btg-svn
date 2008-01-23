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

#ifndef AGAR_IF_H
#define AGAR_IF_H

extern "C"
{
#include <agar/core.h>
#include <agar/gui.h>
#include <agar/core/version.h>
}

#if (AGAR_MAJOR_VERSION == 1 && AGAR_MINOR_VERSION == 2)
#  define AGAR_1_2 1
#elif (AGAR_MAJOR_VERSION >= 1 && AGAR_MINOR_VERSION >= 3)
#  define AGAR_1_3 1
#else
#  error "Unsupported AGAR version."
#endif

#ifndef AGAR_1_2
#  define AGAR_1_2 0
#endif

#ifndef AGAR_1_3
#  define AGAR_1_3 0
#endif

#endif // AGAR_IF_H


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

#ifndef EXTERNALIZATION_MACRO_H
#define EXTERNALIZATION_MACRO_H

            /// Handy macro to check the result of a function and to
            /// call a function to indicate the status of the function
            /// call.
#define BTG_ICHECK(...) { \
         t_int fcall_result = __VA_ARGS__; \
         if (fcall_result <= 0) \
         { \
           failture(); \
           return false; \
         } \
         else \
         { \
            success(); \
         } \
}

#endif // EXTERNALIZATION_MACRO_H

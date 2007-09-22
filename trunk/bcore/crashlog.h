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

#ifndef CRASHLOG_H
#define CRASHLOG_H

namespace btg
{
   namespace core
      {
	/**
	 * \addtogroup core
	 */
	/** @{ */

         /// An attempt to catch unhandled exceptions.
         class crashLog
            {
            public:
               /// Install a handler.
               /// Uses the gcc verbose handler for now.
               static void init();
               /// Print a stack trace.
               static void printStackTrace();
            private:
               /// Implement this to catch uncatched exceptions.
               static void handler();
            };

	 /** @} */

      } // namespace core
} // namespace btg

#endif

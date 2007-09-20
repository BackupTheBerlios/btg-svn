/*
 * btg Copyright (C) 2005 Jesper Nyholm Jensen.
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
 * $Id: limit_base.h,v 1.1.4.4 2006/08/05 23:21:10 wojci Exp $
 */

#ifndef LIMIT_BASE_H
#define LIMIT_BASE_H

namespace btg
{
   namespace core
      {

         class limitBase
            {
            public:
               limitBase();

               enum
                  {
                     LIMIT_DISABLED = -1,   //!< Limit disabled.
                     KiB_to_B       = 1024  //!< KiB in bytes.
                  };

               virtual ~limitBase();
            };

      } // namespace core
} // namespace btg

#endif

/*
 * btg Copyright (C) 2008 Michael Wojciechowski.
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
 * $Id: urlstatus.h 222 2008-03-21 00:55:17Z wojci $
 */

#ifndef FILESTATUS_H
#define FILESTATUS_H

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup core
          */
         /** @{ */

         /// Invalid file id.
         const t_uint FILES_INVALID_FILEID = 0;

         /// Status of a download.
         enum fileStatus
         {
            FILES_UNDEF      = 0, //!< Unknown.
            FILES_WORKING    = 1, //!< Download in progress.
            FILES_FINISHED   = 2, //!< Download finished.
            FILES_ERROR      = 3, //!< Unable to download.
            FILES_CREATE     = 4, //!< Context created.
            FILES_CREATE_ERR = 5, //!< Context not created.
            FILES_ABORTED    = 6  //!< Context aborted.
         };

         /** @} */

      } // namespace core
} // namespace btg

#endif // FILESTATUS_H


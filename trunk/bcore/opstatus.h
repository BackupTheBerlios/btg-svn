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
 * $Id$
 */

#ifndef OPSTATUS_H
#define OPSTATUS_H

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup core
          */
         /** @{ */

         /// Invalid URL id.
         const t_uint OPSTAT_INVALID_ID = 0;

         /// Defined operation status.
         enum statusType
         {
            ST_UNDEF  = 0, //!< Unknown.
            ST_URL    = 1, //!< URLs.
            ST_FILE   = 2  //!< Files.
            // ..
         };

         /// Status of an url or file upload to the daemon.
         enum opStatus
         {
            OP_UNDEF      = 0, //!< Unknown.
            OP_WORKING    = 1, //!< Upload in progress.
            OP_FINISHED   = 2, //!< Upload finished.
            OP_ERROR      = 3, //!< Unable to upload.
            OP_CREATE     = 4, //!< Context created.
            OP_CREATE_ERR = 5, //!< Context not created.
            OP_ABORTED    = 6  //!< Context aborted (user cancelled).
         };

         /** @} */

      } // namespace core
} // namespace btg

#endif // OPSTATUS_H


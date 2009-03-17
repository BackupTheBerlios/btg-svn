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

#ifndef CPIF_H
#define CPIF_H

#include <bcore/type.h>
#include <string>

namespace btg
{
   namespace core
   {
      namespace client
      {
         /**
          * \addtogroup gencli
          */
         /** @{ */

         /// Interface used to report status of a file upload or URL
         /// download to an application.
         class createProgressIf
         {
         public:
            /// Constructor.
            createProgressIf();

            /// Init.
            /// @param [in] _url True - url download, false - file upload.
            virtual void CPIF_init(bool _url);

            /// Initialize, file upload.
            virtual void CPIF_begin(std::string const& _filename) = 0;

            /// Initialize, URL download.
            virtual void CPIF_begin(std::string const& _filename, 
                                    std::string const& _url) = 0;

            /// File upload in progress. Piece a of b total uploaded.
            virtual void CPIF_filePiece(t_uint _number, t_uint _parts) = 0;

            /// URL download in progress.
            virtual void CPIF_urlDlStatus(t_uint _total, t_uint _now, t_uint _speed) = 0;

            /// Error.
            virtual void CPIF_error(std::string const& _error) = 0;

            /// Waiting for the daemon to create the context.
            virtual void CPIF_wait(std::string const& _msg) = 0;

            /// File uploaded and created.
            virtual void CPIF_success(std::string const& _filename) = 0;

            /// Reset the cancel flag.
            virtual void CPIF_reset();

            /// Cancel the current download.
            virtual void CPIF_cancel();

            /// Indicate if the current download should be cancelled.
            virtual bool CPIF_continue() const;

            /// Indicates if an URL is being downloaded.
            bool CPIF_url() const;

            /// Indicates if a file is being uploaded.
            bool CPIF_file() const;

            /// Destructor.
            virtual ~createProgressIf();
         protected:
            /// Indicates that an URL is being downloaded.
            bool url_;
            /// Flag used to indicate if the download should be
            /// cancelled.
            bool continue_;
         };

         /** @} */

      } // client
   } // core
} // btg

#endif // CPIF_H


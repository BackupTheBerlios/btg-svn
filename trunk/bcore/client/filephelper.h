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

#ifndef FILEPHELPER_H
#define FILEPHELPER_H

#include <bcore/type.h>
#include <bcore/logable.h>

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

         /// Interface used to report status of a file upload to an
         /// application.
         class createPartsReportInterface
         {
         public:
            /// Constructor.
            createPartsReportInterface();

            /// Initialize.
            virtual void CPRI_init(std::string const& _filename) = 0;

            /// Piece a of b total uploaded.
            virtual void CPRI_pieceUploaded(t_uint _number, t_uint _parts) = 0;

            /// Error.
            virtual void CPRI_error(std::string const& _error) = 0;

            /// Waiting for the daemon to create the context.
            virtual void CPRI_wait(std::string const& _msg) = 0;

            /// File uploaded and created.
            virtual void CPRI_success(std::string const& _filename) = 0;

            /// Reset the cancel flag.
            virtual void CPRI_reset();

            /// Cancel the current download.
            virtual void CPRI_cancel();

            /// Indicate if the current download should be cancelled.
            virtual bool CPRI_continue() const;

            /// Destructor.
            virtual ~createPartsReportInterface();
         protected:
            /// Flag used to indicate if the download should be
            /// cancelled.
            bool continue_;
         };

         /// Create a context, sending parts of it one at a time.
         /// @param [in] _logwrapper Log wrapper object.
         /// @param [in] _ch         Pointer to client handler instance. 
         /// @param [in] _cpri       Callback interface.
         /// @param [in] _filename   The filename to read data from.
         /// @param [in] _partSize   The size of the chunks the upload consists of.
         /// @return True - torrent uploaded and created, false - otherwise.
         bool createParts(btg::core::LogWrapperType _logwrapper,
                          class clientHandler* _ch,
                          createPartsReportInterface* _cpri,
                          std::string const& _filename,
                          t_uint const _partSize = (64*1024));

         /** @} */

      } // client
   } // core
} // btg

#endif // FILEPHELPER_H


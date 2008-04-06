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

         class createPartsReportInterface
         {
         public:
            createPartsReportInterface();

            virtual void CPRI_init(std::string const& _filename) = 0;
            virtual void CPRI_pieceUploaded(t_uint _number, t_uint _parts) = 0;
            virtual void CPRI_error(std::string const& _error) = 0;
            virtual void CPRI_success(std::string const& _filename) = 0;

            virtual bool CPRI_cancel();
            virtual bool CPRI_continue() const;

            virtual ~createPartsReportInterface();
         protected:
            bool continue_;
         };

         /// Create a context, sending parts of it.
         bool createParts(btg::core::LogWrapperType _logwrapper,
                          class clientHandler* _ch,
                          createPartsReportInterface* _cpri,
                          std::string const& _filename,
                          t_uint const _partSize = (5*1024));

         /** @} */

      } // client
   } // core
} // btg

#endif // FILEPHELPER_H


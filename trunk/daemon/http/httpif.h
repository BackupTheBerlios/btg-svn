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

#ifndef HTTPIF_H
#define HTTPIF_H

#include <string>
#include <bcore/sbuf.h>

namespace btg
{
   namespace daemon
   {
      namespace http
      {
         /**
          * \addtogroup http
          */
         /** @{ */

         /// Http interface.
         class httpInterface
         {
         public:
            /// Status.
            enum Status
            {
               ERROR  = 0, // Error.
               INIT   = 1, // Initalize.
               WAIT   = 2, // Wait, still loading.
               FINISH = 3, // Done loading.
            };

         public:
            /// Constructor.
            httpInterface();

            /// Start fetching an URL.
            /// @return Id used to query result.
            virtual t_uint Fetch(std::string const& _url, 
                                 std::string const& _filename) = 0;

            /// Get status.
            virtual httpInterface::Status getStatus(const t_uint _id) = 0;

            /// Get the result of a fetch.
            virtual bool Result(const t_uint _id, 
                                btg::core::sBuffer & _buffer) = 0;

            /// Terminate a result of a search.
            virtual void Terminate(const t_uint _id) = 0;

            /// Destructor.
            virtual ~httpInterface();
         protected:
            /// Status of the URL download.
            httpInterface::Status status;
         };

         /** @} */

      } // namespace http
   } // namespace daemon
} // namespace btg

#endif // HTTPIF_H


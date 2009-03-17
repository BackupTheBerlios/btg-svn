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

#ifndef OPID_H
#define OPID_H

#include <bcore/type.h>
#include <bcore/logable.h>

namespace btg
{
   namespace daemon
      {
         /// Generate a operation ID. This is used each time a file or
         /// url is uploaded to the daemon to track the status of the
         /// file or url by the clients.
         class opId: public btg::core::Logable
            {
            public:
               /// Constructor.
               opId(btg::core::LogWrapperType _logwrapper);

               /// Get a new id.
               t_uint id();

               /// Destructor.
               ~opId();
            protected:
               /// Current id.
               t_uint current_id;
               /// Max id.
               const t_uint max_id;
            };

      } // namespace daemon
} // namespace btg

#endif // OPID_H


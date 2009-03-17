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

#ifndef EMULE_H
#define EMULE_H

#include "ipfilterif.h"

namespace btg
{
   namespace daemon
      {

         /**
          * \addtogroup daemon Daemon
          */
         /** @{ */

         class Emule: public IpFilterIf
            {
            public:
               /// Constructor.
               Emule(btg::core::LogWrapperType _logwrapper,
                     bool const _verboseFlag, std::string const& _filename);

               void set(libtorrent::session & _session) const;

               std::string getName() const;

               /// Destructor.
               virtual ~Emule();
            };

         /** @} */

      } // namespace daemon
} // namespace btg

#endif // LEVELONE_H


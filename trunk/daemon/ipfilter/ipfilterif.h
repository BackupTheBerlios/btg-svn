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
 * $Id: ipfilterif.h,v 1.1.2.9 2007/07/22 11:53:17 wojci Exp $
 */

#ifndef IPFILTERIF_H
#define IPFILTERIF_H

#include <bcore/type.h>
#include <libtorrent/session.hpp>

#include <daemon/lt_version.h>
#include <libtorrent/ip_filter.hpp>

#include <string>

namespace btg
{
   namespace daemon
      {

         /**
          * \addtogroup daemon Daemon
          */
         /** @{ */

         /// IPv4 filter interface.
         class IpFilterIf
            {
            public:

               /// Type of filter.
               enum TYPE
                  {
                     IPF_UNDEF = 0, //!< Undefined filter.
                     IPF_LEVELONE,  //!< Level one.
                     IPF_EMULE      //!< Emule.
                  };

               /// Constructor.
               IpFilterIf(bool const _verboseFlag, 
                          TYPE const _type, 
                          std::string const& _filename);

               /// Return true if the file was opened and read.
               virtual bool initialized() const;

               /// Get the number of entries read from file.
               virtual t_uint numberOfEntries() const;

               /// Make libtorrent use the loaded list.
               virtual void set(libtorrent::session & _session) const = 0;

               /// Get the name of this filter.
               virtual std::string getName() const = 0;

               /// Destructor.
               virtual ~IpFilterIf();

            protected:
               /// Indicates if verbose logging should be performed.
               bool                  verboseFlag_;
               /// Type of filter.
               TYPE                  type_;
               /// Filename used to read the filter from.
               std::string           filename_;
               /// True, if an instance is initialized.
               bool                  initialized_;
               /// The total number of rules.
               t_uint                numberOfentries_;
               /// Libtorrent filter object.
               libtorrent::ip_filter filter_;
            };

         /** @} */

      } // namespace daemon
} // namespace btg

#endif // IPFILTERIF_H


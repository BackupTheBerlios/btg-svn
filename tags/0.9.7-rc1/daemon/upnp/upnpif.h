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

#ifndef UPNPIF_H
#define UPNPIF_H

#include <bcore/type.h>
#include <utility>

namespace btg
{
   namespace daemon
      {
         namespace upnp
            {
               /**
                * \addtogroup upnp
                */
               /** @{ */

               /// Interface: UPNP.
               class upnpIf
                  {
                  public:
                     /// Constructor.
                     /// @param [in] verboseFlag_ Indicates if this interface should do verbose logging.
                     upnpIf(bool const verboseFlag_);

                     /// Return true if the interface is initialized.
                     bool initialized() const;

                     /// Open a port range.
                     /// @param [in] _range Reference to a range to open.
                     /// @return True - success. False - failture.
                     virtual bool open(std::pair<t_int, t_int> const& _range) = 0;

                     /// Close an already opened port range.
                     virtual bool close() = 0;

                     /// Destructor.
                     virtual ~upnpIf();

                  protected:
                     /// Indicates if this interface should do verbose
                     /// logging.
                     bool const verboseFlag_;
                     /// Indicates if this interface is initialized.
                     bool initialized_;
                  };

               /** @} */

            } // namespace upnp
      } // namespace daemon
} // namespace btg

#endif // UPNPIF_H


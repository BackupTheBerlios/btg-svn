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
 * $Id: serializable.h,v 1.1.2.5 2007/01/16 20:26:36 wojci Exp $
 */

#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <bcore/type.h>
#include <bcore/dbuf.h>
#include <bcore/externalization/externalization.h>

namespace btg
{
   namespace core
      {
	/**
	 * \addtogroup core
	 */
	/** @{ */

         /// Interface: classes inheriting this class can be
         /// serialized/deserialized.
         class Serializable
            {
            public:
               /// Constructor.
               Serializable() {};
               /// Serialize.
               /// @param [out] _e Pointer to the externalization which is used.
               /// \return True - success. False - failture.
               virtual bool serialize(btg::core::externalization::Externalization* _e) const = 0;
               /// Deserialize.
               /// @param [in] _e Pointer to the externalization which is used.
               /// \return True - success. False - failture.
               virtual bool deserialize(btg::core::externalization::Externalization* _e) = 0;
               /// Destructor.
               virtual ~Serializable() {};
            };

	 /** @} */

      } // namespace core
} // namespace btg

#endif // SERIALIZABLE_H

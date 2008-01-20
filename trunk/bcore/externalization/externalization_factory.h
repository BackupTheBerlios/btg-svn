/*
 * btg Copyright (C) 2007 Roman Rybalko.
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

#ifndef EXTERNALIZATION_FACTORY_H
#define EXTERNALIZATION_FACTORY_H

#include "externalization.h"
#include <bcore/logger/logger.h>

namespace btg
{
   namespace core
      {
         namespace externalization
         {
            /**
             * \addtogroup externalization
             */
            /** @{ */

            /// Externalization types.
            enum eExternalizationType
            {
               etXml    = 0, //< For XMLRPC serializator.
               etMax         //< Max.
            };
            
            /// Serializator class factory.
            class Factory
            {
            public:
               /// Creates serializator instance.
               /// @param [in] _logwrapper Pointer used to send logs to.
               /// @param [in] _et         Serializator type.
               /// @return Pointer to externalization implementation or 0.
               static Externalization* createExternalization(LogWrapperType _logwrapper,
                                                             eExternalizationType _et = etXml);

               /// Destroyes serializator instance.
               /// @param [in] _ext Pointer to instance.
               static void destroyExternalization(Externalization* & _ext);
            };

            /** @} */

         } // namespace externalization
      } // namespace core
} // namespace btg

#endif

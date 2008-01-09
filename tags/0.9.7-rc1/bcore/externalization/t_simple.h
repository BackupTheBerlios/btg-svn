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

#ifndef T_SIMPLE_H
#define T_SIMPLE_H

#include <iostream>
#include <sstream>
#include <string>

#include <assert.h>

#include <bcore/type.h>
#include <bcore/dbuf.h>

/**
 * \addtogroup externalization
 */
/** @{ */

namespace btg
{
   namespace core
      {
         namespace externalization
         {

            /// Templates used by the Util class to serialize/deserialize data.

            /// Convert a single type as int, long, float a byte array.
            /// Parameters must be pointers, since memcpy is used.
            template<typename sourceType, typename destinationType>
               t_int genericSDTSerialize(sourceType* _source, destinationType & _destination)
            {
               return _destination.addBytes(reinterpret_cast<t_byteCP>(_source), sizeof(sourceType));
            }

            /// Convert a simple type represented as a byte array.
            /// Parameters must be pointers, since memcpy is used.
            template<typename sourceType, typename destinationType>
               t_int genericSDTDeserialize(sourceType & _source, destinationType *_destination)
            {
               t_int const size = sizeof(destinationType);
               return _source.getNBytes(size, reinterpret_cast<t_byteP>(_destination), size);
            }

            /// Convert a container to a byte array.
            /// Sourcetype needs to be a pointer to a container.
            /// destinationType needs to be a pointer to something memcpy can write to.
            template<typename sourceType, typename souceIteratorType, typename containedType, typename destinationType>
               t_int genericContainerSerialize(sourceType *_source, destinationType & _destination)
            {
               t_int size           = 0;
               t_int count          = 0;

               // First write the size of the container.
               t_int container_size = _source->size();
               count                = genericSDTSerialize<t_int>(&container_size, _destination);
               size                += count;

               // Then write the containers elements.
               souceIteratorType iter;
               containedType value;
               for (iter=_source->begin(); iter!=_source->end(); iter++)
                  {
                     value = *iter;
                     count = genericSDTSerialize<containedType, destinationType>(&value, _destination);
                     size += count;
                  }

               return size;
            }

            /// Convert a byte array to a container.
            /// Sourcetype needs to be a pointer to a container.
            /// destinationType needs to be a pointer.
            template<typename sourceType, typename containedType, typename destinationType>
            t_int genericContainerDeserialize(sourceType & _source, destinationType *_destination)
            {
               t_int size           = 0;
               t_int count          = 0;
               t_int container_size = 0;
               // Get the number of serialized container elements.
               count              = genericSDTDeserialize<sourceType, t_int>(_source, &container_size);
               size              += count;

               // Convert each element.
               if (container_size > 0)
                  {
                     // Remove the clearing from here, as string::clear is not supported by older g++.
                     containedType value;
                     for (t_int i=0; i<container_size; i++)
                        {
                           count = genericSDTDeserialize<sourceType, containedType>(_source, &value);
                           size += count;
                           _destination->push_back(value);
                        }

                  } // if

               assert(static_cast<t_int>(_destination->size()) == container_size);

               return size;
            }

         } // namespace externalization
      } // namespace core
} // namespace btg

/** @} */

#endif

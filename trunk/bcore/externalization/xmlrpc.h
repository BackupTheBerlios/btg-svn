/*
 * btg Copyright (C) 2005 Michael Wojciechowski.
 * HTTP/XMLRPC part written by Johan Ström.
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

#ifndef XMLRPC_H
#define XMLRPC_H

#include <string>
#include <bcore/dbuf.h>
#include "externalization.h"
extern "C"
{
#include <external/xmlrpc-epi/src/xmlrpc.h>
}

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

            /// XML-RPC externalization.
            class XMLRPC: public Externalization
            {
            public:
               /// Default constructor.
               XMLRPC(LogWrapperType _logwrapper);

               /// Destructor.
               virtual ~XMLRPC();

               bool setBuffer(dBuffer & _buffer);
               t_int getBufferSize();
               void getBuffer(dBuffer & _buffer);
               void reset();
               t_byte determineCommandType(t_int & _command);
               void setDirection(btg::core::DIRECTION _dir);
               t_byte getCommand(t_int & _command);
               t_byte setCommand(t_int const _command);

               bool boolToBytes(bool const _source);
               bool bytesToBool(bool & _destination);
               bool addByte(t_byte const _byte);
               bool getByte(t_byte & _byte);
               bool addBytes(t_byteCP _bytes, t_uint const _size);
               bool getBytes(t_byteP* _bytes, t_uint &_size);
               bool intToBytes(const t_int* _source);
               bool bytesToInt(t_int* _destination);
               bool uintToBytes(const t_uint* _source);
               bool bytesToUint(t_uint* _destination);
               bool longToBytes(const t_long* _source);
               bool uLongToBytes(const t_ulong* _source);
               bool bytesToLong(t_long* _destination);
               bool bytesToULong(t_ulong* _destination);
               bool floatToBytes(const t_float* _source);
               bool bytesToFloat(t_float* _destination);
               bool intListToBytes(const t_intList* _source);
               bool bytesToIntList(t_intList* _destination);
               bool longListToBytes(const t_longList* _source);
               bool bytesToLongList(t_longList* _destination);
               bool stringListToBytes(const t_strList* _source);
               bool bytesToStringList(t_strList* _destination);
               bool stringToBytes(const std::string* _source);
               bool bytesToString(std::string* _destination);

               std::string dumpData() const;
            private:
               /// XMLRPC request used.
               XMLRPC_REQUEST xmlrpc_request;

               /// Fetch the next value from the request.
               XMLRPC_VALUE getNextValue();

               /// Set this to 1 to rewind the xmlrpc_request's
               /// internal value pointer. This should be set to true
               /// whenever the xmlrpc_request is loaded with new data
               /// (ie FromXML or new)
               bool doRewind;

               /// Convert a single type (such as int, long) to an XMLRPC_VALUE.
               template<typename sourceType>
                  t_int genericSDTSerialize(sourceType* _source);

               /// Convert a simple type represented as a XMLRPC_VALUE back to its type.
               template<typename destinationType>
                  t_int genericSDTDeserialize(destinationType* _destination);

               /// Convert a container, using genericSDTSerialize, to a list of XMLRPC_VALUEs.
               template<typename sourceType, typename souceIteratorType, typename containedType>
                  t_int genericContainerSerialize(sourceType* _source);

               /// Convert a list of XMLRPC_VALUEs back to a container.
               template<typename containedType, typename destinationType>
                  t_int genericContainerDeserialize(destinationType* _destination);
            private:
               /// Copy constructor.
               XMLRPC(XMLRPC const& _xmlrpc);
               /// Assignment operator.
               XMLRPC& operator=(XMLRPC const& _xmlrpc);
            };

            /** @} */

         } // namespace externalization
      } // namespace core
} // namespace btg

#endif

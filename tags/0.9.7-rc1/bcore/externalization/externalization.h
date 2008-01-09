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

#ifndef EXTERNALIZATION_H
#define EXTERNALIZATION_H

#include <bcore/type.h>
#include <bcore/dbuf.h>
#include <bcore/project.h>
#include <bcore/transport/direction.h>

#include <string>

#include <bcore/logable.h>

namespace btg
{
   namespace core
      {
         namespace externalization
         {

            /** @} */

            /**
             * \addtogroup externalization
             */
            /** @{ */

            /// The externalization interface used. Before
            /// sending/receiving using a channel, commands are
            /// serialized/deserialized.

            /// \note Each command that serialzes/deserializes should
            /// call success or fauilture to indicate the status of
            /// the operation.

            class Externalization: public btg::core::Logable
            {
            public:
               /// Default constructor.
               Externalization(LogWrapperType _logwrapper);

               /// Destructor.
               virtual ~Externalization();

               /// Get a string representation of a command using its ID.
               std::string getCommandName(t_int const _type);

               /// Given a string representation of a command's name, get its ID.
               t_int getCommandID(std::string const _methodName);

               /// Set the bytes of the internal buffer.
               /// @param [in] _buffer Buffer to use.
               /// The return value is useful for implementations
               /// which decode input in this function.
               /// @return True on success, false on failture.
               virtual bool setBuffer(dBuffer & _buffer) = 0;

               /// Get the size of the internal buffer.
               virtual t_int getBufferSize() = 0;

               /// Get the bytes contained in the internal buffer and clear the buffer.
               virtual void getBuffer(dBuffer & _buffer) = 0;

               /// Clear the contained buffer.
               virtual void reset() = 0;

               /// Return a byte which can be used to establish which
               /// btg command will be deserialized next.
               virtual t_byte determineCommandType(t_int & _command) = 0;

               /// Sets the direction of the message
               virtual void setDirection(btg::core::DIRECTION _dir) = 0;

               /// Sets the name and requirements of the parameter that is going to be
               /// serialized / deserialized. Used for generating
               /// interface documentation.
               /// If a parameter is optional (ie. there is an preceeding bool argument telling if its set or not)
               /// then _required should be set to false.
               virtual void setParamInfo(std::string const& _name, bool _required);

               /// Sets the name of the command to be serialized
               virtual t_byte setCommand(t_int const _command) = 0;

               /// Get the command id of the current command.
               virtual t_byte getCommand(t_int & _command) = 0;

               /// Convert an boolean to unsigned char and write it to the buffer.
               /// @param [in]  _source       Value.
               /// @return                    Allways 1.
               virtual bool boolToBytes(bool const _source) = 0;

               /// Convert a byte to boolean.
               /// @param [out] _destination Reference to the destination.
               /// @return                   Allways 1.
               virtual bool bytesToBool(bool & _destination) = 0;

               /// Add a byte.
               virtual bool addByte(t_byte const _byte) = 0;

               /// Get a byte.
               virtual bool getByte(t_byte & _byte) = 0;

               /// Add multiple bytes.
               /// @param [in] _bytes Pointer to the bytes
               /// @param [in] _size The number of bytes to add
               virtual bool addBytes(t_byteCP _bytes, t_uint const _size) = 0;

               /// Get multiple bytes.
               /// @param [out] _bytes Pointer to a pointer to memory allocated for the bytes.
               /// @param [out] _size The number of bytes added
               /// Caller is responsible for delete[]'ing these bytes!
               virtual bool getBytes(t_byteP* _bytes, t_uint &_size) = 0;

               /// Convert an int to unsigned char and write it to the buffer.
               /// @return The number of bytes written.
               /// @param [in]  _source       Value.
               virtual bool intToBytes(const t_int* _source) = 0;

               /// Convert a portion of a byte array to an int.
               /// @param [out] _destination Pointer to the value.
               /// @return                   The number of bytes read from the buffer.
               virtual bool bytesToInt(t_int *_destination) = 0;

               /// Convert an unsigned integer to unsigned char and write it to the buffer.
               /// @return The number of bytes written.
               /// @param [in]  _source       Value.
               virtual bool uintToBytes(const t_uint* _source) = 0;

               /// Convert a portion of a byte array to an unsigned integer.
               /// @param [out] _destination Pointer to the value.
               /// @return                   The number of bytes read from the buffer.
               virtual bool bytesToUint(t_uint *_destination) = 0;

               /// Convert a long to unsigned char and write it to the buffer.
               /// @return The number of bytes written.
               /// @param [in]  _source       Value.
               virtual bool longToBytes(const t_long* _source) = 0;

               /// Convert a unsigned long to unsigned char and write it to the buffer.
               /// @return The number of bytes written.
               /// @param [in]  _source       Value.
               virtual bool uLongToBytes(const t_ulong* _source) = 0;

               /// Convert a portion of a byte array to a long.
               /// @param [out] _destination Pointer to the value.
               /// @return                   The number of bytes read from the buffer.
               virtual bool bytesToLong(t_long *_destination) = 0;

               /// Convert a portion of a byte array to a unsigned long.
               /// @param [out] _destination Pointer to the value.
               /// @return                   The number of bytes read from the buffer.
               virtual bool bytesToULong(t_ulong *_destination) = 0;

               /// Convert a float to unsigned char and write it to the buffer.
               /// @return The number of bytes written.
               /// @param [in]  _source       Value.
               virtual bool floatToBytes(const t_float* _source) = 0;

               /// Convert a portion of a byte array to a float.
               /// @param [out] _destination Pointer to the value.
               /// @return                   The number of bytes read from the buffer.
               virtual bool bytesToFloat(t_float *_destination) = 0;

               /// Convert a list of ints to a byte array. The list is prefixed with its length.
               /// @param [in]  _source       The list.
               /// @return                    The number of bytes written.
               virtual bool intListToBytes(const t_intList* _source) = 0;

               /// Convert a byte array that represents a list of ints to a list.
               /// @param [out] _destination Pointer to the list.
               /// @returns                  The number of bytes read.
               virtual bool bytesToIntList(t_intList *_destination) = 0;

               /// Convert a list of longs to a byte array. The list is prefixed with its length.
               /// @param [in]  _source       The list.
               /// @return                    The number of bytes written.
               virtual bool longListToBytes(const t_longList* _source) = 0;

               /// Convert a byte array that represents a list of longs to a list.
               /// @param [out] _destination Pointer to the list.
               /// @returns                  The number of bytes read.
               virtual bool bytesToLongList(t_longList* _destination) = 0;

               /// Convert a list of strings to a byte array. The list is prefixed with its length.
               /// @param [in]  _source       The list.
               /// @return                    The number of bytes written.
               virtual bool stringListToBytes(const t_strList* _source) = 0;

               /// Convert a byte array that represents a list of strings to a list.
               /// @param [out] _destination Pointer to the list.
               /// @returns                  The number of bytes read.
               virtual bool bytesToStringList(t_strList *_destination) = 0;

               /// Convert a string to a byte array. The byte representation is
               /// prefixed with the length of the string.
               /// @param [in]  _source       The list.
               /// @return                    The number of bytes written.
               virtual bool stringToBytes(const std::string* _source) = 0;

               /// Convert a byte array which represents a string into a string.
               /// @param [out] _destination Pointer to the list.
               /// @return                   The number of bytes read.
               virtual bool bytesToString(std::string *_destination) = 0;

               /// The status of the last command executed.
               bool status() const;

               /// Dump the contained data. For internal use only.
               virtual std::string dumpData() const = 0;
            protected:

               /// The last command failed.
               void failture();
               /// The last command succeded.
               void success();

               /// Status of the last command.
               bool status_;
            };

            /** @} */

         } // namespace externalization
      } // namespace core
} // namespace btg

#endif

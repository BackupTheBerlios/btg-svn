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

#include "simple.h"
#include "t_simple.h"
#include "ext_macro.h"

#include <bcore/helpermacro.h>
#include <bcore/logmacro.h>

namespace btg
{
   namespace core
   {
      namespace externalization
      {

         Simple::Simple(LogWrapperType _logwrapper)
            : Externalization(_logwrapper),
              buffer_()
         {
         }
         Simple::~Simple() {}

         bool Simple::setBuffer(dBuffer & _buffer)
         {
            buffer_ = _buffer;

            return true;
         }

         t_int Simple::getBufferSize()
         {
            return buffer_.size();
         }

         void Simple::getBuffer(dBuffer & _buffer)
         {
            _buffer = buffer_;

            reset();
         }

         t_byte Simple::determineCommandType(t_int & _command)
         {
            _command = buffer_.peek();
            return 1;
         }

         t_byte Simple::setCommand(t_int const _command)
         {
            intToBytes(&_command);
            return 1;
         }

         void Simple::setDirection(btg::core::DIRECTION _dir)
         {
         }

         t_byte Simple::getCommand(t_int & _command)
         {
            bytesToInt(&_command);
            return 1;
         }

         void Simple::reset()
         {
            buffer_.erase();
            // TODO: is this needed?
            buffer_.resetRWCounters();
         }

         bool Simple::boolToBytes(bool const _source)
         {
            if (_source == true)
               {
                  buffer_.addByte(0x1);
               }
            else
               {
                  buffer_.addByte(0x0);
               }
            return true;
         }

         bool Simple::bytesToBool(bool & _destination)
         {
            t_byte c     = 0;
            t_int result = buffer_.getByte(&c);

            if (result == -1)
               {
                  return 0;
               }
            else
               {
                  switch (c)
                     {
                     case 0x0:
                        _destination = false;
                        break;
                     case 0x1:
                        _destination = true;
                        break;
                     default:
                        return false;
                     }
               }

            return true;
         }

         bool Simple::addByte(t_byte const _byte)
         {
            BTG_ICHECK( buffer_.addByte(_byte) );

            return true;
         }

         bool Simple::getByte(t_byte & _byte)
         {
            BTG_ICHECK( buffer_.getByte(&_byte) );

            return true;
         }

         bool Simple::addBytes(t_byteCP _bytes, t_uint const _size)
         {
            BTG_ICHECK( uintToBytes(&_size) );

            for(t_uint i=0; i < _size; i++)
               {
                  BTG_ICHECK(addByte(_bytes[i]));
               }

            return true;
         }

         bool Simple::getBytes(t_byteP* _bytes, t_uint &_size)
         {
            *_bytes = 0;
            _size   = 0;

            BTG_ICHECK( bytesToUint(&_size) );
            if(_size == 0)
               return true;

            try
               {
                  *_bytes = new t_byte[_size];
               }
            catch(...)
               {
                  BTG_ERROR_LOG(logWrapper(), "Simple::getBytes(), Failed to allocate " << _size << " bytes of memory.");
                  return false;
               }

            BTG_ICHECK( buffer_.getNBytes(_size, *_bytes, _size) );
            return true;
         }

         bool Simple::intToBytes(const t_int* _source)
         {
            BTG_ICHECK( genericSDTSerialize<t_int const, dBuffer>(_source, buffer_) );

            return true;
         }

         bool Simple::bytesToInt(t_int *_destination)
         {
            BTG_ICHECK( genericSDTDeserialize<dBuffer, t_int>(buffer_, _destination) );

            return true;
         }

         bool Simple::uintToBytes(const t_uint* _source)
         {
            BTG_ICHECK( genericSDTSerialize<t_uint const, dBuffer>(_source, buffer_) );

            return true;
         }

         bool Simple::bytesToUint(t_uint *_destination)
         {
            BTG_ICHECK( genericSDTDeserialize<dBuffer, t_uint>(buffer_, _destination) );

            return true;
         }

         bool Simple::longToBytes(const t_long* _source)
         {
            BTG_ICHECK( genericSDTSerialize<t_long const, dBuffer>(_source, buffer_) );

            return true;
         }

         bool Simple::uLongToBytes(const t_ulong* _source)
         {
            BTG_ICHECK( genericSDTSerialize<t_ulong const, dBuffer>(_source, buffer_) );

            return true;
         }

         bool Simple::bytesToLong(t_long *_destination)
         {
            BTG_ICHECK( genericSDTDeserialize<dBuffer, t_long>(buffer_, _destination) );

            return true;
         }

         bool Simple::bytesToULong(t_ulong *_destination)
         {
            BTG_ICHECK( genericSDTDeserialize<dBuffer, t_ulong>(buffer_, _destination) );

            return true;
         }

         bool Simple::floatToBytes(const t_float* _source)
         {
            BTG_ICHECK( genericSDTSerialize<t_float const, dBuffer>(_source, buffer_) );

            return true;
         }

         bool Simple::bytesToFloat(t_float *_destination)
         {
            BTG_ICHECK( genericSDTDeserialize<dBuffer, t_float>(buffer_, _destination) );

            return true;
         }


         bool Simple::stringToBytes(const std::string* _source)
         {
            BTG_ICHECK( genericContainerSerialize<std::string const, std::string::const_iterator, char, dBuffer>(_source, buffer_) );

            return true;
         }

         bool Simple::bytesToString(std::string *_destination)
         {
            // Clear the destination before adding any chars.
#if HAVE_STRING_CLEAR == 1
            _destination->clear();
#else
            _destination->erase(_destination->begin(), _destination->end());
#endif
            BTG_ICHECK( genericContainerDeserialize<dBuffer, char, std::string>(buffer_, _destination) );


            return true;
         }

         bool Simple::intListToBytes(const t_intList* _source)
         {
            BTG_ICHECK( genericContainerSerialize<t_intList const, t_intList::const_iterator, t_int, dBuffer>(_source, buffer_) );

            return true;
         }

         bool Simple::bytesToIntList(t_intList *_destination)
         {
            _destination->clear();
            BTG_ICHECK( genericContainerDeserialize<dBuffer, t_int, t_intList >(buffer_, _destination) );

            return true;
         }

         bool Simple::longListToBytes(const t_longList* _source)
         {
            BTG_ICHECK( genericContainerSerialize<t_longList const, t_longList::const_iterator, t_long, dBuffer>(_source, buffer_) );

            return true;
         }

         bool Simple::bytesToLongList(t_longList* _destination)
         {
            _destination->clear();
            BTG_ICHECK( genericContainerDeserialize<dBuffer, t_long, t_longList >(buffer_, _destination) );

            return true;
         }

         bool Simple::stringListToBytes(const t_strList* _source)
         {
            t_int const list_size = _source->size();

            // Write the length of the list:
            BTG_ICHECK( intToBytes(&list_size) );

            // Write the characters in the string to the byte array:
            t_strList::const_iterator ci;
            std::string ii("");
            for (ci = _source->begin(); ci != _source->end(); ci++)
               {
                  ii          = "";
                  ii          = *ci;
                  BTG_ICHECK( stringToBytes(&ii) );
               }


            return true;
         }

         bool Simple::bytesToStringList(t_strList *_destination)
         {
            t_int list_size          = 0;

            _destination->clear();

            // First, get the size of the list:
            BTG_ICHECK( bytesToInt(&list_size) );

            for (t_int i=0; i<list_size; i++)
               {
                  std::string element("");
                  BTG_ICHECK(bytesToString(&element));
                  _destination->push_back(element);
               }

            return true;
         }

         std::string Simple::dumpData() const
         {
            return buffer_.toString();
         }

      } // namespace externalization
   } // namespace core
} // namespace btg

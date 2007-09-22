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

#include "xmlrpc.h"

#include <bcore/helpermacro.h>
#include <sstream>
#include <bcore/util.h>
#include "ext_macro.h"
#include <bcore/logmacro.h>

extern "C"
{
#include <assert.h>
}

namespace btg
{
   namespace core
   {
      namespace externalization
      {

         XMLRPC::XMLRPC()
            : xmlrpc_request(0),
              doRewind(false)
         {
            reset();
         }

         XMLRPC::~XMLRPC()
         {
            XMLRPC_RequestFree(xmlrpc_request, 1);
         }

         void XMLRPC::reset()
         {
#if BTG_EXTERNALIZATION_DEBUG
            BTG_NOTICE("XMLRPC::reset");
#endif
            XMLRPC_RequestFree(xmlrpc_request, 1);
            xmlrpc_request = XMLRPC_RequestNew();
            XMLRPC_RequestSetData(xmlrpc_request, XMLRPC_CreateVector(NULL, xmlrpc_vector_array));
            doRewind = true;
         }

         XMLRPC_VALUE XMLRPC::getNextValue()
         {
            XMLRPC_VALUE params = XMLRPC_RequestGetData(xmlrpc_request);
            XMLRPC_VALUE param;
            if(doRewind)
               {
                  doRewind = false;
                  param = XMLRPC_VectorRewind(params);
               }else
                  param = XMLRPC_VectorNext(params);

#if BTG_EXTERNALIZATION_DEBUG
            BTG_NOTICE("XMLRPC::getNextValue() retrieved param of type " << XMLRPC_GetValueType(param));
#endif
            return param;
         }

         template<typename sourceType>
         t_int XMLRPC::genericSDTSerialize(sourceType *_source)
         {
            return XMLRPC_AddValueToVector(
                                           XMLRPC_RequestGetData(xmlrpc_request),
                                           XMLRPC_CreateValueInt(NULL, static_cast<int>(*_source))
                                           );
         }

         template<typename destinationType>
         t_int XMLRPC::genericSDTDeserialize(destinationType *_destination)
         {
            destinationType d;
            XMLRPC_VALUE v=getNextValue();
            if(XMLRPC_GetValueType(v) != xmlrpc_int)
               {
                  return 0;
               }

            d = XMLRPC_GetValueInt(v);
            (*_destination) = d;

            return sizeof(d);
         }

         template<typename sourceType, typename souceIteratorType, typename containedType>
         t_int XMLRPC::genericContainerSerialize(sourceType *_source)
         {
            t_int size           = 0;
            t_int count          = 0;

            // First write the size of the container.
            t_int container_size = _source->size();
            count                = genericSDTSerialize<t_int>(&container_size);
            size                += count;

            // Then write the containers elements.
            souceIteratorType iter;
            containedType value;
            for (iter=_source->begin(); iter!=_source->end(); iter++)
               {
                  value = *iter;
                  count = genericSDTSerialize<containedType>(&value);
                  size += count;
               }

            return size;
         }

         template<typename containedType, typename destinationType>
         t_int XMLRPC::genericContainerDeserialize(destinationType *_destination)
         {
            t_int size           = 0;
            t_int count          = 0;
            t_int container_size = 0;
            // Get the number of serialized container elements.
            count              = genericSDTDeserialize<t_int>(&container_size);
            if(!count)
               return 0;
            size              += count;

            // Convert each element.
            if (container_size > 0)
               {
                  // Remove the clearing from here, as string::clear is not supported by older g++.
                  containedType value;
                  for (t_int i=0; i<container_size; i++)
                     {
                        count = genericSDTDeserialize<containedType>(&value);
                        if(!count)
                           return 0;
                        size += count;
                        _destination->push_back(value);
                     }
               } // if

            assert(static_cast<t_int>(_destination->size()) == container_size);

            return size;
         }


         bool XMLRPC::setBuffer(dBuffer & _buffer)
         {
            XMLRPC_REQUEST temp_request;

            t_uint buffSize = _buffer.size();

            char *s=new char[buffSize+1];
            _buffer.getBytes(reinterpret_cast<t_byteP>(s), buffSize);
            s[buffSize] = 0;

            /* in xml_element.c:695 fprintf(stderr...) is used on
               error conditions.. Might wan't to remove that */
#if BTG_EXTERNALIZATION_DEBUG
            BTG_NOTICE("Deserializing command from " << s << " of size " << buffSize);
#endif // BTG_EXTERNALIZATION_DEBUG
            temp_request = XMLRPC_REQUEST_FromXML(s, buffSize, NULL);

            delete[] s;

            if(!temp_request)
               {
                  return false;
               }

            if (XMLRPC_RequestGetError(temp_request))
               {
                  XMLRPC_RequestFree(temp_request, 1);
                  return false;
               }

            /* Parsed OK, free the current request */
            XMLRPC_RequestFree(xmlrpc_request, 1);
            xmlrpc_request = temp_request;
            doRewind = true;
            return true;
         }

         t_int XMLRPC::getBufferSize()
         {
            /* Since the caller want's to know the size of the data getBuffer() might return
             * we have to render the object to XML to get the size */
            char* buff;
            int   buffsize;

            buff = XMLRPC_REQUEST_ToXML(xmlrpc_request, &buffsize);

            if(!buff)
               {
                  return 0;
               }

            free(buff);

            return buffsize;
         }

         void XMLRPC::getBuffer(dBuffer & _buffer)
         {
            char* buff;
            int   buffsize;

            XMLRPC_REQUEST_OUTPUT_OPTIONS opt = XMLRPC_RequestGetOutputOptions(xmlrpc_request);

            opt->version = xmlrpc_version_1_0;
            opt->xml_elem_opts.encoding = encoding_utf_8;
            opt->xml_elem_opts.escaping = xml_elem_markup_escaping;

				// Skip this call, since we did a get we uses a ref to the actual opt struct anyway.
				// using set here would cause a "Source and destination overlap in memcpy" (valgrind)
				// unless we do a copy of opt and feed set with it. 
            //XMLRPC_RequestSetOutputOptions(xmlrpc_request, opt);

            buff = XMLRPC_REQUEST_ToXML(xmlrpc_request, &buffsize);


            if (!buff)
               {
                  return;
               }

#if BTG_EXTERNALIZATION_DEBUG
            BTG_NOTICE("Serialized command to " << buff);
#endif // BTG_EXTERNALIZATION_DEBUG
            _buffer.erase();
            _buffer.addBytes(reinterpret_cast<t_byteCP>(buff), buffsize);
            free(buff);

            reset();
         }

         t_byte XMLRPC::determineCommandType(t_int & _command)
         {
            return getCommand(_command);
         }

         void XMLRPC::setDirection(btg::core::DIRECTION _dir)
         {
            if(_dir == btg::core::FROM_SERVER)
               {
#if BTG_EXTERNALIZATION_DEBUG
                  BTG_NOTICE("XMLRPC::setDirection: xmlrcp_request_response");
#endif // BTG_EXTERNALIZATION_DEBUG
                  XMLRPC_RequestSetRequestType(xmlrpc_request, xmlrpc_request_response);
               }
            else
               {
#if BTG_EXTERNALIZATION_DEBUG
                  BTG_NOTICE("XMLRPC::setDirection: xmlrcp_request_call");
#endif // BTG_EXTERNALIZATION_DEBUG
                  XMLRPC_RequestSetRequestType(xmlrpc_request, xmlrpc_request_call);
               }
         }

         t_byte XMLRPC::setCommand(t_int const _command)
         {
            reset();
#if BTG_EXTERNALIZATION_DEBUG
            BTG_NOTICE("XMLRPC::setCommand() " << _command << " :" <<getCommandName(_command));
#endif // BTG_EXTERNALIZATION_DEBUG
            XMLRPC_RequestSetMethodName(xmlrpc_request, getCommandName(_command).c_str());
            addByte(_command);
            return 1;
         }

         t_byte XMLRPC::getCommand(t_int & _command)
         {
            doRewind = true;
            t_byte cmd;
            getByte(cmd);
            _command = cmd;

            if (XMLRPC_RequestGetRequestType(xmlrpc_request) == xmlrpc_request_call)
               {
                  const char* str = XMLRPC_RequestGetMethodName(xmlrpc_request);
                  if (str)
                     {
                        _command = getCommandID(std::string(str));
                     }
               }

#if BTG_EXTERNALIZATION_DEBUG
            BTG_NOTICE("XMLRPC::getCommand() " << _command << " :" << getCommandName(_command));
#endif // BTG_EXTERNALIZATION_DEBUG

            return _command;
         }

         bool XMLRPC::boolToBytes(bool const _source)
         {
            int val=0;

            if (_source == true)
               val=1;

            XMLRPC_AddValueToVector(
                                    XMLRPC_RequestGetData(xmlrpc_request),
                                    XMLRPC_CreateValueBoolean(NULL, val)
                                    );
            return true;
         }

         bool XMLRPC::bytesToBool(bool & _destination)
         {
            XMLRPC_VALUE v = getNextValue();
            if(XMLRPC_GetValueType(v) != xmlrpc_boolean)
               {
                  return 0;
               }

            int res = XMLRPC_GetValueBoolean(v);

            switch (res)
               {
               case 0:
                  _destination = false;
                  break;
               case 1:
                  _destination = true;
                  break;
               default:
                  return false;
               }

            return true;
         }

         bool XMLRPC::addByte(t_byte const _byte)
         {
            XMLRPC_AddValueToVector(
                                    XMLRPC_RequestGetData(xmlrpc_request),
                                    XMLRPC_CreateValueInt(NULL, _byte)
                                    );

            return true;
         }

         bool XMLRPC::getByte(t_byte & _byte)
         {
            XMLRPC_VALUE v = getNextValue();
            if(XMLRPC_GetValueType(v) != xmlrpc_int)
               {
                  return 0;
               }

            int res = XMLRPC_GetValueInt(v);
            _byte = static_cast<t_byte>(res);

            return true;
         }

         bool XMLRPC::addBytes(t_byteCP _bytes, t_uint const _size)
         {
            t_uint size = _size;
            BTG_ICHECK( genericSDTSerialize<t_uint>(&size) );
            if(size == 0)
               {
                  return true;
               }

            XMLRPC_AddValueToVector(
                                    XMLRPC_RequestGetData(xmlrpc_request),
                                    XMLRPC_CreateValueBase64(NULL, reinterpret_cast<const char*>(_bytes), _size)
                                    );
            return true;
         }

         bool XMLRPC::getBytes(t_byteP* _bytes, t_uint &_size)
         {
            *_bytes = 0;
            _size   = 0;

            BTG_ICHECK(genericSDTDeserialize<t_uint>(&_size));
            if(_size == 0)
               {
                  return true;
               }

            XMLRPC_VALUE v = getNextValue();
            if(XMLRPC_GetValueType(v) != xmlrpc_base64)
               {
                  return 0;
               }

            // Determine size and get pointer
            _size         = XMLRPC_GetValueStringLen(v);
            const char *p = XMLRPC_GetValueBase64(v);

            // Copy from xmlrpc-epi internal buffer to new buffer
            *_bytes = new t_byte[_size];
            for (t_uint i=0; i < _size; i++)
               {
                  (*_bytes)[i] = p[i];
               }

            return true;
         }

         bool XMLRPC::intToBytes(const t_int* _source)
         {
            BTG_ICHECK( genericSDTSerialize<t_int const>(_source) );

            return true;
         }

         bool XMLRPC::bytesToInt(t_int* _destination)
         {
            BTG_ICHECK( genericSDTDeserialize<t_int>(_destination) );

            return true;
         }

         bool XMLRPC::uintToBytes(const t_uint* _source)
         {
            std::stringstream ss;
            ss << *_source;
            std::string s = ss.str();
            BTG_ICHECK( stringToBytes( &s ) );

            return true;
         }

         bool XMLRPC::bytesToUint(t_uint* _destination)
         {
            std::string s;
            BTG_ICHECK( bytesToString(&s) );

            return btg::core::stringToNumber<t_uint>(s, *_destination);
         }

         bool XMLRPC::longToBytes(const t_long* _source)
         {
            std::stringstream ss;
            ss << *_source;
            std::string s = ss.str();

            BTG_ICHECK( stringToBytes( &s ) );

            return true;
         }

         bool XMLRPC::uLongToBytes(const t_ulong* _source)
         {
            std::stringstream ss;
            ss << *_source;
            std::string s = ss.str();

            BTG_ICHECK( stringToBytes( &s ) );

            return true;
         }

         bool XMLRPC::bytesToLong(t_long* _destination)
         {
            std::string s;
            BTG_ICHECK( bytesToString(&s) );

            return btg::core::stringToNumber<t_long>(s, *_destination);
         }

         bool XMLRPC::bytesToULong(t_ulong* _destination)
         {
            std::string s;
            BTG_ICHECK( bytesToString(&s) );

            return btg::core::stringToNumber<t_ulong>(s, *_destination);
         }

         bool XMLRPC::floatToBytes(const t_float* _source)
         {
            XMLRPC_AddValueToVector(
                                    XMLRPC_RequestGetData(xmlrpc_request),
                                    XMLRPC_CreateValueDouble(NULL, (*_source))
                                    );

            return true;
         }

         bool XMLRPC::bytesToFloat(t_float* _destination)
         {
            XMLRPC_VALUE v = getNextValue();
            if(XMLRPC_GetValueType(v) != xmlrpc_double)
               {
                  return 0;
               }

            double d = XMLRPC_GetValueDouble(v);
            (*_destination) = static_cast<t_float>(d);

            return true;
         }


         bool XMLRPC::stringToBytes(const std::string* _source)
         {
            XMLRPC_AddValueToVector(
                                    XMLRPC_RequestGetData(xmlrpc_request),
                                    XMLRPC_CreateValueString(NULL, _source->c_str(), _source->size())
                                    );

            return true;
         }

         bool XMLRPC::bytesToString(std::string* _destination)
         {
            // Clear the destination before adding any chars.
#if HAVE_STRING_CLEAR == 1
            _destination->clear();
#else
            _destination->erase(_destination->begin(), _destination->end());
#endif
            XMLRPC_VALUE v = getNextValue();
            if(XMLRPC_GetValueType(v) != xmlrpc_string)
               {
                  return 0;
               }

            const char* s = XMLRPC_GetValueString(v);

            if(s == 0)
               {
                  return false;
               }

            _destination->append(s);

            return true;
         }

         bool XMLRPC::intListToBytes(const t_intList* _source)
         {
            BTG_ICHECK( genericContainerSerialize<t_intList const, t_intList::const_iterator, t_int>(_source) );

            return true;
         }

         bool XMLRPC::bytesToIntList(t_intList* _destination)
         {
            _destination->clear();
            BTG_ICHECK( genericContainerDeserialize<t_int, t_intList >(_destination) );

            return true;
         }

         bool XMLRPC::longListToBytes(const t_longList* _source)
         {
            BTG_ICHECK( genericContainerSerialize<t_longList const, t_longList::const_iterator, t_long>(_source) );

            return true;
         }

         bool XMLRPC::bytesToLongList(t_longList* _destination)
         {
            _destination->clear();
            BTG_ICHECK( genericContainerDeserialize<t_long, t_longList >(_destination) );

            return true;
         }

         bool XMLRPC::stringListToBytes(const t_strList* _source)
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

         bool XMLRPC::bytesToStringList(t_strList* _destination)
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

         std::string XMLRPC::dumpData() const
         {
            char*buff;
            int buffsize;
            std::string s;

            buff = XMLRPC_REQUEST_ToXML(xmlrpc_request, &buffsize);
            if(!buff)
               return s;
            s = buff;
            free(buff);

            return s;
         }

      } // namespace externalization
   } // namespace core
} // namespace btg

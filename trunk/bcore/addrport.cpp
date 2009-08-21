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

#include "addrport.h"
#include <bcore/t_string.h>
#include <bcore/util.h>
#include <bcore/helpermacro.h>

namespace btg
{
   namespace core
   {
      Address::Address()
         : type_(ADT_UNKN)
      {
         ip_[0] = 0;
         ip_[1] = 0;
         ip_[2] = 0;
         ip_[3] = 0;
         ip_[4] = 0;
         ip_[5] = 0;
      }

      Address::Address(t_byte const _ip0,
                       t_byte const _ip1,
                       t_byte const _ip2,
                       t_byte const _ip3)
         : type_(ADT_IPV4)
      {
         ip_[0] = _ip0;
         ip_[1] = _ip1;
         ip_[2] = _ip2;
         ip_[3] = _ip3;
      }

      Address::Address(t_byte const _ip0,
                       t_byte const _ip1,
                       t_byte const _ip2,
                       t_byte const _ip3,
                       t_byte const _ip4,
                       t_byte const _ip5)
         : type_(ADT_IPV6)
      {
         ip_[0] = _ip0;
         ip_[1] = _ip1;
         ip_[2] = _ip2;
         ip_[3] = _ip3;
         ip_[4] = _ip4;
         ip_[5] = _ip5;
      }

      Address::Address(Address const& _address)
         : Serializable(),
           type_(_address.type_)
      {
         ip_[0] = _address.ip_[0];
         ip_[1] = _address.ip_[1];
         ip_[2] = _address.ip_[2];
         ip_[3] = _address.ip_[3];
         ip_[4] = _address.ip_[4];
         ip_[5] = _address.ip_[5];
      }

      bool Address::fromString(std::string const& _input, AddrType const _type)
      {
         bool status = false;

         AddrType iptype = _type;

         if (iptype == ADT_UNKN)
            {
               iptype = detectIpType(_input);
            }

         switch (iptype)
            {
            case ADT_UNKN:
               {
                  status = false;
                  break;
               }
            case ADT_IPV4:
               {
                  type_ = ADT_IPV4;
                  status = fromStringImpl(_input, 4);
                  break;
               }
            case ADT_IPV6:
               {
                  status = fromStringImpl(_input, 6);
                  break;
               }
            }

         // Set type.
         if (status)
            {
               type_ = iptype;
            }

         return status;
      }

      Address::AddrType Address::detectIpType(std::string const& _input)
      {
         AddrType result = ADT_UNKN;

         t_int dotCount = 0; 
         for (std::string::const_iterator iter = _input.begin();
              iter != _input.end();
              iter++)
            {
               if (*iter == '.')
                  {
                     dotCount++;
                  }
            }

         switch (dotCount)
            {
            case 3:
               {
                  result = ADT_IPV4;
                  break;
               }
            case 5:
               {
                  result = ADT_IPV6;
                  break;
               }
            default:
               {
                  result = ADT_UNKN;
                  break;
               }
            }
         return result;
      }

      bool Address::fromStringImpl(std::string const& _input, 
                                   t_uint const _digits)
      {
         bool status = false;

         t_uint const tempArraySize = _digits;
         
         t_strList ip_lst = Util::splitLine(_input, ".");

         if (ip_lst.size() >= tempArraySize)
            {
               t_int temp_ip[tempArraySize];

               for (t_uint i=0; i<tempArraySize; i++)
                  {
                     temp_ip[i] = -1;
                  }

               for (t_uint i=0; i<tempArraySize; i++)
                  {
                     t_int ip_part = 0;
                     
                     btg::core::stringToNumber<t_int>(ip_lst[i], ip_part);
                     
                     if ((ip_part >= 0) && (ip_part <= 255))
                        {
                           temp_ip[i] = ip_part;
                        }
                  }

               // Check that no negative numbers exist in the array.
               
               for (t_uint i=0; i<tempArraySize; i++)
                  {
                     if (temp_ip[i] < 0)
                        {
                           return status;
                        }
                  }
               
               status = true;

               // Everything ok, assign.
               for (t_uint i=0; i<tempArraySize; i++)
                  {
                     ip_[i] = temp_ip[i];
                  }
            }

         return status;
      }

      std::string Address::toString() const
      {
         std::string output;

         output += convertToString<t_int>(ip_[0]);
         output += ".";
         output += convertToString<t_int>(ip_[1]);
         output += ".";
         output += convertToString<t_int>(ip_[2]);
         output += ".";
         output += convertToString<t_int>(ip_[3]);

         if (type_ == ADT_IPV6)
            {
               output += ".";
               output += convertToString<t_int>(ip_[4]);
               output += ".";
               output += convertToString<t_int>(ip_[5]);
            }

         return output;
      }

      bool Address::operator!= (Address const& _address) const
      {
         return !(*this == _address);
      }

      bool Address::operator== (Address const& _address) const
      {
         bool status = true;

         if (type_ != _address.type_)
            {
               status = false;
            }

         if (ip_[0] != _address.ip_[0])
            {
               status = false;
            }

         if (ip_[1] != _address.ip_[1])
            {
               status = false;
            }

         if (ip_[2] != _address.ip_[2])
            {
               status = false;
            }

         if (ip_[3] != _address.ip_[3])
            {
               status = false;
            }

         if (type_ == ADT_IPV6)
            {
               if (ip_[4] != _address.ip_[4])
                  {
                     status = false;
                  }

               if (ip_[5] != _address.ip_[5])
                  {
                     status = false;
                  }
            }

         return status;
      }

      Address& Address::operator= (Address const& _address)
      {
         if (*this != _address)
            {
               type_  = _address.type_;
               ip_[0] = _address.ip_[0];
               ip_[1] = _address.ip_[1];
               ip_[2] = _address.ip_[2];
               ip_[3] = _address.ip_[3];

               if (type_ == ADT_IPV6)
                  {
                     ip_[4] = _address.ip_[4];
                     ip_[5] = _address.ip_[5];
                  }
            }

         return *this;
      }

      bool Address::serialize(btg::core::externalization::Externalization* _e) const
      {
         t_byte type = 0;
         switch (type_)
            {
            case ADT_UNKN:
               type = 0;
               break;
            case ADT_IPV4:
               type = 1;
               break;
            case ADT_IPV6:
               type = 2;
               break;
            }
         BTG_RCHECK(_e->addByte(type) );

         t_int ipmax = 4; 
         if (type_ == ADT_IPV6)
            {
               ipmax = 6;
            }

         for (t_int i = 0; i<ipmax; i++)
            {
               BTG_RCHECK(_e->addByte(ip_[i]) );
            }

         return true;
      }

      bool Address::deserialize(btg::core::externalization::Externalization* _e)
      {
         t_byte type = 0;

         BTG_RCHECK( _e->getByte(type) );

         switch (type)
            {
            case 0:
               type_ = ADT_UNKN;
               break;
            case 1:
               type_ = ADT_IPV4;
               break;
            case 2:
               type_ = ADT_IPV6;
               break;
            }
         BTG_RCHECK(_e->addByte(type) );

         t_int ipmax = 4; 
         if (type_ == ADT_IPV6)
            {
               ipmax = 6;
            }

         for (t_int i = 0; i<ipmax; i++)
            {
               t_byte b = 0;
               BTG_RCHECK( _e->getByte(b) );

               ip_[i] = b;
            }

         return true;
      }

      bool Address::valid() const
      {
         bool status = true;

         switch (type_)
            {
            case ADT_IPV4:
               {
                  if ( (ip_[0] == 0) && (ip_[1] == 0) && (ip_[2] == 0) && (ip_[3] == 0))
                     {
                        status = false;
                     }
                  break;
               }
            case ADT_IPV6:
               {
                  if ( (ip_[0] == 0) && (ip_[1] == 0) && (ip_[2] == 0) && (ip_[3] == 0) && 
                       (ip_[4] == 0) && (ip_[5] == 0))
                     {
                        status = false;
                     }
                  break;
               }
            default:
               status = false;
               break;
            }

         return status;
      }

      Address::~Address()
      {

      }

      /**
       */
      /**/
      /**/
      /**/
      /**/

      addressPort::addressPort()
         : Address(),
           port_(0)
      {
      }

      addressPort::addressPort(t_byte const _ip0,
                               t_byte const _ip1,
                               t_byte const _ip2,
                               t_byte const _ip3,
                               t_uint const _port)
         : Address(_ip0, _ip1, _ip2, _ip3),
           port_(_port)
      {
      }

      addressPort::addressPort(t_byte const _ip0,
                               t_byte const _ip1,
                               t_byte const _ip2,
                               t_byte const _ip3,
                               t_byte const _ip4,
                               t_byte const _ip5,
                               t_uint const _port)
         : Address(_ip0, _ip1, _ip2, _ip3, _ip4, _ip5),
           port_(_port)
      {
      }

      addressPort::addressPort(addressPort const& _addressport)
         : Address(_addressport),
           port_(_addressport.port_)
      {
      }

      t_uint addressPort::getPort() const
      {
         return port_;
      }

      bool addressPort::fromString(std::string const& _input)
      {
         bool status = false;

         std::string input_cpy = _input;

         t_strList lst = Util::splitLine(input_cpy, ":");

         if (lst.size() < 2)
            {
               return status;
            }

         std::string input_ip   = lst[0];
         std::string input_port = lst[1];

         status = Address::fromString(input_ip);

         if (!status)
            {
               return status;
            }

         t_int temp_port = 0;
         btg::core::stringToNumber<t_int>(input_port, temp_port);

         if (temp_port > 0)
            {
               port_  = temp_port;
            }
         else
            {
               status = false;
            }

         return status;
      }

      std::string addressPort::toString() const
      {
         std::string output(Address::toString());

         output += ":";
         output += convertToString<t_int>(port_);

         return output;
      }

      bool addressPort::operator!= (addressPort const& _addressport) const
      {
         return !(*this == _addressport);
      }

      bool addressPort::operator== (addressPort const& _addressport) const
      {
         bool status = Address::operator==(_addressport);

         if (port_ != _addressport.port_)
            {
               status = false;
            }

         return status;
      }

      addressPort& addressPort::operator= (addressPort const& _addressport)
      {
         if (*this != _addressport)
            {
               Address::operator=(_addressport);
               port_  = _addressport.port_;
            }

         return *this;
      }

      bool addressPort::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK(Address::serialize(_e));
         BTG_RCHECK( _e->uintToBytes(&port_) );

         return true;
      }

      bool addressPort::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK(Address::deserialize(_e));
         BTG_RCHECK( _e->bytesToUint(&port_) );

         return true;
      }

      bool addressPort::valid() const
      {
         bool status = true;

         if (!Address::valid())
            {
               status = false;
            }

         if (!status)
            {
               // IPv4 address contains all zeros, but port is not
               // empty.
               if (port_ > 0)
                  {
                     status = true;
                  }
            }
         else
            {
               // Address ok, but port is zero.
               if (port_ == 0)
                  {
                     status = false;
                  }
            }
         return status;
      }

      addressPort::~addressPort()
      {

      }

   } // namespace core
} // namespace btg



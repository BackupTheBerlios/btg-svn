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

#ifndef ADDRPORT_H
#define ADDRPORT_H

#include <bcore/type.h>
#include <bcore/serializable.h>

#include <string>

namespace btg
{
   namespace core
      {

         /**
          * \addtogroup core
          */
         /** @{ */

         /// IPv4 address.
         class Address: public Serializable
            {
            public:
               /// Default constructor.
               /// Crates an instance of this class (0.0.0.0) on
               /// which valid() will return false.
               Address();

               /// Constructor.
               /// @param [in] _ip0  IPv4, byte 0.
               /// @param [in] _ip1  IPv4, byte 1.
               /// @param [in] _ip2  IPv4, byte 2.
               /// @param [in] _ip3  IPv4, byte 3.
               Address(t_byte const _ip0,
                       t_byte const _ip1,
                       t_byte const _ip2,
                       t_byte const _ip3);

               /// Copy constructor.
               Address(Address const& _address);

               /// Get a string represenation of this class.
               std::string getIp() const { return toString(); }

               /// Setup an instance of this class with information
               /// contained in a string. The string should contain "a.b.c.d".
               /// @return True if successful, false otherwise.
               bool fromString(std::string const& _input);

               /// Get this address port as "ip:port" string.
               std::string toString() const;

               /// The equality operator.
               virtual bool operator== (Address const& _address) const;

               /// The not equal operator.
               virtual bool operator!= (Address const& _address) const;

               /// The assignment operator.
               Address& operator= (Address const& _address);

               virtual bool serialize(btg::core::externalization::Externalization* _e) const;

               virtual bool deserialize(btg::core::externalization::Externalization* _e);

               /// Returns true if, address is not 0.0.0.0.
               bool valid() const;

               /// Destructor.
               virtual ~Address();
            protected:
               /// IPv4 address.
               t_byte ip_[4];
            };

         /// IPv4 and port information.
         class addressPort: public Address
            {
            public:
               /// Default constructor.
               /// Crates an instance of this class (0.0.0.0:0) on
               /// which valid() will return false.
               addressPort();

               /// Constructor.
               /// @param [in] _ip0  IPv4, byte 0.
               /// @param [in] _ip1  IPv4, byte 1.
               /// @param [in] _ip2  IPv4, byte 2.
               /// @param [in] _ip3  IPv4, byte 3.
               /// @param [in] _port Port.
               addressPort(t_byte const _ip0,
                           t_byte const _ip1,
                           t_byte const _ip2,
                           t_byte const _ip3,
                           t_uint const _port);

               /// Copy constructor.
               addressPort(addressPort const& _addressport);

               /// Get the port.
               t_uint    getPort() const;

               /// Setup an instance of this class with information
               /// contained in a string. The string should contain "a.b.c.d:port".
               /// @return True if successful, false otherwise.
               bool fromString(std::string const& _input);

               /// Setup an instance of this class with information contained in a string. 
               /// Ignore port.
               bool addressFromString(std::string const& _input);

               /// Get this address port as "ip:port" string.
               std::string toString() const;

               /// The equality operator.
               bool operator== (addressPort const& _addressport) const;

               /// The not equal operator.
               bool operator!= (addressPort const& _addressport) const;

               /// The assignment operator.
               addressPort& operator= (addressPort const& _addressport);

               bool serialize(btg::core::externalization::Externalization* _e) const;

               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Returns true if, address is not 0.0.0.0 and port is
               /// greater than 0 or if the address is all zeros and
               /// the port is greater than zero.
               bool valid() const;

               /// Destructor.
               virtual ~addressPort();
            private:
               /// Port
               t_uint port_;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

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
 * $Id: peer.h,v 1.1.2.10 2007/01/16 20:26:36 wojci Exp $
 */

#ifndef PEER_H
#define PEER_H

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

         /// IPv4 address of a peer.
         class peerAddress: public Serializable
            {
            public:
               /// Default constructor.
               peerAddress();

               /// Constructor.
               /// @param [in] _ip0 IPv4, byte 0.
               /// @param [in] _ip1 IPv4, byte 1.
               /// @param [in] _ip2 IPv4, byte 2.
               /// @param [in] _ip3 IPv4, byte 3.

               peerAddress(t_byte const _ip0,
                           t_byte const _ip1,
                           t_byte const _ip2,
                           t_byte const _ip3);

               /// Copy constructor.
               peerAddress(peerAddress const& _peeraddress);

               /// Get the address as a string.
               std::string getAddress() const;

               /// Equality operator.
               bool operator== (peerAddress const& _peeraddress) const;

               /// Assignment operator.
               peerAddress& operator= (peerAddress const& _peeraddress);

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Get the string representation of an instance of
               /// this class.
               std::string toString() const;

               /// Destructor.
               virtual ~peerAddress();
            private:
               /// IPv4 address.
               t_byte ip_[4];
            };

         /// Used for transfering peer information about contexts.
         class Peer: public Serializable
            {
            public:

               /// Default constructor.
               Peer();

               /// Constructor.
               Peer(peerAddress const& _peeraddress,
                    bool const _seeder,
                    std::string const& _identification);

               /// Copy constructor.
               Peer(Peer const& _peer);

               /// The equality operator.
               bool operator== (Peer const& _peer) const;

               /// The assignment operator.
               Peer& operator= (Peer const& _peer);

               bool serialize(btg::core::externalization::Externalization* _e) const;

               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Get the string representation of an instance of
               /// this class.
               std::string toString() const;

               /// Returns true if this peer is a seeder.
               bool seeder() const { return seeder_; };

               /// Get a string representing the contained address.
               std::string address() const;

               /// Get a string which identifies the peer.
               std::string identification() const;

               /// Destructor.
               virtual ~Peer();
            private:
               /// The address of this peer.
               peerAddress peeraddress_;

               /// True if this peer is a seeder.
               bool        seeder_;

               /// Description of this peer.
               std::string identification_;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

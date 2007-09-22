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

#include "peer.h"

#include <bcore/t_string.h>
#include <bcore/helpermacro.h>

namespace btg
{
   namespace core
   {
      peerAddress::peerAddress()
         : Serializable()
      {
         ip_[0] = 0;
         ip_[1] = 0;
         ip_[2] = 0;
         ip_[3] = 0;
      }

      peerAddress::peerAddress(t_byte const _ip0,
                               t_byte const _ip1,
                               t_byte const _ip2,
                               t_byte const _ip3)
         : Serializable()
      {
         ip_[0] = _ip0;
         ip_[1] = _ip1;
         ip_[2] = _ip2;
         ip_[3] = _ip3;
      }

      peerAddress::peerAddress(peerAddress const& _peeraddress)
         : Serializable()
      {
         ip_[0] = _peeraddress.ip_[0];
         ip_[1] = _peeraddress.ip_[1];
         ip_[2] = _peeraddress.ip_[2];
         ip_[3] = _peeraddress.ip_[3];
      }

      bool peerAddress::operator== (peerAddress const& _peeraddress) const
      {
         bool status = false;

         if (
             (ip_[0] == _peeraddress.ip_[0]) &&
             (ip_[1] == _peeraddress.ip_[1]) &&
             (ip_[2] == _peeraddress.ip_[2]) &&
             (ip_[3] == _peeraddress.ip_[3])
             )
            {
               status = true;
            }
         return status;
      }

      peerAddress& peerAddress::operator= (peerAddress const& _peeraddress)
      {
         bool eq = (_peeraddress == *this);
         if (!eq)
            {
               ip_[0] = _peeraddress.ip_[0];
               ip_[1] = _peeraddress.ip_[1];
               ip_[2] = _peeraddress.ip_[2];
               ip_[3] = _peeraddress.ip_[3];
            }
         return *this;
      }

      std::string peerAddress::getAddress() const
      {
         std::string output;

         output += convertToString<t_int>(ip_[0]);
         output += ".";
         output += convertToString<t_int>(ip_[1]);
         output += ".";
         output += convertToString<t_int>(ip_[2]);
         output += ".";
         output += convertToString<t_int>(ip_[3]);

         return output;
      }

      bool peerAddress::serialize(btg::core::externalization::Externalization* _e) const
      {
         for (t_uint i = 0; i<4; i++)
            {
               _e->addByte(ip_[i]);
               BTG_RCHECK(_e->status());
            }

         return true;
      }

      bool peerAddress::deserialize(btg::core::externalization::Externalization* _e)
      {
         t_byte b;
         for (t_uint i = 0; i<4; i++)
            {
               _e->getByte(b);
               BTG_RCHECK(_e->status());
               ip_[i] = b;
            }

         return true;
      }

      std::string peerAddress::toString() const
      {
         std::string output = getAddress();

         return output;
      }

      peerAddress::~peerAddress()
      {

      }

      /* */
      /* */
      /* */

      Peer::Peer()
         : Serializable(),
           peeraddress_(),
           seeder_(false),
           identification_()
      {

      }

      Peer::Peer(peerAddress const& _peeraddress,
                 bool const _seeder,
                 std::string const& _identification)
         : Serializable(),
           peeraddress_(_peeraddress),
           seeder_(_seeder),
           identification_(_identification)
      {
      }

      Peer::Peer(Peer const& _peer)
         : Serializable(),
           peeraddress_(_peer.peeraddress_),
           seeder_(_peer.seeder_),
           identification_(_peer.identification_)
      {
      }

      bool Peer::operator== (Peer const& _peer) const
      {
         bool status = false;

         if ((peeraddress_ == _peer.peeraddress_) &&
             (seeder_ == _peer.seeder_) &&
             (identification_ == _peer.identification_))
            {
               status = true;
            }

         return status;
      }

      Peer& Peer::operator= (Peer const& _peer)
      {
         bool eq = (_peer == *this);

         if (!eq)
            {
               peeraddress_    = _peer.peeraddress_;
               seeder_         = _peer.seeder_;
               identification_ = _peer.identification_;
            }
         return *this;
      }

      bool Peer::serialize(btg::core::externalization::Externalization* _e) const
      {
         peeraddress_.serialize(_e);
         BTG_RCHECK(_e->status());

         _e->boolToBytes(seeder_);
         BTG_RCHECK(_e->status());

         _e->stringToBytes(&identification_);
         BTG_RCHECK(_e->status());

         return true;
      }

      bool Peer::deserialize(btg::core::externalization::Externalization* _e)
      {
         peeraddress_.deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->bytesToBool(seeder_);
         BTG_RCHECK(_e->status());

         _e->bytesToString(&identification_);
         BTG_RCHECK(_e->status());

         return true;
      }

      std::string Peer::toString() const
      {
         std::string output;

         output = peeraddress_.toString();

         if (seeder_)
            {
               output += ", seeding";
            }

         output += ", ";
         output += identification_;

         return output;
      }

      std::string Peer::address() const
      {
         std::string output = peeraddress_.toString();

         return output;
      }

      std::string Peer::identification() const
      {
         return identification_;
      }

      Peer::~Peer()
      {

      }

   } // namespace core
} // namespace btg



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

#include <bcore/hrr.h>
#include <bcore/hrt.h>
#include <bcore/hru.h>
#include <bcore/t_string.h>
#include <bcore/helpermacro.h>

namespace btg
{
   namespace core
   {
      peerAddress::peerAddress() :
         Serializable()
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
      
      /* */
      /* */
      /* */

      enum PeerExParams
      {
         max_pieces_cnt = 1024
      };
      
      enum PeerFlags
      {
         interesting = 0x1, // I
         choked = 0x2, // Ch
         remote_interested = 0x4, // rI
         remote_choked = 0x8, // rCh
         supports_extensions = 0x10, // Ex
         local_connection = 0x20, // L
         handshake = 0x40, // H
         connecting = 0x80, // C
         queued = 0x100, // Q
         on_parole = 0x200, // P
         seed = 0x400, // S
         optimistic_unchoke = 0x800, // uCh
         rc4_encrypted = 0x100000, // E4
         plaintext_encrypted = 0x200000 // Ep
      };

      enum PeerSourceFlags
      {
         tracker = 0x1, // T
         dht = 0x2, // D
         pex = 0x4, // P
         lsd = 0x8, // L
         resume_data = 0x10, // R
         incoming = 0x20 // I
      };      

      enum ConnectionType
      {
         standard_bittorrent = 0, // BT
         web_seed = 1 // W
      };

      PeerEx::PeerEx() :
         flags_(),
         source_(),
         down_speed_(), up_speed_(),
         payload_down_speed_(), payload_up_speed_(),
         total_download_(), total_upload_(),
         pieces_(),
         download_limit_(), upload_limit_(),
         load_balancing_(),
         download_queue_length_(), upload_queue_length_(),
         downloading_piece_index_((t_uint)-1), downloading_block_index_(),
         downloading_progress_(), downloading_total_(),
         client_(),
         connection_type_((t_uint)-1),
         /* LT-13 -related */
         last_request_(), last_active_(),
         num_hashfails_(), failcount_(),
         target_dl_queue_length_(),
         remote_dl_rate_()
      {
         country_[0] = ' ';
         country_[1] = ' ';
      }
      
      PeerEx::PeerEx(
         const t_uint _flags,
         const t_uint _source,
         const t_uint _down_speed, const t_uint _up_speed,
         const t_uint _payload_down_speed, const t_uint _payload_up_speed,
         const t_ulong _total_download, const t_ulong _total_upload,
         const std::vector<bool> & _pieces,
         const t_uint _download_limit, const t_uint _upload_limit,
         const char * _country,
         const t_long _load_balancing,
         const t_uint _download_queue_length, const t_uint _upload_queue_length,
         const t_uint _downloading_piece_index, const t_uint _downloading_block_index,
         const t_uint _downloading_progress, const t_uint _downloading_total,
         const std::string & _client,
         const t_uint _connection_type,
         /* LT-13 -related */
         const t_uint _last_request, const t_uint _last_active,
         const t_uint _num_hashfails, const t_uint _failcount,
         const t_uint _target_dl_queue_length,
         const t_uint _remote_dl_rate
         ) :
            flags_(_flags),
            source_(_source),
            down_speed_(_down_speed), up_speed_(_up_speed),
            payload_down_speed_(_payload_down_speed), payload_up_speed_(_payload_up_speed),
            total_download_(_total_download), total_upload_(_total_upload),
            pieces_(_pieces),
            download_limit_(_download_limit), upload_limit_(_upload_limit),
            load_balancing_(_load_balancing),
            download_queue_length_(_download_queue_length), upload_queue_length_(_upload_queue_length),
            downloading_piece_index_(_downloading_piece_index), downloading_block_index_(_downloading_block_index),
            downloading_progress_(_downloading_progress), downloading_total_(_downloading_total),
            client_(_client),
            connection_type_(_connection_type),
            /* LT-13 -related */
            last_request_(_last_request), last_active_(_last_active),
            num_hashfails_(_num_hashfails), failcount_(_failcount),
            target_dl_queue_length_(_target_dl_queue_length),
            remote_dl_rate_(_remote_dl_rate)
      {
         /*
          * Disabled due [ Bug #15073 ] Peers of any kind appear to have all pieces
          * 
         pieces_.aggregate(_pieces, max_pieces_cnt);
          *
          */
         
         if (_country && _country[0])
         {
            country_[0] = _country[0];
            country_[1] = _country[1];
         }
         else
         {
            country_[0] = ' ';
            country_[1] = ' ';
         }
      }

      bool PeerEx::operator== (PeerEx const& _peerEx) const
      {
         return flags_ == _peerEx.flags_
            && source_ == _peerEx.source_
            && down_speed_ == _peerEx.down_speed_
            && up_speed_ == _peerEx.up_speed_
            && payload_down_speed_ == _peerEx.payload_down_speed_
            && payload_up_speed_ == _peerEx.payload_up_speed_
            && total_download_ == _peerEx.total_download_
            && total_upload_ == _peerEx.total_upload_
            && pieces_ == _peerEx.pieces_
            && download_limit_ == _peerEx.download_limit_
            && upload_limit_ == _peerEx.upload_limit_
            && country_[0] == _peerEx.country_[0] && country_[1] == _peerEx.country_[1]
            && load_balancing_ == _peerEx.load_balancing_
            && download_queue_length_ == _peerEx.download_queue_length_
            && upload_queue_length_ == _peerEx.upload_queue_length_
            && downloading_piece_index_ == _peerEx.downloading_piece_index_
            && downloading_block_index_ == _peerEx.downloading_block_index_
            && downloading_progress_ == _peerEx.downloading_progress_
            && downloading_total_ == _peerEx.downloading_total_
            && client_ == _peerEx.client_
            && connection_type_ == _peerEx.connection_type_
            && last_request_ == _peerEx.last_request_
            && last_active_ == _peerEx.last_active_
            && num_hashfails_ == _peerEx.num_hashfails_
            && failcount_ == _peerEx.failcount_
            && target_dl_queue_length_ == _peerEx.target_dl_queue_length_
            && remote_dl_rate_ == _peerEx.remote_dl_rate_
            ;
      }
      
      bool PeerEx::serialize(btg::core::externalization::Externalization* _e) const
      {
         _e->uintToBytes(&flags_);
         BTG_RCHECK(_e->status());
         
         _e->uintToBytes(&source_);
         BTG_RCHECK(_e->status());
         
         _e->uintToBytes(&down_speed_);
         BTG_RCHECK(_e->status());
         _e->uintToBytes(&up_speed_);
         BTG_RCHECK(_e->status());
         
         _e->uintToBytes(&payload_down_speed_);
         BTG_RCHECK(_e->status());
         _e->uintToBytes(&payload_up_speed_);
         BTG_RCHECK(_e->status());
         
         _e->uLongToBytes(&total_download_);
         BTG_RCHECK(_e->status());
         _e->uLongToBytes(&total_upload_);
         BTG_RCHECK(_e->status());
         
         pieces_.serialize(_e);
         BTG_RCHECK(_e->status());
         
         _e->uintToBytes(&download_limit_);
         BTG_RCHECK(_e->status());
         _e->uintToBytes(&upload_limit_);
         BTG_RCHECK(_e->status());
         
         _e->addBytes(country_, sizeof(country_));
         BTG_RCHECK(_e->status());
         
         _e->longToBytes(&load_balancing_);
         BTG_RCHECK(_e->status());
         
         _e->uintToBytes(&download_queue_length_);
         BTG_RCHECK(_e->status());
         _e->uintToBytes(&upload_queue_length_);
         BTG_RCHECK(_e->status());
         
         _e->uintToBytes(&downloading_piece_index_);
         BTG_RCHECK(_e->status());
         _e->uintToBytes(&downloading_block_index_);
         BTG_RCHECK(_e->status());
         _e->uintToBytes(&downloading_progress_);
         BTG_RCHECK(_e->status());
         _e->uintToBytes(&downloading_total_);
         BTG_RCHECK(_e->status());
         
         /* As it turned out,
          * this string can contain any random character
          * so we can't transfer it as a string.
          * Considering it as a raw data.
          */
         _e->addBytes((t_byteCP)client_.c_str(), client_.size());
         BTG_RCHECK(_e->status());
         
         _e->uintToBytes(&connection_type_);
         BTG_RCHECK(_e->status());
         

         /*
          * LT-13 -related parameters
          */
         
         _e->uintToBytes(&last_request_);
         BTG_RCHECK(_e->status());
         _e->uintToBytes(&last_active_);
         BTG_RCHECK(_e->status());

         _e->uintToBytes(&num_hashfails_);
         BTG_RCHECK(_e->status());
         _e->uintToBytes(&failcount_);
         BTG_RCHECK(_e->status());
         
         _e->uintToBytes(&target_dl_queue_length_);
         BTG_RCHECK(_e->status());
         
         _e->uintToBytes(&remote_dl_rate_);
         BTG_RCHECK(_e->status());
         
         return true;
      }

      bool PeerEx::deserialize(btg::core::externalization::Externalization* _e)
      {
         _e->bytesToUint(&flags_);
         BTG_RCHECK(_e->status());
         
         _e->bytesToUint(&source_);
         BTG_RCHECK(_e->status());
         
         _e->bytesToUint(&down_speed_);
         BTG_RCHECK(_e->status());
         _e->bytesToUint(&up_speed_);
         BTG_RCHECK(_e->status());
         
         _e->bytesToUint(&payload_down_speed_);
         BTG_RCHECK(_e->status());
         _e->bytesToUint(&payload_up_speed_);
         BTG_RCHECK(_e->status());
         
         _e->bytesToULong(&total_download_);
         BTG_RCHECK(_e->status());
         _e->bytesToULong(&total_upload_);
         BTG_RCHECK(_e->status());
         
         pieces_.deserialize(_e);
         BTG_RCHECK(_e->status());
         
         _e->bytesToUint(&download_limit_);
         BTG_RCHECK(_e->status());
         _e->bytesToUint(&upload_limit_);
         BTG_RCHECK(_e->status());
         
         t_byteP bytes;
         t_uint size;

         bytes = 0;
         size = 0;
         _e->getBytes(&bytes, size);
         if (size > 1)
         {
            country_[0] = bytes[0];
            country_[1] = bytes[1];
         }
         else
         {
            country_[0] = ' ';
            country_[1] = ' ';
         }
         delete[] bytes;
         BTG_RCHECK(_e->status());
         
         _e->bytesToLong(&load_balancing_);
         BTG_RCHECK(_e->status());
         
         _e->bytesToUint(&download_queue_length_);
         BTG_RCHECK(_e->status());
         _e->bytesToUint(&upload_queue_length_);
         BTG_RCHECK(_e->status());
         
         _e->bytesToUint(&downloading_piece_index_);
         BTG_RCHECK(_e->status());
         _e->bytesToUint(&downloading_block_index_);
         BTG_RCHECK(_e->status());
         _e->bytesToUint(&downloading_progress_);
         BTG_RCHECK(_e->status());
         _e->bytesToUint(&downloading_total_);
         BTG_RCHECK(_e->status());
         
         bytes = 0;
         size = 0;
         _e->getBytes(&bytes, size);
         client_= std::string((const char *)bytes, size);
         delete[] bytes;
         BTG_RCHECK(_e->status());
         
         _e->bytesToUint(&connection_type_);
         BTG_RCHECK(_e->status());
         

         /*
          * LT-13 -related parameters
          */
         
         _e->bytesToUint(&last_request_);
         BTG_RCHECK(_e->status());
         _e->bytesToUint(&last_active_);
         BTG_RCHECK(_e->status());
         
         _e->bytesToUint(&num_hashfails_);
         BTG_RCHECK(_e->status());
         _e->bytesToUint(&failcount_);
         BTG_RCHECK(_e->status());
         
         _e->bytesToUint(&target_dl_queue_length_);
         BTG_RCHECK(_e->status());
         
         _e->bytesToUint(&remote_dl_rate_);
         BTG_RCHECK(_e->status());
         
         return true;
      }
      
      PeerEx::~PeerEx()
      {
         
      }
      
      std::string PeerEx::flags() const
      {
         std::string flags;
         if (flags_ & interesting)
            flags += "I";
         if (flags_ & choked)
            flags += "Ch";
         if (flags_ & remote_interested)
            flags += "rI";
         if (flags_ & remote_choked)
            flags += "rCh";
         if (flags_ & supports_extensions)
            flags += "Ex";
         if (flags_ & local_connection)
            flags += "L";
         if (flags_ & handshake)
            flags += "H";
         if (flags_ & connecting)
            flags += "C";
         if (flags_ & queued)
            flags += "Q";
         if (flags_ & on_parole)
            flags += "P";
         if (flags_ & seed)
            flags += "S";
         if (flags_ & optimistic_unchoke)
            flags += "uCh";
         if (flags_ & rc4_encrypted)
            flags += "E4";
         if (flags_ & plaintext_encrypted)
            flags += "Ep";
         return flags;
      }
      
      std::string PeerEx::source() const
      {
         std::string source;
         if (source_ & tracker)
            source += "T";
         if (source_ & dht)
            source += "D";
         if (source_ & pex)
            source += "P";
         if (source_ & lsd)
            source += "L";
         if (source_ & resume_data)
            source += "R";
         if (source_ & incoming)
            source += "I";
         return source;
      }
      
      std::string PeerEx::down_speed() const
      {
         return humanReadableRate::convert(down_speed_).toString(true);
      }
      
      std::string PeerEx::up_speed() const
      {
         return humanReadableRate::convert(up_speed_).toString(true);
      }
      
      std::string PeerEx::payload_down_speed() const
      {
         return humanReadableRate::convert(payload_down_speed_).toString(true);
      }
      
      std::string PeerEx::payload_up_speed() const
      {
         return humanReadableRate::convert(payload_up_speed_).toString(true);
      }
      
      std::string PeerEx::total_download() const
      {
         return humanReadableUnit::convert(total_download_).toString();
      }
      
      std::string PeerEx::total_upload() const
      {
         return humanReadableUnit::convert(total_upload_).toString();
      }
      
      const t_bitVector & PeerEx::pieces() const
      {
         return pieces_;
      }
      
      std::string PeerEx::download_limit() const
      {
         if (download_limit_ == (t_uint)-1)
         {
            return "";
         }
         else
         {
            return humanReadableRate::convert(download_limit_).toString(true);
         }
      }
      
      std::string PeerEx::upload_limit() const
      {
         if (upload_limit_ == (t_uint)-1)
         {
            return "";
         }
         else
         {
            return humanReadableRate::convert(upload_limit_).toString(true);
         }
      }
      
      std::string PeerEx::country() const
      {
         return std::string((const char *)country_, sizeof(country_));
      }
      
      std::string PeerEx::load_balancing() const
      {
         return convertToString(load_balancing_);
      }
      
      std::string PeerEx::download_queue_length() const
      {
         return convertToString(download_queue_length_);
      }
      
      std::string PeerEx::upload_queue_length() const
      {
         return convertToString(upload_queue_length_);
      }
      
      std::string PeerEx::downloading_piece_index() const
      {
         if (downloading_piece_index_ != (t_uint)-1)
            return convertToString(downloading_piece_index_);
         else
            return "";
      }
      
      std::string PeerEx::downloading_block_index() const
      {
         if (downloading_piece_index_ != (t_uint)-1)
            return convertToString(downloading_block_index_);
         else
            return "";
      }
      
      t_uint PeerEx::downloading_progress() const
      {
         if (downloading_piece_index_ != (t_uint)-1)
            return downloading_progress_ * 100 / downloading_total_;
         else
            return 0;
      }
      
      std::string PeerEx::downloading_total() const
      {
         if (downloading_piece_index_ != (t_uint)-1)
            return humanReadableUnit::convert(downloading_total_).toString();
         else
            return "";
      }
      
      std::string PeerEx::client() const
      {
         return client_;
      }
      
      std::string PeerEx::connection_type() const
      {
         switch (connection_type_)
         {
         case standard_bittorrent:
            return "BT";
         case web_seed:
            return "W";
         }

         return "?";
      }
      
      std::string PeerEx::last_request() const
      {
         if (last_request_ != (t_uint)-1)
            return humanReadableTime::convert(last_request_).toString();
         else
            return "";
      }
      
      std::string PeerEx::last_active() const
      {
         if (last_active_ != (t_uint)-1)
            return humanReadableTime::convert(last_active_).toString();
         else
            return "";
      }
      
      std::string PeerEx::num_hashfails() const
      {
         if (num_hashfails_ != (t_uint)-1)
            return convertToString(num_hashfails_);
         else
            return "";
      }
      
      std::string PeerEx::failcount() const
      {
         if (failcount_ != (t_uint)-1)
            return convertToString(failcount_);
         else
            return "";
      }
      
      std::string PeerEx::target_dl_queue_length() const
      {
         if (target_dl_queue_length_ != (t_uint)-1)
            return convertToString(target_dl_queue_length_);
         else
            return "";
      }
      
      std::string PeerEx::remote_dl_rate() const
      {
         if (remote_dl_rate_ != (t_uint)-1)
            return humanReadableRate::convert(remote_dl_rate_).toString(true);
         else
            return "";
      }
      
   } // namespace core
} // namespace btg

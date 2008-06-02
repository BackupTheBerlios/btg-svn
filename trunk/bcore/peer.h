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

#ifndef PEER_H
#define PEER_H

#include <bcore/serializable.h>
#include <bcore/bitvector.h>
#include <string>
#include <vector>

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
               /// Default constructor. (when we're deserializing the object)
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

               /// Get the address as a string.
               std::string getAddress() const;

               /// Equality operator (actually is needed only in tests for now)
               bool operator== (peerAddress const& _peeraddress) const;

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

               /// Default constructor. (when we're deserializing the object)
               Peer();
               
               /// Constructor.
               Peer(peerAddress const& _peeraddress,
                    bool const _seeder,
                    std::string const& _identification);

               /// Equality operator (actually is needed only in tests for now)
               bool operator== (Peer const& _peer) const;

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
         
         /// Extended peer information.
         /// @see libtorrent::peer_info.
         class PeerEx: public Serializable
         {
         public:
            /// Default constructor. (is used when object is initializing by deserialization).
            PeerEx();
            
            /// Constructor.
            PeerEx(
               const t_uint flags,
               const t_uint source,
               const t_uint down_speed, const t_uint up_speed,
               const t_uint payload_down_speed, const t_uint payload_up_speed,
               const t_ulong total_download, const t_ulong total_upload,
               const std::vector<bool> & pieces,
               const t_uint download_limit, const t_uint upload_limit,
               const char * country,
               const t_long load_balancing,
               const t_uint download_queue_length, const t_uint upload_queue_length,
               const t_uint downloading_piece_index, const t_uint downloading_block_index,
               const t_uint downloading_progress, const t_uint downloading_total,
               const std::string & client,
               const t_uint connection_type,
               /* LT-13 - related */
               const t_uint last_request, const t_uint last_active,
               const t_uint num_hashfails, const t_uint failcount,
               const t_uint target_dl_queue_length,
               const t_uint remote_dl_rate
               );
            
            /// Equality operator (actually is needed only in tests for now).
            bool operator== (PeerEx const& _peerEx) const;
            
            bool serialize(btg::core::externalization::Externalization* _e) const;

            bool deserialize(btg::core::externalization::Externalization* _e);
            
            /// Destructor
            virtual ~PeerEx();
            
            /// LT peer flags (peer state).
            std::string flags() const;
            
            /// Peer source.
            std::string source() const;
            
            /// Download speed.
            std::string down_speed() const;
            
            /// Upload speed to this peer.
            std::string up_speed() const;
            
            /// Payload data download speed.
            std::string payload_down_speed() const;
            
            /// Payload data upload speed.
            std::string payload_up_speed() const;
            
            /// Count of downloaded payload data from this peer.
            std::string total_download() const;
            
            /// Count of uploaded payload data to this peer.
            std::string total_upload() const;
            
            /// Pieces this peer has.
            const t_bitVector & pieces() const;
            
            /// Number of bytes per second this peer is allowed to
            /// receive.
            std::string download_limit() const;
            
            /// Number of bytes per second we are allowed to send to
            /// this peer every second.
            std::string upload_limit() const;
            
            /// ISO 3166 country code.
            std::string country() const;
            
            /// Measurement of the balancing of free download (that we get)
            /// and free upload that we give.
            std::string load_balancing() const;
            
            /// Number of piece-requests we have sent to this peer
            /// that hasn't been answered with a piece yet.
            std::string download_queue_length() const;

            /// Number of piece-requests we have received from this peer
            /// that we haven't answered with a piece yet.
            std::string upload_queue_length() const;
            
            /// Index of the piece that is currently being downloaded.
            std::string downloading_piece_index() const;
            
            /// Index of the block (or sub-piece) that is being downloaded.
            std::string downloading_block_index() const;
            
            /// Number of bytes of this block we have received from the peer.
            t_float downloading_progress() const;
            
            /// Total number of bytes in this block
            std::string downloading_total() const;
            
            /// String describing the software at the other end of the connection.
            std::string client() const;
            
            /// Standard_bittorrent or web_seed.
            std::string connection_type() const;
            
            /// Time since last request (milliseconds).
            std::string last_request() const;
            
            /// Time since last download or upload (milliseconds).
            std::string last_active() const;
            
            /// The number of failed hashes for this peer.
            std::string num_hashfails() const;
            
            /// The number of times this IP has failed to connect.
            std::string failcount() const;
            
            /// The number of requests that is tried to be maintained. 
            std::string target_dl_queue_length() const;
            
            /// The approximate peer download rate.
            std::string remote_dl_rate() const;
            
         private:
            /*
             * Many of these comments are taken from
             * Libtorrent Rasterbar manual
             * http://www.rasterbar.com/products/libtorrent/manual.html#peer-info
             */
            
            /// LT peer flags (peer state).
            t_uint flags_;
            
            /// Peer source.
            t_uint source_;
            
            /// Download speed.
            t_uint down_speed_;
            
            /// Upload speed to this peer.
            t_uint up_speed_;

            /// Payload data download speed.
            t_uint payload_down_speed_;
            
            /// Payload data upload speed.
            t_uint payload_up_speed_;
            
            /// Count of downloaded payload data from this peer.
            t_ulong total_download_;
            
            /// Count of uploaded payload data to this peer.
            t_ulong total_upload_;
            
            /// Pieces this peer has
            t_bitVector pieces_;
            
            /// Number of bytes per second this peer is allowed to receive.
            /// (int)-1 means it's unlimited.
            t_uint download_limit_;
            
            /// Number of bytes per second we are allowed to send to this peer every second.
            /// It may be (int)-1 if there's no local limit on the peer.
            t_uint upload_limit_;
            
            /// ISO 3166 country code.
            t_byte country_[2];
            
            /// Measurement of the balancing of free download (that we get)
            /// and free upload that we give.
            /// Every peer gets a certain amount of free upload,
            /// but this member says how much extra free upload this peer has got.
            /// If it is a negative number it means that this was a peer
            /// from which we have got this amount of free download.
            t_long load_balancing_;
            
            /// Number of piece-requests we have sent to this peer
            /// that hasn't been answered with a piece yet.
            t_uint download_queue_length_;
            
            /// Number of piece-requests we have received from this peer
            /// that we haven't answered with a piece yet.
            t_uint upload_queue_length_;
            
            /// Index of the piece that is currently being downloaded.
            /// This may be set to (int)-1 if there's currently no
            /// piece downloading from this peer.
            /// If it is >= 0, the other three members are valid.
            t_uint downloading_piece_index_;

            /// Index of the block (or sub-piece) that is being
            /// downloaded.
            t_uint downloading_block_index_;
            
            /// Number of bytes of this block we have received from
            /// the peer.
            t_uint downloading_progress_;
            
            /// Total number of bytes in this block.
            t_uint downloading_total_;

            /// String describing the software at the other end of the connection.
            /// In some cases this information is not available,
            /// then it will contain a string that may give away
            /// something about which software is running in the other end.
            /// 
            /// In the case of a web seed, the server type and version
            /// will be a part of this string.
            std::string client_;
            
            /// standard_bittorrent or web_seed.
            t_uint connection_type_;
            

            /*
             * LT-13 -related parameters
             */
            
            /// Time since last request (milliseconds).
            t_uint last_request_;

            /// Time since last download or upload (seconds)
            t_uint last_active_;
            
            /// The number of failed hashes for this peer.
            t_uint num_hashfails_;

            /// The number of times this IP has failed to connect.
            t_uint failcount_;

            /// The number of requests that is tried to be maintained 
            /// (this is typically a function of download speed).
            t_uint target_dl_queue_length_;
            
            /// Approximate peer download rate.
            t_uint remote_dl_rate_;

         };
         
         /** @} */

      } // namespace core
} // namespace btg

#endif

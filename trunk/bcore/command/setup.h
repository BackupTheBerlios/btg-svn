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
 * Id    : $Id: setup.h,v 1.15.4.16 2007/06/02 18:31:30 jstrom Exp $
 */

#ifndef COMMAND_SETUP_H
#define COMMAND_SETUP_H

#include "command.h"
#include <bcore/dbuf.h>
#include <bcore/addrport.h>
#include <vector>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup commands
          */
         /** @{ */

         class requiredSetupData: public Serializable, public Printable
            {
            public:
               /// Default constructor.
               requiredSetupData();

               /// Constructor.
               /// @param [in] _buildID            The build ID.
               /// @param [in] _seedLimit          This is how many % we should seed before a torrent is stopped. -1 to seed forever.
               /// @param [in] _seedTimeout        This is how many seconds we should seed before a torrent is stopped. -1 to seed forever.
               /// @param [in] _useDHT             Indicates that DHT should be used.
               /// @param [in] _useEncryption             Indicates that encryption should be used.
               requiredSetupData(std::string const& _buildID,
                                 t_int const _seedLimit,
                                 t_long const _seedTimeout,
                                 bool const _useDHT,
                                 bool const _useEncryption);

               /// Get the build ID.
               std::string getBuildID() const;

               /// Returns value for seed limit. This is how many % we
               /// should seed before stopping torrent.
               t_int getSeedLimit() const;

               /// Returns value for seed timeout. This is how many
               /// seconds we should seed before stopping torrent.
               t_long getSeedTimeout() const;

               /// Indicates if DHT should be used.
               bool useDHT() const;

               /// Indicates if encryption should be used.
               bool useEncryption() const;
                
               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);
               std::string toString() const;

               /// Destructor.
               virtual ~requiredSetupData();
            private:
               /// Build ID, used to decide if a client should be able
               /// to connect.
               std::string   buildID_;

               /// Leech mode limit
               t_int         seedLimit_;

               /// Leech mode timeout
               t_long        seedTimeout_;

               /// Indicates if DHT should be used.
               bool          useDHT_;

               /// Indicates that encryption should be used.
               bool          useEncryption_;
            };

         /// Setup request, informs the daemon of the setup the client wishes
         /// to use.
         class setupCommand: public Command
            {
            public:
               /// Default constructor.
               setupCommand();

               /// Constructor.
               setupCommand(requiredSetupData const& _rsd);

               /// Get a reference to the data which is required for setup of a client.
               requiredSetupData const & getRequiredData() const;

               std::string toString() const;
               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Destructor.
               virtual ~setupCommand();
            private:
               /// Required data.
               requiredSetupData rsd_;
            };

         /// Response with id of the new session
         class setupResponseCommand: public Command
            {
            public:
               /// Default constructor.
               setupResponseCommand();
               /// Constructor.
               /// @param [in] _session The session id.
               setupResponseCommand(t_long const _session);

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Get the contained session.
               t_long getSession() const;

               /// Set the contained session.
               void setSession(t_long const _session);

               /// Destructor.
               virtual ~setupResponseCommand();
            private:
               /// Session ID
               t_long session_;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif

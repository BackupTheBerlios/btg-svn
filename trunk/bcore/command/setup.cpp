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

#include "setup.h"
#include <bcore/t_string.h>
#include <bcore/helpermacro.h>
#include <bcore/command/limit_base.h>

#include <iostream>

namespace btg
{
   namespace core
   {
      requiredSetupData::requiredSetupData():
         buildID_(),
         seedLimit_(btg::core::limitBase::LIMIT_DISABLED),
         seedTimeout_(btg::core::limitBase::LIMIT_DISABLED),
         useDHT_(false),
         useEncryption_(false)
      {
      }

      requiredSetupData::requiredSetupData(std::string const& _buildID,
                                           t_int const _seedLimit,
                                           t_long const _seedTimeout,
                                           bool const _useDHT,
                                           bool const _useEncryption)
         : buildID_(_buildID),
           seedLimit_(_seedLimit),
           seedTimeout_(_seedTimeout),
           useDHT_(_useDHT),
           useEncryption_(_useEncryption)
      {

      }

      std::string requiredSetupData::getBuildID() const
      {
         return buildID_;
      }

      t_int requiredSetupData::getSeedLimit() const
      {
         return seedLimit_;
      }

      t_long requiredSetupData::getSeedTimeout() const
      {
         return seedTimeout_;
      }

      bool requiredSetupData::useDHT() const
      {
         return useDHT_;
      }

      bool requiredSetupData::useEncryption() const
      {
         return useEncryption_;
      }
       
      bool requiredSetupData::serialize(btg::core::externalization::Externalization* _e) const
      {
         _e->setParamInfo("build id", true);
         BTG_RCHECK( _e->stringToBytes(&buildID_) );

         _e->setParamInfo("flag: seed limit", true);
         BTG_RCHECK( _e->intToBytes(&seedLimit_) );

         _e->setParamInfo("flag: seed timeout", true);
         BTG_RCHECK( _e->longToBytes(&seedTimeout_) );

         _e->setParamInfo("flag: use DHT", true);
         BTG_RCHECK( _e->boolToBytes(useDHT_) );

         _e->setParamInfo("flag: use Encryption", true);
         BTG_RCHECK( _e->boolToBytes(useEncryption_) );

         return true;
      }

      bool requiredSetupData::deserialize(btg::core::externalization::Externalization* _e)
      {
         _e->setParamInfo("build id", true);
         BTG_RCHECK( _e->bytesToString(&buildID_) );

         _e->setParamInfo("flag: seed limit", true);
         BTG_RCHECK( _e->bytesToInt(&seedLimit_) );

         _e->setParamInfo("flag: seed timeout", true);
         BTG_RCHECK( _e->bytesToLong(&seedTimeout_) );

         _e->setParamInfo("flag: use DHT", true);
         BTG_RCHECK( _e->bytesToBool(useDHT_) );

         _e->setParamInfo("flag: use Encryption", true);
         BTG_RCHECK( _e->bytesToBool(useEncryption_) );

         return true;
      }

      std::string requiredSetupData::toString() const
      {
         std::stringstream output;

         output << "\n";
         output << "Required Data:";
         output << "\n";
         output << "leech mode limit " << seedLimit_ << "%, timeout " << seedTimeout_ << "s.";
         output << "\n";
         output << "use encryption: " << useEncryption_ << ".";
         output << "\n";
         output << "use DHT: " << useDHT_ << ".";

         return output.str();
      }

      requiredSetupData::~requiredSetupData()
      {

      }

      /*
       *
       */

      setupCommand::setupCommand()
         : Command(Command::CN_GSETUP),
           rsd_()
      {
      }

      setupCommand::setupCommand(requiredSetupData const& _rsd)
         : Command(Command::CN_GSETUP),
           rsd_(_rsd)
      {}

      requiredSetupData const & setupCommand::getRequiredData() const
      {
         return rsd_;
      }

      std::string setupCommand::toString() const
      {
         std::string output = Command::toString();

         output += rsd_.toString();

         return output;
      }

      bool setupCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         _e->setParamInfo("required part", true);
         BTG_RCHECK( rsd_.serialize(_e) );

         return true;
      }

      bool setupCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         _e->setParamInfo("required part", true);
         BTG_RCHECK( rsd_.deserialize(_e) );

         return true;
      }

      setupCommand::~setupCommand()
      {}

      /*
       *
       */
      setupResponseCommand::setupResponseCommand()
         : Command(Command::CN_GSETUPRSP),
           session_(0)
      {

      }

      setupResponseCommand::setupResponseCommand(t_long const _session)
         : Command(Command::CN_GSETUPRSP),
           session_(_session)
      {

      }

      bool setupResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         _e->setParamInfo("Session", true);
         BTG_RCHECK(_e->longToBytes(&session_) );

         return true;
      }

      bool setupResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         _e->setParamInfo("Session", true);
         BTG_RCHECK( _e->bytesToLong(&session_) );

         return true;
      }

      t_long setupResponseCommand::getSession() const
      {
         return session_;
      }

      void setupResponseCommand::setSession(t_long const _session)
      {
         session_ = _session;
      }

      setupResponseCommand::~setupResponseCommand()
      {

      }

   } // namespace core
} // namespace btg

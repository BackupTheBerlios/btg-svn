/*
 * btg Copyright (C) 2008 Michael Wojciechowski.
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
 * Id    : $Id$
 */

#include "opstat.h"
#include <bcore/opstatus.h>
#include <bcore/helpermacro.h>
#include <bcore/t_string.h>

namespace btg
{
   namespace core
   {
      opStatusCommand::opStatusCommand()
         : Command(Command::CN_OPSTATUS),
           id_(OPSTAT_INVALID_ID),
           type_()
      {
      }

      opStatusCommand::opStatusCommand(t_uint const _id,
                                                     t_uint const _type)
         : Command(Command::CN_OPSTATUS),
           id_(_id),
           type_(_type)
      {
      }

      bool opStatusCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         Command::serialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("id", true);
         _e->uintToBytes(&id_);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("type", true);
         _e->uintToBytes(&type_);
         BTG_RCHECK(_e->status());

         return true;
      }

      bool opStatusCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         Command::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("id", true);
         _e->bytesToUint(&id_);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("id", true);
         _e->bytesToUint(&type_);
         BTG_RCHECK(_e->status());

         return true;
      }

      opStatusCommand::~opStatusCommand()
      {

      }

      /* */

      opStatusResponseCommand::opStatusResponseCommand()
         : Command(Command::CN_OPSTATUSRSP),
           id_(OPSTAT_INVALID_ID),
           type_(ST_UNDEF),
           status_(OP_UNDEF),
           dataSet_(false),
           data_()
      {
      }

      opStatusResponseCommand::opStatusResponseCommand(t_uint _id, 
                                                                     t_uint _type,
                                                                     t_uint _status)
         : Command(Command::CN_OPSTATUSRSP),
           id_(_id),
           type_(_type),
           status_(_status),
           dataSet_(false),
           data_()
      {

      }

      void opStatusResponseCommand::setData(std::string const& _data)
         {
            data_ = _data;
            if (data_.size() > 0)
               {
                  dataSet_ = true;
               }
            else
               {
                  dataSet_ = false;
               }
         }

      bool opStatusResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         Command::serialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("id", true);
         _e->uintToBytes(&id_);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("type", true);
         _e->uintToBytes(&type_);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("status", true);
         _e->uintToBytes(&status_);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("data set indicator", true);
         _e->boolToBytes(dataSet_);
         BTG_RCHECK(_e->status());
         
         if (dataSet_)
         {
            _e->setParamInfo("data", false);
            _e->stringToBytes(&data_);
            BTG_RCHECK(_e->status());
         }

         return true;
      }

      bool opStatusResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         Command::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("id", true);
         _e->bytesToUint(&id_);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("type", true);
         _e->bytesToUint(&type_);
         BTG_RCHECK(_e->status());
         
         _e->setParamInfo("status", true);
         _e->bytesToUint(&status_);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("data set indicator", true);
         _e->bytesToBool(dataSet_);
         BTG_RCHECK(_e->status());
         
         if (dataSet_)
         {
            _e->setParamInfo("data", false);
            _e->bytesToString(&data_);
            BTG_RCHECK(_e->status());
         }
         
         return true;
      }

      opStatusResponseCommand::~opStatusResponseCommand()
      {

      }

      /* */

      opAbortCommand::opAbortCommand()
         : Command(Command::CN_OPABORT),
           id_(OPSTAT_INVALID_ID)
      {
      }

      opAbortCommand::opAbortCommand(t_uint const _id, 
                                                   t_uint const _type)
         : Command(Command::CN_OPABORT),
           id_(_id),
           type_(_type)
      {
      }

      bool opAbortCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         Command::serialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("url id", true);
         _e->uintToBytes(&id_);
         BTG_RCHECK(_e->status());

         return true;
      }

      bool opAbortCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         Command::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("url id", true);
         _e->bytesToUint(&id_);
         BTG_RCHECK(_e->status());

         return true;
      }

      opAbortCommand::~opAbortCommand()
      {

      }

   } // namespace core
} // namespace btg




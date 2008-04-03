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
 * $Id: context_create_url.cpp 220 2008-03-20 15:04:21Z wojci $
 */

#include "context_create_file.h"
#include <bcore/helpermacro.h>
#include <iostream>

namespace btg
{
   namespace core
   {
      contextCreateFromFileCommand::contextCreateFromFileCommand()
         : Command(Command::CN_CCREATEFROMFILE),
           filename(),
           numberOfParts_(0),
           start(false)
      {}

      contextCreateFromFileCommand::contextCreateFromFileCommand(std::string const& _filename,
                                                                 t_uint _numberOfParts,
                                                                 bool const _start)
         : Command(Command::CN_CCREATEFROMFILE),
           filename(_filename),
           numberOfParts_(_numberOfParts),
           start(_start)
      {}

      bool contextCreateFromFileCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         Command::serialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("torrent filename", true);
         _e->stringToBytes(&filename);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("number of parts", true);
         _e->uintToBytes(&numberOfParts_);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("flag: start torrent", true);
         BTG_RCHECK( _e->boolToBytes(start) );

         return true;
      }

      bool contextCreateFromFileCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         Command::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("torrent filename", true);
         _e->bytesToString(&filename);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("number of parts", true);
         _e->bytesToUint(&numberOfParts_);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("flag: start torrent", true);
         BTG_RCHECK( _e->bytesToBool(start) );

         return true;
      }

      contextCreateFromFileCommand::~contextCreateFromFileCommand()
      {}

      /* */
      
      contextCreateFromFileResponseCommand::contextCreateFromFileResponseCommand()
         : Command(Command::CN_CCREATEFROMFILERSP),
           id_(FILES_INVALID_FILEID)
      {}

      contextCreateFromFileResponseCommand::contextCreateFromFileResponseCommand(t_uint const _id)
         : Command(Command::CN_CCREATEFROMFILERSP),
           id_(_id)
      {}

      bool contextCreateFromFileResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         Command::serialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("url id", true);
         _e->uintToBytes(&id_);
         BTG_RCHECK(_e->status());

         return true;
      }

      bool contextCreateFromFileResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         Command::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("url id", true);
         _e->bytesToUint(&id_);
         BTG_RCHECK(_e->status());

         return true;
      }

      contextCreateFromFileResponseCommand::~contextCreateFromFileResponseCommand()
      {}

      /* */

      contextCreateFromFilePartCommand::contextCreateFromFilePartCommand()
         : Command(Command::CN_CCREATEFROMFILEPART),
           id_(FILES_INVALID_FILEID),
           part_(0),
           data_()
      {
      }

      contextCreateFromFilePartCommand::contextCreateFromFilePartCommand(t_uint _id,
                                                                         t_uint _part,
                                                                         sBuffer const& _data)
         : Command(Command::CN_CCREATEFROMFILEPART),
           id_(_id),
           part_(_part),
           data_(_data)
      {

      }

      bool contextCreateFromFilePartCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         Command::serialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("id", true);
         _e->uintToBytes(&id_);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("part", true);
         _e->uintToBytes(&part_);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("data", true);
         BTG_RCHECK( data_.serialize(_e) );

         return true;
      }

      bool contextCreateFromFilePartCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         Command::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("id", true);
         _e->bytesToUint(&id_);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("part", true);
         _e->bytesToUint(&part_);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("data", true);
         BTG_RCHECK( data_.deserialize(_e) );

         return true;
      }

      contextCreateFromFilePartCommand::~contextCreateFromFilePartCommand()
      {

      }

      /* */
      
      contextFileStatusCommand::contextFileStatusCommand()
         : Command(Command::CN_CCRFILESTATUS),
           id_(FILES_INVALID_FILEID)
      {}

      contextFileStatusCommand::contextFileStatusCommand(t_uint const _id)
         : Command(Command::CN_CCRFILESTATUS),
           id_(_id)
      {}

      bool contextFileStatusCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         Command::serialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("url id", true);
         _e->uintToBytes(&id_);
         BTG_RCHECK(_e->status());

         return true;
      }

      bool contextFileStatusCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         Command::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("url id", true);
         _e->bytesToUint(&id_);
         BTG_RCHECK(_e->status());

         return true;
      }

      contextFileStatusCommand::~contextFileStatusCommand()
      {}

      /* */

      contextFileStatusResponseCommand::contextFileStatusResponseCommand()
         : Command(Command::CN_CCRFILESTATUSRSP),
           id_(FILES_INVALID_FILEID),
           status_(FILES_UNDEF)
      {}

      contextFileStatusResponseCommand::contextFileStatusResponseCommand(t_uint const _id,
                                                                         btg::core::fileStatus const _status)
         : Command(Command::CN_CCRFILESTATUSRSP),
           id_(_id),
           status_(_status)
      {}

      bool contextFileStatusResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         Command::serialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("url id", true);
         _e->uintToBytes(&id_);
         BTG_RCHECK(_e->status());

         t_uint temp_status = 0;
         switch (status_)
            {
            case FILES_UNDEF:
               temp_status = FILES_UNDEF;
               break;
            case FILES_WORKING:
               temp_status = FILES_WORKING;
               break;
            case FILES_FINISHED:
               temp_status = FILES_FINISHED;
               break;
            case FILES_ERROR:
               temp_status = FILES_ERROR;
               break;
            case FILES_CREATE:
               temp_status = FILES_CREATE;
               break;
            case FILES_CREATE_ERR:
               temp_status = FILES_CREATE_ERR;
               break;
            }

         _e->setParamInfo("url status", true);
         _e->uintToBytes(&temp_status);
         BTG_RCHECK(_e->status());

         return true;
      }

      bool contextFileStatusResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         Command::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("url id", true);
         _e->bytesToUint(&id_);
         BTG_RCHECK(_e->status());

         t_uint temp_status = 0;
         _e->setParamInfo("url status", true);
         _e->bytesToUint(&temp_status);
         BTG_RCHECK(_e->status());
         
         switch (temp_status)
            {
            case FILES_UNDEF:
               status_ = FILES_UNDEF;
               break;
            case FILES_WORKING:
               status_ = FILES_WORKING;
               break;
            case FILES_FINISHED:
               status_ = FILES_FINISHED;
               break;
            case FILES_ERROR:
               status_ = FILES_ERROR;
               break;
            case FILES_CREATE:
               status_ = FILES_CREATE;
               break;
            case FILES_CREATE_ERR:
               status_ = FILES_CREATE_ERR;
               break;
            default:
               status_ = FILES_UNDEF;
               break;
            }

         return true;
      }

      contextFileStatusResponseCommand::~contextFileStatusResponseCommand()
      {}

   } // namespace core
} // namespace btg

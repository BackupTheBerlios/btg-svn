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
 * $Id$
 */

#include "context_create_url.h"
#include <bcore/helpermacro.h>
#include <iostream>

namespace btg
{
   namespace core
   {
      contextCreateFromUrlCommand::contextCreateFromUrlCommand()
         : Command(Command::CN_CCREATEFROMURL),
           filename(),
           url(),
           start(false)
      {}

      contextCreateFromUrlCommand::contextCreateFromUrlCommand(std::string const& _filename,
                                                               std::string const& _url,
                                                               bool const _start)
         : Command(Command::CN_CCREATEFROMURL),
           filename(_filename),
           url(_url),
           start(_start)
      {}

      std::string contextCreateFromUrlCommand::toString() const
      {
         std::string output = Command::toString() + " " + "Filename = " + filename + ".";
         if (start)
            {
               output += " Start flag set.";
            }

         return output;
      }

      bool contextCreateFromUrlCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         Command::serialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("torrent filename", true);
         _e->stringToBytes(&filename);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("torrent url", true);
         _e->stringToBytes(&url);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("flag: start torrent", true);
         BTG_RCHECK( _e->boolToBytes(start) );

         return true;
      }

      bool contextCreateFromUrlCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         Command::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("torrent filename", true);
         _e->bytesToString(&filename);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("torrent url", true);
         _e->bytesToString(&url);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("flag: start torrent", true);
         BTG_RCHECK( _e->bytesToBool(start) );

         return true;
      }

      contextCreateFromUrlCommand::~contextCreateFromUrlCommand()
      {}

      /* */
      
      contextCreateFromUrlResponseCommand::contextCreateFromUrlResponseCommand()
         : Command(Command::CN_CCREATEFROMURLRSP),
           id_(URLS_INVALID_URLID)
      {}

      contextCreateFromUrlResponseCommand::contextCreateFromUrlResponseCommand(t_uint const _id)
         : Command(Command::CN_CCREATEFROMURLRSP),
           id_(_id)
      {}

      bool contextCreateFromUrlResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         Command::serialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("url id", true);
         _e->uintToBytes(&id_);
         BTG_RCHECK(_e->status());

         return true;
      }

      bool contextCreateFromUrlResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         Command::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("url id", true);
         _e->bytesToUint(&id_);
         BTG_RCHECK(_e->status());

         return true;
      }

      contextCreateFromUrlResponseCommand::~contextCreateFromUrlResponseCommand()
      {}

      /* */
      
      contextUrlStatusCommand::contextUrlStatusCommand()
         : Command(Command::CN_CURLSTATUS),
           id_(URLS_INVALID_URLID)
      {}

      contextUrlStatusCommand::contextUrlStatusCommand(t_uint const _id)
         : Command(Command::CN_CURLSTATUS),
           id_(_id)
      {}

      bool contextUrlStatusCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         Command::serialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("url id", true);
         _e->uintToBytes(&id_);
         BTG_RCHECK(_e->status());

         return true;
      }

      bool contextUrlStatusCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         Command::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("url id", true);
         _e->bytesToUint(&id_);
         BTG_RCHECK(_e->status());

         return true;
      }

      contextUrlStatusCommand::~contextUrlStatusCommand()
      {}

      /* */

      contextUrlStatusResponseCommand::contextUrlStatusResponseCommand()
         : Command(Command::CN_CURLSTATUSRSP),
           id_(URLS_INVALID_URLID),
           status_(URLS_UNDEF)
      {}

      contextUrlStatusResponseCommand::contextUrlStatusResponseCommand(t_uint const _id,
                                                                       btg::core::urlStatus const _status)
         : Command(Command::CN_CURLSTATUSRSP),
           id_(_id),
           status_(_status),
           bDlProgress_(false),
           dltotal_(0.0),
           dlnow_(0.0),
           dlspeed_(0.0)
      {}

      bool contextUrlStatusResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         Command::serialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("url id", true);
         _e->uintToBytes(&id_);
         BTG_RCHECK(_e->status());

         t_uint temp_status = 0;
         switch (status_)
            {
            case URLS_UNDEF:
               temp_status = URLS_UNDEF;
               break;
            case URLS_WORKING:
               temp_status = URLS_WORKING;
               break;
            case URLS_FINISHED:
               temp_status = URLS_FINISHED;
               break;
            case URLS_ERROR:
               temp_status = URLS_ERROR;
               break;
            case URLS_CREATE:
               temp_status = URLS_CREATE;
               break;
            case URLS_CREATE_ERR:
               temp_status = URLS_CREATE_ERR;
               break;
            case URLS_ABORTED:
               temp_status = URLS_ABORTED;
               break;
            }

         _e->setParamInfo("url status", true);
         _e->uintToBytes(&temp_status);
         BTG_RCHECK(_e->status());
         
         _e->setParamInfo("donwload progress info flag", true);
         _e->boolToBytes(bDlProgress_);
         BTG_RCHECK(_e->status());
         
         if (bDlProgress_)
         {
            _e->setParamInfo("total bytes to download", false);
            _e->floatToBytes(&dltotal_);
            BTG_RCHECK(_e->status());
            
            _e->setParamInfo("bytes downloaded", false);
            _e->floatToBytes(&dlnow_);
            BTG_RCHECK(_e->status());
            
            _e->setParamInfo("download speed (bytes/sec)", false);
            _e->floatToBytes(&dlspeed_);
            BTG_RCHECK(_e->status());
         }

         return true;
      }

      bool contextUrlStatusResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
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
            case URLS_UNDEF:
               status_ = URLS_UNDEF;
               break;
            case URLS_WORKING:
               status_ = URLS_WORKING;
               break;
            case URLS_FINISHED:
               status_ = URLS_FINISHED;
               break;
            case URLS_ERROR:
               status_ = URLS_ERROR;
               break;
            case URLS_CREATE:
               status_ = URLS_CREATE;
               break;
            case URLS_CREATE_ERR:
               status_ = URLS_CREATE_ERR;
               break;
            case URLS_ABORTED:
               status_ = URLS_ABORTED;
               break;
            default:
               status_ = URLS_UNDEF;
               break;
            }

         _e->setParamInfo("donwload progress info flag", true);
         _e->bytesToBool(bDlProgress_);
         BTG_RCHECK(_e->status());
         
         if (bDlProgress_)
         {
            _e->setParamInfo("total bytes to download", false);
            _e->bytesToFloat(&dltotal_);
            BTG_RCHECK(_e->status());
            
            _e->setParamInfo("bytes downloaded", false);
            _e->bytesToFloat(&dlnow_);
            BTG_RCHECK(_e->status());
            
            _e->setParamInfo("download speed (bytes/sec)", false);
            _e->bytesToFloat(&dlspeed_);
            BTG_RCHECK(_e->status());
         }
         
         return true;
      }

      contextUrlStatusResponseCommand::~contextUrlStatusResponseCommand()
      {}

      /* */
      
      contextUrlAbortCommand::contextUrlAbortCommand()
         : Command(Command::CN_CCREATEURLABORT),
           id_(URLS_INVALID_URLID)
      {}

      contextUrlAbortCommand::contextUrlAbortCommand(t_uint const _id)
         : Command(Command::CN_CCREATEURLABORT),
           id_(_id)
      {}

      bool contextUrlAbortCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         Command::serialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("url id", true);
         _e->uintToBytes(&id_);
         BTG_RCHECK(_e->status());

         return true;
      }

      bool contextUrlAbortCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         Command::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("url id", true);
         _e->bytesToUint(&id_);
         BTG_RCHECK(_e->status());

         return true;
      }

      contextUrlAbortCommand::~contextUrlAbortCommand()
      {}
      
   } // namespace core
} // namespace btg

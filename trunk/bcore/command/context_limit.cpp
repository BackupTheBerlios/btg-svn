/*
 * btg Copyright (C) 2005 Jesper Nyholm Jensen.
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

#include "context_limit.h"
#include <bcore/helpermacro.h>

#include <iostream>

namespace btg
{
   namespace core
   {
      contextLimitCommand::contextLimitCommand()
         : contextCommand(Command::CN_CLIMIT, contextCommand::UNDEFINED_CONTEXT, false),
           limitDwnld(0),
           limitUpld(0),
           seedLimit(0),
           seedTimeout(0)
      {}

      contextLimitCommand::contextLimitCommand(t_int const _context_id, t_int const _limitBytesUpld, t_int const _limitBytesDwnld, t_int const _seedLimit, t_long const _seedTimeout, bool const _allContexts)
         : contextCommand(Command::CN_CLIMIT, _context_id, _allContexts),
           limitDwnld(_limitBytesDwnld),
           limitUpld(_limitBytesUpld),
           seedLimit(_seedLimit),
           seedTimeout(_seedTimeout)
      {}

      contextLimitCommand::~contextLimitCommand()
      {}

      t_int contextLimitCommand::getUploadLimit() const
      {
         return this->limitUpld;
      }

      t_int contextLimitCommand::getDownloadLimit() const
      {
         return this->limitDwnld;
      }

      t_int contextLimitCommand::getSeedLimit() const
      {
         return this->seedLimit;
      }

      t_long contextLimitCommand::getSeedTimeout() const
      {
         return this->seedTimeout;
      }

      bool contextLimitCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( contextCommand::serialize(_e) );

         _e->setParamInfo("upload speed in bytes/sec", true);
         BTG_RCHECK( _e->intToBytes(&this->limitUpld) );

         _e->setParamInfo("download speed in bytes/sec", true);
         BTG_RCHECK( _e->intToBytes(&this->limitDwnld) );

         _e->setParamInfo("percents to seed before torrent is stopped", true);
         BTG_RCHECK( _e->intToBytes(&this->seedLimit) );

         _e->setParamInfo("seconds to seed before torrent is stopped", true);
         BTG_RCHECK( _e->longToBytes(&this->seedTimeout) );

         return true;
      }

      bool contextLimitCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( contextCommand::deserialize(_e) );

         _e->setParamInfo("upload speed in bytes/sec", true);
         BTG_RCHECK( _e->bytesToInt(&this->limitUpld) );

         if(this->limitUpld < 0 && this->limitUpld != limitBase::LIMIT_DISABLED)
            {
               // BTG_NOTICE("Deserialize of contextLimitCommand failed, limitUpld is negative but not limitBase::LIMIT_DISABLED");
               return false;
            }

         _e->setParamInfo("download speed in bytes/sec", true);
         BTG_RCHECK( _e->bytesToInt(&this->limitDwnld) );

         if(this->limitDwnld < 0 && this->limitDwnld != limitBase::LIMIT_DISABLED)
            {
               // BTG_NOTICE("Deserialize of contextLimitCommand failed, limitUpld is negative but not limitBase::LIMIT_DISABLED");
               return false;
            }

         _e->setParamInfo("percents to seed before torrent is stopped", true);
         BTG_RCHECK( _e->bytesToInt(&this->seedLimit) );

         if(this->seedLimit < 0 && this->seedLimit != limitBase::LIMIT_DISABLED)
            {
               // BTG_NOTICE("Deserialize of contextLimitCommand failed, seedLimit is negative but not limitBase::LIMIT_DISABLED");
               return false;
            }

         _e->setParamInfo("seconds to seed before torrent is stopped", true);
         BTG_RCHECK( _e->bytesToLong(&this->seedTimeout) );

         if(this->seedTimeout < 0 && this->seedTimeout != limitBase::LIMIT_DISABLED)
            {
               // BTG_NOTICE("Deserialize of contextLimitCommand failed, seedTimeout is negative but not limitBase::LIMIT_DISABLED");
               return false;
            }

         return true;
      }
   } // namespace core
} // namespace btg

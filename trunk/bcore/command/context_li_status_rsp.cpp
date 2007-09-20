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
 * $Id: context_li_status_rsp.cpp,v 1.2.4.11 2006/08/05 23:21:09 wojci Exp $
 */

#include "context_li_status_rsp.h"
#include <bcore/helpermacro.h>

#include <iostream>

namespace btg
{
   namespace core
   {
      contextLimitStatusResponseCommand::contextLimitStatusResponseCommand()
         : contextCommand(Command::CN_CLIMITSTATUSRSP,
                          contextCommand::UNDEFINED_CONTEXT,
                          false),
           limitDwnld(btg::core::limitBase::LIMIT_DISABLED),
           limitUpld(btg::core::limitBase::LIMIT_DISABLED),
           seedLimit(btg::core::limitBase::LIMIT_DISABLED),
           seedTimeout(btg::core::limitBase::LIMIT_DISABLED)
      {
      }

      contextLimitStatusResponseCommand::contextLimitStatusResponseCommand(t_int const _context_id,
                                                                           t_int const _limitBytesUpld,
                                                                           t_int const _limitBytesDwnld,
                                                                           t_int const _seedLimit,
                                                                           t_long const _seedTimeout)
         : contextCommand(Command::CN_CLIMITSTATUSRSP,
                          _context_id,
                          false),
           limitDwnld(_limitBytesDwnld),
           limitUpld(_limitBytesUpld),
           seedLimit(_seedLimit),
           seedTimeout(_seedTimeout)
      {
      }

      contextLimitStatusResponseCommand::~contextLimitStatusResponseCommand()
      {}

      t_int contextLimitStatusResponseCommand::getUploadLimit() const
      {
         return this->limitUpld;
      }

      t_int contextLimitStatusResponseCommand::getDownloadLimit() const
      {
         return this->limitDwnld;
      }

      t_int contextLimitStatusResponseCommand::getSeedLimit() const
      {
         return this->seedLimit;
      }

      t_long contextLimitStatusResponseCommand::getSeedTimeout() const
      {
         return this->seedTimeout;
      }

      bool contextLimitStatusResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( contextCommand::serialize(_e));

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

      bool contextLimitStatusResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( contextCommand::deserialize(_e) );

         _e->setParamInfo("upload speed in bytes/sec", true);
         BTG_RCHECK( _e->bytesToInt(&this->limitUpld) );

         _e->setParamInfo("download speed in bytes/sec", true);
         BTG_RCHECK( _e->bytesToInt(&this->limitDwnld) );

         _e->setParamInfo("percents to seed before torrent is stopped", true);
         BTG_RCHECK( _e->bytesToInt(&this->seedLimit) );

         _e->setParamInfo("seconds to seed before torrent is stopped", true);
         BTG_RCHECK( _e->bytesToLong(&this->seedTimeout) );

         return true;
      }
   } // namespace core
} // namespace btg


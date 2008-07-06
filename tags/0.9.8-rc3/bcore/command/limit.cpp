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

#include "limit.h"
#include <bcore/helpermacro.h>

#include <iostream>

namespace btg
{
   namespace core
   {
      limitCommand::limitCommand()
         : Command(Command::CN_GLIMIT),
           limitDwnld(0),
           limitUpld(0),
           maxUplds(0),
           maxConnections(0)
      {}

      limitCommand::limitCommand(t_int const  _limitBytesUpld,
                                 t_int const  _limitBytesDwnld,
                                 t_int const  _maxUplds,
                                 t_long const _maxConnections)
         : Command(Command::CN_GLIMIT),
           limitDwnld(_limitBytesDwnld),
           limitUpld(_limitBytesUpld),
           maxUplds(_maxUplds),
           maxConnections(_maxConnections)
      {}

      limitCommand::~limitCommand()
      {}

      t_int limitCommand::getUploadLimit() const
      {
         return this->limitUpld;
      }

      t_int limitCommand::getDownloadLimit() const
      {
         return this->limitDwnld;
      }

      t_int limitCommand::getMaxUplds() const
      {
         return maxUplds;
      }

      t_int limitCommand::getMaxConnections() const
      {
         return maxConnections;
      }

      bool limitCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         _e->setParamInfo("upload speed in bytes/sec", true);
         BTG_RCHECK( _e->intToBytes(&this->limitUpld) );

         _e->setParamInfo("download speed in bytes/sec", true);
         BTG_RCHECK( _e->intToBytes(&this->limitDwnld) );

         _e->setParamInfo("Max uploads", true);
         BTG_RCHECK( _e->intToBytes(&this->maxUplds) );

         _e->setParamInfo("Max connections", true);
         BTG_RCHECK( _e->intToBytes(&this->maxConnections) );

         return true;
      }

      bool limitCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         _e->setParamInfo("upload speed in bytes/sec", true);
         BTG_RCHECK( _e->bytesToInt(&this->limitUpld) );

         if (this->limitUpld < 0 && this->limitUpld != limitBase::LIMIT_DISABLED)
            {
               // BTG_NOTICE("Deserialize of limitCommand failed, limitUpld is negative but not limitBase::LIMIT_DISABLED");
               return false;
            }

         _e->setParamInfo("download speed in bytes/sec", true);
         BTG_RCHECK( _e->bytesToInt(&this->limitDwnld) );

         if (this->limitDwnld < 0 && this->limitDwnld != LIMIT_DISABLED)
            {
               // BTG_NOTICE("Deserialize of limitCommand failed, limitUpld is negative but not limitBase::LIMIT_DISABLED");
               return false;
            }

         _e->setParamInfo("Max uploads", true);
         BTG_RCHECK( _e->bytesToInt(&this->maxUplds) );

         if (this->maxUplds < 0 && this->maxUplds != limitBase::LIMIT_DISABLED)
            {
               // BTG_NOTICE("Deserialize of limitCommand failed, maxUplds is negative but not limitBase::LIMIT_DISABLED");
               return false;
            }

         _e->setParamInfo("Max connections", true);
         BTG_RCHECK( _e->bytesToInt(&this->maxConnections) );

         if (this->maxConnections < 0 && this->maxConnections != limitBase::LIMIT_DISABLED)
            {
               // BTG_NOTICE("Deserialize of limitCommand failed, maxConnections is negative but not limitBase::LIMIT_DISABLED");
               return false;
            }

         return true;
      }

      /* */
      /* */
      /* */

      limitStatusCommand::limitStatusCommand()
         : Command(Command::CN_GLIMITSTAT)
      {

      }

      bool limitStatusCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );
         return true;
      }

      bool limitStatusCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );
         return true;
      }

      limitStatusCommand::~limitStatusCommand()
      {
      }
      
      /* */
      /* */
      /* */

      limitStatusResponseCommand::limitStatusResponseCommand()
         : Command(Command::CN_GLIMITSTATRSP),
           limitDwnld(0),
           limitUpld(0),
           maxUplds(0),
           maxConnections(0)
      {
      }

      limitStatusResponseCommand::limitStatusResponseCommand(t_int const  _limitBytesUpld,
                                                             t_int const  _limitBytesDwnld,
                                                             t_int const  _maxUplds,
                                                             t_long const _maxConnections)
         : Command(Command::CN_GLIMITSTATRSP),
           limitDwnld(_limitBytesDwnld),
           limitUpld(_limitBytesUpld),
           maxUplds(_maxUplds),
           maxConnections(_maxConnections)
      {
      }

      t_int limitStatusResponseCommand::getUploadLimit() const
      {
         return this->limitUpld;
      }

      t_int limitStatusResponseCommand::getDownloadLimit() const
      {
         return this->limitDwnld;
      }

      t_int limitStatusResponseCommand::getMaxUplds() const
      {
         return maxUplds;
      }

      t_int limitStatusResponseCommand::getMaxConnections() const
      {
         return maxConnections;
      }

      bool limitStatusResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         _e->setParamInfo("upload speed in bytes/sec", true);
         BTG_RCHECK( _e->intToBytes(&this->limitUpld) );

         _e->setParamInfo("download speed in bytes/sec", true);
         BTG_RCHECK( _e->intToBytes(&this->limitDwnld) );

         _e->setParamInfo("Max uploads", true);
         BTG_RCHECK( _e->intToBytes(&this->maxUplds) );

         _e->setParamInfo("Max connections", true);
         BTG_RCHECK( _e->intToBytes(&this->maxConnections) );

         return true;
      }
      
      bool limitStatusResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         _e->setParamInfo("upload speed in bytes/sec", true);
         BTG_RCHECK( _e->bytesToInt(&this->limitUpld) );

         if (this->limitUpld < 0 && this->limitUpld != limitBase::LIMIT_DISABLED)
            {
               // BTG_NOTICE("Deserialize of limitStatusResponseCommand failed, limitUpld is negative but not limitBase::LIMIT_DISABLED");
               return false;
            }

         _e->setParamInfo("download speed in bytes/sec", true);
         BTG_RCHECK( _e->bytesToInt(&this->limitDwnld) );

         if (this->limitDwnld < 0 && this->limitDwnld != LIMIT_DISABLED)
            {
               // BTG_NOTICE("Deserialize of limitStatusResponseCommand failed, limitUpld is negative but not limitBase::LIMIT_DISABLED");
               return false;
            }

         _e->setParamInfo("Max uploads", true);
         BTG_RCHECK( _e->bytesToInt(&this->maxUplds) );

         if (this->maxUplds < 0 && this->maxUplds != limitBase::LIMIT_DISABLED)
            {
               // BTG_NOTICE("Deserialize of limitStatusResponseCommand failed, maxUplds is negative but not limitBase::LIMIT_DISABLED");
               return false;
            }

         _e->setParamInfo("Max connections", true);
         BTG_RCHECK( _e->bytesToInt(&this->maxConnections) );

         if (this->maxConnections < 0 && this->maxConnections != limitBase::LIMIT_DISABLED)
            {
               // BTG_NOTICE("Deserialize of limitStatusResponseCommand failed, maxConnections is negative but not limitBase::LIMIT_DISABLED");
               return false;
            }

         return true;
      }

      limitStatusResponseCommand::~limitStatusResponseCommand()
      {

      }
      
   } // namespace core
} // namespace btg

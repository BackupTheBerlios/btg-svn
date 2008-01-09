/*
 * btg Copyright (C) 2005 Michael Wojciechowski.
 * Connection handler by Johan Ström.
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

#include <bcore/logmacro.h>
#include "connectionhandler.h"

#include "modulelog.h"

namespace btg
{
   namespace daemon
   {
      const std::string moduleName("coh");

      Connection::Connection(t_int _connectionID):
         connectionID_(_connectionID),
         sessionID_(0),
         isAuthed_(false),
         username_("")
      {
      }

      Connection::~Connection()
      {
      }

      ConnectionExtraState& Connection::ExtraState()
      {
         return extraState_;
      }

      std::string Connection::toString()
      {
         std::stringstream s;

         s << "Connection, id=" << connectionID_;
         if (isAuthed_)
            {
               s << " authed = " << username_;
            }
         else
            {
               s << " auth = none";
            }
         if (sessionID_ != 0)
            {
               s << ", session = " << sessionID_ << ".";
            }
         else
            {
               s << ", session = none.";
            }

         return s.str();
      }

      void Connection::setSession(t_long const _session)
      {
         // BTG_MNOTICE("connection " << connectionID_ << " changed session to " << _session);
         sessionID_ = _session;
      }

      t_long Connection::getSession() const
      {
         return sessionID_;
      }

      void Connection::setUsername(std::string const _username)
      {
         // BTG_MNOTICE("connection " << connectionID_ << " is now authed as user " << _username);
         username_ = _username;
         isAuthed_ = true;
      }

      std::string Connection::getUsername() const
      {
         return username_;
      }

      bool Connection::isAuthed() const
      {
         return isAuthed_;
      }

      /**************************************
       **************************************
       **************************************/

      connectionHandler::connectionHandler()
         : connections_()
      {
      }

      bool connectionHandler::addConnection(t_int const _connectionID)
      {
         // Make sure _connectionID is not already in use before we add it.
         if (getConnection(_connectionID) != 0)
            {
               // BTG_ERROR_LOG("connetionHandler::addConnection(), Tried to add connection " << _connectionID << ", but it already exists!");
               return false;
            }

         // BTG_MNOTICE("adding connection " << _connectionID);
         Connection * c = new Connection(_connectionID);
         connections_[_connectionID] = c;
         return true;
      }

      bool connectionHandler::removeConnection(t_int const _connectionID)
      {
         std::map<t_int, Connection*>::iterator iter = connections_.find(_connectionID);

         if (iter == connections_.end())
            {
               // BTG_ERROR_LOG("connetionHandler::removeConnection(), Tried to remove connection " << _connectionID << ", which we dont know anything about!");
               return false;
            }

         // BTG_MNOTICE("removing connection " << _connectionID);
         delete iter->second;
         iter->second = 0;

         connections_.erase(iter);

         return true;
      }

      Connection *connectionHandler::getConnection(t_int const _connectionID)
      {
         std::map<t_int, Connection*>::const_iterator iter = connections_.find(_connectionID);
         if (iter == connections_.end())
            {
               return 0;
            }

         return iter->second;
      }

      connectionHandler::~connectionHandler()
      {
         std::map<t_int, Connection*>::iterator iter;

         // Delete all Connections
         for (iter = connections_.begin(); iter != connections_.end(); iter++)
            {
               delete iter->second;
               iter->second = 0;
            }

         connections_.clear();
      }
   } // namespace daemon
} // namespace btg


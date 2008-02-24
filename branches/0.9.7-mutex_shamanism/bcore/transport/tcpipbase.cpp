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

#include "tcpipbase.h"

#include <bcore/logmacro.h>

#include <bcore/os/socket_exception.h>

namespace btg
{
   namespace core
   {
      tcpClient::tcpClient(LogWrapperType _logwrapper)
         : btg::core::Logable(_logwrapper),
           socket(0),
           connectionID(0)
      {
      }

      tcpClient::~tcpClient()
      {
#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE(logWrapper(), "tcpClient::~tcpClient, deleting socket " << socket);
#endif // BTG_TRANSPORT_DEBUG
         delete socket;
         socket=0;
      }

      bool tcpClient::operator== (tcpClient const & _tcpClient) const
      {
         bool status = false;

         if (this->socket == _tcpClient.socket)
            {
               status = true;
            }

         return status;
      }

      /* */
      /* */
      /* */

      tcpTransportBase::tcpTransportBase(LogWrapperType _logwrapper,
                                         btg::core::externalization::Externalization* _e,
                                         t_int const _bufferSize,
                                         DIRECTION const _direction,
                                         t_uint const _timeout)
         : btg::core::Logable(_logwrapper),
           messageTransport(_e, _bufferSize, _direction, _timeout),
           sndrec_buffer(new t_byte[_bufferSize]),
           server(0),
           client(0),
           clients(),
           socketGroup(_logwrapper, timeout)
      {
      }

      void tcpTransportBase::setSocket(btg::core::os::Socket* _socket)
      {
         switch (direction)
            {
            case TO_SERVER:
               {
                  if (client)
                     {
                        socketGroup.removeSocket(client);
                        delete client;
                        client = 0;
                     }

                  client = _socket;
                  server = 0;

                  if (client != 0)
                     {
                        socketGroup.addSocket(client);
                     }
                  break;
               }
            case FROM_SERVER:
               {
                  if(server)
                     {
                        socketGroup.removeSocket(server);
                        delete server;
                     }

                  server = _socket;
                  client = 0;

                  if(server != 0)
                     {
                        socketGroup.addSocket(server);
                     }
                  break;
               }
            }
      }

      t_int tcpTransportBase::write(dBuffer & _msg)
      {
#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE(logWrapper(), "tcpipTransport::write/0");
#endif // BTG_TRANSPORT_DEBUG

         // No connection id
         return write(_msg, NO_CONNECTION_ID);
      }

      t_int tcpTransportBase::write(dBuffer & _msg, t_int const _connectionID)
      {
#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE(logWrapper(), "tcpipTransport::write/1");
#endif // BTG_TRANSPORT_DEBUG
         t_int const message_size = _msg.size();

         if (message_size <= 0)
            {
               return tcpTransportBase::OPERATION_FAILED;
            }

         t_byteP message = new t_byte[message_size];

         if (message == 0)
            {
               return tcpTransportBase::OPERATION_FAILED;
            }

         _msg.getBytes(message, message_size);

         t_int ret_val = write(message, message_size, _connectionID);
         delete [] message;
         _msg.erase();
         return ret_val;
      }


      t_int tcpTransportBase::write(t_byteCP _msg, t_int const _size, t_int const _connectionID)
      {
#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE(logWrapper(), "tcpipTransport::write/2");
#endif // BTG_TRANSPORT_DEBUG
         t_int status = _size;

         switch (this->direction)
            {
            case TO_SERVER:
               {
                  if (!client->write(_msg, _size))
                     {
                        status = tcpTransportBase::OPERATION_FAILED;
                     }

                  break;
               }
            case FROM_SERVER:
               {
                  tcpClient *c = resolve(_connectionID);
                  if (!c)
                     {
#if BTG_TRANSPORT_DEBUG
                        BTG_NOTICE(logWrapper(), "tcpipTransport::write() Tried to write to unknown connection " << _connectionID << "!");
#endif // BTG_TRANSPORT_DEBUG
                        status = tcpTransportBase::OPERATION_FAILED;
                        break;
                     }

                  if (!c->socket->write(_msg, _size))
                     {
                        status = tcpTransportBase::OPERATION_FAILED;
                     }
                  break;
               }
            }

         return status;
      }

      t_int tcpTransportBase::read(dBuffer & _buffer)
      {
         if(this->direction != TO_SERVER)
            {
               BTG_ERROR_LOG(logWrapper(), "tcpTransportBase::read(dBuffer) can only be used on client side!! Not on server side!");
               return 0;
            }

         t_int connectionID;
         return read(_buffer, connectionID);
      }

      t_int tcpTransportBase::read(dBuffer & _buffer, t_int & _connectionID)
      {
         t_int status = tcpTransportBase::OPERATION_FAILED;

         _buffer.erase();
         _connectionID = NO_CONNECTION_ID;

#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE(logWrapper(), "tcpTransportBase::read/0");
#endif // BTG_TRANSPORT_DEBUG
         
         socketGroup.doSelect();

         if (socketGroup.size() == 0)
            {
               // No sockets with data, most likely timeout.
               return status;
            }

         switch (this->direction)
            {
            case TO_SERVER:
               {
                  // We only have one socket in the socketGroup so no
                  // need to determine which socket it was...

                  t_int rsize       = 0;

                  while (continueReading(client))
                     {
                        memset(sndrec_buffer, 0, buffer_size);
                        rsize = client->read(sndrec_buffer, buffer_size);

                        if (rsize > 0)
                           {
#if BTG_TRANSPORT_DEBUG
                              BTG_NOTICE(logWrapper(), "tcpTransportBase::read, got " << rsize << " bytes from connection.");
#endif // BTG_TRANSPORT_DEBUG
                              _buffer.addBytes(sndrec_buffer, rsize);
                           }
               			else
               			  {
               			    break;
               			  }
                     } // while

                  if (_buffer.size() > 0)
                     {
                        status = _buffer.size();
                     }

                  break;
               }
            case FROM_SERVER:
               {
#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE(logWrapper(), "tcpTransportBase::read, FROM_SERVER");
#endif // BTG_TRANSPORT_DEBUG

                  // Read from all clients.

                  btg::core::os::Socket* socket = 0;

                  while (socketGroup.getNext(socket))
                     {
                        if (socket == server)
                           {
                              // A new connection is here.
                              this->acceptNewConnections();
                              continue;
                           }

                        tcpClient* client = resolve(dynamic_cast<btg::core::os::Socket*>(socket));

                        if (client->socket->deleted() ||
                            client->socket->getSockId() == btg::core::os::Socket::UNINITIALIZED)
                           {
#if BTG_TRANSPORT_DEBUG
                              BTG_NOTICE(logWrapper(), "tcpTransportBase::read: deleting dead client " << client);
#endif // BTG_TRANSPORT_DEBUG
                              // Terminate the conneciotn
                              endConnection(client->connectionID);

                              // Client is gone now.
                              client = 0;

                              // Next socket.
                              continue;
                           }

                        t_int rsize       = 0;

                        while (continueReading(client->socket))
                           {
                              memset(sndrec_buffer, 0, buffer_size);
                              rsize = client->socket->read(sndrec_buffer, buffer_size);

                              if (rsize > 0)
                                 {
#if BTG_TRANSPORT_DEBUG
                                    BTG_NOTICE(logWrapper(), "tcpTransportBase::read, got " << rsize << " bytes from connection  "<< client->connectionID);
#endif // BTG_TRANSPORT_DEBUG
                                    _buffer.addBytes(sndrec_buffer, rsize);
                                 }
               			      else
                     				{
                     				  break;
                     				}
                           } // while

                        if (_buffer.size() > 0)
                           {
                              _connectionID = client->connectionID;
                              status        = _buffer.size();
                           }

                        break;
                     }

                  break;
               }
            }
         return status;
      }

      bool tcpTransportBase::continueReading(const btg::core::os::Socket* _socket)
      {
         // Only continue reading if it can be
         // done without blocking.

         bool result = false;

         t_uint oldTimeout = socketGroup.getTimeout();
         socketGroup.setTimeout(0);
         if (socketGroup.doSelect(_socket))
            {
               result = true;
            }
         socketGroup.setTimeout(oldTimeout);

#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE(logWrapper(), "tcpTransportBase::continueReading, socket id = " << _socket->getSockId() << ", result = " << result);
#endif // BTG_TRANSPORT_DEBUG

         return result;
      }

      tcpClient* tcpTransportBase::resolve(btg::core::os::Socket* _socket)
      {
         std::map<t_int, tcpClient*>::iterator iter;
         for(iter = clients.begin(); iter != clients.end(); iter++)
            {
               if(iter->second->socket == _socket)
                  return iter->second;
            }
#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE(logWrapper(), "resolve failed to lookup socket #" << _socket->getSockId());
#endif // BTG_TRANSPORT_DEBUG
         return NULL;
      }

      tcpClient * tcpTransportBase::resolve(t_int _connectionID)
      {
         std::map<t_int, tcpClient*>::iterator iter;
         iter = clients.find(_connectionID);
         if(iter != clients.end())
            {
               return iter->second;
            }
#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE(logWrapper(), "resolve failed to lookup connection " << _connectionID);
#endif // BTG_TRANSPORT_DEBUG
         return NULL;
      }

      bool tcpTransportBase::isInitialized() const
      {
         return initialized;
      }

      messageTransport::TRANSPORT tcpTransportBase::getType() const
      {
         return messageTransport::UNDEFINED;
      }

      void tcpTransportBase::endConnection(t_int _connectionID)
      {
#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE(logWrapper(), "tcpTransportBase::endConnection, Ending connection " << _connectionID);
#endif // BTG_TRANSPORT_DEBUG
         std::map<t_int, tcpClient*>::iterator iter = clients.find(_connectionID);

         if(iter == clients.end())
            {
#if BTG_TRANSPORT_DEBUG
               BTG_NOTICE(logWrapper(), "tcpTransportBase::endConnection, Connection " << _connectionID << " not found in clients map");
#endif // BTG_TRANSPORT_DEBUG
               return;
            }

         // Mark connection ID as dead
#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE(logWrapper(), "Marking connection ID " <<  _connectionID << " as dead.");
#endif // BTG_TRANSPORT_DEBUG
         dead_connections.push_back(_connectionID);

         socketGroup.removeSocket(iter->second->socket);

         delete iter->second;
         clients.erase(iter);
      }

      tcpTransportBase::~tcpTransportBase()
      {
#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE(logWrapper(), "tcpTransportBase::~tcpTransportBase, cleaning");
#endif // BTG_TRANSPORT_DEBUG
         std::map<t_int, tcpClient*>::iterator iter;
         for (iter = clients.begin(); iter != clients.end(); iter++)
            {
               clients.erase(iter);
            }

         // Delete all clients
         clients.clear();
         socketGroup.clear();

         delete client;
         client = 0;
         delete server;
         server = 0;

         delete [] sndrec_buffer;
         sndrec_buffer = 0;
      }

   } // namespace core
} // namespace btg

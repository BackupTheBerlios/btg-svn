/*
 * btg Copyright (C) 2005 Michael Wojciechowski.
 * HTTP/XMLRPC part written by Johan Ström.
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

#include "http.h"

#include <bcore/logmacro.h>
#include <bcore/util.h>
#include <bcore/os/socket_exception.h>
#include <bcore/os/sleep.h>

#include <bcore/os/gzip.h>

namespace btg
{
   namespace core
   {
      httpClient::httpClient(btg::core::os::gzipIf* _gzipif)
         : httpRequest(_gzipif)
      {

      }

      httpTransport::httpTransport(btg::core::externalization::Externalization* _e,
                                   t_int const _bufferSize,
                                   DIRECTION const _direction,
                                   addressPort const & _addressPort,
                                   t_uint const _timeout)
         : tcpTransport(_e, _bufferSize, _direction, _addressPort, true, _timeout),
           clientRequest(0),
           gzipif(new btg::core::os::gzip())
      {
         setup(_direction);
      }

      httpTransport::httpTransport(btg::core::externalization::Externalization* _e,
                                   t_int const _bufferSize,
                                   DIRECTION const _direction,
                                   addressPort const & _addressPort,
                                   bool const _createSocket,
                                   t_uint const _timeout)
         : tcpTransport(_e, _bufferSize, _direction, _addressPort, _createSocket, _timeout),
           clientRequest(0),
           gzipif(new btg::core::os::gzip())
      {
         setup(_direction);
      }

      void httpTransport::setup(DIRECTION const _direction)
      {
         if (isInitialized())
            {
               switch(_direction)
                  {
                  case TO_SERVER:
                     clientRequest = new httpRequestClientSide(this->getGzipIf());
                     break;
                  case FROM_SERVER:
                     clientRequest = 0;
                     break;
                  }
            }
      }

      t_int httpTransport::write(t_byteCP _msg, t_int const _size, t_int const _connectionID)
      {
         t_int status = _size;
         std::string data(reinterpret_cast<const char*>(_msg), _size);
#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE("httpTransport::write, size=" << _size);
#endif // BTG_TRANSPORT_DEBUG

         switch (this->direction)
            {
            case TO_SERVER:
               {
                  /* Add HTTP client request headers. Use \r\n for linebreaks
                   *
                   * POST / HTTP/1.1
                   * Host: < where is this? >
                   * Accept: * / * << no spaces.. but without spaces we would mess up this comment
                   * Content-Type: text/xml
                   * Content-Length: <bytes of data>
                   *
                   * End headers with dual linebreaks (\r\n\r\n) then follow with content-data (_msg).
                   */
                  std::stringstream ss;
                  ss << "POST / HTTP/1.0\r\n";
                  // ss << "Host: \r\n"; This is illegal to omit in http/1.1.. so speak http/1.0 then..
                  ss << "Accept: */*\r\n";
                  ss << "Accept-Encoding: gzip;q=1, *;q=0.1\r\n";
                  ss << "Content-Type: text/xml\r\n";
                  ss << "Content-Length: " << _size << "\r\n";
                  ss << "\r\n";

                  // We have to append() the message to a string since
                  // it may contain 0 chars (is this valid with
                  // text/xml?? only unittest does this anyways).

                  std::string buff(ss.str());
                  buff.append(data);

#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE("httpTransport::write(), Sending to server ("<<buff.length()<<" bytes)"); //  << buff);
#endif // BTG_TRANSPORT_DEBUG

                  if (!client->write(reinterpret_cast<t_byteCP>(buff.c_str()), buff.length()))
                     {
                        BTG_NOTICE("httpTransport::write(), Failed to write " << buff.length() << " bytes to server");
                        status = httpTransport::OPERATION_FAILED;
                     }
                  break;
               }
            case FROM_SERVER:
               {
                  tcpClient *c = resolve(_connectionID);
                  if (!c)
                     {
                        BTG_NOTICE("httpTransport::write() Tried to write to unknown connection " << _connectionID << "!");
                        status = httpTransport::OPERATION_FAILED;
                        break;
                     }

                  httpClient* client = dynamic_cast<httpClient*>(c);
#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE("httpTransport::write(), Using client (" << reinterpret_cast<void*>(client) << ") #" << client->socket->getSockId());
#endif // BTG_TRANSPORT_DEBUG

                  /* Add HTTP server response headers. Use \r\n for linebreaks
                   *
                   * 200 OK
                   * Server: btg-<BUILD>
                   * Date: Sun, 06 Nov 1994 08:49:37 GMT
                   * Content-Type: text/xml
                   * Content-Length: <bytes of data>
                   * Connection: keep-alive
                   *
                   * End headers with dual linebreaks (\r\n\r\n) then follow with content-data (_msg).
                   */
                  std::string accept_encoding;
                  bool use_gzip = false;
                  if (client->httpRequest.getHeader("Accept-Encoding", accept_encoding))
                     {
                        if (accept_encoding.find("gzip") != std::string::npos)
                           {
                              use_gzip = true;
                              std::string tmp;
#if BTG_TRANSPORT_DEBUG
                              BTG_NOTICE("httpTransport::write(), Using GZIP encoding. Uncompressed size: " << data.length()); // << ": " << buff);
#endif // BTG_TRANSPORT_DEBUG
                              try
                                 {
                                    gzipif->gzip_compress(data, tmp);
                                    data = tmp;
                                 }
                              catch(...)
                                 {
                                    use_gzip = false;
                                    BTG_ERROR_LOG("gzip_compress failed, not using compression.");
                                 }
                              
#if BTG_TRANSPORT_DEBUG
                              BTG_NOTICE("httpTransport::write(), compressed size: " << data.length()); // << ": " << buff);
#endif // BTG_TRANSPORT_DEBUG
                           }
                     }
                  client->httpRequest.reset();

                  std::stringstream ss;
                  ss << "HTTP/1.0 200 OK\r\n";
                  ss << "Server: btg-" << GPD->sBUILD() << "\r\n";
                  ss << "Date: " << createTimestamp() << "\r\n";
                  ss << "Content-Type: text/xml\r\n";
                  ss << "Content-Length: " << data.length() << "\r\n";
                  ss << "Connection: keep-alive\r\n";
                  if (use_gzip)
                     {
                        ss << "Content-Encoding: gzip\r\n";
                     }
                  ss << "\r\n";

                  std::string buff(ss.str());
                  buff.append(data);

#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE("httpTransport::write(), Sending to current session"); // << ": " << buff);
#endif // BTG_TRANSPORT_DEBUG

                  if (!client->socket->write(reinterpret_cast<t_byteCP>(buff.c_str()), buff.length()))
                     {
                        status = httpTransport::OPERATION_FAILED;
                     }
                  break;
               }
            }

         return status;
      }

      t_int httpTransport::read(dBuffer & _buffer)
      {
         if (this->direction != TO_SERVER)
            {
               BTG_ERROR_LOG("httpTransport::read(dBuffer) can only be used on client side!! Not on server side!");
               return 0;
            }

         t_int connectionID;
         return read(_buffer, connectionID);
      }

      t_int httpTransport::read(dBuffer & _buffer, t_int & _connectionID)
      {

#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE("httpTransport::read");
#endif // BTG_TRANSPORT_DEBUG
         t_int status = httpTransport::OPERATION_FAILED;

         _buffer.erase();
         _connectionID = NO_CONNECTION_ID;

         /// Select on all sockets in the group.
         socketGroup.doSelect();

         if (socketGroup.size() == 0)
            {
               // No sockets with data, most likely timeout
               return status;
            }

         switch (this->direction)
            {
            case TO_SERVER:
               {
#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE("httpTransport::read, TO_SERVER");
#endif // BTG_TRANSPORT_DEBUG
                  // We only have one socket in the socketGroup so no
                  // need to determine which socket it was...

                  // The above comment works for tcp/ip sockets, but
                  // not in this class. Added checks.
                  
                  btg::core::os::Socket* socket = 0;

                  if (!socketGroup.getNext(socket))
                     {
                        return status;
                     }

                  if (client != socket)
                     {
                        return status;
                     }

                  t_int rsize = client->read(sndrec_buffer, buffer_size);
                  if (rsize > 0)
                     {
#if BTG_TRANSPORT_DEBUG
                        BTG_NOTICE("httpTransport::read(), TO_SERVER, read " <<
                                   rsize << " bytes from server");
#endif // BTG_TRANSPORT_DEBUG
                        status = rsize;
                        clientRequest->addBytes(sndrec_buffer, rsize);
                     }

                  if (clientRequest->peek())
                     {
#if BTG_TRANSPORT_DEBUG
                        BTG_NOTICE("httpTransport::read(), TO_SERVER, got a full request.");
#endif // BTG_TRANSPORT_DEBUG
                        status = clientRequest->getContent(_buffer);
                        clientRequest->reset();
                        if ((status == 0) && (clientRequest->abort()))
                           {
                              BTG_NOTICE("httpTransport::read(), TO_SERVER, failed to retreive request! Bad gzip?");
                              // Probably broken gzip compression,
                              // shutdown.
                              client->shutdown();
                           }
                     }

                  break;
               }
            case FROM_SERVER:
               {
#if BTG_TRANSPORT_DEBUG
                  BTG_NOTICE("httpTransport::read, FROM_SERVER");
#endif // BTG_TRANSPORT_DEBUG

                  // Read from all clients.

                  btg::core::os::Socket* socket = 0;

                  while (socketGroup.getNext(socket))
                     {
                        if (socket == server)
                           {
                              // A new connection is here
                              this->acceptNewConnections();
                              continue;
                           }

                        // Got data on a socket
                        httpClient* client = dynamic_cast<httpClient*>( resolve( socket ) );
                        if (client->socket->deleted())
                           {
#if BTG_TRANSPORT_DEBUG
                              BTG_NOTICE("httpTransport::read: deleting dead client " << client);
#endif // BTG_TRANSPORT_DEBUG
                              // Terminate the conneciotn
                              endConnection(client->connectionID);

                              // Client is gone now!!
                              client = 0;

                              // Next socket...
                              continue;
                           }

                        t_int rsize = client->socket->read(sndrec_buffer, buffer_size);
                        if (rsize > 0)
                           {
#if BTG_TRANSPORT_DEBUG
                              BTG_NOTICE("httpTransport::read(), FROM_SERVER, read " << rsize << " bytes from client "<< client);
#endif // BTG_TRANSPORT_DEBUG
                              client->httpRequest.addBytes(sndrec_buffer, rsize);
                              if (client->httpRequest.abort())
                                 {
                                    BTG_NOTICE("httpRequest::read(), Fatal error from client " << client); // << ": " << buff);
                                    // A fatal error occured.

                                    std::stringstream ss;
                                    ss << "HTTP/1.0 400 Bad Request\r\n";
                                    ss << "Server: btg-" << GPD->sBUILD() << "\r\n";
                                    ss << "Date: " << createTimestamp() << "\r\n";
                                    ss << "Connection: close\r\n";
                                    ss << "\r\n";

                                    client->socket->write(reinterpret_cast<t_byteCP>(ss.str().c_str()), ss.str().length());
#if BTG_TRANSPORT_DEBUG
                                    BTG_NOTICE("httpTransport::read(), shutting down socket "<< client->socket->getSockId());
#endif // BTG_TRANSPORT_DEBUG
                                    client->socket->shutdown();
                                 }

                              if (client->httpRequest.peek())
                                 {
                                    rsize = client->httpRequest.getContent(_buffer);
                                    if (rsize > 0)
                                       {
                                          status = rsize;
                                          _connectionID = client->connectionID;
                                          break;
                                       }
                                    else if (client->httpRequest.abort())
                                       {
#if BTG_TRANSPORT_DEBUG
                                          BTG_NOTICE("httpTransport::read(), shutting down socket " <<
                                                     client->socket->getSockId());
#endif // BTG_TRANSPORT_DEBUG
                                          // Probably broken gzip
                                          // compression, shutdown.
                                          client->socket->shutdown();
                                       }
                                 }
                           }
                     } // while can read a socket.

                  break;
               }
            }
         return status;
      }

      messageTransport::TRANSPORT httpTransport::getType() const
      {
         return messageTransport::XMLRPC;
      }

      btg::core::os::gzipIf* httpTransport::getGzipIf() const
      {
         return gzipif;
      }

      void httpTransport::acceptNewConnections()
      {
         // Handle accepting new clients here.
         btg::core::os::ServerSocket* acceptsock = new btg::core::os::ServerSocket();

         if (server->accept(*acceptsock))
            {

#if BTG_TRANSPORT_DEBUG
               BTG_NOTICE("Accepted new connection. (" << reinterpret_cast<void*>(acceptsock) << ") id=" << acceptsock->getSockId());
#endif // BTG_TRANSPORT_DEBUG

               // Got a new client.
               httpClient* hc   = new httpClient(this->getGzipIf());
               hc->socket       = acceptsock;
               hc->connectionID = -1;

               // Find a new connection ID
               for(int i = 0; i < MAX_CONNECTIONS_ALLOWED; i++)
                  {
                     if (clients.find(i) == clients.end())
                        {
                           hc->connectionID = i;
                           break;
                        }
                  }

               if (hc->connectionID == NO_CONNECTION_ID)
                  {
                     BTG_NOTICE("All connections are in use. Dropping connection.");
                     acceptsock->shutdown();
                     delete hc;
                     hc = 0;
                     delete acceptsock;
                     acceptsock = 0;
                     return;
                  }

               socketGroup.addSocket(acceptsock);

               BTG_NOTICE("Added new connection " << hc->connectionID);
               clients[hc->connectionID] = hc;
            }
         else
            {
               delete acceptsock;
               acceptsock = 0;
            }
      }

      std::string httpTransport::createTimestamp()
      {
         char timebuff[50];
         time_t tt     = time(NULL);
         struct tm *st = gmtime(&tt);

         // Date field according to RFC.
         strftime(timebuff, 50, "%a, %d %b %Y %H:%M:%S GMT", st);

         std::string s(timebuff);
         return s;
      }

      httpTransport::~httpTransport()
      {
         delete gzipif;
         gzipif = 0;
      }
   } // namespace core
} // namespace btg

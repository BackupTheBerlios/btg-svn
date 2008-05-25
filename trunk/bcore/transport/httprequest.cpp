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

#include <bcore/logmacro.h>
#include <bcore/util.h>
#include "httprequest.h"
#include <bcore/os/gzip.h>

namespace btg
{
   namespace core
   {
      httpRequest::httpRequest(LogWrapperType _logwrapper,
                               btg::core::os::gzipIf* _gzipif)
         : Logable(_logwrapper),
           recvBuffer(),
           request_headers(),
           request_content(),
           request_content_length(0),
           bMethodReceived(false),
           bHeadersReceived(false),
           bRequestReceived(false),
           bAbort(false),
           gzipif(_gzipif)
      {
      }

      httpRequest::~httpRequest()
      {
#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE(logWrapper(), "httpRequest cleanup");
#endif // BTG_TRANSPORT_DEBUG
      }

      httpRequestServerSide::httpRequestServerSide(LogWrapperType _logwrapper,
                                                   btg::core::os::gzipIf* _gzipif)
         : httpRequest(_logwrapper, _gzipif)
      {
      }

      httpRequestServerSide::~httpRequestServerSide()
      {
#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE(logWrapper(), "httpRequest cleanup");
#endif // BTG_TRANSPORT_DEBUG
      }

      httpRequestClientSide::httpRequestClientSide(LogWrapperType _logwrapper,
                                                   btg::core::os::gzipIf* _gzipif)
         : httpRequest(_logwrapper, _gzipif)
      {
      }

      httpRequestClientSide::~httpRequestClientSide()
      {
#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE(logWrapper(), "httpRequest cleanup");
#endif // BTG_TRANSPORT_DEBUG
      }

      void httpRequest::addBytes(t_byteCP _buffer, t_int _len)
      {
         recvBuffer += std::string(reinterpret_cast<const char*>(_buffer), static_cast<std::string::size_type>(_len));
         parse();
      }

      bool httpRequest::getHeader(std::string header, std::string &value)
      {
#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE(logWrapper(), "httpRequest::getHeader() Checking for header '" << header << "'.");
#endif // BTG_TRANSPORT_DEBUG
         if (request_headers.find(header) == request_headers.end())
            {
#if BTG_TRANSPORT_DEBUG
               BTG_NOTICE(logWrapper(), "httpRequest::getHeader() Not found");
#endif // BTG_TRANSPORT_DEBUG
               return false;
            }

         value = request_headers[header];
#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE(logWrapper(), "httpRequest::getHeader() Found, value '" << value << "'");
#endif // BTG_TRANSPORT_DEBUG
         return true;
      }

      t_int httpRequest::getContent(dBuffer &content)
      {
         if (!bRequestReceived)
            {
               return 0;
            }

         std::string content_encoding;
         if (getHeader("Content-Encoding", content_encoding))
            {
               if (content_encoding.find("gzip") != std::string::npos)
                  {
#if BTG_TRANSPORT_DEBUG
                     BTG_NOTICE(logWrapper(), "httpRequest::getContent(), decompressing content using GZIP. Compressed size is " << request_content.length());
#endif // BTG_TRANSPORT_DEBUG

                     try
                        {
                           std::string tmp;
                           gzipif->gzip_decompress(request_content, tmp);
                           request_content = tmp;
                        }
                     catch(...)
                        {
                           BTG_ERROR_LOG(logWrapper(), "httpRequest::getContent(), failed to decompress GZIP'ed content.");
                           bAbort = true;
                           return 0;
                        }

#if BTG_TRANSPORT_DEBUG
                     BTG_NOTICE(logWrapper(), "httpRequest::getContent(), decompressed, uncompressed size is " << request_content.length());
#endif // BTG_TRANSPORT_DEBUG
                  }
            }

         content.erase();
         content.addBytes(reinterpret_cast<t_byteCP>(request_content.c_str()), request_content.size());

         //reset();
         return content.size();
      }

      void httpRequest::reset()
      {
#if BTG_TRANSPORT_DEBUG
         BTG_NOTICE(logWrapper(), "httpRequest::reset()");
#endif // BTG_TRANSPORT_DEBUG
         request_content_length = 0;

         request_content = "";
         request_headers.clear();

         bHeadersReceived = false;
         bMethodReceived  = false;
         bRequestReceived = false;
         bAbort           = false;
      }

      bool httpRequest::parse()
      {
         /* A http request/response looks like this:
          * <firstline>\r\n
          * <headers>\r\n
          *\ r\n
          * <content-data>\r\n
          *
          * ("real" newlines here added for visibility)
          * The first line is different for a request and a response.
          * For a request it looks like this:
          *
          * <method> <uri> HTTP/1.x
          *
          * where method most likely is POST or GET. Since only POST allows for content-data,
          * this is the only method the server-side will accept (this check is done in
          * httpRequestServerSide::checkFirstline()).
          *
          * For a response the firstline looks like this
          * HTTP/1.x <response-code> <response-text>
          *
          * Response code is normally 200 (and response-text OK) for successfull requests.
          * This is checked in HttpRequestClientSide::checkFirstline()
          *
          *
          * The headers may vary between servers/clients, the only (by us) required header
          * is Content-Length, which specifyes the length of the content-data.
          * The HTTP RFC says a server should always have a Date header included on their responses.
          */

         if (!bHeadersReceived)
            {
	      while (1)
                  {
                     std::string::size_type pos = recvBuffer.find_first_of("\r\n");
                     if (pos == std::string::npos)
                        {
			  // Not ready to parse yet...
                           return false;
                        }

                     // Copy the line to a local buffer.
                     std::string line(recvBuffer, 0, pos);

                     // Erase the line from the class instance buffer.
                     recvBuffer.erase(0, pos+2);   // +2 for the \r\n

                     if (!bMethodReceived)
                        {
			  // Call the subclass checkFirstline method,
			  // this will make sure the first line is
			  // "OK". Since we realy dont care about the
			  // info in it on either the server side or
			  // the client side we just check it so it
			  // makes sense (ie not totally broken remote
			  // end)

                           if (!checkFirstline(line))
                              {
				// Broken client/server in remote end.
                                 bAbort=true;
#if BTG_TRANSPORT_DEBUG
                                 BTG_NOTICE(logWrapper(), "httpRequest::parse() Broken firstline");
#endif // BTG_TRANSPORT_DEBUG
                                 return false;
                              }

                           // Looks OK.
                           bMethodReceived=true;
                        }
                     else
                        {
                           if (line.length() == 0)
                              {
				// Last header line recieved.

				// Get content length.
                                 if (request_headers.find("Content-Length") == request_headers.end())
                                    {
				      // No content length.
                                       bAbort=true;
#if BTG_TRANSPORT_DEBUG
                                       BTG_NOTICE(logWrapper(), "httpRequest::parse() Missing Content-Length header");
#endif // BTG_TRANSPORT_DEBUG
                                       return false;
                                    }

                                 // Convert to t_long.
                                 std::stringstream ss(request_headers["Content-Length"]);
                                 ss >> request_content_length;

                                 if (request_content_length == 0)
                                    {
				      // Zero content?.
                                       bAbort=true;
#if BTG_TRANSPORT_DEBUG
                                       BTG_NOTICE(logWrapper(), "httpRequest::parse() Zero content: " << request_headers["Content-Length"]);
#endif // BTG_TRANSPORT_DEBUG
                                       return false;
                                    }

                                 bHeadersReceived=true;

                                 // Now break out of our header line
				 // parsing loop.
                                 break;
                              }
                           else
                              {
				// A header line was received. Split
				// key/value.
                                 std::string::size_type pos2;
                                 pos = line.find_first_not_of(' ');  // First non whitespace
                                 pos2 = line.find_first_of(':');     // First :
                                 if (pos2 == std::string::npos) // no : found
                                    {
				      // Broken request.
#if BTG_TRANSPORT_DEBUG
                                       BTG_NOTICE(logWrapper(), "httpRequest::parse() Broken header line: " << line);
#endif // BTG_TRANSPORT_DEBUG
                                       bAbort=true;
                                       return false;
                                    }

                                 std::string key(line, pos, pos2);   // from first non-whitespace to :
                                 // Find first non-space after the ":".
                                 pos = line.find_first_not_of(' ', pos2+1);   
                                 std::string value(line, pos);
                                 request_headers[key] = value;

                              }  // line.lenght() == 0*
                        }  // bMethodReceived
                  }  // while (1)
            }  // !bHeadersReceived

         // When we get to this point, all headers has been received
         // and only content data is left.
         if (recvBuffer.length() >= request_content_length)
            {
	      // All data has been received. 
	      // Copy it to the request_content var and erase it from
	      // the buffer.

               request_content = recvBuffer.substr(0, request_content_length);
               recvBuffer.erase(0, request_content_length);

               // We're ready to be used.
               bRequestReceived = true;
               return true;
            }
         return false;
      }

      bool httpRequestServerSide::checkFirstline(std::string line)
      {
	// Starts with POST? We realy dont care about URI for
	// now.. Maybe a check for HTTP/1.x should be done...

         if (line.find("POST") == 0)
            {
               return true;
            }
         return false;
      }

      bool httpRequestClientSide::checkFirstline(std::string line)
      {
	// Contains 200 OK? Check for HTTP/1.x should probably be done
	// here too...

         if (line.find("200 OK") != std::string::npos)
            {
               return true;
            }

         return false;
      }
   }
}

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


#include <string>
#include <map>
#include <bcore/dbuf.h>
#include <bcore/type.h>
#include <bcore/os/gzipif.h>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup transport
          */
         /** @{ */

         /// A class implementing a HTTP Request/Response parser().
         /// Add incomming data with addBytes, get headers/content with getHeader()/getContent() when peek() says its OK.
         /// Do not use this class directly, has to be subclassed.
         class httpRequest
            {
            public:
               /// Default constructor.
               httpRequest(btg::core::os::gzipIf* _gzipif);

               /// Destructor.
               virtual ~httpRequest();

               /// Add bytes to the internal buffer. After bytes
               /// has been added, parse() will be called. If a full HTTP request has been received,
               /// peek() will return 1, and getContent() can be used to retreive the request payload.
               /// Multiple requests (from different sources if not fragmented, or only from same source
               /// if there is risk for fragmentation) may be added as long as the Content-Lenght header
               /// is correct.
               void addBytes(t_byteCP _buffer, t_int _len);

               /// Check if the request is ready to be processed (ie,
               /// all data has been received).
               bool peek() { return bRequestReceived; }

               /// Check if the request has found an error. Check this
               /// when parse() returns false.
               bool abort() { return bAbort; }

               /// Get the specified header. Use before getContent()!
               bool getHeader(std::string header, std::string &value);

               /// Get the content (payload) of the request. After payload has been copied to content, all the headers
               /// and the content will be erased.
               t_int getContent(dBuffer &content);

               /// Reset the internal buffers representing the current request.
               /// \note Does not reset the recieved buffer.
               void reset();

            protected:
               /// Each byte recieved from the other end is put in this buffer,
               /// from which parse() reads data and puts in other buffers
               /// as soon as enough correct data has been received.
               std::string recvBuffer;

               /// Received headers in current request
               std::map<std::string,std::string> request_headers;

               /// Received content (payload) in current request
               std::string request_content;

               /// Content-length of current request
               t_long request_content_length;

               /// Indicates that the method (firts line) was received.
               bool bMethodReceived;

               /// Indicates that all headers were received.
               bool bHeadersReceived;

               /// Indicates that a full request (headers+body)  was received.
               bool bRequestReceived;

               /// Indicates that parsing was aborted.
               bool bAbort;

               /// Pointer to the gzip interface used.
               btg::core::os::gzipIf* gzipif;

               /// Parse the data recieved from the remote end.
               bool parse();

               /// Check the first line in the recieved data. Called from parse().
               virtual bool checkFirstline(std::string line)=0;
            };

         /// Server side request.
         class httpRequestServerSide: public httpRequest
            {
            public:
               /// Default constructor.
               httpRequestServerSide(btg::core::os::gzipIf* _gzipif);

               /// Destructor.
               virtual ~httpRequestServerSide();
            private:
               /// Check the first line for a client request looking like
               /// \code POST <uri> HTTP/1.x \endcode
               bool checkFirstline(std::string line);
            };

         /// Client side request.
         class httpRequestClientSide: public httpRequest
            {
            public:
               /// Default constructor.
               httpRequestClientSide(btg::core::os::gzipIf* _gzipif);

               /// Destructor.
               virtual ~httpRequestClientSide();
            private:
               /// Check the first line for a server response looking like
               /// 200 OK HTTP/1.x
               bool checkFirstline(std::string line);
            };

         /** @} */

      } // namespace core
} // namespace btg


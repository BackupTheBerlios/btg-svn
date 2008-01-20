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

#include "logger.h"
#include "logif.h"

namespace btg
{
   namespace core
   {
      namespace logger
      {

         logBuffer::logBuffer(logInterface* _target, t_int _bufferSize):
            std::streambuf(),
            buffer(0),
            target(_target)
         {
            if (_bufferSize)
               {
                  buffer = new char[_bufferSize];
                  setp(buffer, buffer + _bufferSize);
               }
            else
               {
                  setp(0, 0);
               }

            setg(0, 0, 0);
         }

         logBuffer::~logBuffer()
         {
            // sync();
            // delete[] pbase();
            delete [] buffer;
         }

         int logBuffer::overflow(int _ch)
         {
            put_buffer();

            if (_ch != EOF)
               {
                  if (pbase() == epptr())
                     {
                        put_char(_ch);
                     }
                  else
                     {
                        sputc(_ch);
                     }
               }

            return 0;
         }

         int logBuffer::sync()
         {
            put_buffer();
            return 0;
         }

         logInterface* logBuffer::getTarget() const
         {
            return target;
         }

         logStream::logStream(logBuffer *_logbuffer):
            std::ostream(_logbuffer),
            logbuffer(_logbuffer)
         {}

         logStream::~logStream()
         {
            delete logbuffer;
            logbuffer = 0;
         }

         /* *** */

         logWrapper::logWrapper()
            : logstream(),
              minMessagePriority(logWrapper::PRIO_DEBUG),
              logstreamset(false)
         {
         }

         logWrapper::~logWrapper()
         {
         }

         void logWrapper::setLogStream(boost::shared_ptr<logStream> _logstream)
         {
            logstream    = _logstream;
            logstreamset = true;
         }

         bool logWrapper::logStreamSet() const
         {
            return logstreamset;
         }

         boost::shared_ptr<logStream> logWrapper::getLogStream() const
         {
            return logstream;
         }

         bool logWrapper::logInput(logWrapper::MESSAGEPRIORITY _messagePriority) const
         {
            if (static_cast<t_uint>(_messagePriority) < static_cast<t_uint>(this->minMessagePriority))
               {
                  return false;
               }
            return true;
         }

         void logWrapper::setMinMessagePriority(logWrapper::MESSAGEPRIORITY _minMessagePriority)
         {
            this->minMessagePriority = _minMessagePriority;
         }

      } // namespace logger
   } // namespace core
} // namespace btg

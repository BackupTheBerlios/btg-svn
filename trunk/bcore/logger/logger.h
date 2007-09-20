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
 * $Id: logger.h,v 1.1.2.5 2006/12/25 23:25:54 wojci Exp $
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <bcore/type.h>
#include <config.h>

#if HAVE_STREAMBUF
#  include <streambuf>
#elif HAVE_STREAMBUF_H
#  include <streambuf.h>
#else
#  error "No <stremabuf> or <streambuf.h>"
#endif

#if HAVE_OSTREAM
#  include <ostream>
#elif HAVE_OSTREAM_H
#  include <ostream.h>
#else
#  error "No <ostream> or <ostream.h>"
#endif

#include <string>

namespace btg
{
   namespace core
      {
         namespace logger
            {

               /**
                * \addtogroup logger
                */
               /** @{ */

               class logInterface;

               /// Buffer used for keeping debug information.
               /// Implement debugBuffer::overflow(int) and debugBuffer::sync() in
               /// order to send the logging information to a destination.
               class logBuffer: public std::streambuf
                  {
                  public:
                     /// Constructor.

                     /// Logger type.
                     enum LOGGER_TYPE
                        {
                           UNDEF  = 0, //!< Undefined.
                           STDOUT,     //!< Standard out.
                           LOGFILE,    //!< File.
                           SYSLOG      //!< Syslog.
                        };

                     /// @param _target Pointer to a class which will
                     /// be used by the put_buffer and put_char
                     /// functions to write strings to.
                     /// @param _bufferSize       The size of this buffer.
                     logBuffer(logInterface* _target = 0, t_int _bufferSize = 1024);

                     /// Destructor.
                     virtual ~logBuffer();

                  protected:
                     /// Used when this buffer overflows.
                     int overflow(int _ch);

                     /// Used to sync this buffer.
                     int sync();

                     // Define these two functions to send chars to target.
                     /// Used to write a string to _target.
                     virtual void put_buffer(void)    = 0;
                     /// Used to write a character to _target.
                     virtual void put_char(int _char) = 0;

                     /// Get the target, used for writing strings or characters to.
                     logInterface* getTarget() const;

                     /// Buffer used.
                     char* buffer;
                  private:
                     /// The target.
                     logInterface* target;

                     /// Copy constructor.
                     logBuffer(logBuffer const& _logbuffer);

                     /// Assignment operator.
                     logBuffer& operator=(logBuffer const& _logbuffer);
                  };

               /// Wrapper around the buffer class, so the user can use the operator <<.
               class logStream: public std::ostream
                  {
                  public:
                     /// Constructor.
                     /// @param _logbuffer Pointer to an instance of the the buffer
                     /// class.
                     logStream(logBuffer *_logbuffer);

                     /// Destructor.
                     ~logStream();
                  private:
                     /// Pointer to the buffer used.
                     logBuffer* logbuffer;

                     /// Copy constructor.
                     logStream(logStream const& _ls);

                     /// Assignment operator.
                     logStream& operator=(logStream const& _ls);
                  };

               /// Wraps aroud an instance of debugStream.
               /// This is a singleton.
               /// \note This class logs everthing per default. Use
               /// debugWrapper::setMinMessagePriority to change this
               /// behaviour.
               class logWrapper
                  {
                  public:
                     /// The error priority.
                     enum MESSAGEPRIORITY
                        {
                           PRIO_ERROR   = 254,   //!< Errors.
                           PRIO_VERBOSE = 128,   //!< Verbose output/logging.
                           PRIO_NOTICE  = 1,     //!< Notices, which can be ignored.
                           PRIO_DEBUG   = 0      //!< Debug output, which can be ignored.
                        };
                     /// Get an instance of the contained class.
                     /// Initially the pointer to the instance of debugStream
                     /// class is set to 0.
                     static logWrapper* getInstance();

                     /// Kill the singleton.
                     static void killInstance();

                     /// Set the debugStream instance used for logging.
                     void setLogStream(logStream *_logstream);

                     /// Get a pointer to the debugStream instance used for logging.
                     logStream* getLogStream() const;

                     /// Sets the message min priority, which will be logged.
                     /// @param [in] _minMessagePriority The min. priority.
                     void setMinMessagePriority(logWrapper::MESSAGEPRIORITY _minMessagePriority);

                     /// Returns true if this class should log its input.
                     bool logInput(logWrapper::MESSAGEPRIORITY _messagePriority) const;

                     /// The destructor.
                     ~logWrapper();
                  private:
                     /// Constructor.
                     logWrapper();

                     /// Copy constructor.
                     logWrapper(logWrapper const& _lw);

                     /// Assignment operator.
                     logWrapper& operator=(logWrapper const& _lw);

                     /// State variable.
                     static bool                   instance;

                     /// Pointer to the instance.
                     static logWrapper*            instancePointer;

                     /// Pointer to an instance of debugStream.
                     logStream*                    logstream;

                     /// Logging priority.
                     logWrapper::MESSAGEPRIORITY   minMessagePriority;
                  };

               /** @} */

            } // namespace logger
      } // namespace core
} // namespace btg

#endif

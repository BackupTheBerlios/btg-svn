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

#ifndef LOGMACRO_H
#define LOGMACRO_H

#include <bcore/project.h>

#include <string>
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <bcore/logger/logger.h>

/**
 * \addtogroup logger
 */
/** @{ */

/*! \file logmacro.h
  \brief Logging macros.
*/

/// Set to 1 to log/output debug information.
#define BTG_TRANSPORT_DEBUG 0

/// Set to 1 to log/output debug information.
#define BTG_EXTERNALIZATION_DEBUG 0

/// Set to 1 to see _lots_ of output from the clients statemachine.
#define BTG_STATEMACHINE_DEBUG 0

/// Set to 1 to see _lots_ of output from the auth implementation.
#define BTG_AUTH_DEBUG 0

/// Used for writing fatal error message to stdout or log.
#define BTG_FATAL_ERROR(_CLASS, _ERRORTEXT) { BTG_MESSAGE_LOG(btg::core::logger::logWrapper::PRIO_ERROR, _CLASS << ": " << _ERRORTEXT, 1, false); }

/// Write an error message with the line and file where it occured.
/// \note Only outputs if BTG_DEBUG is set to 1.
#define BTG_ERROR_LOG(_ERRORTEXT) { BTG_MESSAGE_LOG(btg::core::logger::logWrapper::PRIO_ERROR, _ERRORTEXT, BTG_DEBUG, true); }

/// Used to write verbose messages.
#define BTG_VERBOSE(_TEXT) { BTG_MESSAGE_LOG(btg::core::logger::logWrapper::PRIO_VERBOSE, _TEXT, 1 /* always enabled */, false); }

/// Write a notice to std out or log.
/// \note Only outputs if BTG_DEBUG is set to 1.
#define BTG_NOTICE(_TEXT) { BTG_MESSAGE_LOG(btg::core::logger::logWrapper::PRIO_NOTICE, _TEXT, BTG_DEBUG, true); }

/// Write a message to logger, update message counter.
#define BTG_MESSAGE_LOG(_LEVEL, _TEXT, _BTG_DEBUG, _PREPEND_FILELINE) { \
if (_BTG_DEBUG == 1) \
{ \
   if (btg::core::logger::logWrapper::getInstance()->logInput(_LEVEL)) \
   { \
      boost::posix_time::ptime t(boost::posix_time::second_clock::local_time()); \
      btg::core::logger::logStream* logstream = btg::core::logger::logWrapper::getInstance()->getLogStream(); \
         if (logstream != 0 ) \
         { \
            *logstream << "[" << boost::posix_time::to_simple_string(t) << "] ";\
            if (_PREPEND_FILELINE == true) \
            { \
               *logstream << "(" << __FILE__ << ", " << __LINE__ << ") "; \
            } \
            *logstream << _TEXT << std::endl; \
         } \
         else \
         { \
            /* No stream, so write to std output. */ \
            std::cout << "[" << boost::posix_time::to_simple_string(t) << "] ";\
            if (_PREPEND_FILELINE == true) \
               { \
                  std::cout << "(" << __FILE__ << ", " << __LINE__ << ") "; \
               } \
            std::cout << _TEXT << std::endl; \
         } \
   } \
   } \
}

/** @} */

#endif // LOGMACRO_H

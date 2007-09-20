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
 * $Id: modulelog.h,v 1.1.2.4 2007/08/26 15:37:41 wojci Exp $
 */

#ifndef MODULELOG_H
#define MODULELOG_H

#include <bcore/logmacro.h>
#include <bcore/verbose.h>

// The following macros all expect a string, moduleName, to be avaiable.

/// Used to write uniform notices for different modules.
/// 
/// \note Adds punctuation character.
#define BTG_MNOTICE(_TEXT) { BTG_MESSAGE_LOG(btg::core::logger::logWrapper::PRIO_NOTICE, moduleName << ": " << _TEXT << ".", BTG_DEBUG, false); }

/// Used to write uniform errors for different modules.
/// 
/// \note Adds punctuation character.
#define BTG_MERROR(_TEXT) { BTG_MESSAGE_LOG(btg::core::logger::logWrapper::PRIO_ERROR, moduleName << ", error: " << _TEXT << ".", BTG_DEBUG, false); }

/// A function enter.
#define BTG_MENTER(_FUNCTION_NAME, _ARGUMENTS)   { BTG_MNOTICE("en:" << _FUNCTION_NAME << ", arg: " << _ARGUMENTS); }

/// A function exit.
#define BTG_MEXIT(_FUNCTION_NAME, _RETURN_VALUE) { BTG_MNOTICE("ex:" << _FUNCTION_NAME << ", ret: " << _RETURN_VALUE); }

/// Verbose log for a certain module.
#define MVERBOSE_LOG(_moduleName, _flag, _text) { VERBOSE_LOG(_flag, _moduleName << ": " << _text); }

#endif // MODULELOG_H


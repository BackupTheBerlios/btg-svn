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
 * $Id: type.h,v 1.13.4.5 2007/04/12 12:53:35 wojci Exp $
 */

#ifndef BTG_TYPE_H
#define BTG_TYPE_H

#include <stdint.h>
#include <vector>
#include <map>
#include <string>

/**
 * \addtogroup core
 */
/** @{ */

/*! \file type.h
  \brief Typedefs of the different types which are in use.
*/

/// 8-bit unsigned integer.
typedef uint8_t                                  t_byte;

/// Pointer to an 8-bit unsigned integer.
typedef t_byte*                                  t_byteP;

/// Const pointer to an 8-bit unsigned integer.
typedef const t_byte*                            t_byteCP;

/// Unsigned 32-bit integer.
typedef uint32_t                                 t_uint;

/// Signed 32-bit integer.
typedef int32_t                                  t_int;

/// Signed 64-bit integer.
typedef int64_t                                  t_long;

/// Unsigned 64-bit integer.
typedef uint64_t                                 t_ulong;

/// Unsigned 64-bit integer.
typedef uint64_t                                 t_ullong;

/// Float.
typedef float                                    t_float;

// Vector constructs and iterators.

/// List of bytes.
typedef std::vector<t_byte>                      t_byteList;

/// List of bytes - iterator.
typedef std::vector<t_byte>::const_iterator      t_byteList_CI;

/// List of bytes - const iterator.
typedef std::vector<t_byte>::iterator            t_byteList_I;

/// List of itegers.
typedef std::vector<t_int>                       t_intList;

/// List of itegers - interator.
typedef std::vector<t_int>::iterator             t_intListI;

/// List of itegers - const iterator.
typedef std::vector<t_int>::const_iterator       t_intListCI;

/// List of itegers.
typedef std::vector<t_long>                      t_longList;

/// List of itegers - iterator.
typedef std::vector<t_long>::iterator            t_longListI;

/// List of itegers - const iterator.
typedef std::vector<t_long>::const_iterator      t_longListCI;

/// List of strings.
typedef std::vector<std::string>                 t_strList;

/// List of strings - iterator.
typedef std::vector<std::string>::iterator       t_strListI;

/// List of strings - const iterator.
typedef std::vector<std::string>::const_iterator t_strListCI;

/// List of bits.
typedef std::vector<bool>                         t_bitList;

/// List of bits - iterator.
typedef std::vector<bool>::iterator               t_bitListI;

/// List of bits - const iterator.
typedef std::vector<bool>::const_iterator         t_bitListCI;

/// Mapping (integer, string) - const iterator.
typedef std::map<t_int, std::string>                  t_intStrMap;

/// Mapping (integer, string) - iterator.
typedef std::map<t_int, std::string>::iterator        t_intStrMapI;

/// Mapping (integer, string) - const iterator.
typedef std::map<t_int, std::string>::const_iterator  t_intStrMapCI;

/// Pair.
typedef std::pair<t_int, t_int>                       t_intPair;

/** @} */

#endif // BTG_TYPE_H


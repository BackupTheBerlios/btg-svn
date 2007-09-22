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

#ifndef TYPE_BTG_H
#define TYPE_BTG_H

#include "status.h"
#include "command/command.h"
#include "file_info.h"
#include "peer.h"

/**
 * \addtogroup core
 */
/** @{ */

/*! \file type_btg.h
  \brief Typedefs of the different BTG specific types which are in use.
*/

/// List of status objects.
typedef std::vector<btg::core::Status>                          t_statusList;

/// List of status objects - interator.
typedef std::vector<btg::core::Status>::iterator                t_statusListI;

/// List of status objects - const iterator.
typedef std::vector<btg::core::Status>::const_iterator          t_statusListCI;

/// List of file info objects.
typedef std::vector<btg::core::fileInformation>                 t_fileInfoList;

/// List of file info objects - iterator.
typedef std::vector<btg::core::fileInformation>::iterator       t_fileInfoListI;

/// List of file info objects - const iterator.
typedef std::vector<btg::core::fileInformation>::const_iterator t_fileInfoListCI;

/// List of peer objects.
typedef std::vector<btg::core::Peer>                            t_peerList;

/// List of peer objects - iterator.
typedef std::vector<btg::core::Peer>::iterator                  t_peerListI;

/// List of peer objects - const iterator.
typedef std::vector<btg::core::Peer>::const_iterator            t_peerListCI;

/// List of command pointers.
typedef std::vector<btg::core::Command*>                        t_commandPointerList;

/// List of command pointers - iterator.
typedef std::vector<btg::core::Command*>::iterator              t_commandPointerListI;

/// List of command pointers - const iterator.
typedef std::vector<btg::core::Command*>::const_iterator        t_commandPointerListCI;

/** @} */

#endif // TYPE_BTG_H


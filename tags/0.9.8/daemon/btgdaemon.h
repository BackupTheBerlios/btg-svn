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

#ifndef BTGDAEMON_H
#define BTGDAEMON_H

// Send a command and destroy the data.
bool sendCommand(btg::core::externalization::Externalization* _e,
                 btg::core::messageTransport* _transport,
                 t_int _connectionID,
                 btg::core::Command* _command);

// Check that a file exists, if not write an error message.
bool checkFile(std::string const& _purpose, std::string & _filename);

#endif // BTGDAEMON_H


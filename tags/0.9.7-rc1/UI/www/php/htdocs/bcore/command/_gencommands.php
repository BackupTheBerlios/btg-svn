<?php
/*
 * btg Copyright (C) 2005 Michael Wojciechowski.
 * PHP client written by Johan Ström.
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
 * $Id: _gencommands.php,v 1.1.2.6 2007/04/24 07:02:13 jstrom Exp $
 */

/** This file outputs a list of consts with commands which should be "pasted" in command.php.
 *	Only use when updates are done to the commands.
 * Note that the maps has to be manually updated in command.php
 */

$x=0;
$x++;	echo "	const CN_GINITCONNECTION = {$x};  //!< Init session.\n";
$x++;	echo "	const CN_GSETUP = {$x};           //!< Setup command.\n";
$x++;	echo "	const CN_GSETUPRSP = {$x};        //!< Setup response command.\n";
$x++;	echo "	const CN_GKILL = {$x};            //!< Bring down the daemon.\n";
$x++;	echo "	const CN_GLIMIT = {$x};           //!< Limit.\n";
$x++;	echo "	const CN_GLIMITSTAT = {$x};       //!< Limit request.\n";
$x++;	echo "	const CN_GLIMITSTATRSP = {$x};    //!< Limit request response.\n";
$x++;	echo "	const CN_GUPTIME = {$x};          //!< Get daemon uptime.\n";
$x++;	echo "	const CN_GUPTIMERSP = {$x};       //!< Daemon uptime response.\n";
$x++;	echo "	const CN_GLIST = {$x};            //!< List request.\n";
$x++;	echo "	const CN_GLISTRSP = {$x};         //!< List request response.\n";
$x++;	echo "	const CN_CCREATEWITHDATA = {$x};  //!< Create a context, using embedded data.\n";
$x++;	echo "	const CN_CLAST = {$x};            //!< Last context.\n";
$x++;	echo "	const CN_CLASTRSP = {$x};         //!< Last context response.\n";
$x++;	echo "	const CN_CSTART = {$x};           //!< Start a context.\n";
$x++;	echo "	const CN_CSTOP = {$x};            //!< Stop a context.\n";
$x++;	echo "	const CN_CABORT = {$x};           //!< Abort a context.\n";
$x++;	echo "	const CN_CSTATUS = {$x};          //!< Request status information about a context.\n";
$x++;	echo "	const CN_CSTATUSRSP = {$x};       //!< Status information about one context.\n";
$x++;	echo "	const CN_CALLSTATUSRSP = {$x};    //!< Status information about all contexts.\n";
$x++;	echo "	const CN_CFILEINFO = {$x};        //!< Request file info information about a context.\n";
$x++;	echo "	const CN_CFILEINFORSP = {$x};     //!< File info information about a context.\n";
$x++;	echo "	const CN_CALLFILEINFORSP = {$x};  //!< File info information about all contexts.\n";
$x++;	echo "	const CN_CCLEAN = {$x};           //!< Request to clean finished contexts.\n";
$x++;	echo "	const CN_CCLEANRSP = {$x};        //!> Response that informs the client that some contexts have been cleaned.\n";
$x++;	echo "	const CN_CLIMIT = {$x};           //!< Limit a context.\n";
$x++;	echo "	const CN_CLIMITSTATUS = {$x};     //!< Limit status.\n";
$x++;	echo "	const CN_CLIMITSTATUSRSP = {$x};  //!< Limit status response.\n";
$x++;	echo "	const CN_CPEERS = {$x};           //!< Request a list of peers.\n";
$x++;	echo "	const CN_CPEERSRSP = {$x};        //!< Response containing a list of peers.\n";
$x++;	echo "	const CN_ERROR = {$x};            //!< General error.\n";
$x++;	echo "	const CN_ACK = {$x};              //!< General acknowlegment.\n";
$x++;	echo "	const CN_SATTACH = {$x};          //!< Attach to a running session.\n";
$x++;	echo "	const CN_SDETACH = {$x};          //!< Detach from a running session.\n";
$x++;	echo "	const CN_SERROR = {$x};           //!< Daemon uses this command to tell clients that their session is invalid.\n";
$x++;	echo "	const CN_SQUIT = {$x};            //!< Quit the current session.\n";
$x++;	echo "	const CN_SLIST = {$x};            //!< Get a list of sessions.\n";
$x++;	echo "	const CN_SLISTRSP = {$x};         //!< Response that lists sessions.\n";
$x++;	echo "	const CN_MOREAD = {$x};           //!< Only read only access. Usefull for writing monitor applets.\n";
$x++;	echo "	const CN_MOWRITE = {$x};          //!< Default write mode.\n";
$x++;	echo "	const CN_UNDEFINED = {$x};        //!< Undefined command.\n";
?>

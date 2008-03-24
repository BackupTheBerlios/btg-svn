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
 * $Id: command.php,v 1.1.2.7 2007/06/22 20:08:40 wojci Exp $
 */
if(!defined("BTG_BCORE_PATH"))
	define("BTG_BCORE_PATH", "bcore");

require_once(BTG_BCORE_PATH."/btgexception.php");
require_once(BTG_BCORE_PATH."/serializable.php");

class Command extends BTGSerializable
{
	const INVALID_SESSION = -1;
	
	/* This list of available commands is generated from _gencommands.php! 
		In VIM the following command can be used to exec the command and put the text here:

		:r! php -q _gencommands.php

		Only the const stuff is added from _gencommands, not the maps!
	*/

	const CN_GINITCONNECTION = 1;
	const CN_GSETUP          = 10;
	const CN_GSETUPRSP       = 11;
	const CN_GKILL           = 12;
	const CN_GLIMIT          = 13;
	const CN_GLIMITSTAT      = 14;
	const CN_GLIMITSTATRSP   = 15;
	const CN_GUPTIME         = 16;
	const CN_GUPTIMERSP      = 17;
	const CN_GLIST           = 18;
	const CN_GLISTRSP        = 19;

	const CN_CCREATEWITHDATA = 40;
	const CN_CLAST           = 41;
	const CN_CLASTRSP        = 42;
	const CN_CSTART          = 43;
	const CN_CSTOP           = 44;
	const CN_CABORT          = 45;
	const CN_CSTATUS         = 46;
	const CN_CSTATUSRSP      = 47;
	const CN_CALLSTATUSRSP   = 48;
  	const CN_CMSTATUS        = 49;
	const CN_CMSTATUSRSP     = 50;
	const CN_CFILEINFO       = 51;
	const CN_CFILEINFORSP    = 52;
	const CN_CALLFILEINFORSP = 53;
	const CN_CCLEAN          = 54;
	const CN_CCLEANRSP       = 55;
	const CN_CLIMIT          = 56;
	const CN_CLIMITSTATUS    = 57;
	const CN_CLIMITSTATUSRSP = 58;
	const CN_CPEERS          = 59;
	const CN_CPEERSRSP       = 60;
	const CN_CSETFILES       = 61;
	const CN_CGETFILES       = 62;
	const CN_CGETFILESRSP    = 63;
	const CN_CGETTRACKERS    = 64;
	const CN_CGETTRACKERSRSP = 65;
	const CN_CMOVE           = 66;
	const CN_CCREATEFROMURL  = 67;
	const CN_CCREATEFROMURLRSP = 68;
	const CN_CURLSTATUS        = 69;
	const CN_CURLSTATUSRSP     = 70;

	const CN_ERROR           = 100;
	const CN_ACK             = 101;

	const CN_SATTACH   = 120;
	const CN_SDETACH   = 121;
	const CN_SERROR    = 122;
	const CN_SINFO     = 123;
	const CN_SINFORSP  = 124;
	const CN_SQUIT     = 125;
	const CN_SLIST     = 126;
	const CN_SLISTRSP  = 127;
	const CN_SNAME     = 128;
	const CN_SNAMERSP  = 129;
	const CN_SSETNAME  = 130;
	const CN_MOREAD    = 131;
	const CN_MOWRITE   = 132;

	const CN_UNDEFINED = 254;


	/* Map differnt commands to descriptive names */
	private $idToNameMap = array(
         // General:
         Command::CN_GINITCONNECTION => "(g) Init connection",
         Command::CN_GSETUP          => "(g) Setup command",
         Command::CN_GSETUPRSP       => "(g) Setup response command",
         Command::CN_GKILL           => "(g) Kill daemon",
         Command::CN_GLIMIT          => "(g) Limit",
         Command::CN_GLIMITSTAT      => "(g) Limit status",
         Command::CN_GLIMITSTATRSP   => "(g) Limit status response",
         Command::CN_GUPTIME         => "(g) Daemon uptime request",
         Command::CN_GUPTIMERSP      => "(g) Daemon uptime response",
         Command::CN_GLIST           => "(g) List request",
         Command::CN_GLISTRSP        => "(g) List request response",
         // Context:
         Command::CN_CCREATEWITHDATA => "(c) Create context (data)",
         Command::CN_CLAST           => "(c) Last context",
         Command::CN_CLASTRSP        => "(c) Last context response",
         Command::CN_CSTART          => "(c) Start context",
         Command::CN_CSTOP           => "(c) Stop context",
         Command::CN_CABORT          => "(c) Abort context",
         Command::CN_CSTATUS         => "(c) Status",
         Command::CN_CSTATUSRSP      => "(c) Status response",
         Command::CN_CALLSTATUSRSP   => "(c) Status all response",
         Command::CN_CMSTATUS        => "(c) Status, multiple",
         Command::CN_CMSTATUSRSP     => "(c) Status, multiple response",
         Command::CN_CFILEINFO       => "(c) File info",
         Command::CN_CFILEINFORSP    => "(c) File info response",
         Command::CN_CALLFILEINFORSP => "(c) File info all response",
         Command::CN_CCLEAN          => "(c) Clean",
         Command::CN_CCLEANRSP       => "(c) Clean response",
         Command::CN_CLIMIT          => "(c) Limit",
         Command::CN_CLIMITSTATUS    => "(c) Limit status",
         Command::CN_CLIMITSTATUSRSP => "(c) Limit status response",
         Command::CN_CPEERS          => "(c) List peers",
         Command::CN_CPEERSRSP       => "(c) List peers response",
         Command::CN_CSETFILES       => "(c) Set files",
			Command::CN_CGETFILES       => "(c) Get selected files",
			Command::CN_CGETFILESRSP    => "(c) Get selected files response",
         Command::CN_CGETTRACKERS    => "(c) Get used trackers",
         Command::CN_CGETTRACKERSRSP => "(c) Get used trackers response",
			Command::CN_CMOVE           => "(c) Move",
			Command::CN_CCREATEFROMURL  => "(c) Create context from URL",
			Command::CN_CCREATEFROMURLRSP => "(c) Create context from URL response",
			Command::CN_CURLSTATUS        => "(c) URL download status",
			Command::CN_CURLSTATUSRSP     => "(c) URL download status response",

         // General:
         Command::CN_ERROR           => "(g) Error",
         Command::CN_ACK             => "(g) Ack",
         // Session:
         Command::CN_SATTACH         => "(s) Attach",
         Command::CN_SDETACH         => "(s) Detach",
         Command::CN_SERROR          => "(s) Error",
			Command::CN_SINFO           => "(s) Info",
			Command::CN_SINFORSP        => "(s) Info response",
         Command::CN_SQUIT           => "(s) Quit",
         Command::CN_SLIST           => "(s) List",
         Command::CN_SLISTRSP        => "(s) List response",
         Command::CN_SNAME           => "(s) Session name",
         Command::CN_SNAMERSP        => "(s) Session name response",
         Command::CN_SSETNAME        => "(s) Set session name",
         Command::CN_MOREAD          => "(m) RO mode",
         Command::CN_MOWRITE         => "(m) RW mode",
         // Undefined.
         Command::CN_UNDEFINED       => "Undefined"
      );

	/* Map differnt commands to method names */
	private $idToMethodNameMap = array(
            // General:
            Command::CN_GINITCONNECTION => "general.initconnection",
            Command::CN_GSETUP          => "general.setup",
            Command::CN_GSETUPRSP       => "general.setuprsp",
            Command::CN_GKILL           => "general.kill",
            Command::CN_GLIMIT          => "general.limit",
            Command::CN_GLIMITSTAT      => "general.limitstat",
            Command::CN_GLIMITSTATRSP   => "general.limitstatrsp",
            Command::CN_GUPTIME         => "general.uptime",
            Command::CN_GUPTIMERSP      => "general.uptimersp",
            Command::CN_GLIST           => "general.list",
            Command::CN_GLISTRSP        => "general.listrsp",
            // Context:
            Command::CN_CCREATEWITHDATA => "context.createwithdata",
            Command::CN_CLAST           => "context.last",
            Command::CN_CLASTRSP        => "context.lastrsp",
            Command::CN_CSTART          => "context.start",
            Command::CN_CCREATEWITHDATA => "context.createwithdata",
            Command::CN_CSTART          => "context.start",
            Command::CN_CSTOP           => "context.stop",
            Command::CN_CABORT          => "context.abort",
            Command::CN_CSTATUS         => "context.status",
            Command::CN_CSTATUSRSP      => "context.statusrsp",
            Command::CN_CALLSTATUSRSP   => "context.statusallrsp",
            Command::CN_CMSTATUS        => "context.statusm",
            Command::CN_CMSTATUSRSP     => "context.statusmrsp",
            Command::CN_CFILEINFO       => "context.fileinfo",
            Command::CN_CFILEINFORSP    => "context.fileinforsp",
            Command::CN_CALLFILEINFORSP => "context.fileinfoallrsp",
            Command::CN_CCLEAN          => "context.clean",
            Command::CN_CCLEANRSP       => "context.cleanrsp",
            Command::CN_CLIMIT          => "context.limit",
            Command::CN_CLIMITSTATUS    => "context.limitstatus",
            Command::CN_CLIMITSTATUSRSP => "context.limitstatusrsp",
            Command::CN_CPEERS          => "context.peers",
            Command::CN_CPEERSRSP       => "context.peersrsp",
            Command::CN_CSETFILES       => "context.setfiles",
            Command::CN_CGETFILES       => "context.getfiles",
            Command::CN_CGETFILESRSP    => "context.getfilesrsp",
            Command::CN_CGETTRACKERS    => "context.gettrackers",
            Command::CN_CGETTRACKERSRSP => "context.gettrackersrsp",
				Command::CN_CMOVE           => "context.move",
				Command::CN_CCREATEFROMURL  => "context.createfromurl",
				Command::CN_CCREATEFROMURLRSP => "context.createfromurlrsp",
				Command::CN_CURLSTATUS        => "context.urlstatus",
				Command::CN_CURLSTATUSRSP     => "context.urlstatusrsp",

            // General:
            Command::CN_ERROR           => "general.error",
            Command::CN_ACK             => "general.ack",
            // Session:
            Command::CN_SATTACH         => "session.attach",
            Command::CN_SDETACH         => "session.detach",
            Command::CN_SERROR          => "session.error",
				Command::CN_SINFO           => "session.info",
				Command::CN_SINFORSP        => "session.inforsp",
            Command::CN_SQUIT           => "session.quit",
            Command::CN_SLIST           => "session.list",
            Command::CN_SLISTRSP        => "session.listrsp",
            Command::CN_SNAME           => "session.name",
            Command::CN_SNAMERSP        => "session.namersp",
            Command::CN_SSETNAME        => "session.setname",
            Command::CN_MOREAD          => "session.romode",
            Command::CN_MOWRITE         => "session.rwmode",
            // Undefined.
            Command::CN_UNDEFINED       => "Undefined"
		);
	
	/* Command specific vars */
	private $type;

	public function Command($type)
	{
		$this->type = $type;
	}

	public function getType()
	{
		return $this->type;
	}

	public function getName()
	{
		return $this->getNameById($this->type);
	}

	public function getNameById($type)
	{
		if(isset($this->idToNameMap[$type]))
			return $this->idToNameMap[$type];
		else
			return $this->idToNameMap[Command::CN_UNDEFINED];
	}

	public function getMethodName()
	{
		return $this->getMethodNameById($this->type);
	}

	public function getMethodNameById($type)
	{
		if(isset($this->idToMethodNameMap[$type]))
			return $this->idToMethodNameMap[$type];
		else
			return $this->idToMethodNameMap[Command::CN_UNDEFINED];
	}

	/**
		Returns an array with items, ready to send to xmlrpc clients query_arg()
	*/
	public function serialize(&$a = array())
	{
		$a[] = $this->getMethodName();
		$this->intToBytes($a, $this->type);
		return $a;
	}

	public function deserialize(&$data)
	{
		$this->bytesToInt($cmdid, $data);

		if($this->getType() !== $cmdid)
			throw new BTGException("Command::deserialize failed, deserialization says $cmdid, command says ".$this->getType()."\n");
	}

	public function isInitialized()
	{
		if($this->getType() != Command::CN_UNDEFINED)
			return true;
		else
			return false;
	}

	public function toString()
	{
		if($this->isInitialized())
			return $this->getName() . ".";
		else
			return $this->getName() . ", not initialized.";
	}
}

?>

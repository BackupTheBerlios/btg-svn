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
 * $Id: uptime.php,v 1.1.2.1 2006/03/27 07:17:08 jstrom Exp $
 */
if(!defined("BTG_BCORE_PATH"))
	define("BTG_BCORE_PATH", "bcore");

require_once(BTG_BCORE_PATH."/command/command.php");

class uptimeCommand extends Command
{
	public function uptimeCommand()
	{
		parent::__construct(Command::CN_GUPTIME);
	}
}

class uptimeResponseCommand extends Command
{
	private $uptime;

	public function uptimeResponseCommand($uptime=0)
	{
		parent::__construct(Command::CN_GUPTIMERSP);
		$this->uptime = $uptime;
	}

	public function toString()
	{
		return parent::toString() . 
			" Uptime=" . $this->uptime;
	}

	public function getUptime()
	{
		return $this->uptime;
	}

	public function serialize(&$a = array())
	{
		$a = parent::serialize();
		$this->longToBytes($a, $this->uptime);
		
		return $a;
	}

	public function deserialize(&$data)
	{
		parent::deserialize($data);

		$this->bytesToLong($this->uptime, $data);
	}
}

?>

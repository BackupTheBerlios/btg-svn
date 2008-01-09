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
 * $Id: ack.php,v 1.1.2.1 2006/03/27 07:17:08 jstrom Exp $
 */
if(!defined("BTG_BCORE_PATH"))
	define("BTG_BCORE_PATH", "bcore");

require_once(BTG_BCORE_PATH."/command/command.php");

class ackCommand extends Command
{
	private $command_type;
	public function ackCommand($command_type=Command::CN_UNDEFINED)
	{
		parent::__construct(Command::CN_ACK);
		$this->command_type = $command_type;
	}

	public function toString()
	{
		return parent::toString() . 
			" Ack for " . $this->getNameById($this->command_type).".";
	}

	public function setCommandType($command_type)
	{
		$this->command_type = $command_type;
	}

	public function getCommandType()
	{
		return $this->command_type;
	}

	public function serialize(&$a = array())
	{
		$a = parent::serialize();
		$this->intToBytes($a, $this->command_type);
		return $a;
	}

	public function deserialize(&$data)
	{
		parent::deserialize($data);
		$this->bytesToInt($this->command_type, $data);
	}
}

?>

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
 * $Id: session_error.php,v 1.1.2.1 2006/03/27 07:17:08 jstrom Exp $
 */
if(!defined("BTG_BCORE_PATH"))
	define("BTG_BCORE_PATH", "bcore");

require_once(BTG_BCORE_PATH."/command/command.php");

class sessionErrorCommand extends Command
{
	private $which_command;

	public function sessionErrorCommand($which_command=Command::CN_UNDEFINED)
	{
		parent::__construct(Command::CN_SERROR);
		$this->which_command = $which_command;
	}

	public function toString()
	{
		return parent::toString() . 
			" Failed command=" . $this->getNameById($this->which_command);
	}

	public function getErrorCommand()
	{
		return $this->which_command;
	}

	public function serialize(&$a = array())
	{
		$a = parent::serialize();

		$command_present = true;
		if($this->which_command == Command::CN_UNAVAILABLE)
			$command_present = false;
		
		$this->boolToBytes($a, $command_present);

		if($command_present)
			$this->intToBytes($a, $this->which_command);
		
		return $a;
	}

	public function deserialize(&$data)
	{
		parent::deserialize($data);

		$this->which_command = Command::CN_UNDEFINED;

		$command_present = false;
		$this->bytesToBool($command_present, $data);

		if($command_present)
			$this->bytesToInt($this->which_command, $data);
	}
}

?>

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
 * $Id: session_list.php,v 1.1.2.1 2006/03/27 07:17:08 jstrom Exp $
 */
if(!defined("BTG_BCORE_PATH"))
	define("BTG_BCORE_PATH", "bcore");

require_once(BTG_BCORE_PATH."/command/command.php");

class sessionNameCommand extends Command
{
	public function sessionNameCommand()
	{
		parent::__construct(Command::CN_SNAME);
	}
}

class sessionNameResponseCommand extends Command
{
	private $name;
	public function sessionNameResponseCommand()
	{
		parent::__construct(Command::CN_SNAMERSP);
		$this->name    = "";
	}

	public function toString()
	{
		return parent::toString();
	}

	public function getName()
	{
		return $this->name;
	}

	public function serialize(&$a = array())
	{
		$a = parent::serialize();

		$this->stringToBytes($a, $this->name);

		return $a;
	}

	public function deserialize(&$data)
	{
		parent::deserialize($data);

		$this->bytesToString($this->name, $data);

	}
}

class setSessionNameCommand extends Command
{
	private $name;
	public function setSessionNameCommand($sessionname)
	{
		parent::__construct(Command::CN_SSETNAME);
		$this->name = $sessionname;
	}

	public function toString()
	{
		return parent::toString();
	}

	public function getName()
	{
		return $this->name;
	}

	public function serialize(&$a = array())
	{
		$a = parent::serialize();

		$this->stringToBytes($a, $this->name);

		return $a;
	}

	public function deserialize(&$data)
	{
		parent::deserialize($data);

		$this->bytesToString($this->name, $data);
	}
}

?>

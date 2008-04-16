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
 * $Id: initconnection.php,v 1.1.2.1 2006/03/27 07:17:08 jstrom Exp $
 */
if(!defined("BTG_BCORE_PATH"))
	define("BTG_BCORE_PATH", "bcore");

require_once(BTG_BCORE_PATH."/command/command.php");
require_once(BTG_BCORE_PATH."/hash.php");

class initConnectionCommand extends Command
{
	private $username;
	private $hash;

	// @param $username Username to use
	// @param $hash If a string, it is used as a cleartext password from which a hash is generated. If its a Hash object, that hash object is used.
	public function initConnectionCommand($username = "", $hash = "")
	{
		parent::__construct(Command::CN_GINITCONNECTION);
		$this->username = $username;
		if(!$hash)
			$this->hash = new Hash();
		else
		{
			// A string, create a new hash object
			if(is_string($hash))
			{
				$this->hash = new Hash();
				$this->hash->generate($hash);
			}else if($hash instanceof Hash)
			{
				$this->hash = $hash;
			}else
			{
				throw new BTGException("initConnectionCommand::initConnectionCommand failed. Hash is '$hash' of type ".gettype($hash).".");
			}
		}
	}

	public function getUsername()
	{
		return $this->username;
	}

	public function getPassword()
	{
		return $this->hash;
	}

	public function setUsername($username)
	{
		$this->username = $username;
	}

	public function setPassword($password)
	{
		$this->hash = $password;
	}

	public function serialize(&$a = array())
	{
		$a = parent::serialize();
		$this->stringToBytes($a, $this->username);
		$this->hash->serialize($a);
		return $a;
	}

	public function deserialize(&$data)
	{
		parent::deserialize($data);

		$this->stringToBytes($this->username);
		$hash = new Status();
		$hash->deserialize($data);
		$this->status = $hash;
	}
}

?>

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
 * $Id: peer.php,v 1.1.2.2 2006/06/20 22:39:17 jstrom Exp $
 */
if(!defined("BTG_BCORE_PATH"))
	define("BTG_BCORE_PATH", "bcore");

require_once(BTG_BCORE_PATH."/serializable.php");

class peerAddress extends BTGSerializable
{
	/// IP
	private $ip;

	/// Constructor, $pieces should be an array of booleans
	public function peerAddress($ip0=0, $ip1=0, $ip2=0, $ip3=0)
	{
		$this->ip = array($ip0, $ip1, $ip2, $ip3);
	}

	/// Get the address as a string.
	public function getAddress()
	{
		return join(".", $this->ip);
	}

	public function serialize(&$a = array())
	{
		for($x = 0; $x < 4; $x++)
		{
			$this->intToBytes($a, $this->ip[$x]);
		}
	}

	public function deserialize(&$data)
	{
		for($x = 0; $x < 4; $x++)
		{
			$b = 0;
			$this->bytesToInt($b, $data);
			$this->ip[$x] = $b;
		}
	}

	public function toString()
	{
		return $this->getAddress();
	}
}

/**********/

class Peer extends BTGSerializable
{
	/// The address of this peer.
	private $peeraddress;

	/// True if this peer is a seeder.
	private $seeder;

	/// Peer identification 
	private $identification;

	/// Constructor
	public function Peer($peeraddress=NULL, $seeder = false, $identification="")
	{
		if($peeraddress == NULL)
			$this->peeraddress = new peerAddress();
		else
			$this->peeraddress = $peeraddress;

		$this->seeder = (bool)$seeder;
		$this->identification = $identification;
	}
	
	public function serialize(&$a = array())
	{
		$this->peeraddress->serialize($a);
		$this->boolToBytes($a, $this->seeder);
		$this->stringToBytes($a, $this->identification);
	}

	public function deserialize(&$data)
	{
		$this->peeraddress->deserialize($data);
		$this->bytesToBool($this->seeder, $data);
		$this->bytesToString($this->identification, $data);
	}

	public function getAddress()
	{
		return $this->peeraddress;
	}

	public function getSeeder()
	{
		return $this->seeder;
	}

	public function getIdentification()
	{
		return $this->identification;
	}

	public function toString()
	{
		return $this->peeraddress->toString() . ($this->seeder?", seeding":"") . ", ". $this->getIdentification();
	}
}
?>

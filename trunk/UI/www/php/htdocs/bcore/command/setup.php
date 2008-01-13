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
 * $Id: setup.php,v 1.1.2.3 2007/06/02 18:31:30 jstrom Exp $
 */
if(!defined("BTG_BCORE_PATH"))
	define("BTG_BCORE_PATH", "bcore");

require_once(BTG_BCORE_PATH."/btgexception.php");
require_once(BTG_BCORE_PATH."/command/command.php");
require_once(BTG_BCORE_PATH."/serializable.php");
require_once(BTG_BCORE_PATH."/addrport.php");

class requiredSetupData extends BTGSerializable
{
	/// Build ID
	private $buildID;
	/// Seed limit
	private $seedLimit;
	/// Seed timeout
	private $seedTimeout;
	/// Use DHT
	private $useDHT;
	/// Use Encryption
	private $useEncryption;

	public function requiredSetupData($buildID="", $seedLimit = -1, $seedTimeout = -1, $DHT = true, $Encryption = true)
	{
		$this->buildID       = $buildID;
		$this->seedLimit     = $seedLimit;
		$this->seedTimeout   = $seedTimeout;
		$this->useDHT        = $DHT;
		$this->useEncryption = $Encryption;
	}

	public function toString()
	{
		$ret = "\nRequired Data:\n";
		$ret.= "seed limit: ".$this->seedLimit.", seed timeout: ".$this->seedTimeout.", use dht: ".$this->useDHT.", use encryption: ".$this->useEncryption."\n";
		return $ret;
	}

	public function getBuildID()
	{
		return $this->buildID;
	}
	
	public function getSeedLimit()
	{
		return $this->seedLimit;
	}

	public function getSeedTimeout()
	{
		return $this->seedTimeout;
	}

	public function serialize(&$a = array())
	{
		$this->stringToBytes($a, $this->buildID);
		$this->intToBytes($a, $this->seedLimit);
		$this->longToBytes($a, $this->seedTimeout);
		$this->boolToBytes($a, $this->useDHT);
		$this->boolToBytes($a, $this->useEncryption);
		return $a;
	}

	public function deserialize(&$data)
	{
		parent::deserialize($data);

		$this->bytesToString($this->buildID, $data);
		$this->bytesToInt($this->seedLimit, $data);
		$this->bytesToLong($this->seedTimeout, $data);
		$this->bytesToBool($this->useDHT, $data);
		$this->bytesToBool($this->useEncryption, $data);
	}
}

class setupCommand extends Command
{
	private $rsd;
	public function setupCommand($rsd=NULL)
	{
		parent::__construct(Command::CN_GSETUP);
		if(!($rsd instanceof requiredSetupData))
			throw new BTGException("Invalid rsd parameter to setupCommand");

		$this->rsd = $rsd;
	}

	public function toString()
	{
		return parent::toString() . $this->rsd->toString();
	}

	public function getRequiredData()
	{
		return $this->rsd;
	}

	public function serialize(&$a = array())
	{
		$a = parent::serialize();
		$this->rsd->serialize($a);
		return $a;
	}

	public function deserialize(&$data)
	{
		parent::deserialize($data);
		$this->rsd->deserialize($data);
	}
}

class setupResponseCommand extends Command
{
	private $setup;

	public function setupResponseCommand($session=0)
	{
		parent::__construct(Command::CN_GSETUPRSP);
		$this->session = $session;
	}

	public function toString()
	{
		return parent::toString() . 
			" session=" . $this->session;
	}

	public function setSession($session)
	{
		$this->session = $session;
	}

	public function getSession()
	{
		return $this->session;
	}

	public function serialize(&$a = array())
	{
		$a = parent::serialize();
		$this->longToBytes($a, $this->session);
		
		return $a;
	}

	public function deserialize(&$data)
	{
		parent::deserialize($data);

		$this->bytesToLong($this->session, $data);
	}
}

?>

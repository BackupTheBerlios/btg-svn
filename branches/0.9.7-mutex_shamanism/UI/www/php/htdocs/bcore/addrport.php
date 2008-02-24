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
 * $Id: addrport.php,v 1.1.2.1 2006/03/27 07:17:08 jstrom Exp $
 */

if(!defined("BTG_BCORE_PATH"))
	define("BTG_BCORE_PATH", "bcore");

require_once(BTG_BCORE_PATH."/serializable.php");

class addressPort extends BTGSerializable
{
	/// IP
	private $ip;
	/// Port 
	private $port;

	/// Constructor, $pieces should be an array of booleans
	public function addressPort($ip0=0, $ip1=0, $ip2=0, $ip3=0, $port=0)
	{
		$this->ip = array($ip0, $ip1, $ip2, $ip3);
		$this->port = $port;
	}

	public function getPort()
	{
		return $this->port;
	}

	/// Get the ip as a string.
	public function getIp()
	{
		return join(".", $this->ip);
	}

	/// Read IP in format a.b.c.d:p
	public function fromString($s)
	{
		$lst = split(":", $s);
		if(count($lst) >= 2)
		{
			$input_ipv4 = $lst[0];
			$input_port = $lst[1];

			$ip_lst = split(".", $input_ipv4);
			if(count($ip_lst) == 4)
			{
				$temp_ip = array();
				for($i=0; $i<4; $i++)
				{
					$ip_part = (int)$ip_lst[$i];
					if($ip_part < 0 || $ip_part > 254)
						return false;
				}

				$input_port = (int)$input_port;
				if($input_port < 0 || $input_port > 65535)
					return false;
				
				$this->ip = $temp_ip;
				$this->port = $input_port;
				return true;
			}
		}
		return false;
	}

	/// Return in format a.b.c.d:p
	public function toString()
	{
		return join(".", $this->ip) . ":". $this->port;
	}

	/// Check if the IP and port is valid
	public function valid()
	{
		if($this->ip[0] == 0 && 
			$this->ip[1] == 0 && 
			$this->ip[2] == 0 && 
			$this->ip[3] == 0)
			return false;
		
		if($this->port == 0)
			return false;

		return true;
	}

	public function serialize(&$a = array())
	{
		for($x = 0; $x < 4; $x++)
		{
			$this->intToBytes($a, $this->ip[$x]);
		}
		$a[] = $this->port;
	}

	public function deserialize(&$data)
	{
		for($x = 0; $x < 4; $x++)
		{
			$b = 0;
			$this->bytesToInt($b, $data);
			$this->ip[$x] = $b;
		}
		$this->bytesToInt($this->port, $data);
	}
}

?>

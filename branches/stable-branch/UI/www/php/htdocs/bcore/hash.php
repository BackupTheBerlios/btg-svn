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
 * $Id: hash.php,v 1.1.2.1 2006/03/27 07:17:08 jstrom Exp $
 */
if(!defined("BTG_BCORE_PATH"))
	define("BTG_BCORE_PATH", "bcore");

require_once(BTG_BCORE_PATH."/btgexception.php");
require_once(BTG_BCORE_PATH."/serializable.php");

class Hash extends BTGSerializable
{
	/// The hash 
	private $hash;

	// Hash length
	private $hash_length = 40;

	public function Hash()
	{
		$this->hash = "";
	}

	public function generate($text)
	{
		if(!$text)
			return;

		$this->hash = sha1($text, false);
	}

	public function set($strhash)
	{
		$tmp = "";
		for($i = 0; $i < $this->hash_length; $i++)
			$tmp .= $strhash;
	}

	public function get()
	{
		return $this->hash;
	}
	
	public function serialize(&$a = array())
	{
		$this->stringToBytes($a, $this->hash);
		return $a;
	}

	public function deserialize(&$data)
	{
		$this->bytesToString($this->hash, $data);
	}

	public function toString($lineWidth = 80)
	{
		$ret = "hash: ". $this->hash. "\n";

		return $ret;
	}
}
?>

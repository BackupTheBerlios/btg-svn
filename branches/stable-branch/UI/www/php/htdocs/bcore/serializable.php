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
 * $Id: serializable.php,v 1.1.2.2 2006/05/11 22:55:38 jstrom Exp $
 */

require_once(BTG_BCORE_PATH."/btgexception.php");
require_once(BTG_BCORE_PATH."/xmlrpc.php");

class BTGSerializable
{
	/* Externalization functions, to check that the types are valid */
	protected function addByte(&$dst, &$src)
	{
		if(!is_int($src))
			throw new BTGException("BTGSerializable::addByte failed. Value is '$src' of type ".gettype($src).".");
		$dst[] = $src;
	}

	protected function getByte(&$dst, &$src)
	{
		$value = array_shift($src);
		if(!is_int($value))
			throw new BTGException("BTGSerializable::getByte failed. Value is '$src' of type ".gettype($src).".");
		$dst = $value;
	}

	protected function boolToBytes(&$dst, &$src)
	{
		if(!is_bool($src))
			throw new BTGException("BTGSerializable::boolToBytes failed. Value is '$src' of type ".gettype($src).".");
		$dst[] = $src;
	}

	protected function bytesToBool(&$dst, &$src)
	{
		$value = array_shift($src);
		if(!is_bool($value))
			throw new BTGException("BTGSerializable::bytesToBool failed. Value is '$value' of type ".gettype($value).".");

		$dst = $value;
	}

	protected function intToBytes(&$dst, &$src)
	{
		if(!is_numeric($src) || !is_int($src))
			throw new BTGException("BTGSerializable::intToBytes failed. Value is '$src' of type ".gettype($src).".");
		$dst[] = $src;
	}

	protected function bytesToInt(&$dst, &$src)
	{
		$value = array_shift($src);

		if(!is_numeric($value) || !is_int($value))
			throw new BTGException("BTGSerializable::bytesToInt failed. Value is '$value' of type ".gettype($value).".");

		$dst = $value;
	}

	protected function uintToBytes(&$dst, &$src)
	{
		if(!is_numeric($src) || !is_int($src))
			throw new BTGException("BTGSerializable::uintToBytes failed. Value is '$src' of type ".gettype($src).".");
		$dst[] = (string)$src;
	}

	protected function bytesToUint(&$dst, &$src)
	{
		$value = array_shift($src);

		if(!is_string($value))
			throw new BTGException("BTGSerializable::bytesToUint failed. Value is '$value' of type ".gettype($value).".");

		$value = (int)$value;

		if(!is_numeric($value) || !is_int($value))
			throw new BTGException("BTGSerializable::bytesToUint failed. Value is '$value' of type ".gettype($value).".");

		$dst = $value;
	}

	protected function longToBytes(&$dst, &$src)
	{
		// use floats since php doenst handle long ints very well
		if(!is_numeric($src) || (!is_int($src) && is_float($src) && round($src, 0) != $src ) ) 
			throw new BTGException("BTGSerializable::longToBytes failed. Value is '$src' of type ".gettype($src).".");
		$dst[] = (string)$src;
	}

	protected function bytesToLong(&$dst, &$src)
	{
		$value = array_shift($src);

		if(!is_string($value))
			throw new BTGException("BTGSerializable::bytesToLong failed. Value is '$value' of type ".gettype($value).".");

		// Use float/double since php doesnt handle big ints very well...
		$value = (float)$value;

		if(!is_numeric($value) || !is_float($value))
			throw new BTGException("BTGSerializable::bytesToLong failed. Value is '$value' of type ".gettype($value).".");

		$dst = $value;
	}
	
	protected function uLongToBytes(&$dst, &$src)
	{
		if(!is_numeric($src) || !is_int($src))
			throw new BTGException("BTGSerializable::ulongToBytes failed. Value is '$src' of type ".gettype($src).".");
		$dst[] = (string)$src;
	}

	protected function bytesToULong(&$dst, &$src)
	{
		$value = array_shift($src);

		if(!is_string($value))
			throw new BTGException("BTGSerializable::bytesTUoLong failed. Value is '$value' of type ".gettype($value).".");

		$value = (double)$value;

		if(!is_numeric($value) || !is_double($value))
			throw new BTGException("BTGSerializable::bytesToULong failed. Value is '$value' of type ".gettype($value).".");

		$dst = $value;
	}

	protected function floatToBytes(&$dst, &$src)
	{
		if(!is_numeric($src) || !is_float($src))
			throw new BTGException("BTGSerializable::floatToBytes failed. Value is '$src' of type ".gettype($src).".");
		$dst[] = $src;
	}

	protected function bytesToFloat(&$dst, &$src)
	{
		$value = array_shift($src);
		if(!is_numeric($value) || !is_float($value))
			throw new BTGException("BTGSerializable::bytesToFloat failed. Value is '$value' of type ".gettype($value).".");

		$dst = $value;
	}

	protected function intListToBytes(&$dst, &$src)
	{
		$dst[] = count($src);

		foreach($src as $subvalue)
		{
			if(!is_numeric($subvalue) || !is_int($subvalue))
				throw new BTGException("BTGSerializable::intListToBytes failed. Subvalue is '$subvalue' of type ".gettype($subvalue).".");

			$dst[] = $subvalue;
		}
	}
	
	protected function bytesToIntList(&$dst, &$src)
	{
		$this->bytesToInt($numItems, $src);

		$dst = array();
		for($x = 0; $x < $numItems; $x++)
		{
			$this->bytesToInt($value, $src);
			$dst[] = $value;
		}
	}

	protected function stringListToBytes(&$dst, &$src)
	{
		$dst[] = count($src);

		foreach($src as $subvalue)
		{
			if(!is_string($subvalue))
				throw new BTGException("BTGSerializable::stringListToBytes failed. Subvalue is '$subvalue' of type ".gettype($subvalue).".");

			$dst[] = $subvalue;
		}
	}

	protected function bytesToStringList(&$dst, &$src)
	{
		$this->bytesToInt($numItems, $src);

		$dst = array();
		for($x = 0; $x < $numItems; $x++)
		{
			$this->bytesToString($value, $src);
			$dst[] = $value;
		}
	}

	protected function stringToBytes(&$dst, &$src)
	{
		if(!is_string($src))
			throw new BTGException("BTGSerializable::stringToBytes failed. Value is '$src' of type ".gettype($src).".");
		$dst[] = htmlspecialchars($src);
	}

	protected function bytesToString(&$dst, &$src)
	{
		$value = array_shift($src);
		if(!is_string($value))
			throw new BTGException("BTGSerializable::bytesToString failed. Value is '$value' of type ".gettype($value).".");

		$dst = $value;
	}

	protected function addBytes(&$dst, &$src, $size)
	{
		$dst[] = (int)$size;
		$dst[] = new XMLRPC_Base64($src);
	}

	protected function getBytes(&$dst, &$src, &$size)
	{
		$this->bytesToInt($size, $src);

		$value = array_shift($src);
		if(!is_string($value))
			throw new BTGException("BTGSerializable::getBytes failed. Value is '$value' of type ".gettype($value).".");

		$dst = $value;
	}

	public function serialize(&$a = array())
	{
		throw new BTGException("Not implemented!");
	}

	public function deserialize(&$data)
	{
		throw new BTGException("Not implemented!");
	}
}
?>

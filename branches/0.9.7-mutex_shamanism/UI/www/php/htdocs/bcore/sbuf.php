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
 * $Id: sbuf.php,v 1.1.2.1 2006/03/27 07:17:08 jstrom Exp $
 */
if(!defined("BTG_BCORE_PATH"))
	define("BTG_BCORE_PATH", "bcore");

require_once(BTG_BCORE_PATH."/serializable.php");

class sBuffer extends BTGSerializable
{
	/// The buffer. Yes PHP(5) strings are binary safe... Even for \0's
	private $buffer;

	/// Constructor
	public function sBuffer($data="")
	{
		$this->buffer = $data;
	}

	/// Read a file to the buffer
	public function read($file)
	{
		$buffer = "";
		$fh = @fopen($file, "rb");
		if(!$fh)
			throw new BTGException("Cannot open file $file.");

		$this->buffer = fread($fh, filesize($file));
		fclose($fh);
	}

	/// Return the size of the buffer.
	public function size()
	{
		return strlen($this->buffer);
	}

	public function serialize(&$a = array())
	{
		$this->addBytes($a, $this->buffer, strlen($this->buffer));
		return $a;
	}

	public function deserialize(&$data)
	{
		$numBytes = 0;
		$this->getBytes($data, $this->buffer, $numBytes);
	}

	public function toString()
	{
		$ret = "sBuffer, size = ". strlen($this->buffer). "\n";
		return $ret;
	}
}
?>

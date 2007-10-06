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
 * $Id: context_create.php,v 1.1.2.3 2007/01/21 02:05:42 jstrom Exp $
 */
if(!defined("BTG_BCORE_PATH"))
	define("BTG_BCORE_PATH", "bcore");

require_once(BTG_BCORE_PATH."/btgexception.php");
require_once(BTG_BCORE_PATH."/command/command.php");
require_once(BTG_BCORE_PATH."/sbuf.php");

class contextCreateWithDataCommand extends Command
{
	/// Torrent file data, in sBuffer
	private $torrent_file;
	/// Filename
	private $filename;

	public function contextCreateWithDataCommand($filename="", &$torrent_file=NULL, $start=true)
	{
		parent::__construct(Command::CN_CCREATEWITHDATA);
		$this->filename = $filename;
		if($torrent_file == NULL)
			$torrent_file = new sBuffer();

		if(!($torrent_file instanceof sBuffer))
			throw new BTGException("torrent_file parameter should be an sBuffer()!");
			
		$this->torrent_file = $torrent_file;

		if(!is_bool($start))
			$start = true;

		$this->start = $start;
	}

	public function toString()
	{
		return parent::toString() . 
			" Filename=" . $this->filename."." +($this->start?" Start flag set.":"");
	}

	public function getFilename()
	{
		return $this->filename;
	}

	public function getFile()
	{
		return $this->torrent_file;
	}

	public function getStart()
	{
		return $this->start;
	}

	public function serialize(&$a = array())
	{
		$a = parent::serialize();
		$this->stringToBytes($a, $this->filename);
		$this->torrent_file->serialize($a);
		$this->boolToBytes($a, $this->start);
		return $a;
	}

	public function deserialize(&$data)
	{
		parent::deserialize($data);
		$this->bytesToString($this->filename, $data);
		$this->torrent_file = new sBuffer();
		$this->torrent_file->deserialize($data);
		$this->bytesToBool($this->start, $data);
	}
}

?>

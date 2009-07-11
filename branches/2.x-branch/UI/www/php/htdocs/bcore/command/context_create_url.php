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

class contextCreateFromUrlCommand extends Command
{
	/// URL to torrent
	private $url;
	/// Filename
	private $filename;
	// Start on create
	private $start;

	public function contextCreateFromUrlCommand($filename="", $url="", $start=true)
	{
		parent::__construct(Command::CN_CCREATEFROMURL);
		$this->filename = $filename;
		$this->url = $url;

		if(!is_bool($start))
			$start = true;

		$this->start = $start;
	}

	public function toString()
	{
		return parent::toString() . 
			" Filename=" . $this->filename.". URL=".$this->url.".". ($this->start?" Start flag set.":"");
	}

	public function getFilename()
	{
		return $this->filename;
	}

	public function getUrl()
	{
		return $this->url;
	}

	public function getStart()
	{
		return $this->start;
	}

	public function serialize(&$a = array())
	{
		$a = parent::serialize();
		$this->stringToBytes($a, $this->filename);
		$this->stringToBytes($a, $this->url);
		$this->boolToBytes($a, $this->start);
		return $a;
	}

	public function deserialize(&$data)
	{
		parent::deserialize($data);
		$this->bytesToString($this->filename, $data);
		$this->bytesToString($this->url, $data);
		$this->bytesToBool($this->start, $data);
	}
}

class contextCreateFromUrlResponseCommand extends Command
{
	/// URL id
	private $id;

	public function contextCreateFromUrlResponseCommand($id=-1)
	{
		parent::__construct(Command::CN_CCREATEFROMURLRSP);
		$this->id = $id;
	}

	public function toString()
	{
		return parent::toString() . 
			" id=".$this->id;
	}

	public function getId()
	{
		return $this->id;
	}

	public function serialize(&$a = array())
	{
		$a = parent::serialize();
		$this->uuintToBytes($a, $this->id);
		return $a;
	}

	public function deserialize(&$data)
	{
		parent::deserialize($data);
		$this->bytesToUint($this->id, $data);
	}
}

?>

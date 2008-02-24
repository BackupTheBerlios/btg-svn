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
 * $Id: list_rsp.php,v 1.1.2.1 2006/03/27 07:17:08 jstrom Exp $
 */
if(!defined("BTG_BCORE_PATH"))
	define("BTG_BCORE_PATH", "bcore");

require_once(BTG_BCORE_PATH."/command/command.php");

class listCommandResponse extends Command
{
	private $context_ids;
	private $filenames;
	public function listCommandResponse($context_ids=array(), $filenames=array())
	{
		parent::__construct(Command::CN_GLISTRSP);
		$this->context_ids = $context_ids;
		$this->filenames = $filenames;
	}

	public function getStringList()
	{
		$ret="";
		$i=0;
		foreach($this->context_ids as $context_id)
		{
			$ret.=$context_id . ": ".$this->filenames[$i]. "\n";
			$i++;
		}
		return $ret;
	}

	public function getIDs()
	{
		return $this->context_ids;
	}

	public function getFilenames()
	{
		return $this->filenames;
	}

	public function serialize(&$a = array())
	{
		$a = parent::serialize();
		$this->intListToBytes($this->context_ids);
		$this->stringListToBytes($this->filenames);
		return $a;
	}

	public function deserialize(&$data)
	{
		parent::deserialize($data);

		$this->bytesToIntList($this->context_ids, $data);
		$this->bytesToStringList($this->filenames, $data);
	}
}

?>

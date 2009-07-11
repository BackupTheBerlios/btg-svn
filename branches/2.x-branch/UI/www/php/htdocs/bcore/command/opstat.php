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

class opStatusCommand extends Command
{
	/// Id.
	private $opid;
	/// Type.
	private $optype;

	public function opStatusCommand($opid=-1, $optype=-1)
	{
		parent::__construct(Command::CN_OPSTATUS);
		$this->opid   = $opid;
		$this->optype = $optype;
	}

	public function getId()
	{
		return $this->opid;
	}

	public function getOpType()
	{
		return $this->optype;
	}

	public function serialize(&$a = array())
	{
		$a = parent::serialize();
		$this->uintToBytes($a, $this->opid);
		$this->uintToBytes($a, $this->optype);
		return $a;
	}

	public function deserialize(&$data)
	{
		parent::deserialize($data);
		$this->bytesToUint($this->opid, $data);
		$this->bytesToUint($this->optype, $data);
	}
}

class opStatusResponseCommand extends Command
{
	/// Id.
	private $opid;
	/// Type.
	private $optype;
	/// Value.
	private $opstatus;
	/// String, some data.
	private $opdatastr;
	public function opStatusResponseCommand($opid=-1, $optype=-1, $opstatus=-1)
	{
		parent::__construct(Command::CN_OPSTATUSRSP);
		$this->opid      = $opid;
		$this->optype    = $optype;
		$this->opstatus  = $opstatus;
		$this->opdatastr = "";
	}

	public function getId()
	{
		return $this->opid;
	}

	public function getOpType()
	{
		return $this->optype;
	}

	public function getStatus()
	{
		return $this->opstatus;
	}

	public function serialize(&$a = array())
	{
		$a = parent::serialize();
		$this->uintToBytes($a, $this->opid);
		$this->uintToBytes($a, $this->optype);
		$this->uintToBytes($a, $this->opstatus);

		if (strlen($this->opdatastr) > 0)
		{
			$b = true;
			$this->boolToBytes($a, $b);
			$this->stringToBytes($a, $this->opdatastr);
		}
		else
		{
			$b = false;
			$this->boolToBytes($a, $b);
		}

		return $a;
	}

	public function deserialize(&$data)
	{
		parent::deserialize($data);
		$this->bytesToUint($this->opid, $data);
		$this->bytesToUint($this->optype, $data);
		$this->bytesToUint($this->opstatus, $data);

		$b = false;
		$this->bytesToBool($b, $data);
/*
		if ($b == true)
		{
			$this->bytesToString($this->opdatastr, $data);
		}
*/
	}
}

?>

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
 * $Id: context_abort.php,v 1.1.2.2 2006/06/01 00:38:13 jstrom Exp $
 */
if(!defined("BTG_BCORE_PATH"))
	define("BTG_BCORE_PATH", "bcore");

require_once(BTG_BCORE_PATH."/command/command.php");

class contextAbortCommand extends contextCommand
{
	private $eraseData; // bool
	public function contextAbortCommand($context_id=contextCommand::UNDEFINED_CONTEXT, $eraseData = false, $allContexts = false)
	{
		parent::__construct(Command::CN_CABORT, $context_id, $allContexts);
		$this->eraseData = $eraseData;
	}

	public function serialize(&$a = array())
	{
		$a = parent::serialize();
		$this->boolToBytes($a, $this->eraseData);
		return $a;
	}

	public function deserialize(&$data)
	{
		parent::deserialize($data);
		$this->bytesToBool($this->eraseData, $data);
	}

	public function eraseData()
	{
		return $this->eraseData;
	}
}

?>

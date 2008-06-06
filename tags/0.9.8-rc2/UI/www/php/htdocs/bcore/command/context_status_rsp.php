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
 * $Id: context_status_rsp.php,v 1.1.2.1 2006/03/27 07:17:08 jstrom Exp $
 */
if(!defined("BTG_BCORE_PATH"))
	define("BTG_BCORE_PATH", "bcore");

require_once(BTG_BCORE_PATH."/command/context.php");
require_once(BTG_BCORE_PATH."/status.php");

class contextStatusResponseCommand extends contextCommand
{
	/// Status object
	private $status;
	public function contextStatusResponseCommand($context_id=contextCommand::UNDEFINED_CONTEXT, $status = NULL)
	{
		parent::__construct(Command::CN_CSTATUSRSP, $context_id);
		if(!$status)
			$this->status = new Status();
		else
			$this->status = $status;
	}

	public function getStatus()
	{
		return $this->status;
	}

	public function serialize(&$a = array())
	{
		$a = parent::serialize();
		$this->status->serialize($a);
		return $a;
	}

	public function deserialize(&$data)
	{
		parent::deserialize($data);
		$status = new Status();
		$status->deserialize($data);
		$this->status = $status;
	}
}

class contextAllStatusResponseCommand extends contextCommand
{
	/// Array of Status objects
	private $status;
	public function contextAllStatusResponseCommand($context_id=contextCommand::UNDEFINED_CONTEXT, $status = array())
	{
		parent::__construct(Command::CN_CALLSTATUSRSP, $context_id);
		$this->status = $status;
	}

	public function getStatus()
	{
		return $this->status;
	}

	public function serialize(&$a = array())
	{
		$a = parent::serialize();

		// Size of list
		$this->intToBytes($a, count($this->status));

		foreach($this->status as $status)
			$status->serialize($a);

		return $a;
	}

	public function deserialize(&$data)
	{
		parent::deserialize($data);

		$size = 0;
	  	$this->bytesToInt($size, $data);

		for($i=0; $i < $size; $i++)
		{
			$status = new Status();
			$status->deserialize($data);
			$this->status[] = $status;
		}
	}
}
?>

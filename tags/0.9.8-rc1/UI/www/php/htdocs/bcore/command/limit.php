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
 * $Id: limit.php,v 1.1.2.4 2007/06/27 19:18:14 wojci Exp $
 */
if(!defined("BTG_BCORE_PATH"))
	define("BTG_BCORE_PATH", "bcore");

require_once(BTG_BCORE_PATH."/command/command.php");
require_once(BTG_BCORE_PATH."/command/limit_base.php");

class limitCommand extends Command
{
	private $limitUpld;
	private $limitDwnld;
	private $maxUploads;
	private $maxConnections;
	public function limitCommand($limitBytesUpld = limitBase::LIMIT_DISABLED, $limitBytesDwnld = limitBase::LIMIT_DISABLED, $maxUploads= limitBase::LIMIT_DISABLED, $maxConnections = limitBase::LIMIT_DISABLED)
	{
		parent::__construct(Command::CN_GLIMIT);
		$this->limitUpld = $limitBytesUpld;
		$this->limitDwnld = $limitBytesDwnld;
		$this->maxUploads = $maxUploads;
		$this->maxConnections = $maxConnections;
	} 

	public function getUploadLimit()
	{
		return $this->limitUpld;
	}

	public function getDownloadLimit()
	{
		return $this->limitDwnld;
	}

	public function getMaxUploads()
	{
		return $this->maxUploads;
	}

	public function getMaxConnections()
	{
		return $this->maxConnections;
	}

	public function serialize(&$a = array())
	{
		$a = parent::serialize();
		$this->intToBytes($a, $this->limitUpld);
		$this->intToBytes($a, $this->limitDwnld);
		$this->intToBytes($a, $this->maxUploads);
		$this->intToBytes($a, $this->maxConnections);
		return $a;
	}

	public function deserialize(&$data)
	{
		parent::deserialize($data);
		$this->bytesToInt($this->limitUpld, $data);
		$this->bytesToInt($this->limitDwnld, $data);
		$this->bytesToInt($this->maxUploads, $data);
		$this->bytesToInt($this->maxConnections, $data);
	}
}

class limitStatusCommand extends Command
{
	public function limitStatusCommand()
	{
		parent::__construct(Command::CN_GLIMITSTAT);
	} 
}

class limitStatusResponseCommand extends Command
{
	private $limitUpld;
	private $limitDwnld;
	private $maxUploads;
	private $maxConnections;

	public function limitStatusResponseCommand($limitBytesUpld = limitBase::LIMIT_DISABLED, $limitBytesDwnld = limitBase::LIMIT_DISABLED, $maxUploads= limitBase::LIMIT_DISABLED, $maxConnections = limitBase::LIMIT_DISABLED)
	{
		parent::__construct(Command::CN_GLIMITSTATRSP);
		$this->limitUpld = $limitBytesUpld;
		$this->limitDwnld = $limitBytesDwnld;
		$this->maxUploads = $maxUploads;
		$this->maxConnections = $maxConnections;
	} 

	public function getUploadLimit()
	{
		return $this->limitUpld;
	}

	public function getDownloadLimit()
	{
		return $this->limitDwnld;
	}

	public function getMaxUploads()
	{
		return $this->maxUploads;
	}

	public function getMaxConnections()
	{
		return $this->maxConnections;
	}

	public function serialize(&$a = array())
	{
		$a = parent::serialize();
		$this->intToBytes($a, $this->limitUpld);
		$this->intToBytes($a, $this->limitDwnld);
		$this->intToBytes($a, $this->maxUploads);
		$this->intToBytes($a, $this->maxConnections);
		return $a;
	}

	public function deserialize(&$data)
	{
		parent::deserialize($data);
		$this->bytesToInt($this->limitUpld, $data);
		$this->bytesToInt($this->limitDwnld, $data);
		$this->bytesToInt($this->maxUploads, $data);
		$this->bytesToInt($this->maxConnections, $data);
	}
}
?>

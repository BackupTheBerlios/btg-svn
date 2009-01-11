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
 * $Id: status.php,v 1.1.2.5 2007/02/09 07:33:30 jstrom Exp $
 */
if(!defined("BTG_BCORE_PATH"))
	define("BTG_BCORE_PATH", "bcore");

require_once(BTG_BCORE_PATH."/btgexception.php");
require_once(BTG_BCORE_PATH."/serializable.php");
require_once(BTG_BCORE_PATH."/trackerstatus.php");

class Status extends BTGSerializable
{
	const ts_undefined = 0;		// Not defined
	const ts_queued = 1;			// In queue for downloading.
	const ts_checking	= 2;		// The client is hash-checking the data.
	const ts_connecting = 3;	// The client is connecting.
	const ts_downloading = 4;	// The client is downloading.
	const ts_seeding = 5;		// The client is seeding. 
	const ts_stopped = 6;		// The client stopped downloading
	const ts_finished = 7;		// The client finished downloading and is not seeding.

	private $statusNames = array(
		"Undefined", 
		"Queued",
		"Checking data",
		"Connecting",
		"Downloading",
		"Seeding",
		"Stopped",
		"Finished",
		);
	
	/// The context ID
	private $contextID;
	/// The filename
	private $filename;
	/// The status, should be one of the above ts_* consts.
	private $status;
	/// Total downloaded bytes
	private $downloadTotal;
	/// Total uploaded bytes
	private $uploadTotal;
	/// Failed bytes
	private $failedBytes;
	/// Download rate
	private $downloadRate;
	/// Upload rate
	private $uploadRate;
	/// Done in percent
	private $done;
	/// Total filesize in bytes
	private $filesize;
	/// Number of leechers.
	private $leechers;
	/// Number of seeders
	private $seeders;

	// Time left, in days hours minutes and seconds
	private $time_left_d;
	private $time_left_h;
	private $time_left_m;
	private $time_left_s;

	// Trackerstatus
	private $trackerStatus;

	// Activity counter
	private $activityCounter;

	private $hash;

        private $url;

	public function Status($contextID = -1, $filename="undefined", $status=0, $dn_total=0, $ul_total=0, $failed_bytes=0, $dn_rate=0, $ul_rate=0, $done=0, $filesize=0, $leechers=0, $seeders=0, $time_left_d=0, $time_left_h=0, $time_left_m=0, $time_left_s=0, $trackerStatus = null, $activityCounter = 0, $hash = "", $url = "")
	{
		if($trackerStatus == null)
				$trackerStatus = new trackerStatus();
		$this->set($contextID, $filename, $status, $dn_total, $ul_total, $failed_bytes, $dn_rate, $ul_rate, $done, $filesize, $leechers, $seeders, $time_left_d, $time_left_h, $time_left_m, $time_left_s, $trackerStatus, $activityCounter, $hash, $url);
	}

	public function set($contextID, $filename, $status, $dn_total, $ul_total, $failed_bytes, $dn_rate, $ul_rate, $done, $filesize, $leechers, $seeders, $time_left_d, $time_left_h, $time_left_m, $time_left_s, $trackerStatus, $activityCounter, $hash, $url)
	{
		$this->contextID = $contextID;
		$this->filename = $filename;
		$this->status = $status;
		$this->downloadTotal = $dn_total;
		$this->uploadTotal = $ul_total;
		$this->failedBytes = $failed_bytes;
		$this->downloadRate = $dn_rate;
		$this->uploadRate = $ul_rate;
		$this->done = $done;
		$this->filesize = $filesize;
		$this->leechers = $leechers;
		$this->seeders = $seeders;
		$this->time_left_d = $time_left_d;
		$this->time_left_h = $time_left_h;
		$this->time_left_m = $time_left_m;
		$this->time_left_s = $time_left_s;
		$this->trackerStatus = $trackerStatus;
		$this->activityCounter = $activityCounter;
		$this->hash            = $hash;
		$this->url             = $url;
	}

	function getContextID() { return $this->contextID; }
	function getFilename()	{ return $this->filename; }
	function getStatus()	{ return $this->status; }
	function getStatusName()	{ return $this->statusNames[$this->status]; }
	function getDownloadTotal()	{ return $this->downloadTotal; }
	function getUploadTotal()	{ return $this->uploadTotal; }
	function getfailedBytes()	{ return $this->failedBytes; }
	function getDownloadRate()	{ return $this->downloadRate; }
	function getUploadRate()	{ return $this->uploadRate; }
	function getDone()	{ return $this->done; }
	function getFilesize()	{ return $this->filesize; }
	function getLeechers()	{ return $this->leechers; }
	function getSeeders()	{ return $this->seeders; }
	function getTimeLeftD()	{ return $this->time_left_d; }
	function getTimeLeftH()	{ return $this->time_left_h; }
	function getTimeLeftM()	{ return $this->time_left_m; }
	function getTimeLeftS()	{ return $this->time_left_s; }
	function getTrackerStatus()	{ return $this->trackerStatus; }
	function getActivityCounter()	{ return $this->activityCounter; }

	function getHash() { return $this->hash; }
   function getAnnounceURL() { return $this->url; }

	public function serialize(&$a = array())
	{
		$this->intToBytes($a, $this->contextID);
		$this->stringToBytes($a, $this->filename);
		$this->intToBytes($a, $this->status);
		$this->uLongToBytes($a, $this->downloadTotal);
		$this->uLongToBytes($a, $this->uploadTotal);
		$this->uLongToBytes($a, $this->failedBytes);
		$this->uLongToBytes($a, $this->downloadRate);
		$this->uLongToBytes($a, $this->uploadRate);
		$this->intToBytes($a, $this->done);
		$this->uLongToBytes($a, $this->filesize);
		$this->intToBytes($a, $this->leechers);
		$this->intToBytes($a, $this->seeders);
		$this->intToBytes($a, $this->time_left_d);
		$this->uLongToBytes($a, $this->time_left_h);
		$this->intToBytes($a, $this->time_left_m);
		$this->intToBytes($a, $this->time_left_s);
		$this->trackerStatus->serialize($a);
		$this->uLongToBytes($a, $this->activityCounter);
		$this->stringToBytes($a, $this->hash);
		$this->stringToBytes($a, $this->url);
		return $a;
	}

	public function deserialize(&$data)
	{
		$this->bytesToInt($this->contextID, $data);
		$this->bytesToString($this->filename, $data);

		$temp_status = 0;
		$this->bytesToInt($temp_status, $data);

		switch($temp_status)
		{
			case Status::ts_undefined:
			case Status::ts_queued:
			case Status::ts_checking:
			case Status::ts_connecting:
			case Status::ts_downloading:
			case Status::ts_seeding:
			case Status::ts_stopped:
			case Status::ts_finished:
				break;
			default:
				throw new BTGException("Status::deserialize failed to deserialize status. Invalid status '$temp_status'.");
		}
		$this->status = $temp_status;

		$this->bytesToULong($this->downloadTotal, $data);
		$this->bytesToULong($this->uploadTotal, $data);
		$this->bytesToULong($this->failedBytes, $data);
		$this->bytesToULong($this->downloadRate, $data);
		$this->bytesToULong($this->uploadRate, $data);
		$this->bytesToInt($this->done, $data);
		$this->bytesToULong($this->filesize, $data);
		$this->bytesToInt($this->leechers, $data);
		$this->bytesToInt($this->seeders, $data);
		$this->bytesToInt($this->time_left_d, $data);
		$this->bytesToULong($this->time_left_h, $data);
		$this->bytesToInt($this->time_left_m, $data);
		$this->bytesToInt($this->time_left_s, $data);
		$this->trackerStatus->deserialize($data);
		$this->bytesToULong($this->activityCounter, $data);
		$this->bytesToString($this->hash, $data);
		$this->bytesToString($this->url, $data);
	}

	public function toString($lineWidth = 80)
	{
		$ret = "context id: ". $this->contextID . "\n";
		$ret.= "filename: ". $this->filename . "\n";
		$ret.= "status: ". $this->status . "\n";
		$ret.= "dn_total: ". $this->downloadTotal . "\n";
		$ret.= "ul_total: ". $this->uploadTotal . "\n";
		$ret.= "failed_bytes: ". $this->failedBytes . "\n";
		$ret.= "dn_rate: ". $this->downloadRate . "\n";
		$ret.= "ul_rate: ". $this->uploadRate . "\n";
		$ret.= "done: ". $this->done . "\n";
		$ret.= "filesize: ". $this->filesize . "\n";
		$ret.= "leechers: ". $this->leechers. "\n";
		$ret.= "seeders: ". $this->seeders . "\n";
		$ret.= "time left: ". $this->time_left_d. "d, ". $this->time_left_h . "h, " . $this->time_left_m . "m, " . $this->time_left_s . "s\n";
		$ret.= "trackerstatus: ".$this->trackerStatus->toString()."\n";
		$ret.= "activity counter: ".$this->activityCounter."\n";

		return $ret;
	}
}
?>

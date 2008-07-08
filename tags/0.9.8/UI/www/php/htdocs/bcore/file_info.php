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
 * $Id: file_info.php,v 1.1.2.1 2006/03/27 07:17:08 jstrom Exp $
 */
if(!defined("BTG_BCORE_PATH"))
	define("BTG_BCORE_PATH", "bcore");

require_once(BTG_BCORE_PATH."/serializable.php");

class selectedFileEntry extends BTGSerializable
{
   private $filename;
   private $selected;

   public function selectedFileEntry($filename = "", $selected = false)
   {
      $this->filename = $filename;
      $this->selected = $selected;
   }

   public function select()
   {
      $this->selected = true;
   }

   public function deSelect()
   {
      $this->selected = false;
   }

   public function getFilename()
   {
      return $this->filename;
   }

   public function getSelected()
   {
      return $this->selected;
   }

   public function serialize(&$a = array())
	{
      $this->stringToBytes($a, $this->filename);
		$this->boolToBytes($a, $this->selected);

      return $a;
   }

   public function deserialize(&$data)
   {
      $this->bytesToString($this->filename, $data);
		$this->bytesToBool($this->selected, $data);
   }
}

class fileInformation extends BTGSerializable
{
	const MAX_PASS_COUNT = 5;
	const DONE_ALL = 1;
	const DONE_NONE = 2;
	const DONE_UNDEF = 0;

	const BITS_PER_BYTE = 7;
	/// The filename this class holds piece-info about.
	private $filename;
	/// The pieces of this file.
	private $pieces;
	/// The number of pieces.
	private $pieces_size;
	/// The number of bytes each piece represents.
	private $bytesPerPiece;
	/// The exact file size.
	private $fileSize;
	/// Status
	private $status;

	/// Constructor, $pieces should be an array of booleans
	public function fileInformation($filename="undefined file", $pieces=array(), $bytesPerPiece=0, $file_size=0)
	{
		$this->filename = $filename;
		$this->bytesPerPiece = $bytesPerPiece;
		$this->fileSize = $file_size;
		$this->status = fileInformation::DONE_UNDEF;

		if(count($pieces) > 0)
		{
			$b_set = 0;
			foreach($pieces as $piece)
			{
				if($piece == true)
					$b_set++;
			}

			if($b_set == count($pieces))
				$this->status = fileInformation::DONE_ALL;
			else if($b_set == 0)
				$this->status = fileInformation::DONE_NONE;
			else
			{
				$this->status = fileInformation::DONE_UNDEF;
				$this->pieces = $pieces;
			}
		}else
		{
			$this->status = fileInformation::DONE_UNDEF;
		}

		$this->pieces_size = count($pieces);
	}

	/// Return the number of pieces this class holds.
	public function size()
	{
		return $this->pieces_size;
	}

	/// Return the exact file size.
	public function getFileSize()
	{
		return $this->fileSize;
	}

	/// Return the filename
	public function getFilename()
	{
		return $this->filename;
	}

	/// Return the bits.
	public function getBits()
	{
		if(($this->status != fileInformation::DONE_ALL) && ($this->status != fileInformation::DONE_NONE))
			return $this->pieces;

		$ret = array();
		if($this->status == fileInformation::DONE_ALL)
		{
			for($i = 0; $i < $this->pieces_size; $i++)
				$ret[] = true;
		}
		else if($this->status == fileInformation::DONE_NONE)
		{
			for($i = 0; $i < $this->pieces_size; $i++)
				$ret[] = false;
		}

		return $ret;
	}

	/// Returns true if all bits are set.
	public function isFull()
	{
		if($this->status == fileInformation::DONE_ALL)
			return true;
		return false;
	}

	/// Returns true if none of the contained bits are set.
	public function isEmpty()
	{
		if($this->status == fileInformation::DONE_NONE)
			return true;
		return false;
	}

	public function serialize(&$a = array())
	{
		$this->stringToBytes($a, $this->filename);
		$this->intToBytes($a, $this->status);
		$this->intToBytes($a, $this->bytesPerPiece);
		$this->uLongToBytes($a, $this->fileSize);
		$this->intToBytes($a, $this->pieces_size);

		if($this->status == fileInformation::DONE_UNDEF)
		{
			$bit_counter = 0;
			$output = 0;
			$data = "";

			foreach($this->pieces as $piece)
			{
				if($bit_counter >= fileInformation::BITS_PER_BYTE)
				{
					$data .= chr($output);
					$output = 0;
					$bit_counter = 0;
				}

				if($piece)
					$output|= 1 << $bit_counter;

				$bit_counter++;
			}

			if($bit_counter > 0)
			{
				$data .= chr($output);
			}
			
			$this->intToBytes($a, $bit_counter);
			$this->addBytes($a, $data, strlen($data));
		}
		return $a;
	}

	public function deserialize(&$data)
	{
		$this->bytesToString($this->filename, $data);
		$this->bytesToInt($this->status, $data);
		$this->bytesToInt($this->bytesPerPiece, $data);
		$this->bytesToULong($this->fileSize, $data);
		$this->bytesToInt($this->pieces_size, $data);
		
		if($this->status == fileInformation::DONE_UNDEF)
		{
			$bytes = "";
			$bits = 0;
			$octets = 0;
			$this->bytesToInt($bits, $data);
			$this->getBytes($bytes, $data, $octets);

			$this->pieces = array();

			$remainder =  $octets * fileInformation::BITS_PER_BYTE - $bits;
			$octets -= $remainder ? 1 : 0;

			if($octets > 0)
			{
				for($i=0; $i < $octets; $i++)
				{
					$b = $bytes[$i];
					for($n=0; $n < fileInformation::BITS_PER_BYTE; $n++)
					{
						if( (($b >> $n) & 1) == 1 )
							$this->pieces[] = true;
						else
							$this->pieces[] = false;
					}
				}
			}

			if($remainder > 0)
			{
				$b = $bytes[$octets];
				for($n = 0; $n < $remainder; $n++)
				{
					if( (($b >> $n) & 1) == 1 )
						$this->pieces[] = true;
					else
						$this->pieces[] = false;
				}
			}
		}
	}

	public function toString($lineWidth = 80)
	{
		$ret = "Filename: ". $this->filename . "\n";
		$ret.= "Pieces: ". $this->pieces_size ."\n";
		$ret.= "Bytes per piece: ". $this->bytesPerPiece ."\n";

		switch($this->status)
		{
		case fileInformation::DONE_ALL:
			$ret.= "All bits set.\n";
			break;
		case fileInformation::DONE_NONE:
			$ret.= "All bits unset.\n";
			break;
		case fileInformation::DONE_UNDEF:
			if($this->pieces_size <= 0)
				$ret.= "Undefined.\n";
			else
			{
				$charcount = 0;
				foreach($this->pieces as $piece)
				{
					if($piece)
						$ret.= "X";
					else
						$ret.= "_";
					
					$charcount++;
					if($charcount >= $lineWidth)
					{
						$charcount = 0;
						$ret.= "\n";
					}
				}
				$ret.="\n";
			}
			break;
		}

		return $ret;
	}
}
?>

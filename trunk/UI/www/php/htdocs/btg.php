<?php
/*
 * btg Copyright (C) 2005 Michael Wojciechowski.
 * Web client written by Johan Ström.
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
 * $Id: btg.php,v 1.1.2.47 2007/08/08 22:18:32 jstrom Exp $
 */

// Check PHP version, we require at least PHP 5
if(substr(phpversion(), 0,1) < 5)
	die("Sorry, btg-www requires at least PHP 5");

if(!function_exists("xml_parser_create"))
	die("Sorry, btg-www requires XML support in your PHP installation.");

// Start PHP Session
session_start();

require_once("aphpstr.php");
require_once("phpajax.php");

define('BTG_BCORE_PATH', "bcore");
require_once(BTG_BCORE_PATH."/command_factory.php");
require_once(BTG_BCORE_PATH."/xmlrpc.php");

class BTG
{
	/// Error codes:
	const ERR_CONNECTING = 1;
	const ERR_INITCONNECTION = 2;
	const ERR_ATTACHSESSION = 3;
	const ERR_SETUPSESSION = 4;
	
	/// The XMLRPC client
	private $xmlrpc = NULL;
	/// The client build ID to report when attaching
	private $buildID;
	/// Debug mode. Debug output will be added with <debug>blaha</debug> 
	private $debug = 0;
	/// Session we're using
	private $sessionID = -1;
	/// If we are attached to a session
	private $sessionAttached = false;
	/// Username we are authorized as
	private $username = NULL;
	/// Any extra output added from executeCommand, <error> and <debug> element.
	private $extraOutput = "";
	/// Start time
	private $initTime = 0;
	/// Autostart downloads
	private $autostart = true;

	public function __construct($url="http://127.0.0.1:16000/", $buildID, $autostart)
	{
		$this->xmlrpc = new XMLRPC_Client($url);
		$this->xmlrpc->enableGZIP(true);

		$this->buildID = $buildID;
		$this->initTime = microtime(true);
		$this->autostart = $autostart;
	}

	public function __destruct()
	{
		$this->teardown();
	}

	/**
	 * Set SSL parameters (used when a URL in form https:// is used).
	 * The params are ssl_ca_cert, path to the the Certificate Authority cert 
	 * and ssl_client_cert, which is path to this clients certificate, PEM encoded, without encryption.
	 */
	function setSSLparams($ca_cert, $client_cert)
	{
		$this->xmlrpc->setSSLparams($ca_cert, $client_cert);
	}
	
	private function authLast()
	{
		// Check if we got a session that was attached last time this client was here.
		if(!$this->username && isset($_SESSION['btg_username']))
		{
			$this->auth($_SESSION['btg_username'], $_SESSION['btg_password']);
		}	
	}

	private function attachLast()
	{
		// Check if we got a session that was attached last time this client was here.
		if(isset($_SESSION['btg_attached_session']) && !$this->sessionAttached)
		{
			$this->authLast();
			$this->sessionAttach((int)$_SESSION['btg_attached_session'], false);
		}	
	}

	/// Tear down the interface
	public function teardown()
	{
		if($this->sessionAttached)
		{
			// Detach session
			$this->sessionDetach();
		}
		$this->close();
	}

	/// Enable/disble debug mode
	public function setDebug($debug)
	{
		$this->debug = ($debug?1:0);
	}

	public function close()
	{
		$this->xmlrpc->close();
	}

	/// Get the sessionID
	public function getSessionID()
	{
		return $this->sessionID;
	}

	/// Log a debug message, if debuging is enabled
	private function log_debug($str)
	{
		if($this->debug)
			$this->extraOutput.="<debug>".htmlspecialchars($str)."</debug>\n";
	}

	/// Log an error message
	private function log_error($str, $errorCode = -1)
	{
		if($errorCode != -1)
			$this->extraOutput.="<error code=\"$errorCode\">$str</error>\n";
		else
			$this->extraOutput.="<error>$str</error>\n";
	}

	/// Get the extra output produced by log_debug/log_error
	private function addExtraOutput($str="")
	{
		$output = $str . $this->extraOutput;
		if($this->debug)
			$output .= "<execTime>".(microtime(true) - $this->initTime)."</execTime>";
		return $output;
	}

	/// Try to execute a btg command against the server
	public function executeCommand($cmd, $ignoreError=false)
	{
		$this->log_debug("Sending command ".$cmd->toString());
		$data = $cmd->serialize();
		if( !$this->xmlrpc->query_arg($data) )
		{
			// Failed to send
			$this->log_error($this->xmlrpc->getErrorMessage(), BTG::ERR_CONNECTING);
			return NULL;
		}
		
		// Executed OK, check response
		$resp_data = $this->xmlrpc->getResponse();

		try
		{
			// Try to deserialize it
			$resp_cmd = commandFactory::createFromBytes($resp_data);
		}catch(BTGException $e)
		{
			// Failed to receive.
			$this->log_error("Failed to decode received command:". $e->getMessage());
			$this->log_debug("BTGException on createFromBytes: ". $e->getTraceAsString());
			return NULL;
		}

		$this->log_debug("Received command: ". $resp_cmd->toString() . "\n");

		if(!$ignoreError && $resp_cmd instanceof errorCommand)
		{
			$this->log_error($resp_cmd->getMessage());
		}
		return $resp_cmd;
	}

	/// List active sessions 
	public function sessionList()
	{
		$this->authLast();
		$r = $this->executeCommand(new listSessionCommand());
		if($r == NULL)
			return $this->addExtraOutput("");

		$output = "<sessions>";

		if($r instanceof listSessionResponseCommand)
		{
			$name_counter = 0;
			$names = $r->getNames();
			foreach($r->getSessions() as $session)
			{
				$output.="<session>$session</session>";
				$output.="<name>$names[$name_counter]</name>";
				$name_counter++;
			}
		}

		$output.= "</sessions>";
		return $this->addExtraOutput($output);
	}

	/// Authorize
	public function auth($username="", $password="")
	{
		$output = "";
		$d_username=decodeURIComponent($username);
		$d_password=decodeURIComponent($password);
		$this->log_debug("Authorizing as ".$d_username);
		$r = $this->executeCommand(new initConnectionCommand($d_username, $d_password), true);
		if($r == NULL)
			return $this->addExtraOutput("");

		if($r instanceof errorCommand)
		{
			$this->log_error($r->getMessage(), BTG::ERR_INITCONNECTION);
		}
		else if($r instanceof ackCommand)
		{
			$this->log_debug("Authorized as $d_username.");
			$this->username = $username;
			$_SESSION['btg_username'] = $this->username;
			// Maybee we should save hash instead?
			$_SESSION['btg_password'] = $password;
			$output.="<ack/>";
		}

		return $this->addExtraOutput($output);
	}

	/// Authorize
	public function deauth()
	{
		$this->username = null;
		unset($_SESSION['btg_username']);
		unset($_SESSION['btg_password']);
		unset($_SESSION['btg_attached_session']);
		return "<ack/>";
	}

	/// Attach to a session
	public function sessionAttach($newSession)
	{
		$this->authLast();
		$newSession = (int)$newSession;
		$output = "";
		$this->log_debug("Attaching to session ".$newSession." with ". $this->buildID);
		$r = $this->executeCommand(new attachSessionCommand($this->buildID, $newSession), false);
		if($r == NULL)
			return $this->addExtraOutput("");

		if($r instanceof errorCommand)
		{
			$this->log_error($r->getMessage(), BTG::ERR_ATTACHSESSION);
		}
		else if($r instanceof ackCommand)
		{
			$this->sessionAttached = true;
			$this->log_debug("Attached to session $newSession.");
			$this->sessionID = $newSession;
			$_SESSION['btg_attached_session'] = $this->sessionID;
			$output.="<ack/>";
		}

		return $this->addExtraOutput($output);
	}

	/// Setup a new session
	public function sessionSetup($seedLimit = limitBase::LIMIT_DISABLED, $seedTimeout = limitBase::LIMIT_DISABLED)
	{
		$this->authLast();
		$output = "";
		$this->log_debug("Setting up session, seedLimit ".$seedLimit.", seedTimeout ".$seedTimeout);
		$r = $this->executeCommand(new setupCommand(new requiredSetupData($this->buildID, (int)$seedLimit, (double)$seedTimeout)), true);

		if($r instanceof errorCommand)
		{
			$this->log_error($r->getMessage(), BTG::ERR_SETUPSESSION);
		}
		else if($r instanceof setupResponseCommand)
		{
			$newSession = $r->getSession();
			$this->sessionAttached = true;
			$this->log_debug("Attached to new session $newSession.");
			$this->sessionID = $newSession;
			$_SESSION['btg_attached_session'] = $this->sessionID;
			$output.="<ack/>";
		}

		return $this->addExtraOutput($output);
	}

	/// Simulate a detach for the client, reset the _SESSION var
	public function sessionDoDetach()
	{
		unset($_SESSION['btg_attached_session']);

		return $this->addExtraOutput();
	}

	/// Detach from current 
	public function sessionDetach()
	{
		if(!$this->sessionAttached)
			$this->log_error("Cannot detach, not attached.");
		else
		{
			$r = $this->executeCommand(new detachSessionCommand());

			if($r instanceof ackCommand)
			{
				$this->sessionAttached = 0;
				$this->sessionID = -1;
			}
		}

		return $this->addExtraOutput();
	}

	/// Terminate current session
	public function sessionQuit()
	{
		$this->attachLast();

		if(!$this->sessionAttached)
			$this->log_error("Cannot quit, not attached.");
		else
		{
			$r = $this->executeCommand(new quitSessionCommand());

			if($r instanceof ackCommand)
			{
				$this->sessionAttached = 0;
				$this->sessionID = -1;
				unset($_SESSION['btg_attached_session']);
			}
		}

		return $this->addExtraOutput();
	}
	
	public function cleanAll()
	{
		$this->attachLast();

		if(!$this->sessionAttached)
		{
			$this->log_error("Cannot quit, not attached.");
			return $this->addExtraOutput("");
		}
		else
		{
			$r = $this->executeCommand(
				new contextCleanCommand(contextCommand::UNDEFINED_CONTEXT, true)
			);

			$output = "";
			if($r instanceof ackCommand)
			{
				$output .= "<ack/>\n";
			}
			return $this->addExtraOutput($output);
		}
	}

	/// Set global limit
	function globalLimit($uploadLimit, $downloadLimit, $maxUploads, $maxConnections)
	{
		$this->attachLast();
		if(!$this->sessionAttached)
			return $this->addExtraOutput("");
		
		$output = "";
		if(!$this->sessionAttached)
			$this->log_error("No session attached, can't set limits.");
		else
		{
			$r = $this->executeCommand(new limitCommand((int)$uploadLimit, (int)$downloadLimit, (int)$maxUploads, (int)$maxConnections), false);
			if($r instanceof ackCommand)
			{
				$output .= "<ack/>\n";
			}
		}
		return $this->addExtraOutput($output);
	}

	function sessionName()
	{
		$this->attachLast();
		if(!$this->sessionAttached)
			return $this->addExtraOutput("");

		$output = "";
		$r = $this->executeCommand(new sessionNameCommand(), false);
		if($r instanceof sessionNameResponseCommand)
		{
			$output .= "<sessionname>".$this->getSessionID().":".$r->getName()."</sessionname>\n";
		}
		return $this->addExtraOutput($output);
	}

	/// Get global limit
	function globalLimitStatus()
	{
		$this->attachLast();
		if(!$this->sessionAttached)
			return $this->addExtraOutput("");
		
		$output = "";
		if(!$this->sessionAttached)
			$this->log_error("No session attached, can't set limits.");
		else
		{
			$r = $this->executeCommand(new limitStatusCommand(), false);
			if($r instanceof limitStatusResponseCommand)
			{
				$output .= "<globallimit>\n";
				$output .= "<uploadLimit>".$r->getUploadLimit()."</uploadLimit>\n";
				$output .= "<downloadLimit>".$r->getDownloadLimit()."</downloadLimit>\n";
				$output .= "<maxuploads>".$r->getMaxUploads()."</maxuploads>\n";
				$output .= "<maxconnections>".$r->getMaxConnections()."</maxconnections>\n";
				$output .= "</globallimit>\n";
			}
		}
		return $this->addExtraOutput($output);
	}

	/// Create a new context (torrent)
	/// $file should be an entry from PHP's $_FILE[]
	function contextCreateFromUpload($file)
	{
		if(!isset($file) || !isset($file['name']) || !isset($file['size']) || !isset($file['tmp_name']) || !isset($file['error']) )
		{
			$this->log_error("Broken upload.");
			return $this->addExtraOutput();
		}

		if($file['error'] != 0)
		{
			$errors = array(
				0=>"There is no error, the file uploaded with success.",
				1=>"The uploaded file exceeds the upload_max_filesize directive in php.ini.",
				2=>"The uploaded file exceeds the MAX_FILE_SIZE directive that was specified in the HTML form.",
				3=>"The uploaded file was only partially uploaded. Please try again.",
				4=>"File missing.",
				6=>"Missing a temporary folder."
			);
			$this->log_error("Failed: ".$errors[(int)$file['error']], (int)$file['error']);
			return $this->addExtraOutput();
		}

		if(!is_uploaded_file($file['tmp_name']))
		{
			$this->log_error("Failed: Temporary file is not uploaded!");
			return $this->addExtraOutput();
		}

		// File should be OK.
		$filename = basename($file['name']);

		return $this->contextCreateWithData($filename, $file['tmp_name']);
	}

	/// Start a torrent
	public function contextCreateWithData($torrent_filename, $tmpfile="")
	{
		$this->attachLast();
		if(!$this->sessionAttached)
			return $this->addExtraOutput("");

		if($tmpfile == "")
			// If no specific tempfile was specified, use the torrents filename for this.
			$tmpfile = $torrent_filename;
		
		// We are only interested in the files name, path information is not interesting...
		$torrent_filename = basename($torrent_filename);

		$data = new SBuffer();
		$data->read($tmpfile);

		$r = $this->executeCommand(new contextCreateWithDataCommand($torrent_filename, $data, $this->autostart));
		if($r == NULL)
			return $this->addExtraOutput("");

		$output = "";
		if($r instanceof ackCommand)
			$output.="<ack/>";

		return $this->addExtraOutput($output);
	}
	
	/// Get status of one or more contexts
	function contextStatus($contextID=contextCommand::UNDEFINED_CONTEXT, $showAll=true)
	{
		$this->attachLast();
		if(!$this->sessionAttached)
			return $this->addExtraOutput("");
		
		$output = "";
		$r = $this->executeCommand(new contextStatusCommand((int)$contextID, (bool)$showAll), true);
		if($r instanceof contextStatusResponseCommand || $r instanceof contextAllStatusResponseCommand)
		{
			$output = "<contexts>\n";
			$status = $r->getStatus();
			if(is_array($status))
			{
				foreach($status as $contextStatus)
				{
					$output .= "<context>\n";
					$output .= "<id>".$contextStatus->getContextID()."</id>\n";
					$output .= "<filename>".htmlspecialchars(basename($contextStatus->getFilename()))."</filename>\n";
					$output .= "<status>".$contextStatus->getStatus()."</status>\n";
					$output .= "<statustext>".$contextStatus->getStatusName()."</statustext>\n";
					$output .= "<downloadtotal>".$contextStatus->getDownloadTotal()."</downloadtotal>\n";
					$output .= "<uploadtotal>".$contextStatus->getUploadTotal()."</uploadtotal>\n";
					$output .= "<failedbytes>".$contextStatus->getFailedBytes()."</failedbytes>\n";
					$output .= "<downloadrate>".$contextStatus->getDownloadRate()."</downloadrate>\n";
					$output .= "<uploadrate>".$contextStatus->getUploadRate()."</uploadrate>\n";
					$output .= "<done>".$contextStatus->getDone()."</done>\n";
					$output .= "<filesize>".$contextStatus->getFilesize()."</filesize>\n";
					$output .= "<leechers>".$contextStatus->getLeechers()."</leechers>\n";
					$output .= "<seeders>".$contextStatus->getSeeders()."</seeders>\n";

					$d = $contextStatus->getTimeLeftD();
					$h = $contextStatus->getTimeLeftH();
					$m = $contextStatus->getTimeLeftM();
					$s = $contextStatus->getTimeLeftS();
					if($d > 0)
						$timeleft = sprintf("%dd, %dh, %dm", $d, $h, $m);
					elseif($h > 0)
						$timeleft = sprintf("%dh, %dm, %ds", $h, $m, $s);
					elseif($m > 0)
						$timeleft = sprintf("%dm, %ds", $m, $s);
					else
						$timeleft = sprintf("%ds", $s);

					$output .= "<timeleft>".$timeleft."</timeleft>\n";
					$trackerStatus = $contextStatus->getTrackerStatus();
					$output .= "<trackerstatus>".$trackerStatus->getStatus()."</trackerstatus>\n";
					$output .= "<trackerstatustext>".htmlspecialchars($trackerStatus->getDescription())."</trackerstatustext>\n";
					$output .= "<trackerstatusmessage>".htmlspecialchars($trackerStatus->getMessage())."</trackerstatusmessage>\n";
					$output .= "<activitycounter>".$contextStatus->getActivityCounter()."</activitycounter>\n";
					$output .= "</context>\n";
				}
			}else
			{
				$output .= "<context>\n";
				$output .= "<id>".$status->getContextID()."</id>\n";
				$output .= "<filename>".htmlspecialchars(basename($status->getFilename()))."</filename>\n";
				$output .= "<status>".$status->getStatus()."</status>\n";
				$output .= "<statustext>".$status->getStatusName()."</statustext>\n";
				$output .= "<downloadtotal>".$status->getDownloadTotal()."</downloadtotal>\n";
				$output .= "<uploadtotal>".$status->getUploadTotal()."</uploadtotal>\n";
				$output .= "<failedbytes>".$status->getFailedBytes()."</failedbytes>\n";
				$output .= "<downloadrate>".$status->getDownloadRate()."</downloadrate>\n";
				$output .= "<uploadrate>".$status->getUploadRate()."</uploadrate>\n";
				$output .= "<done>".$status->getDone()."</done>\n";
				$output .= "<filesize>".$status->getFilesize()."</filesize>\n";
				$output .= "<leechers>".$status->getLeechers()."</leechers>\n";
				$output .= "<seeders>".$status->getSeeders()."</seeders>\n";

				$h = $status->getTimeLeftH();
				$m = $status->getTimeLeftM();
				$s = $status->getTimeLeftS();
				if($h > 0)
					$timeleft = sprintf("%dh, %dm, %ds", $h, $m, $s);
				elseif($m > 0)
					$timeleft = sprintf("%dm, %ds", $m, $s);
				else
					$timeleft = sprintf("%ds", $s);

				$output .= "<timeleft>".$timeleft."</timeleft>\n";
				$trackerStatus = $status->getTrackerStatus();
				$output .= "<trackerstatus>".$trackerStatus->getStatus()."</trackerstatus>\n";
				$output .= "<trackerstatustext>".htmlspecialchars($trackerStatus->getDescription())."</trackerstatustext>\n";
				$output .= "<trackerstatusmessage>".htmlspecialchars($trackerStatus->getMessage())."</trackerstatusmessage>\n";
				$output .= "</context>\n";
			}
			$output .="</contexts>\n";
		}
		else if($contextID == contextCommand::UNDEFINED_CONTEXT && $r instanceof errorCommand)
		{
			$output .= "<contexts/>";
		}else if($r instanceof errorCommand)
		{
			$this->log_error($r->getMessage());
		}
		return $this->addExtraOutput($output);
	}

	/// Set limit
	function contextLimit($contextID=contextCommand::UNDEFINED_CONTEXT, $uploadLimit, $downloadLimit, $seedLimit, $seedTimeout, $allContexts=false)
	{
		$this->attachLast();
		if(!$this->sessionAttached)
			return $this->addExtraOutput("");
		
		$output = "";
		if(!$this->sessionAttached)
			$this->log_error("No session attached, can't set limits.");
		else
		{
			$r = $this->executeCommand(new contextLimitCommand((int)$contextID, (int)$uploadLimit, (int)$downloadLimit, (int)$seedLimit, (int)$seedTimeout, (bool)$allContexts), false);
			if($r instanceof ackCommand)
			{
				$output .= "<ack/>\n";
			}
		}
		return $this->addExtraOutput($output);
	}


	/// Get limits of a context
	function contextLimitStatus($contextID=contextCommand::UNDEFINED_CONTEXT)
	{
		$this->attachLast();
		
		$output = "";
		if(!$this->sessionAttached)
			$this->log_error("No session attached, can't get context limit status.");
		else
		{
			$output ="<limits>\n";
			if(strstr($contextID, ",") != false)
			{
				// $contextID is actualy a , delimited list of contextIDs
				$ids = explode(",", $contextID);
				foreach($ids as $id)
				{
					$r = $this->executeCommand(new contextLimitStatusCommand((int)$id, false), true);
					if($r instanceof contextLimitStatusResponseCommand)
					{
						$output .= "<limit>\n";
						$output .= "<id>".$id."</id>\n";
						$output .= "<uploadLimit>".$r->getUploadLimit()."</uploadLimit>\n";
						$output .= "<downloadLimit>".$r->getDownloadLimit()."</downloadLimit>\n";
						$output .= "<seedLimit>".$r->getSeedLimit()."</seedLimit>\n";
						$output .= "<seedTimeout>".$r->getSeedTimeout()."</seedTimeout>\n";
						$output .= "</limit>\n";
					}
				}
			}else
			{
				$r = $this->executeCommand(new contextLimitStatusCommand((int)$contextID, false), false);
				if($r instanceof contextLimitStatusResponseCommand)
				{
					$output .= "<limit>\n";
					$output .= "<id>".$contextID."</id>\n";
					$output .= "<uploadLimit>".$r->getUploadLimit()."</uploadLimit>\n";
					$output .= "<downloadLimit>".$r->getDownloadLimit()."</downloadLimit>\n";
					$output .= "<seedLimit>".$r->getSeedLimit()."</seedLimit>\n";
					$output .= "<seedTimeout>".$r->getSeedTimeout()."</seedTimeout>\n";
					$output .= "</limit>\n";
				}
			}
			$output.="</limits>\n";
		}
		return $this->addExtraOutput($output);
	}

	/// Start a torrent
	public function contextStart($id)
	{
		$this->attachLast();
		if(!$this->sessionAttached)
			return $this->addExtraOutput("");

		$r = $this->executeCommand(new contextStartCommand((int)$id, false));
		if($r == NULL)
			return $this->addExtraOutput("");

		$output = "";
		if($r instanceof ackCommand)
			$output.="<ack/>";

		return $this->addExtraOutput($output);
	}

	/// Stop a torrent
	public function contextStop($id)
	{
		$this->attachLast();
		if(!$this->sessionAttached)
			return $this->addExtraOutput("");

		$r = $this->executeCommand(new contextStopCommand((int)$id, false));
		if($r == NULL)
			return $this->addExtraOutput("");

		$output = "";
		if($r instanceof ackCommand)
			$output.="<ack/>";

		return $this->addExtraOutput($output);
	}

	/// Delete a torrent
	public function contextAbort($id, $eraseData=false)
	{
		$this->attachLast();
		if(!$this->sessionAttached)
			return $this->addExtraOutput("");

		$r = $this->executeCommand(new contextAbortCommand((int)$id, (bool)$eraseData, false));
		if($r == NULL)
			return $this->addExtraOutput("");

		$output = "";
		if($r instanceof ackCommand)
			$output.="<ack/>";

		return $this->addExtraOutput($output);
	}

	/// Delete a torrent
	public function contextClean($id)
	{
		$this->attachLast();
		if(!$this->sessionAttached)
			return $this->addExtraOutput("");

		$r = $this->executeCommand(new contextCleanCommand((int)$id, false));
		if($r == NULL)
			return $this->addExtraOutput("");

		$output = "";
		if($r instanceof ackCommand)
			$output.="<ack/>";

		return $this->addExtraOutput($output);
	}

	/// Get peers of a context
	function contextPeers($contextID=contextCommand::UNDEFINED_CONTEXT)
	{
		$this->attachLast();
		if(!$this->sessionAttached)
			return $this->addExtraOutput("");
		
		$output = "";
		if(!$this->sessionAttached)
			$this->log_error("No session attached, can't get context peers.");
		else
		{
			$r = $this->executeCommand(new contextPeersCommand((int)$contextID, false), true);
			if($r instanceof contextPeersResponseCommand)
			{
				$output .= "<peers>\n";
				$output .= "<id>".$contextID."</id>\n";
				foreach($r->getList() as $peer)
				{
					$output .= "<peer>\n";
					$output .= "<ip>".$r->getAddress()->getAddress()."</ip>\n";
					$output .= "<seeder>".$r->getSeeder()."</seeder>\n";
					$output .= "</peer>\n";
				}
				$output .= "</peers>\n";
			}else if($r instanceof errorCommand)
			{
				$output .= "<peers>\n";
				$output .= "<id>".$contextID."</id>\n";
				$output .= "</peers>\n";
			}
		}
		return $this->addExtraOutput($output);
	}

	/// Get file info of a context
	function contextFileInfo($contextID=contextCommand::UNDEFINED_CONTEXT)
	{
		$this->attachLast();
		if(!$this->sessionAttached)
			return $this->addExtraOutput("");
		
		$output = "";
		if(!$this->sessionAttached)
			$this->log_error("No session attached, can't get file info.");
		else
		{
			$r = $this->executeCommand(new contextFileInfoCommand((int)$contextID, false), false);
			if($r instanceof contextFileInfoResponseCommand)
			{
				$output .= "<fileinfo>\n";
				$output .= "<id>".$contextID."</id>\n";
				foreach($r->getFileInfoList() as $file)
				{
					$output .= "<file>\n";
					$output .= "<filename>".$file->getFilename()."</filename>\n";
					$output .= "<filesize>".$file->getFileSize()."</filesize>\n";
					$output .= "<pieces>".$file->size()."</pieces>\n";
					// Save ont the sent data..
					if($file->isFull())
						$output .= "<isfull/>\n";
					if($file->isEmpty())
						$output .= "<isempty/>\n";
					$output .= "<bits>";
					foreach($file->getBits() as $bit)
					{
						if($bit)
							$output.="1";
						else
							$output.="0";
					}
					$output .= "</bits>\n";
					$output .= "</file>\n";
				}
				$output .= "</fileinfo>\n";
			}
		}
		return $this->addExtraOutput($output);
	}
}

// Create BTG interface and ajax interface
try
{
	require_once("config.php");
	require_once("version.php");
	$btg = new BTG($btg_config_url, BTG_BUILD, $btg_config_autostart);

	if(isset($btg_ca_cert) && isset($btg_client_cert))
		$btg->setSSLparams($btg_ca_cert, $btg_client_cert);

	$ajax = new PHPAjax("btg.php");

	if($btg_config_debug >= 1)
		$btg->setDebug(1);

	if($btg_config_debug >= 2)
		$ajax->setDebug(1);

	// Register AJAX functions
	$ajax->register('btg_sessionList', array($btg, 'sessionList'));
	$ajax->register('btg_sessionAttach', array($btg, 'sessionAttach'));
	$ajax->register('btg_sessionSetup', array($btg, 'sessionSetup'));
	$ajax->register('btg_sessionDetach', array($btg, 'sessionDoDetach'));
	$ajax->register('btg_sessionQuit', array($btg, 'sessionQuit'));
	$ajax->register('btg_cleanAll', array($btg, 'cleanAll'));
	$ajax->register('btg_contextStatus', array($btg, 'contextStatus'));
	$ajax->register('btg_contextLimit', array($btg, 'contextLimit'));
	$ajax->register('btg_contextLimitStatus', array($btg, 'contextLimitStatus'));
	$ajax->register('btg_contextStart', array($btg, 'contextStart'));
	$ajax->register('btg_contextStop', array($btg, 'contextStop'));
	$ajax->register('btg_contextAbort', array($btg, 'contextAbort'));
	$ajax->register('btg_contextClean', array($btg, 'contextClean'));
	$ajax->register('btg_contextPeers', array($btg, 'contextPeers'));
	$ajax->register('btg_contextFileInfo', array($btg, 'contextFileInfo'));
	$ajax->register('btg_auth', array($btg, 'auth'));
	$ajax->register('btg_deauth', array($btg, 'deauth'));
	$ajax->register('btg_globallimit', array($btg, 'globalLimit'));
	$ajax->register('btg_globallimitstatus', array($btg, 'globalLimitStatus'));
	$ajax->register('btg_sessionName', array($btg, 'sessionName'));


	// Handle any requests
	if($ajax->handle_client_request())
	{
		exit();
	}
}catch(Exception $e)
{
	@header("Content-Type: text/xml");
	$output = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	$output.= "<ajaxResponse>\n";
	$output.= "<error>".$e->getMessage()."</error>";
	$output.= "<debug>Got Exception. Debug trace:\n";
	$output.= $e->getTraceAsString();
	$output.= "</debug>";
	$output.= "</ajaxResponse>\n";
	echo $output;
}
?>

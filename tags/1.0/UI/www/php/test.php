<?php

if(count($argv) < 6)
	die(__LINE__.": usage: test.php <server-path> <build-id> <test-torrent-file> <username> <password>\n");

$host = $argv[1];
$buildID = $argv[2];
$torrentFile = $argv[3];
$username = $argv[4];
$password = $argv[5];

// Not used...But still passed.
$interface = "";

define(BTG_BCORE_PATH, "htdocs/bcore");
require("htdocs/bcore/xmlrpc.php");
require("htdocs/bcore/command_factory.php");
function send($xmlrpc, $cmd)
{
	print "Send: " .$cmd->toString()."\n";

	if(!$xmlrpc->query_arg($cmd->serialize()))
		die(__LINE__.": Failed to send command: ".$xmlrpc->getErrorCode().": ".$xmlrpc->getErrorMessage()."\n");

	$respdata = $xmlrpc->getResponse();
	$resp = commandFactory::createFromBytes($respdata);
	if(!$resp)
		die(__LINE__.": No response!\n");

	print "Recv: ".$resp->toString()."\n";
	return $resp;
}

$c=new XMLRPC_Client($host);
$c->setSSLparams("htdocs/stromnetCA.crt","htdocs/btgclient.pem");
$c->enableGZIP(1);
$c->setDebug(0);

// List available sessions, make sure we get an Not authroized error.
$r = send($c, new listSessionCommand());
if(!($r instanceof errorCommand))
	die(__LINE__.": ListSession wasnt an error command: ".$r->toString()."\n");

// Authorize
$r = send($c, new initConnectionCommand($username, $password));
if(!($r instanceof ackCommand))
	die(__LINE__.": Failed to init connection: ".$r->toString()."\n");

// List available sessions, make sure this is NOT found
$r = send($c, new listSessionCommand());
if(!($r instanceof listSessionResponseCommand) && !($r instanceof errorCommand))
	die(__LINE__.": Failed to list sessions: ".$r->toString()."\n");

if($r instanceof listSesssionResponseCommand)
{
	$sesslist = $r->getSessions();
/*	if(in_array($sesslist));
		die(__LINE__.": We found our own session in the list, should not be there already (or just random same session ID, not very likely.. try again).\n");
*/
}

// Setup a session
$rsd = new requiredSetupData($buildID, -1, -1);
$r = send($c, new setupCommand($rsd));
if(!($r instanceof setupResponseCommand))
	die(__LINE__.": Failed to setup session: ".$r->toString()."\n");

$session = $r->getSession();	
echo("Sucessfully created new session with ID $session");


// Uptime command
$r = send($c, new uptimeCommand());
if(!($r instanceof uptimeResponseCommand))
	die(__LINE__.": Failed to get uptime: ".$r->toString()."\n");

// Detach this session and list session, make sure this is found
$r = send($c, new detachSessionCommand());
if(!($r instanceof ackCommand))
	die(__LINE__.": Failed to deatch session: ".$r->toString()."\n");

// Create new connection
$c->close();
unset($c);
$c=new XMLRPC_Client($host);
$c->setSSLparams("htdocs/stromnetCA.crt","htdocs/btgclient.pem");
$c->enableGZIP(1);
$c->setDebug(0);

// Authorize
$r = send($c, new initConnectionCommand($username, $password));
if(!($r instanceof ackCommand))
	die(__LINE__.": Failed to init connection: ".$r->toString()."\n");

/*
$r = send($c, new initSessionCommand());
if(!($r instanceof ackCommand))
	die(__LINE__.": Failed to init session: ".$r->toString()."\n");
*/

// List available sessions, make sure this IS found
$r = send($c, new listSessionCommand());
if(!($r instanceof listSessionResponseCommand))
	die(__LINE__.": Failed to list sessions: ".$r->toString()."\n");

$sesslist = $r->getSessions();
if(!in_array($session, $sesslist))
	die(__LINE__.": We didnt find own session in the list, should be there now!\n");

// Reattach to our session, no force!
$r = send($c, new attachSessionCommand($buildID, $session));
if(!($r instanceof ackCommand))
	die(__LINE__.": Failed to re-attach to session $session: ".$r->toString()."\n");

// List all torrents, make sure its empty
$r = send($c, new contextStatusCommand(contextCommand::UNDEFINED_CONTEXT, true));
if(!($r instanceof errorCommand))
	die(__LINE__.": Failed to list contexts status, didnt get errorCommand: ".$r->toString().".\n");

// Do the same using listCommand, making sure its empty
$r = send($c, new listCommand());
if(!($r instanceof errorCommand))
	die(__LINE__.": Failed to list contexts, didnt get errorCommand: ".$r->toString().".\n");

// Create a new context/file from $torrentFile
$torrent = new sBuffer();
$torrent->read($torrentFile);
$r = send($c, new contextCreateWithDataCommand(basename($torrentFile), $torrent));
if(!($r instanceof ackCommand))
	die(__LINE__.": Failed to create context: ".$r->toString()."\n");

// List all torrents, make sure we got one
$r = send($c, new contextStatusCommand(contextCommand::UNDEFINED_CONTEXT, true));
if(!($r instanceof contextAllStatusResponseCommand))
	die(__LINE__.": Failed to list contexts status, got: ".$r->toString().".\n");

if(!count($r->getStatus()) == 1)
	die(__LINE__.": Invalid context status count!\n");

$statuses = $r->getStatus();
$status = $statuses[0];
if($status->getContextID() != 0)	// First session, we should realy be 0.. XXX: More checking of Status class content?
	die(__LINE__.": Status missmatching contextID ".$status->getContextID()."\n");

if($status->getFilename() != basename($torrentFile))
	die(__LINE__.": Status missmatching torrent filename: ".$status->getFileName()." != ".basename($torrentFile)."\n");

// Do the same using listCommand, make sure we got one context
$r = send($c, new listCommand());
if(!($r instanceof listCommandResponse))
	die(__LINE__.": Failed to list contexts, got: ".$r->toString().".\n");

if(!count($r->getIDs()) == 1)
	die(__LINE__.": Invalid context id count!\n");
if(!count($r->getFileNames()) == 1)
	die(__LINE__.": Invalid filename count!\n");

$context_ids= $r->getIDs();
if($context_ids[0] != 0)
	die(__LINE__.": Missmatching contextid ".$context_ids[0]."\n");
$filenames = $r->getFileNames();
if($filenames[0] != basename($torrentFile))
	die(__LINE__.": Missmatching torrent filename: ".$status->getFileName()." != ".basename($torrentFile)."\n");

echo "Sleeping 5 seconds to let daemon retrieve torrent information...\n";
sleep(5);
// Get torrent information
$r = send($c, new contextFileInfoCommand(0));
if(!($r instanceof contextFileInfoResponseCommand))
// Dont die...
	echo(__LINE__.": Failed to get file info, got: ".$r->toString().".\n");

/* We dont know how many files the torrent have...
if(count($r->getFileInfoList()) != 1)
	die(__LINE__.": Invalid length of file info list: ".count($r->getFileInfoList()));

$fileinfos = $r->getFileInfoList();
$fileinfo = $fileinfos[0];
if($fileinfo->getFilename() != $torrentFile)
	die(__LINE__.": Invalid filename in file info: ".$fileinfo->getFilename());
*/

// Get torrent peer info 
$r = send($c, new contextPeersCommand(0));
if(!($r instanceof contextPeersResponseCommand))
	echo(__LINE__.": Failed to get peers, got: ".$r->toString().". Could be to short timeout, continuing...\n");
else
{
	// XXX: What tests can we do with getList() here?
	foreach($r->getList() as $peer)
		print $peer->toString();
}

// Try to set a limit on bandwith
$limitUp = rand(100,1000);
$limitDown = rand(100,1000);
$seedLimit = 200;
$seedTimeout = 3600*24;
$r = send($c, new contextLimitCommand(0, $limitUp, $limitDown, $seedLimit, $seedTimeout));
if(!($r instanceof ackCommand))
	die(__LINE__.": Failed to set limit, got: ".$r->toString().".\n");

// Get the limit and make sure its correct
$r = send($c, new contextLimitStatusCommand(0));
if(!($r instanceof contextLimitStatusResponseCommand))
	die(__LINE__.": Failed to get limit, got: ".$r->toString().".\n");

if($r->getUploadLimit() != $limitUp)
	die(__LINE__.": Upload Limit missmatch, I just set limit $limitUp, got: ".$r->getUploadLimit().".\n");

if($r->getDownloadLimit() != $limitDown)
	die(__LINE__.": Download Limit missmatch, I just set limit $limitDown, got: ".$r->getDownloadLimit().".\n");

if($r->getSeedLimit() != $seedLimit)
	die(__LINE__.": Seed Limit missmatch, I just set limit $seedLimit, got: ".$r->getSeedLimit().".\n");

if($r->getSeedTimeout() != $seedTimeout)
	die(__LINE__.": Seed Timeout missmatch, I just set timeout $seedTimeout, got: ".$r->getSeedTimeout().".\n");

// Try to stop the context
$r = send($c, new contextStopCommand(0, false));
if(!($r instanceof ackCommand))
	die(__LINE__.": Failed to stop context, got: ".$r->toString().".\n");

// Make sure its stopped
$r = send($c, new contextStatusCommand(0, false));
if(!($r instanceof contextStatusResponseCommand))
	die(__LINE__.": Failed to get context status, got: ".$r->toString().".\n");

$status = $r->getStatus();
if($status->getStatus() != Status::ts_stopped)
	die(__LINE__.": Status missmatching, not stopped: ".$status->getStatus()."\n");

// Try to restart the context
$r = send($c, new contextStartCommand(0, false));
if(!($r instanceof ackCommand))
	die(__LINE__.": Failed to start context, got: ".$r->toString().".\n");

// Make sure its started
$r = send($c, new contextStatusCommand(0, false));
if(!($r instanceof contextStatusResponseCommand))
	die(__LINE__.": Failed to get context status, got: ".$r->toString().".\n");

$status = $r->getStatus();
if(!in_array($status->getStatus(), array(Status::ts_checking, Status::ts_connecting, Status::ts_downloading, Status::ts_seeding, Status::ts_finished) ))
	die(__LINE__.": Status missmatching, not checking/connecting/downloading/seeding: ".$status->getStatus()."\n");

// Remove the torrent
$r = send($c, new contextAbortCommand(0, false));
if(!($r instanceof ackCommand))
	die(__LINE__.": Failed to abort context, got: ".$r->toString().".\n");

echo "Sleeping 2 seconds to let daemon remove torrent...\n";
sleep(2);

// Tear down session
$r = send($c, new quitSessionCommand());
if(!($r instanceof ackCommand))
	die(__LINE__.": Failed to quit session");

// Quit will make server disconnect us, no more commands here!
$c->close();
die("All tests done. If you got this far, all OK!\n");

?>

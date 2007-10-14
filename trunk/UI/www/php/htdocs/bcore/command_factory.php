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
 * $Id: command_factory.php,v 1.1.2.6 2007/06/26 21:03:33 wojci Exp $
 */
if(!defined("BTG_BCORE_PATH"))
	define("BTG_BCORE_PATH", "bcore");

require_once(BTG_BCORE_PATH."/btgexception.php");
require_once(BTG_BCORE_PATH."/command/error.php");
require_once(BTG_BCORE_PATH."/command/ack.php");
require_once(BTG_BCORE_PATH."/command/list.php");
require_once(BTG_BCORE_PATH."/command/list_rsp.php");
require_once(BTG_BCORE_PATH."/command/setup.php");
require_once(BTG_BCORE_PATH."/command/context_create.php");
require_once(BTG_BCORE_PATH."/command/context_last.php");
require_once(BTG_BCORE_PATH."/command/context_start.php");
require_once(BTG_BCORE_PATH."/command/context_stop.php");
require_once(BTG_BCORE_PATH."/command/context_abort.php");
require_once(BTG_BCORE_PATH."/command/context_status.php");
require_once(BTG_BCORE_PATH."/command/context_status_rsp.php");
require_once(BTG_BCORE_PATH."/command/context_fi.php");
require_once(BTG_BCORE_PATH."/command/context_fi_rsp.php");
require_once(BTG_BCORE_PATH."/command/context_clean.php");
require_once(BTG_BCORE_PATH."/command/context_clean_rsp.php");
require_once(BTG_BCORE_PATH."/command/context_limit.php");
require_once(BTG_BCORE_PATH."/command/context_li_status.php");
require_once(BTG_BCORE_PATH."/command/context_li_status_rsp.php");
require_once(BTG_BCORE_PATH."/command/context_peers.php");
require_once(BTG_BCORE_PATH."/command/context_peers_rsp.php");
require_once(BTG_BCORE_PATH."/command/session_list.php");
require_once(BTG_BCORE_PATH."/command/session_list_rsp.php");
require_once(BTG_BCORE_PATH."/command/session_attach.php");
require_once(BTG_BCORE_PATH."/command/session_quit.php");
require_once(BTG_BCORE_PATH."/command/session_error.php");
require_once(BTG_BCORE_PATH."/command/session_detach.php");
require_once(BTG_BCORE_PATH."/command/session_name.php");
require_once(BTG_BCORE_PATH."/command/kill.php");
require_once(BTG_BCORE_PATH."/command/limit.php");
require_once(BTG_BCORE_PATH."/command/uptime.php");
require_once(BTG_BCORE_PATH."/command/initconnection.php");

class commandFactory
{
	public static function createFromBytes($data)
	{
		if(!is_array($data) || count($data) <= 0)
			throw new BTGException("Cannot create command from no data.");

		/* Determine command type */
		$cmdid = $data[0];

		$c = NULL;
		switch($cmdid)
		{
		case Command::CN_ERROR: // Error.
			$c = new errorCommand();
			break;
		case Command::CN_ACK: // ACK
			$c = new ackCommand();
			break;
		case Command::CN_GLIST: // List of context ids.
			$c = new listCommand();
			break;
		case Command::CN_GLISTRSP: // list response.
			$c = new listCommandResponse();
			break;
		case Command::CN_GSETUP: // general setup.
			$c = new setupCommand();
			break;
		case Command::CN_GSETUPRSP: // general setup.
			$c = new setupResponseCommand();
			break;
		case Command::CN_CCREATEWITHDATA:
			$c = new contextCreateWithDataCommand();
			break;
		case Command::CN_CLAST: // last context
			$c = new lastCIDCommand();
			break;
		case Command::CN_CLASTRSP: // last context response
			$c = new lastCIDResponseCommand();
			break;
		case Command::CN_CSTART: // start downloading.
			$c = new contextStartCommand();
			break;
		case Command::CN_CSTOP: // Stop downloading.
			$c = new contextStopCommand();
			break;
		case Command::CN_CABORT: // Abort a context
			$c = new contextAbortCommand();
			break;
		case Command::CN_CSTATUS: // Status.
			$c = new contextStatusCommand();
			break;
		case Command::CN_CSTATUSRSP: // Status response.
			$c = new contextStatusResponseCommand();
			break;
		case Command::CN_CALLSTATUSRSP: // All status response.
			$c = new contextAllStatusResponseCommand();
			break;
		case Command::CN_CFILEINFO: // File info request.
			$c = new contextFileInfoCommand();
			break;
		case Command::CN_CFILEINFORSP: // File info response.
			$c = new contextFileInfoResponseCommand();
			break;
		case Command::CN_CCLEAN: // Clean request.
			$c = new contextCleanCommand();
			break;
		case Command::CN_CCLEANRSP: // Clean response.
			$c = new contextCleanResponseCommand();
			break;
		case Command::CN_CLIMIT:
			$c = new contextLimitCommand();
			break;
		case Command::CN_CLIMITSTATUS:
			$c = new contextLimitStatusCommand();
			break;
		case Command::CN_CLIMITSTATUSRSP:
			$c = new contextLimitStatusResponseCommand();
			break;
		case Command::CN_CPEERS:
			$c = new contextPeersCommand();
			break;
		case Command::CN_CPEERSRSP:
			$c = new contextPeersResponseCommand();
			break;
		case Command::CN_SLIST: // Context list.
			$c = new listSessionCommand();
			break;
		case Command::CN_SLISTRSP:
			$c = new listSessionResponseCommand();
			break;
		case Command::CN_SATTACH:
			$c = new attachSessionCommand();
			break;
		case Command::CN_SQUIT:
			$c = new quitSessionCommand();
			break;
		case Command::CN_SERROR:
			$c = new sessionErrorCommand();
			break;
		case Command::CN_SDETACH:
			$c = new detachSessionCommand();
			break;
		case Command::CN_GKILL:
			$c = new killCommand();
			break;
		case Command::CN_GLIMIT:
			$c = new limitCommand();
			break;
		case Command::CN_GLIMITSTAT:
			$c = new limitStatusCommand();
			break;
		case Command::CN_GLIMITSTATRSP:
			$c = new limitStatusResponseCommand();
			break;
		case Command::CN_GUPTIME:
			$c = new uptimeCommand();
			break;
		case Command::CN_GUPTIMERSP:
			$c = new uptimeResponseCommand();
			break;
		case Command::CN_GINITCONNECTION:
			$c = new initConnectionCommand();
			break;
		case Command::CN_SNAME:
			$c = new sessionNameCommand();
			break;
		case Command::CN_SNAMERSP:
			$c = new sessionNameResponseCommand();
			break;
		case Command::CN_SSETNAME:
			$c = new setSessionNameCommand();
			break;
		default:
			throw new BTGException("Wrong command type: " . $cmdid ."\n");
			break;
		}

		if ($c != NULL)
		{
			try
			{
				$c->deserialize($data);
			}
			catch(Exception $e)
			{
				$e->setMessage("unable to deserialize ". $c->getName() . ": " . $e->getMessage());
				throw $e;
			}
		}

		return $c;
	}
}
?>

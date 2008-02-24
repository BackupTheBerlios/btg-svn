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
 * $Id: trackerstatus.php,v 1.1.2.6 2007/02/01 23:29:15 jstrom Exp $
 */
if(!defined("BTG_BCORE_PATH"))
	define("BTG_BCORE_PATH", "bcore");

require_once(BTG_BCORE_PATH."/btgexception.php");
require_once(BTG_BCORE_PATH."/serializable.php");

class trackerStatus extends BTGSerializable
{
	const time_out = 0;
	const warning = 1;

	const succeeded_ok = 200;
	const succeeded_created = 201;
	const succeeded_accepted = 202;
	const succeeded_non_authorative_info = 203;
	const succeeded_no_content = 204;
	const succeeded_reset_content = 205;
	const succeeded_partial_content = 206;

	const redirection_multiple_choices = 300;
	const redirection_moved_permanently = 301;
	const redirection_found = 302;
	const redirection_see_other = 303;
	const redirection_not_modified = 304;
	const redirection_use_proxy = 305;
	const redirection_unused = 306;
	const redirection_temporary_redirect = 308;

	const client_bad_request = 400;
	const client_unauthorized = 401;
	const client_payment_required = 402;
	const client_forbidden = 403;
	const client_not_found = 404;
	const client_method_not_allowed = 405;
	const client_not_acceptable = 406;
	const client_proxy_authentication_required = 407;
	const client_request_timeout = 408;
	const client_conflict = 409;
	const client_gone = 410;
	const client_length_required = 411;
	const client_precondition_failed = 412;
	const client_request_entity_too_large = 413;
	const client_request_uri_too_long = 414;
	const client_unsupported_media_type = 415;
	const client_requested_range_not_satisfiable = 416;
	const client_expectation_failed = 417;

	const server_internal_server_error = 500;
	const server_not_implemented = 501;
	const server_bad_gateway = 502;
	const server_service_unavailable = 503;
	const server_gateway_timeout = 504;
	const server_http_version_not_supported = 505;

	const server_internal_malformed_uri = 600;
	const server_internal_connection_timed_out = 601;
	const server_internal_unknown_error = 602;
	const server_internal_could_not_parse_reply = 603;
	const server_internal_protocol_not_supported = 604;

	/// The status (one of the above const values)
	private $status;
	/// Serial
	private $serial;
	/// Message
	private $message = null;

	public function trackerStatus($serial = -1, $httpStatus = -1, $message = null)
	{
		$this->serial = $serial;
		$this->status = -1;
		$this->setStatus($httpStatus);
		$this->message = $message;
	}

	public function statusOk($httpStatus)
	{
		switch($httpStatus)
		{
			case trackerStatus::time_out:
			case trackerStatus::warning:

			case trackerStatus::succeeded_ok:
			case trackerStatus::succeeded_created:
			case trackerStatus::succeeded_accepted:
			case trackerStatus::succeeded_non_authorative_info:
			case trackerStatus::succeeded_no_content:
			case trackerStatus::succeeded_reset_content:
			case trackerStatus::succeeded_partial_content:

			case trackerStatus::redirection_multiple_choices:
			case trackerStatus::redirection_moved_permanently:
			case trackerStatus::redirection_found:
			case trackerStatus::redirection_see_other:
			case trackerStatus::redirection_not_modified:
			case trackerStatus::redirection_use_proxy:
			case trackerStatus::redirection_unused:
			case trackerStatus::redirection_temporary_redirect:

			case trackerStatus::client_bad_request:
			case trackerStatus::client_unauthorized:
			case trackerStatus::client_payment_required:
			case trackerStatus::client_forbidden:
			case trackerStatus::client_not_found:
			case trackerStatus::client_method_not_allowed:
			case trackerStatus::client_not_acceptable:
			case trackerStatus::client_proxy_authentication_required:
			case trackerStatus::client_request_timeout:
			case trackerStatus::client_conflict:
			case trackerStatus::client_gone:
			case trackerStatus::client_length_required:
			case trackerStatus::client_precondition_failed:
			case trackerStatus::client_request_entity_too_large:
			case trackerStatus::client_request_uri_too_long:
			case trackerStatus::client_unsupported_media_type:
			case trackerStatus::client_requested_range_not_satisfiable:
			case trackerStatus::client_expectation_failed:

			case trackerStatus::server_internal_server_error:
			case trackerStatus::server_not_implemented:
			case trackerStatus::server_bad_gateway:
			case trackerStatus::server_service_unavailable:
			case trackerStatus::server_gateway_timeout:
			case trackerStatus::server_http_version_not_supported:

			case trackerStatus::server_internal_malformed_uri:
			case trackerStatus::server_internal_connection_timed_out:
			case trackerStatus::server_internal_unknown_error:
			case trackerStatus::server_internal_could_not_parse_reply:
			case trackerStatus::server_internal_protocol_not_supported:
				return true;
			default:
				return false;
		}
	}

	public function setStatus($httpStatus)
	{
		if($this->statusOk($httpStatus))
			$this->status = $httpStatus;
		else
			$this->status = -1;
	}

	public function setSerial($serial)
	{
		$this->serial = $serial;
	}

	function getStatus()	{ return $this->status; }

	public function setMessage($message)
	{
		$this->message = $message;
	}

	function getMessage()	{ return $this->message; }

	public function serialize(&$a = array())
	{
		$this->intToBytes($a, $this->status);
		$this->intToBytes($a, $this->serial);
		if($this->message != null)
		{
			$this->boolToBytes($a, true);
			$this->stringToBytes($a, $this->message);
		}
		else
			$this->boolToBytes($a, false);
		return $a;
	}

	public function deserialize(&$data)
	{
		$temp_status = 0;
		$this->bytesToInt($temp_status, $data);

		if(!$this->statusOk($temp_status))
			$tempt_status = -1;
			//throw new BTGException("trackerStatus::deserialize failed to deserialize status. Invalid status '$temp_status'.");

		$this->status = $temp_status;

		$this->bytesToInt($this->serial, $data);

		$isSet = false;
		$this->bytesToBool($isSet, $data);
		if($isSet)
			$this->bytesToString($this->message, $data);
		else
			$this->message = null;
	}

	public function toString($lineWidth = 80)
	{
		$ret.= "status: (".$this->serial.")". $this->getDescription() . "\n";

		return $ret;
	}

	public function getDescription() 
	{
		$output = "";

		switch($this->status)
		{
			case trackerStatus::time_out:
				$output = "Time Out";
				break;
			case trackerStatus::warning:
				$output = "Warning";
				break;
			case trackerStatus::succeeded_ok:
				$output = "OK";
				break;
			case trackerStatus::succeeded_created:
				$output = "Created";
				break;
			case trackerStatus::succeeded_accepted:
				$output = "Accepted";
				break;
			case trackerStatus::succeeded_non_authorative_info:
				$output = "Non-Authorative Info";
				break;
			case trackerStatus::succeeded_no_content:
				$output = "No Content";
				break;
			case trackerStatus::succeeded_reset_content:
				$output = "Reset Content";
				break;
			case trackerStatus::succeeded_partial_content:
				$output = "Partial Content";
				break;
			case trackerStatus::redirection_multiple_choices:
				$output = "Multiple Choices";
				break;
			case trackerStatus::redirection_moved_permanently:
				$output = "Moved Permanently";
				break;
			case trackerStatus::redirection_found:
				$output = "Found";
				break;
			case trackerStatus::redirection_see_other:
				$output = "See Other";
				break;
			case trackerStatus::redirection_not_modified:
				$output = "Not Modified";
				break;
			case trackerStatus::redirection_use_proxy:
				$output = "Use Proxy";
				break;
			case trackerStatus::redirection_unused:
				$output = "Unused";
				break;
			case trackerStatus::redirection_temporary_redirect:
				$output = "Temporary Redirect";
				break;
			case trackerStatus::client_bad_request:
				$output = "Bad Request";
				break;
			case trackerStatus::client_unauthorized:
				$output = "Unauthorized";
				break;
			case trackerStatus::client_payment_required:
				$output = "Payment Required";
				break;
			case trackerStatus::client_forbidden:
				$output = "Forbidden";
				break;
			case trackerStatus::client_not_found:
				$output = "Not Found";
				break;
			case trackerStatus::client_method_not_allowed:
				$output = "Method Not Allowed";
				break;
			case trackerStatus::client_not_acceptable:
				$output = "Not Acceptable";
				break;
			case trackerStatus::client_proxy_authentication_required:
				$output = "Proxy Authentication Required";
				break;
			case trackerStatus::client_request_timeout:
				$output = "Request Timeout";
				break;
			case trackerStatus::client_conflict:
				$output = "Conflict";
				break;
			case trackerStatus::client_gone:
				$output = "Gone";
				break;
			case trackerStatus::client_length_required:
				$output = "Length Required";
				break;
			case trackerStatus::client_precondition_failed:
				$output = "Precondition Failed";
				break;
			case trackerStatus::client_request_entity_too_large:
				$output = "Request Entity Too Large";
				break;
			case trackerStatus::client_request_uri_too_long:
				$output = "Request-URI Too Large";
				break;
			case trackerStatus::client_unsupported_media_type:
				$output = "Unsupported Media Type";
				break;
			case trackerStatus::client_requested_range_not_satisfiable:
				$output = "Requested Range Not Satisfiable";
				break;
			case trackerStatus::client_expectation_failed:
				$output = "Expectation Failed";
				break;
			case trackerStatus::server_internal_server_error:
				$output = "Internal Server Error";
				break;
			case trackerStatus::server_not_implemented:
				$output = "Not Implemented";
				break;
			case trackerStatus::server_bad_gateway:
				$output = "Bad Gateway";
				break;
			case trackerStatus::server_service_unavailable:
				$output = "Service Unavailable";
				break;
			case trackerStatus::server_gateway_timeout:
				$output = "Gateway Timeout";
				break;
			case trackerStatus::server_http_version_not_supported:
				$output = "HTTP Version not supported";
				break;
			case trackerStatus::server_internal_malformed_uri:
				$output = "Malformed URI";
				break;
			case trackerStatus::server_internal_connection_timed_out:
				$output = "Connection Timed Out";
				break;
			case trackerStatus::server_internal_unknown_error:
				$output = "Unknown Error";
				break;
			case trackerStatus::server_internal_could_not_parse_reply:
				$output = "Could Not Parse Reply";
				break;
			case trackerStatus::server_internal_protocol_not_supported:
				$output = "Protocol Not Supported";
				break;
			default:
				$output = "Undefined Error";
				break;
		}

		$output .= " (".$this->status.")";

		if($this->message != null)
			$output.= ": ".$this->message;
		return $output;
	}
}
?>

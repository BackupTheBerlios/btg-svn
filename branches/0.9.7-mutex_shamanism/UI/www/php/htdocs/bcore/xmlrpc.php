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
 * This library is, although with heavy modifications, based on:
 *
 * IXR - The Inutio XML-RPC Library - (c) Incutio Ltd 2002 Version 1.61 -
 * Simon Willison, 11th July 2003 (htmlentities -> htmlspecialchars) 
 * Site: http://scripts.incutio.com/xmlrpc/ 
 * Manual: http://scripts.incutio.com/xmlrpc/manual.php
 * Made available under the Artistic License: http://www.opensource.org/licenses/artistic-license.php
 */

/*
 * $Id: xmlrpc.php,v 1.1.2.2 2006/09/01 10:54:27 jstrom Exp $
 */

/**
 * Represents a XMLRPC Value. 
 */
class XMLRPC_Value
{
	private $data;
	private $type;
	function XMLRPC_Value($data, $type = false)
	{
		$this->data = $data;
		if(!$type)
		{
			$type = $this->calculateType();
		}
		$this->type = $type;
		if($type == 'struct')
		{
			/*
			* Turn all the values in the array in to new
			* XMLRPC_Value objects
			*/
			foreach($this->data as $key => $value)
			{
				$this->data[$key] = new XMLRPC_Value($value);
			}
		}
		else if($type == 'array')
		{
			for ($i = 0, $j = count($this->data); $i < $j; $i++)
			{
				$this->data[$i] = new XMLRPC_Value($this->data[$i]);
			}
		}
	}

	/**
	 * Try to figure out what type our data has.
	 */
	function calculateType()
	{
		if($this->data === true || $this->data === false)
		{
			return 'boolean';
		}
		if(is_integer($this->data))
		{
			return 'int';
		}
		if(is_double($this->data))
		{
			return 'double';
		}
		// Deal with XMLRPC object types base64 and date
		if(is_object($this->data) && ($this->data instanceof XMLRPC_Date))
		{
			return 'date';
		}
		if(is_object($this->data) && ($this->data instanceof XMLRPC_Base64))
		{
			return 'base64';
		}
		// If it is a normal PHP object convert it in to a struct
		if(is_object($this->data))
		{
			$this->data = get_object_vars($this->data);
			return 'struct';
		}
		if(!is_array($this->data))
		{
			return 'string';
		}
		/* We have an array - is it an array or a struct ? */
		if($this->isStruct($this->data))
		{
			return 'struct';
		} else
		{
			return 'array';
		}
	}

	/**
	 * Wrap our value in appropriate XML field.
	 */
	function getXml()
	{
		/* Return XML for this value */
		switch ($this->type)
		{
		case 'boolean':
			return '<boolean>'.(($this->data) ? '1' : '0').'</boolean>';
			break;
		case 'int':
			return '<int>'.$this->data.'</int>';
			break;
		case 'double':
			return '<double>'.$this->data.'</double>';
			break;
		case 'string':
			return '<string>'.htmlspecialchars($this->data).'</string>';
			break;
		case 'array':
			$return = '<array><data>'."\n";
			foreach($this->data as $item)
			{
				$return.= '  <value>'.$item->getXml()."</value>\n";
			}
			$return.= '</data></array>';
			return $return;
			break;
		case 'struct':
			$return = '<struct>'."\n";
			foreach($this->data as $name => $value)
			{
				$return.= "  <member><name>$name</name><value>";
				$return.= $value->getXml()."</value></member>\n";
			}
			$return.= '</struct>';
			return $return;
			break;
		case 'date':
		case 'base64':
			// Both date and base64 are special objects.
			return $this->data->getXml();
			break;
		}
		return false;
	}

	/**
	 * Nasty function to check if the value is a struct or not. 
	 */
	function isStruct($array)
	{
		$expected = 0;
		foreach($array as $key => $value)
		{
			if((string) $key != (string) $expected)
			{
				return true;
			}
			$expected++;
		}
		return false;
	}
}


/**
 * A received XMLRPC message. Handles decoding of the XML.
 */
class XMLRPC_Message
{
	private $xml;				// The raw data (XML) 
	public $messageType;		// methodCall / methodResponse / fault
	public $parseError;		// the specific parse error if one occurred
	public $faultCode;		// fault code if a fault occured
	public $faultString;		// fault message
	public $methodName;		// The name of the called method
	public $params;			// Current variable stacks
	private $_arraystructs = array(); // The stack used to keep track of the current array / struct
	private $_arraystructstypes = array(); // Stack keeping track of if things are structs or array
	private $_currentStructName = array(); // A stack as well
	private $_param;
	private $_value;
	private $_currentTag;
	private $_currentTagContents;
	private $_parser;			 //The XML parser

	function XMLRPC_Message($xml)
	{
		$this->xml = $xml;
	}

	/**
	 * Parse our XML body. Will sett $params
	 */
	function parse()
	{
		//first remove the XML declaration
		$this->xml = preg_replace('/<\?xml(.*)?\?'.'>/', '', $this->xml);
		if(trim($this->xml) == '')
		{
			return false;
		}

		$this->_parser = xml_parser_create();

		// Set XML parser to take the case of tags in to account
		xml_parser_set_option($this->_parser, XML_OPTION_CASE_FOLDING, false);

		// Set XML parser callback function s
		xml_set_object($this->_parser, $this);
		xml_set_element_handler($this->_parser, 'tag_open', 'tag_close');
		xml_set_character_data_handler($this->_parser, 'cdata');

		if(!xml_parse($this->_parser, $this->xml))
		{
			$this->parseError = sprintf('XML error: %s at line %d',
					xml_error_string(xml_get_error_code($this->_parser)),
					xml_get_current_line_number($this->_parser) );

			return false;
		}
		xml_parser_free($this->_parser);

		// Grab the error messages, if any
		if($this->messageType == 'fault')
		{
			$this->faultCode = $this->params[0]['faultCode'];
			$this->faultString = $this->params[0]['faultString'];
		}
		return true;
	}

	function tag_open($parser, $tag, $attr)
	{
		$this->currentTag = $tag;
		switch ($tag)
		{
		case 'methodCall':
		case 'methodResponse':
		case 'fault':
			$this->messageType = $tag;
			break;
		/* Deal with stacks of arrays and structs */
		case 'data'://data is to all intents and puposes more interesting than array
			$this->_arraystructstypes[] = 'array';
			$this->_arraystructs[] = array();
			break;
		case 'struct':
			$this->_arraystructstypes[] = 'struct';
			$this->_arraystructs[] = array();
			break;
		}
	}

	function cdata($parser, $cdata)
	{
		$this->_currentTagContents.= $cdata;
	}

	function tag_close($parser, $tag)
	{
		$valueFlag = false;
		switch ($tag)
		{
		case 'int':
		case 'i4':
			$value = (int) trim($this->_currentTagContents);
			$this->_currentTagContents = '';
			$valueFlag = true;
			break;
		case 'double':
			$value = (double) trim($this->_currentTagContents);
			$this->_currentTagContents = '';
			$valueFlag = true;
			break;
		case 'string':
			$value = (string) trim($this->_currentTagContents);
			$this->_currentTagContents = '';
			$valueFlag = true;
			break;
		case 'dateTime.iso8601':
			$value = new XMLRPC_Date(trim($this->_currentTagContents));
			//$value = $iso->getTimestamp();
			$this->_currentTagContents = '';
			$valueFlag = true;
			break;
		case 'value':
			//"If no type is indicated, the type is string."
			if(trim($this->_currentTagContents) != '')
			{
				$value = (string) $this->_currentTagContents;
				$this->_currentTagContents = '';
				$valueFlag = true;
			}
			break;
		case 'boolean':
			$value = (boolean) trim($this->_currentTagContents);
			$this->_currentTagContents = '';
			$valueFlag = true;
			break;
		case 'base64':
			$value = base64_decode($this->_currentTagContents);
			$this->_currentTagContents = '';
			$valueFlag = true;
			break;
		/* Deal with stacks of arrays and structs */
		case 'data':
		case 'struct':
			$value = array_pop($this->_arraystructs);
			array_pop($this->_arraystructstypes);
			$valueFlag = true;
			break;
		case 'member':
			array_pop($this->_currentStructName);
			break;
		case 'name':
			$this->_currentStructName[] = trim($this->_currentTagContents);
			$this->_currentTagContents = '';
			break;
		case 'methodName':
			$this->methodName = trim($this->_currentTagContents);
			$this->_currentTagContents = '';
			break;
		}

		if($valueFlag)
		{
			/*
			if(!is_array($value) && !is_object($value))
			{
				$value = trim($value);
			}
			*/
			if(count($this->_arraystructs) > 0)
			{
				//Add value to struct or array
				if($this->_arraystructstypes[count($this->_arraystructstypes) - 1] == 'struct')
				{
					//Add to struct
					$this->_arraystructs[count($this->_arraystructs) - 1][$this->_currentStructName[count($this->_currentStructName) - 1]] = $value;
				} else
				{
					//Add to array
					$this->_arraystructs[count($this->_arraystructs) - 1][] = $value;
				}
			} else
			{
				//Just add as a paramater
				$this->params[] = $value;
			}
		}
	}
}

/** 
 * A XMLRPC Request object. This is used when a request is sent to a server.
 */
class XMLRPC_Request
{
	private $method;
	private $args;
	private $xml;
	function XMLRPC_Request($method, $args)
	{
		$this->method = $method;
		$this->args = $args;
		$this->xml = <<<EOD
<?xml version="1.0" ?>
<methodCall>
<methodName>{$this->method}</methodName>
<params>
EOD;

		foreach($this->args as $arg)
		{
			$this->xml.= '<param><value>';
			$v = new XMLRPC_Value($arg);
			$this->xml.= $v->getXml();
			$this->xml.= "</value></param>\n";
		}
		$this->xml.= '</params></methodCall>';
	}

	function getLength()
	{
		return strlen($this->xml);
	}

	function getXml()
	{
		return $this->xml;
	}
}

/**
 * The client used to perform requests.
 */
class XMLRPC_Client
{	
	private $gzip_available = false;
	private $use_gzip = false;

	private $method = "http";
	private $ssl_ca_cert = null;
	private $ssl_client_cert = null;
	private $server;
	private $port = 80;
	private $path = "/";
	private $timeout;
	private $context = null;

	private $useragent= "";
	private $response = "";
	private $message = false;
	private $debug = false;
	private $error = false;	//Storage place for an error message
	private $fp = null;
	private $rxbuff = "";

	function XMLRPC_Client($server, $path = false, $port = 80, $timeout=20)
	{
		if(!$path)
		{
			// Assume we have been given a URL instead
			$pieces = parse_url($server);
			$this->method = $pieces['scheme'];
			$this->server = $pieces['host'];
			$this->port = (int)isset($pieces['port']) ? $pieces['port'] : 80;
			$this->path = isset($pieces['path']) ? $pieces['path'] : '/';
			// Make absolutely sure we have a path
			if(!$this->path)
			{
				$this->path = '/';
			}
		} else
		{
			$this->server = $server;
			$this->path = $path;
			$this->port = (int)$port;
		}

		$this->useragent = 'BTG XMLRPC PHP Client/The Incutio XML-RPC PHP Library';

		$this->timeout = $timeout;

		if(function_exists('gzuncompress') && function_exists('gzcompress'))
			$this->gzip_available = true;

	}

	/** 
	 * Enabler/Disable debug mode
	 */
	function setDebug($debug)
	{
		$this->debug = (boolean)$debug;
	}

	/** 
	 * Set timeout
	 */
	function setTimeout($timeout)
	{
		$this->timeout = (int)$timeout;
	}

	/**
	 * Enable/disable GZIP compression of sent data.
	 */
	function enableGZIP($enable)
	{
		if(!$this->gzip_available)
			return false;

		$this->use_gzip = $enable;
		return true;
	}

	/**
	 * Set SSL parameters (used when a URL in form https:// is used).
	 * The params are ssl_ca_cert, path to the the Certificate Authority cert 
	 * and ssl_client_cert, which is path to this clients certificate, PEM encoded, without encryption.
	 */
	function setSSLparams($ca_cert, $client_cert)
	{
		if(!in_array("sslv3", stream_get_transports()))
		{
			$this->error = new XMLRPC_Error(XMLRPC_Error::CONNECT_FAILED, "This PHP version does not have SSL support.");
			return false;
		}
		
		$this->ssl_ca_cert = $ca_cert;
		$this->ssl_client_cert = $client_cert;
		return true;
	}

	/**
	 * Close any connection and clear out buffer.
	 */
	function close()
	{
		if($this->fp)
		{
			fclose($this->fp);
			$this->fp = null;
		}
		$this->rxbuff="";
	}

	/**
    * Perform a query. Takes an unspecified number of arguments.
	 * First argument should be method name to call.
	 * The follow arguments will be passed to the remote server as 
	 * arguments to the method call.
	 */
	function query()
	{
		$args = func_get_args();
		$this->query_arg($args);
	}

	/**
	 * Another way to perform a query, with an array having all parameters.
	 * Works the same way as query(), first item in array should be method name,
	 * any subsequent items will be passed to remote server.
	 */
	function query_arg($args)
	{
		$method = array_shift($args);

		// Create the request and encode it
		$request = new XMLRPC_Request($method, $args);
		$xml = $request->getXml();

//		$this->use_gzip = 0;
		if($this->use_gzip)
		{
			$xml = (pack("C1C1C1C1VC1C1", 0x1f, 0x8b, 8, 0, 0, 2, 0xFF) .
				gzdeflate($xml) .
				pack("VV", crc32($xml), strlen($xml))); 
		}

		$length = strlen($xml);

		// HTTP uses \r\n for lineendings
		$r = "\r\n";

		$request = "POST {$this->path} HTTP/1.0$r";
		$request.= "Host: {$this->server}$r";
		$request.= "Content-Type: text/xml$r";
		$request.= "User-Agent: {$this->useragent}$r";

		if($this->gzip_available)
			$request.= "Accept-Encoding: gzip;q=1, identity;1=0.5$r"; 	// We can decode gzip which we prefer. identify (clear) works too

		if($this->use_gzip)
			$request.= "Content-Encoding: gzip$r"; 	// Send content gziped 
		
		$request.= "Content-Length: {$length}$r$r";
		$reqlen = strlen($request) + $length; 

		$request.= $xml;


		//Now send the request
		if($this->debug)
		{
			echo '<pre>'.$request."</pre>\n\n";
		}

		$errno = 0;
		$errstr = "";
		if(!$this->fp)
		{
			if($this->method == "https")
			{
				if(!is_readable($this->ssl_ca_cert))
				{
					$this->error = new XMLRPC_Error(XMLRPC_Error::CONNECT_FAILED, "SSL CA cert is missing or isnt readable.");
					return false;
				}

				if(!is_readable($this->ssl_client_cert))
				{
					$this->error = new XMLRPC_Error(XMLRPC_Error::CONNECT_FAILED, "SSL client cert missing or isnt readable.");
					return false;
				}

				// HTTPs connection
				$this->context = stream_context_create(
						array('ssl'=>array(
							'cafile'=>$this->ssl_ca_cert,
							'local_cert'=>$this->ssl_client_cert
							))
					);
				

				$this->fp = @stream_socket_client("tls://".$this->server.":".$this->port."/", $errno, $errstr, $this->timeout, STREAM_CLIENT_CONNECT, $this->context);
			}
			else
			{
				$this->fp = @fsockopen($this->server, $this->port, $errno, $errstr, $this->timeout);
			}
		}

		if(!$this->fp)
		{
			$this->error = new XMLRPC_Error(XMLRPC_Error::CONNECT_FAILED, "Failed to connect to ".$this->server.":".$this->port.": $errstr.");
			return false;
		}

		stream_set_blocking ( $this->fp, true);
/*
		$piecelen = 80000000;
		if($reqlen > $piecelen)
		{
			$piece = null;
			$offset = 0;
			while($offset < $reqlen)
			{
				if($reqlen - $offset > $piecelen)
				{
					$piece = substr($request, $offset, $piecelen);
					$offset+=$piecelen;
				}else
				{
					$piece = substr($request, $offset);
					$offset += $reqlen-$offset;
				}
				print "Wriing $offset -+ $piecelen..\n";
				fputs($this->fp, $piece);
			}
		}
		else
 */
		fputs($this->fp, $request);

		$contents = '';
		$headers = array();
		$gotFirstLine = false;
		$gettingHeaders = true;
		$contentLength = 0;

		// We want blocking on first request
		stream_set_blocking ( $this->fp, true);
		while (1)
		{
			$buff = fread($this->fp, 1024);
			// But if we got more data queued we dont want to block...
			stream_set_blocking ( $this->fp, false);
			$this->rxbuff.= $buff;

			if($gettingHeaders)
			{
				while (1)
				{
					$pos = strpos($this->rxbuff, "\r\n");

					if($pos === false)
						break;

					$line = trim(substr($this->rxbuff, 0, $pos));

					$this->rxbuff = substr($this->rxbuff, strlen($line) + 2);

					if(!$gotFirstLine)
					{
						//Check line for '200'
						if(strstr($line, '200') === false)
						{
							$this->error = new XMLRPC_Error(XMLRPC_Error::INVALID_STATUS_CODE, 'Failed to received XMLRPC response. HTTP status code was not 200');
							return false;
						}
						$gotFirstLine = true;
						continue;
					}

					if($line == '')
					{
						$gettingHeaders = false;
						$contentLength = $headers['Content-Length'];
						break;
					}

					list($k, $v) = explode(":", $line);
					$headers[trim($k)] = trim($v);
				}
				if($gettingHeaders)
					continue;
			}

			if(strlen($this->rxbuff) >= $contentLength)
			{
				$contents = substr($this->rxbuff, 0, $contentLength);
				$this->rxbuff = substr($this->rxbuff, $contentLength);
				break;
			}
		}

		if($this->gzip_available && array_key_exists("Content-Encoding", $headers))
		{
			if($headers['Content-Encoding'] == "gzip")
			{
				$ret = gzinflate(substr($contents,10));
				if($ret === false)
				{
					// The decoding failed!
					$this->error = new XMLRPC_Error(XMLRPC_Error::BROKEN_GZIP, 'XMLRPC response headers specified gzip encoding, failed to decompress.');
					return false;
				}

				$contents = $ret;
			}else
			{
				// What encoding is this?!
				$this->error = new XMLRPC_Error(XMLRPC_Error::UNKNOWN_ENCODING, 'XLMRPC response headers specified unknown encoding: '.$headers['Content-Encoding'].'. Cannot decompress..');
				return false;
			}
		}

		if($this->debug)
		{
			echo '<pre>'.$contents."</pre>\n\n";
		}

		// Now parse what we 've got back
		$this->message = new XMLRPC_Message($contents);
		if(!$this->message->parse())
		{
			// XML error
			if($this->debug)
				$this->error = new XMLRPC_Error(XMLRPC_Error::PARSE_ERROR, 'XMLRPC response parse error. Not well formed. Error: '.$this->message->parseError);
			else
				$this->error = new XMLRPC_Error(XMLRPC_Error::PARSE_ERROR, 'XMLRPC response parse error. Not well formed.');
			return false;
		}

		// Is the message a fault ?
		if($this->message->messageType == 'fault')
		{
			$this->error = new XMLRPC_Error($this->message->faultCode, $this->message->faultString);
			return false;
		}

		// Message must be OK
		return true;
	}

	function getResponse()
	{
		// methodResponses can only have one param - return that
		return $this->message->params[0];
	}

	function isError()
	{
		return (is_object($this->error));
	}

	function getErrorCode()
	{
		return $this->error->code;
	}

	function getErrorMessage()
	{
		return $this->error->message;
	}
}


/**
 * A class containing either an XMLRPC fault or an internal error.
 * For internal error codes check $code against the constants defined below.
 */
class XMLRPC_Error
{
	const CONNECT_FAILED = -32300;		// Failed to open socket.
	const INVALID_STATUS_CODE = -32301; // Invalid (non 200) status code returned.
	const BROKEN_GZIP = -32302;			// GZIP failed to decompress.
	const UNKNOWN_ENCODING = -32303;		// Unknown encoding
	const PARSE_ERROR = -32304;			// Parse error.

	public $code;
	public $message;
	function XMLRPC_Error($code, $message)
	{
		$this->code = $code;
		$this->message = $message;
	}
	function getXml()
	{
		$xml = <<<EOD
<methodResponse>
<fault>
<value>
<struct>
<member>
<name>faultCode</name>
<value><int>{$this->code}</int></value>
</member>
<member>
<name>faultString</name>
<value>< tring> {$this->message}</string></value>
</member>
</struct>
</value>
</fault>
</methodResponse>

EOD;
		return $xml;
	}
}

/**
 * Wrapper class for a date value
 */
class XMLRPC_Date
{
	private $year;
	private $month;
	private $day;
	private $hour;
	private $minute;
	private $second;
	function XMLRPC_Date($time)
	{
		//$time can be a PHP timestamp or an ISO one
		if(is_numeric($time))
		{
			$this->parseTimestamp($time);
		} else
		{
			$this->parseIso($time);
		}
	}
	
	function parseTimestamp($timestamp)
	{
		$this->year = date('Y', $timestamp);
		$this->month = date('Y', $timestamp);
		$this->day = date('Y', $timestamp);
		$this->hour = date('H', $timestamp);
		$this->minute = date('i', $timestamp);
		$this->second = date('s', $timestamp);
	}

	function parseIso($iso)
	{
		$this->year = substr($iso, 0, 4);
		$this->month = substr($iso, 4, 2);
		$this->day = substr($iso, 6, 2);
		$this->hour = substr($iso, 9, 2);
		$this->minute = substr($iso, 12, 2);
		$this->second = substr($iso, 15, 2);
	}

	function getIso()
	{
		return $this->year.$this->month.$this->day.'T'.$this->hour.':'.$this->minute.':'.$this->second;
	}

	function getXml()
	{
		return '<dateTime.iso8601>'.$this->getIso().'</dateTime.iso8601>';
	}

	function getTimestamp()
	{
		return mktime($this->hour, $this->minute, $this->second, $this->month, $this->day, $this->year);
	}
}

/**
 * Wrapper class for a base64 value
 */
class XMLRPC_Base64
{
	private $data;
	function XMLRPC_Base64($data)
	{
		$this->data = $data;
	}

	function getXml()
	{
		return '<base64>'.base64_encode($this->data).'</base64>';
	}
}
?>

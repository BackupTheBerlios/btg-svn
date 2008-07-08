<?php
/*
 * PHPAjax class written by Johan Ström.
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
 * $Id: phpajax.php,v 1.1.2.12 2007/08/07 16:26:15 wojci Exp $
 */

class PHPAjax
{
	private $debug = 0;
	private $uri="";
	private $functions = array();

	public function PHPAjax($uri = NULL)
	{
		global $_SERVER;
		if(!isset($uri))
		{
			$uri = findURI();
			// $uri = $_SERVER['REQUEST_URI'];
		}
		$this->uri  = $uri;
	}

	/* Get the URL, including the port information from
	 * globals. 
	 */

	/// Find the URI which will be used for making requests.
	public function findURI()
	{

		/* Taken from php.net, org. written by seb at omegasoft
		 * dot co dot uk. 
		*/

	        if($_SERVER['HTTPS'])
		{
			$rurl = 'https://';
		}
		else
		{
			$rurl = 'http://';
		}

		$rurl .= $_SERVER['HTTP_HOST'];

		$rurl .= ':' . $_SERVER['SERVER_PORT'];

    		$rurl .= $_SERVER['SCRIPT_NAME'];

		if($_SERVER['QUERY_STRING'] != null)
		{
			$rurl .= '?' . $_SERVER['QUERY_STRING'];
		}

		return $rurl;
	}

	/// Enable/disable debug mode
	public function setDebug($debug)
	{
		$this->debug = ($debug?1:0);
	}

	/// Get the URI to call
	public function get_uri()
	{
		return $this->uri;
	}

	/// Regsiter a function with the Client Handler
	/// @param [in] $name 		The name of the function, this will be the function name visible to the javascript.
	/// @param [in] $function 	The function to call. Can be a pure 'function_name' string, an array('class', 'function'), or array($classinstance, 'function')
	/// @param [in] $method 	The HTTP method this function should use. Default is GET, only other value is POST
	public function register($name, $function, $method="GET")
	{
		$this->functions[$name] = array('function'=>$function, 'method'=>$method);
	}

	/// Check if there is a AJAX call to handle. Handles spawning of functions.
	/// Returns true if function was processed (parent should exit!), else false.
	public function handle_client_request()
	{
		$method_name = "";
		if(isset($_POST['ajax_call']))
		{
			$method_name = $_POST['ajax_call'];
			$mode = "POST";

			if(isset($_POST["ajax_args"])) 
				$args = $_POST["ajax_args"];
			else
				$args = array();
		}
		elseif(isset($_GET['ajax_call']))
		{
			$method_name = $_GET['ajax_call'];
			$mode = "GET";

			if(isset($_GET["ajax_args"])) 
				$args = $_GET["ajax_args"];
			else
				$args = array();
		}

		if($method_name == "")
			// No call for us..
			return false;
		
		ob_start();
		if($mode == "GET")
		{
			header ("Expires: Mon, 26 Jul 1997 05:00:00 GMT");    // Date in the past
			header ("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
			header ("Cache-Control: no-cache, must-revalidate");  // HTTP/1.1
			header ("Pragma: no-cache");                          // HTTP/1.0
		}

		if (!array_key_exists($method_name, $this->functions))
		{
			$result = "<fault><faultText>No such method</faultText></fault>";
		}
		else
		{
			$function = $this->functions[$method_name];
			$result = call_user_func_array($function['function'], $args);
		}
		$errors = ob_get_contents();
		ob_end_clean();

		$result = "<result>$result</result>";
		if(strlen($errors) > 0)
		{
			$errors = strip_tags($errors);
			$result.= "<fault>$errors</fault>";
		}
		
		@header("Content-Type: text/xml");
		$output = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		$output.= "<ajaxResponse methodName=\"$method_name\">\n";
		$output.= $result;
		$output.= "</ajaxResponse>\n";

		echo $output;
		
		return true;
	}

	/// Function to get the JavaScript that should be outputed to the webpage
	/// This is where the function wrappers are created
	public function get_js()
	{
		$js = $this->get_common_js();
		
		foreach($this->functions as $fname=>$f)
		{
			$js.="function ".$fname."() {\n";
			$js.="	ajax_call('".$fname."', '".$f['method']."', ".$fname.".arguments);\n";
			$js.="}\n\n";
		}

		return $js;
	}

	/// Function with the standard JavaScript
	private function get_common_js()
	{
		?>
var ajax_request = null;
var phpajax_debug = <?php echo $this->debug;?>;
function ajax_init()
{
	try {
		ajax_request = new XMLHttpRequest();
	} catch (trymicrosoft) {
		try {
			ajax_request = new ActiveXObject("Msxml2.XMLHTTP");
		} catch (othermicrosoft) {
			try {
				ajax_request = new ActiveXObject("Microsoft.XMLHTTP");
			} catch (failed) {
				ajax_request = null;
			} 
		}
	}
	if (!ajax_request)
		alert("Sorry, your browser is to old. To use this page, please make yourself happier and download a newer browser.");
}

function ajax_done()
{
	if(ajax_request)
		delete ajax_request;
}

ajax_init();


function ajax_call(function_name, method, function_args)
{
	if(!ajax_request)
		return false;
	
	if(ajax_request.readyState > 0 && ajax_request.readyState < 4)
	{
		// Another call is running! Ingore this call, hopefully its just the user who is in a hurry...
		return;
	}
	else if(ajax_request.readyState > 4)
	{
		// Initiate a new XMLHTTPRequest object... We have to do this in Opera since they dont seem
		// to support multiple calls per instance...
		ajax_init();
	}
	
	var uri="<?php echo $this->get_uri();?>";
	user_callback = function_args[0];
	user_callback_err = function_args[1];
	if(method == "POST")
	{
	}
	else
	{
		// Assume GET  
		if (uri.indexOf("?") == -1) 
			uri = uri + "?ajax_call=" + escape(function_name);
		else
			uri = uri + "&ajax_call=" + escape(function_name);

		// First arg is callback, dont send that... 
		for (i = 2; i < function_args.length; i++) 
			uri = uri + "&ajax_args[]=" + escape(function_args[i]);

		uri = uri + "&rand=" + new Date().getTime();
		post_data = null;
	}

	ajax_request.open(method, uri, true);
	if (method == "POST") {
		ajax_request.setRequestHeader("Method", "POST " + uri + " HTTP/1.1");
		ajax_request.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
	}
	
	// Our ready-state-change function 
	ajax_request.onreadystatechange = function(){
			if (ajax_request.readyState != 4) 
				return;

			if(ajax_request.status != 200)
			{
				alert("Error: Got status "+ajax_request.status+ " "+ajax_request.statuStext+" from server instead of 200 OK. Call failed.");
				return;
			}

			dom = ajax_request.responseXML;

			if(!dom)
			{
				alert("Error: AJAX call failed, no XML DOM returned! Data: "+ajax_request.responseText);
				return;
			}

			response = dom.getElementsByTagName('ajaxResponse')[0];
			called_method = response.getAttribute('methodName');

			// Debug messages 
			debug = response.getElementsByTagName('debug');
			if(phpajax_debug && debug.length > 0)
			{
				debugStr = "Debug information:\n\n"
				for(i=0; i < debug.length; i++)
					debugStr+=debug[i].childNodes[0].nodeValue+"\n";

				alert(debugStr);
			}

			// A fault is a fatal error, ie php outputed error messages 
			fault = response.getElementsByTagName('fault');
			if(fault.length > 0)
			{
				faultStr = "Error: Failed to execute "+called_method+". Got error "
				for(i=0; i < fault.length; i++)
					faultStr+=fault[i].childNodes[0].nodeValue+"\n";

				alert(faultStr);
				return;
			}

			// A error is a less serous error, ie something made the command fail
			error = response.getElementsByTagName('error');
			if(error.length > 0)
			{
				errStr = ""
				for(i=0; i < error.length; i++)
					errStr+=error[i].childNodes[0].nodeValue+"\n";

				user_callback_err(error, errStr);
				return;
			}

			// All OK, call our method 
			result = response.getElementsByTagName('result')[0];
			user_callback(result);
		};
	// End of ready-state-change function 
	
	ajax_request.send(post_data);
}
		<?php
		echo "\n";
	}
}
?>

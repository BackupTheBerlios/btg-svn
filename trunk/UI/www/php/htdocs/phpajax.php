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
	private $xhrtimeout = 5000;
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

	/// Set XHR timeout
	public function setXHRTimeout($xhrtimeout)
	{
		$this->xhrtimeout = $xhrtimeout;
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
			$js.="function ".$fname."() {";
			$js.="try{\n";
			$js.=	"var req = new XhrReq(arguments);";
			$js.=		"var cb = arguments[0];";
			$js.=		"var cb_err = arguments[1];";
			$js.=		"var args = [];\n";
			$js.=		"for(var x=2; x<arguments.length; x++){\n";
			$js.=			"args.push(arguments[x]);\n";
			$js.=		"}\n";
			$js.=		"req.call('".$fname."', '".$f['method']."', cb, cb_err, args);\n";
			$js.=		"return req;";
			$js.=	"}catch(e){";
			$js.=		"if(console)console.error(e); else alert(e);";
			$js.=	"}";
			$js.="}\n";
		}

		return $js;
	}

	/// Function with the standard JavaScript
	private function get_common_js()
	{
		?>
var phpajax_debug = <?php echo $this->debug;?>;
var xhrTimeout = <?php echo $this->xhrtimeout;?>;
function get_xhr()
{
	var xhr;
	try {
		xhr = new XMLHttpRequest();
	} catch (trymicrosoft) {
		try {
			xhr = new ActiveXObject("Msxml2.XMLHTTP");
		} catch (othermicrosoft) {
			try {
				xhr = new ActiveXObject("Microsoft.XMLHTTP");
			} catch (failed) {
				xhr = null;
			} 
		}
	}
	if (!xhr)
		alert("Sorry, your browser is to old. To use this page, please make yourself happier and download a newer browser.");
	return xhr;
}

var XhrReq = function(args) {
	this.xhr = null;

	this.call = function(function_name, method, cb, cb_err, function_args)
	{
		var uri="<?php echo $this->get_uri();?>";
		var xhr = this.xhr = get_xhr();
		this.cb = cb;
		this.cb_err = cb_err;
		this.args = function_args;

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
			for (i = 0; i < function_args.length; i++) {
				var a = function_args[i];
				if(typeof a == 'object') {
					var aa=a;
					a = "";
					for(var i = 0; i < aa.length; i++)
						a+= (i>0?',':'') + escape(aa[i]);
				}

				uri = uri + "&ajax_args[]=" + escape(a);
			}

			uri = uri + "&rand=" + new Date().getTime();
		}
		var self = this;
		xhr.onreadystatechange = function(){self.onreadystatechange()};
		xhr.open(method, uri, true);
		if (method == "POST") {
			xhr.setRequestHeader("Method", "POST " + uri + " HTTP/1.1");
			xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
		}
		
		this.timer = setTimeout(function(){self.ontimeout(); }, xhrTimeout);
		xhr.send();
		return true;
	};

	// Our ready-state-change function 
	this.onreadystatechange = function(){
		var xhr = this.xhr;
		if (xhr.readyState != 4) 
			return;

		if(this.timer)
			clearTimeout(this.timer);

		if(xhr.status != 200)
		{
			alert("Error: Got status "+xhr.status+ " "+xhr.statusText+" from server instead of 200 OK. Call failed.");
			return;
		}

		dom = xhr.responseXML;

		if(!dom)
		{
			alert("Error: AJAX call failed, no XML DOM returned! Data: "+xhr.responseText);
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
			delete xhr;
			delete this.xhr;
			return;
		}

		// A error is a less serous error, ie something made the command fail
		error = response.getElementsByTagName('error');
		if(error.length > 0)
		{
			errStr = ""
			for(i=0; i < error.length; i++)
				errStr+=error[i].childNodes[0].nodeValue+"\n";

			try {
				this.cb_err(error, errStr, this.args);
			}catch(e) {
				if(console) // Do we have firebug console?
					console.error(e);
				else
					alert("Unhandled exception in user error callback: "+e);
			}
			delete xhr;
			delete this.xhr;
			return;
		}

		// All OK, call our method 
		result = response.getElementsByTagName('result')[0];
		try {
			this.cb(result, this.args);
		}catch(e) {
			if(console) // Do we have firebug console?
				console.error(e);
			else
				alert("Unhandled exception in user callback: "+e);
		}
		delete xhr;
		delete this.xhr;
		return;
	};
	// End of ready-state-change function 
	
	this.ontimeout = function() {
		var xhr = this.xhr;
		if(!this.xhr)
			return console.error ("Timeout without an xhR?!");

		clearTimeout(this.timer);
		delete this.timer;

		try {
			if(xhr.readyState == 4 && xhr.status == 200)
				return this.onreadystatechange();
		} catch(e) {} // status might not always be available
	
		if(console) console.warn("xhr timeout, in state " + xhr.readyState);
		xhr.abort();

		try {
			this.cb_err(error, "Timeout");
		}catch(e) {
			if(console) // Do we have firebug console?
				console.error(e);
			else
				alert("Unhandled exception in user error callback: "+e);
		}
		delete xhr;
		delete this.xhr;
	};
}
		<?php
		echo "\n";
	}
}
?>

<?php
// Check PHP version, we require at least PHP 5
if(substr(phpversion(), 0,1) < 5)
	die("Sorry, btg-www requires at least PHP 5");

// make sure we can find version.php
@include("version.php");
if(!defined("BTG_BUILD"))
{
	echo "You have to run configure with --enable-www in order to use the web interface! version.php missing.";
	die();
}

session_start();
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<meta content="text/html; charset=UTF-8" http-equiv="Content-Type">
<meta http-equiv="Content-Language" content="en-us" />
<meta name="ROBOTS" content="NONE" />
<style type="text/css" media="screen">@import "btg.css";</style>
<script language="javascript" src="btg.js" type="text/javascript"></script>
<script language="javascript" src="ajaxjs.php" type="text/javascript"></script>
<script language="javascript" src="csshover.js" type="text/javascript"></script>
<title>BTG</title>
<!--[if IE]>
<script type="text/javascript">
var t = document.getElementsByTagName("tr");
for(var i=0;i<t.length;i++) {
	var ocn = t[i].className;
	t[i].onmouseover = function() { t[i].className = "hovered" };
	t[i].onmouseout = function() { t[i].className = ocn };
}
</script>
<![endif]-->
</head>
<body onLoad="init(<?php echo isset($_SESSION['btg_username'])?"1":"0";?>, <?php echo isset($_SESSION['btg_attached_session'])?$_SESSION['btg_attached_session']:"-1";?>)">

<div id="header">
	<h1>webBTG</h1>
</div>

<div id="top"></div>
<div id="container" style="display: none;">
	<div id="upload">
		<iframe name="upload_iframe" id="upload_iframe" style="width: 100px; height: 100px; display:none;" onLoad="uploadDone()"></iframe> 
		<form action="upload.php" target="upload_iframe" method="post" enctype="multipart/form-data" onSubmit="return uploadCheck();">
		<input type="hidden" name="MAX_FILE_SIZE" value="1000000" />
		<input class="btn" type="file" id="upload_input" name="torrent_file"/>
		<input id="upload_button" type="submit" value="Upload"/>
		</form>
	</div>

	<input id="logout_button" class="control_buttons" type="button" onClick="deauth();" value="Logout"/>
	<input id="detach_button" class="control_buttons" type="button" onClick="sessionDetach();" value="Detach session"/>
	<input id="quit_button" class="control_buttons" type="button" onClick="sessionQuit();" value="Quit session"/>
	<input id="collapse_button" class="control_buttons" type="button" onClick="hideAllContextDetails();" value="Hide all details"/>
	<input id="cleanall_button" class="control_buttons" type="button" onClick="cleanAllContexts();" value="Clean all"/>
	<input id="glimit_button" class="control_buttons" type="button" onClick="showGlobalLimits();" value="Global limit"/>
	<div id="layer_fatal">
	Fatal error.
	</div>

	<div id="layer_auth">
		<h2>Welcome to webBTG!</h2>
		<p>Please enter your login details:</p>
		<form action="" name="frm_auth">
		<input type="text" id="username" name="username" style="width:100px"/>
		<input type="password" id="password" name="password" style="width:100px"/>
		<input type="submit" id="login_button" onClick="auth(); return false;" value="Login"/>
		</form>
	</div>

	<div id="layer_sessions">
		<h2>Welcome to webBTG!</h2>
		<h3>Attach to session</h3>
		<p>If there are any sessions available, you can select a session here and attach to it.</p>
		<form action="" name="frm_sessionlist">
		<select id="sessionlist" name="sessionlist" style="width:100px"></select>
		<input type="submit" id="attach_button" onClick="sessionAttach(); return false;" value="Attach"/>
		</form>

		<h3>Setup new session</h3>
		<p>If you'd like to setup a new session, enter default seed limit (how many % we should seed before we stop) and/or default seed timeout (how long time we should seed before we stop).
		Both can be empty, in which case you have to stop your torrents by your self, or one of them can be used, in which case that condition will be used, or both can be used, and the first reached will be used.</p>
		<form action="" name="frm_sessionsetup">
		Seed limit <input type="text" id="seedLimit" name="seedLimit" value="" style="width:30px;"/>%,
		seed timeout <input type="text" id="seedTimeout" name="seedTimeout" value="" style="width:40px;"/> minutes
		<input type="submit" id="setup_button" onClick="sessionSetup(); return false;" value="Setup"/>
		</form>
	</div>
	
	<div id="layer_glimit">
		<form action="" name="frm_glimit">
		<table id="glimit_table" cellspacing=0>

		<tr>
			<th class="column_glimit">Limit name</th>
			<th class="column_glimit">Limit value</th>
		</tr>

		<tr>
			<td>Upload</td>
			<td><input type="text" id="gupload" name="gupload" value="0" style="width:30px;"/> KiB/sec</td>
		</tr>
		<tr>
			<td>Download</td>
			<td><input type="text" id="gdownload" name="gdownload" value="0" style="width:40px;"/> KiB/sec</td>
		</tr>
		<tr>
			<td>Max uploads</td>
			<td><input type="text" id="gmaxuploads" name="gmaxuploads" value="0" style="width:40px;"/> KiB/sec</td>
		</tr>

		<tr>
			<td>Max connections</td>
			<td><input type="text" id="gmaxconn" name="gmaxconn" value="0" style="width:30px;"/> KiB/sec</td>
		</tr>

		<tr>
			<td><input type="submit" id="glimit_cancel_button" onClick="cancelGlobalLimits(); return false;" value="Cancel"/></td>
			<td><input type="submit" id="glimit_set_button" onClick="setGlobalLimits(); return false;" value="Set"/></td>
		</tr>
		</form>
		</table>
	</div>

	<div id="layer_contexts">
		<table id="torrent_table" cellspacing=0>
			<tr>
				<th class="column_torrent">Torrent</th>
				<th class="column_status">Status</th>
				<th class="column_control">Control</th>
			</tr>
		</table>
	</div>

	<div id="footer">
			<span id="about">
				<a href="http://btg.berlios.de/" target="_blank">webBTG version 1.1</a>
			</span>
	</div>
</div>

<div id="bottom"></div>
<div id="network_status">
	<pre id="statusMessage">Loading...</pre>
	<input id="refresh_button" class="control_buttons" type="button" name="update" onClick="refreshContextList();" value="Refresh now"/>
	<h2 id="status_download">Total download speed is: 0b/s</h2>
	<h2 id="status_upload">Total upload speed is: 0b/s</h2>
</div>

</body>
</html>

</body>
</html>


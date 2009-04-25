<?php
error_reporting(E_ALL);
header("Expires: Thu, 19 Nov 1981 08:52:00 GMT");
header("Cache-Control: no-store, no-cache, must-revalidate, post-check=0, pre-check=0"); 
header("Pragma: no-cache");
// Check PHP version, we require at least PHP 5
if(substr(phpversion(), 0,1) < 5)
	die($lang['php_die']);

// make sure we can find version.php
@include("version.php");

if(!defined("BTG_BUILD"))
{
	die($lang['version_missing']);
}

session_start();
if (isset($_COOKIE['language'])) {
require_once("languages/".$_COOKIE['language'].".php");
}else{
require_once("languages/en.php");
}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<meta content="text/html; charset=UTF-8" http-equiv="Content-Type">
<meta http-equiv="Content-Language" content="en-us" />
<meta name="ROBOTS" content="NONE" />
<style type="text/css" media="screen">@import "btg.css";</style>
<?

echo"<script language='javascript' src='languages/js/";
if (isset($_COOKIE['language'])) { 
echo $_COOKIE['language'].".js'";
} else {
echo "en.js'";
}
echo"type='text/javascript'></script>";
echo'<script language="javascript" src="btg.js" type="text/javascript"></script>';
?>
<script language="javascript" src="ajaxjs.php" type="text/javascript"></script>
<script language="javascript" src="csshover.js" type="text/javascript"></script>
<title>BTG <?php
   print BTG_VERSION;
if (BTG_REV != "")
   {
      print " (";
      print BTG_REV;
      print ")";
   }
?></title>
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
	<iframe name="upload_iframe" id="upload_iframe" style="width: 100px; height: 10px; display: none;" onLoad="uploadDone()"></iframe> 
	<div id="upload">
		<form action="upload.php" target="upload_iframe" method="post" enctype="multipart/form-data" onSubmit="return uploadCheck();">
		<input type="hidden" name="MAX_FILE_SIZE" value="1000000" />
		<input class="btn" type="file" id="upload_input" name="torrent_file"/>
		<input id="upload_button" type="submit" value="<?=$lang['upload']?>"/>
		</form>
		<div id="loadurl">
			<input class="textinput" type="text" id="loadurl_input"/>
			<input id="loadurl_button" type="button" value="<?=$lang['load_url']?>" onClick="loadUrl()"/>
		</div>
		<div id="loadurl_status" onClick="resetUrl()">
		</div>
	</div>

	<input id="logout_button" class="control_buttons" type="button" onClick="deauth();" value="<?=$lang['logout']?>"/>
	<input id="detach_button" class="control_buttons" type="button" onClick="sessionDetach();" value="<?=$lang['detach']?>"/>
	<input id="quit_button" class="control_buttons" type="button" onClick="sessionQuit();" value="<?=$lang['quit']?>"/>
	<input id="collapse_button" class="control_buttons" type="button" onClick="hideAllContextDetails();" value="<?=$lang['hide']?>"/>
	<input id="cleanall_button" class="control_buttons" type="button" onClick="cleanAllContexts();" value="<?=$lang['clean']?>"/>
	<input id="glimit_button" class="control_buttons" type="button" onClick="showGlobalLimits();" value="<?=$lang['global_limit']?>"/>
	<input id="sesname_button" class="control_buttons" type="button" onClick="showSessionName();" value="<?=$lang['session_name']?>"/>
	<div id="layer_fatal">
	<?=$lang['fatal_error']?>
	</div>

	<div id="layer_auth">
		<h2><?=$lang['welcome']?></h2>
		<p><?=$lang['login_details']?></p>
		<form action="" name="frm_auth">
		<input type="text" id="username" name="username" style="width:100px"/>
		<input type="password" id="password" name="password" style="width:100px"/>
		<input type="submit" id="login_button" onClick="auth(); return false;" value="<?=$lang['login']?>"/>
		</form><br/>
<?
echo $lang['language']."<br/><br/>";
$langdir=scandir("languages/");
foreach ($langdir as $langu) { 
if ($langu!="." AND $langu!=".." AND is_file("languages/".$langu)) {
$lang_name=explode(".",$langu);
echo "<img style='cursor: pointer;' title='".$lang_name[0]."' rel='".$lang_name[0]."' onClick='select_language(\"".$lang_name[0]."\"); return false;' src='images/".$lang_name[0].".gif'>";
}else {}
}

?>
	</div>

	<div id="layer_sessions">
		<h2><?=$lang['welcome']?></h2>
		<h3><?=$lang['session_attach']?></h3>
		<p><?=$lang['session_attach2']?></p>
		<form action="" name="frm_sessionlist">
		<select id="sessionlist" name="sessionlist" style="width:100px"></select>
		<input type="submit" id="attach_button" onClick="sessionAttach(); return false;" value="<?=$lang['attach']?>"/>
		</form>

		<h3><?=$lang['setup_session']?></h3>
		<p><?=$lang['setup_session2']?></p>
		<form action="" name="frm_sessionsetup">
		<?=$lang['encryption']?> <input type="checkbox" id="session_enc" name="session_enc" value="1"/>, 
		<?=$lang['dht']?> <input type="checkbox" id="session_dht" name="session_dht" value="1"/>, 
		<?=$lang['seed_limit']?> <input type="text" id="seedLimit" name="seedLimit" value="" style="width:30px;"/>%,
		<?=$lang['seed_timeout']?> <input type="text" id="seedTimeout" name="seedTimeout" value="" style="width:40px;"/> perc
		<input type="submit" id="setup_button" onClick="sessionSetup(); return false;" value="<?=$lang['setup']?>"/>
		</form>
	</div>
	
	<div id="layer_glimit">
		<form action="" name="frm_glimit">
		<table id="glimit_table" cellspacing=0>

		<tr>
			<th class="column_glimit"><?=$lang['limit_name']?></th>
			<th class="column_glimit"><?=$lang['limit_value']?></th>
		</tr>

		<tr>
			<td><?=$lang['upload']?></td>
			<td><input type="text" id="gupload" name="gupload" value="0" style="width:30px;"/> KiB/sec</td>
		</tr>
		<tr>
			<td><?=$lang['download']?></td>
			<td><input type="text" id="gdownload" name="gdownload" value="0" style="width:40px;"/> KiB/sec</td>
		</tr>
		<tr>
			<td><?=$lang['max_uploads']?></td>
			<td><input type="text" id="gmaxuploads" name="gmaxuploads" value="0" style="width:40px;"/><?=$lang['uploads']?></td>
		</tr>

		<tr>
			<td><?=$lang['max_connections']?></td>
			<td><input type="text" id="gmaxconn" name="gmaxconn" value="0" style="width:30px;"/><?=$lang['connections']?></td>
		</tr>

		<tr>
			<td><input type="submit" id="glimit_cancel_button" onClick="cancelGlobalLimits(); return false;" value="<?=$lang['cancel']?>"/></td>
			<td><input type="submit" id="glimit_set_button" onClick="setGlobalLimits(); return false;" value="<?=$lang['set']?>"/></td>
		</tr>
		</form>
		</table>
	</div>

	<div id="layer_sesname">
		<form action="" name="frm_sesname">
		<table id="sesname_table" cellspacing=0>

		<tr>
			<td><?=$lang['session_name']?></td>
			<td><input type="text" id="session_name" name="sname" value="session name" style="width:130px;"/></td>
		</tr>

		<tr>
			<td><input type="submit" id="sesname_cancel_button" onClick="cancelSessionName(); return false;" value="<?=$lang['cancel']?>"/></td>
			<td><input type="submit" id="sesname_set_button" onClick="setSessionName(); return false;" value="<?=$lang['set']?>"/></td>
		</tr>
		</form>
		</table>
	</div>

	<div id="layer_contexts">
		<table id="torrent_table" cellspacing=0>
			<tr>
				<th class="column_torrent"><?=$lang['torrent']?></th>
				<th class="column_status"><?=$lang['status']?></th>
				<th class="column_control"><?=$lang['control']?></th>
			</tr>
		</table>
	</div>

	<div id="footer">
			<span id="about">
				<a href="http://btg.berlios.de/" target="_blank">webBTG 1.1 <?=$lang['version']?></a>
			</span>
	</div>
</div>

<div id="bottom"></div>
<div id="network_status">
	<pre id="statusMessage">Töltés...</pre>
	<input id="refresh_button" class="control_buttons" type="button" name="update" onClick="refreshContextList();" value="<?=$lang['refresh']?>"/>
	<input id="stop_refresh" class="control_buttons" type="button" name="stop_refresh" onClick="changeUpdateMode();" value="<?=$lang['refresh_stop']?>"/>
	<h2 id="status_download"><?=$lang['total_d_speed']?></h2>
	<h2 id="status_upload"><?=$lang['total_u_speed']?></h2>
</div>

</body>
</html>

</body>
</html>


<!--
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
 * $Id: btg.js,v 1.1.2.53 2007/08/02 19:49:21 wojci Exp $
 */

/**************************************************
 * Global variables
 **************************************************/

/* This list contains all Status() objects presented in the context
 * table. Is used from updateContextTable 
*/

var contextList = new Array();

/* Indicates if we should autoupdate contexts */
var doAutoRefresh = 0;

/* If window is focused or blurred */
var isBlurred = 0;

/* Defines if there are no "custom" statusmessages show */
var isStatusIdle = 0;

/* Context data age, how many seconds since we last got an update of the contexts */
var contextsAge = 0;

/* Refresh timer */
var refreshTimeout = 20;

var timerHandle = null;

// Constant.
var noLimit     = -1;

// Constant.
var bytesPerKiB = 1024;

// Indicates that automatic updates are stopped.
var updatesStopped = 0;

/**************************************************
 * Functions called from user interface in some 
 * way or another (either via user action or other
 * events).
 **************************************************/

/* sessionList, list existing sessions */
function sessionList()
{
	setStatus("Listing available sessions...");
	btg_sessionList(cb_sessionList, cb_sessionList_err);
}

/* auth, authorize  */
function auth()
{
    var username = encodeURIComponent(document.frm_auth.username.value);
    var password = encodeURIComponent(document.frm_auth.password.value);
    document.frm_auth.username.value = "";
    document.frm_auth.password.value = "";
    setStatus("Logging in " + username + "...");
    btg_auth(cb_auth, cb_auth_err, username, password);
}

/* deauth, deauthorize (logout) */
function deauth()
{
	setStatus("Logging out...");
	btg_deauth(cb_deauth, cb_deauth);
}

/* sessionAttach, attach to a existing session */
function sessionAttach()
{
	var id = document.frm_sessionlist.sessionlist.options[document.frm_sessionlist.sessionlist.selectedIndex].value;
	setStatus("Attaching to session " + id + "...");
	btg_sessionAttach(cb_sessionAttach, cb_sessionAttach_err, id);
}

/* sessionSetup, setup a new session */
function sessionSetup()
{
	var seedLimit   = document.frm_sessionsetup.seedLimit.value;
	var seedTimeout = document.frm_sessionsetup.seedTimeout.value;
	var useEncryption  = false;
	var useDHT         = false; 

	if (document.frm_sessionsetup.session_enc.checked)
		useEncryption = true;

	if (document.frm_sessionsetup.session_dht.checked)
		useDHT = true;

	if (seedLimit.length == 0)
		seedLimit = noLimit;

	if (seedTimeout.length == 0)
		seedTimeout = noLimit;
	else
		// User enters minutes...
		seedTimeout=seedTimeout*60;

	setStatus("Setting up new session...");
	btg_sessionSetup(cb_sessionSetup, 
			 cb_sessionSetup_err, 
			 seedLimit, 
			 seedTimeout,
			 useDHT,
			 useEncryption);
}

/* sessionDetach, detach from the current session */
function sessionDetach()
{
	setStatus("Detaching from session...");
	btg_sessionDetach(cb_sessionDetach, cb_sessionDetach_err);
}

/* sessionQuit, quit the currently attached session */
function sessionQuit()
{
	if(!confirm("Are you realy sure you want to terminate this session and all torrents downloading in it?"))
		return;
	
	setStatus("Terminating session...");
	btg_sessionQuit(cb_sessionQuit, cb_sessionQuit_err);
}

function cleanAllContexts()
{
	setStatus("Cleaning session...");
	btg_cleanAll(cb_cleanAll, cb_cleanAll_err);
}

function showGlobalLimits()
{
	setUIState(4);
	setStatus("Setting global limits.");

	btg_globallimitstatus(cb_globallimitstatus, cb_globallimitstatus_err);
}

function cancelGlobalLimits()
{
	setStatus("Not setting global limits.");
	setUIState(3);
}

function setGlobalLimits()
{
	var eUl = document.getElementById('gupload');
	var eDl = document.getElementById('gdownload');
	var eMu = document.getElementById('gmaxuploads');
	var eMc = document.getElementById('gmaxconn');

	var ul, dl, mu, mc;

	if (eUl.value != "" && parseInt(eUl.value) > 0)	
	{
		ul = parseInt(eUl.value)*bytesPerKiB;
	}
	else 
	{
		ul = noLimit;
	}

	if(eDl.value != "" && parseInt(eDl.value) > 0)
	{
		dl = parseInt(eDl.value)*bytesPerKiB;
	}
	else
	{
		dl = noLimit;
	}

	if(eMu.value != "" && parseInt(eMu.value) > 0)
	{
		mu = parseInt(eMu.value);
	}
	else
	{
		mu = noLimit;
	}

	if(eMc.value != "" && parseInt(eMc.value) > 0)
	{
		mc = parseInt(eMc.value);
	}
	else
	{
		mc = noLimit;
	}

	// Set the limits:
	btg_globallimit(cb_globallimit, cb_globallimit_err, ul, dl, mu, mc);
}

function showSessionName()
{
	btg_sessionName(cb_updateSessionNameField, cb_updateSessionNameField_err);

	setUIState(5);
	setStatus("Setting session name.");
}

function cancelSessionName()
{
	setUIState(3);
	setStatus("Setting session name cancelled.");
}

function setSessionName()
{
	var sessionname = document.getElementById('session_name');

	btg_setSessionName(cb_setSessionName, cb_setSessionName_err, sessionname.value);

	setUIState(3);

	updateSessionName();
}

/* contextStart, start a specific torrent */
function contextStop(id)
{
	setStatus("Stopping torrent...");
	btg_contextStop(cb_contextStop, cb_contextStop_err, id);
}

/* contextStop, stop a specific torrent */
function contextStart(id)
{
	setStatus("Starting torrent...");
	btg_contextStart(cb_contextStart, cb_contextStart_err, id);
}

/* contextAbort, remove a specific torrent */
function contextAbort(id)
{
	if(!confirm("Are you sure?"))
		return;

	var eraseData = 0;
	if(confirm("Do you want to remove the downloaded data from disk?"))
		eraseData = 1;

	setStatus("Deleting torrent...");
	btg_contextAbort(cb_contextAbort, cb_contextAbort_err, id, eraseData);
}

/* contextClean, clean/remove a specific torrent */
function contextClean(id)
{
	if(!confirm("Are you sure?"))
		return;
	setStatus("Moving torrent...");
	btg_contextClean(cb_contextClean, cb_contextClean_err, id);
}

/* contextPeers, get peer list for a torrent */
function contextPeers(id)
{
	setStatus("Getting peers list...");
	btg_contextPeers(cb_contextPeers, cb_contextPeers_err, id);
}

/* contextLimitStatus, get limits for a specific torrent (or a list of torrents, comma delimited) */
function contextLimitStatus(id)
{
	setStatus("Getting limit status...");
	btg_contextLimitStatus(cb_contextLimitStatus, cb_contextLimitStatus_err, id);
}

/* toogleContextDetails, toogles the details info for a specific torrent on/off */
function toogleContextDetails(id)
{
	var detailsRow= document.getElementById('context_'+id+'_details_row')
	var controlRow= document.getElementById('context_'+id+'_control_row')
	if(detailsRow.style.display == '')
	{
		detailsRow.style.display='none';
		controlRow.style.display='none';
	}
	else
	{
		detailsRow.style.display='';
		controlRow.style.display='';
	}
}

/* hideAllContextDetails, hides details info for all torrents */
function hideAllContextDetails()
{
	var table = document.getElementById('torrent_table');
	for(var x = 0; x < table.rows.length; x++)
	{
		var row = table.rows[x];
		if(row.id.match(/context_(\d+)_details_row/) || row.id.match(/context_(\d+)_control_row/))
			row.style.display='none';
	}
}

/* saveContextLimit, save limits for a specific context */
function saveContextLimit(id)
{
	var eUl = document.getElementById('txtUploadLimit_'+id);
	var eDl = document.getElementById('txtDownloadLimit_'+id);
	var eSl = document.getElementById('txtSeedLimit_'+id);
	var eSt = document.getElementById('txtSeedTimeout_'+id);

	var ul, dl, sl, st;
	if(eUl.value != "" && parseInt(eUl.value) > 0)	ul = parseInt(eUl.value)*bytesPerKiB;	else ul = noLimit;
	if(eDl.value != "" && parseInt(eDl.value) > 0)	dl = parseInt(eDl.value)*bytesPerKiB;	else dl = noLimit;
	if(eSl.value != "" && parseInt(eSl.value) > 0)	sl = parseInt(eSl.value);	else sl = noLimit;
	if(eSt.value != "" && parseInt(eSt.value) > 0)	st = parseInt(eSt.value)*60;	else st = noLimit;

	btg_contextLimit(cb_contextLimit, cb_contextLimit_err, id, ul, dl, sl, st, 0);
}

/* uploadCheck, called from upload form's onSubmit */
function uploadCheck()
{
	var file = document.getElementById('upload_input');
	if(file.value == '')
	{
		alert('Please select a file.');
		return false;
	}

	/* Make sure file ends with .torrent */
	if(!file.value.match(/\.torrent$/))
	{
		alert('Only .torrent files allowed.');
		return false;
	}

	/* Valid file */
	setStatus('Uploading torrent...');
	return true;
}

/* Init, called from BODYs onLoad */
function init(is_authed, session)
{
	/* Call our timer function (direct now but later once a second) */
	timerHandle = setTimeout(timer, 1);

	if(is_authed == 0)
	{
		setUIState(1);
		setStatus("Waiting for login.");
	}
	else if(session == -1)
	{
		/* Authed, but no session attached, display session list */
		sessionList();
		setUIState(2);
	}else
	{
		/* Authed and attached */
		setStatus("Loading context list...");
		doAutoRefresh = 1;
		refreshContextList();
		setUIState(3);
	}

	/* Everything should be loaded now, show main container */
	document.getElementById('container').style.display='block';
	
	/* Setup handlers so we dont run when we're not focused */
	window.onblur = function(){ isBlurred = true; };
	window.onfocus = function(){ isBlurred = false; timer(); };
}

/**
 * Updates the UI state.
 * @param state 	Value 0 indicates state not known/daemon dead, dont show any controls.
 *						Value 1 indicates not authed, show auth controls.
 *						Value 2 indicates authed but not attached to any session, show attach and create.
 *						Value 3 indicates attached, show context list etc.
 */
function setUIState(state)
{
	if(state == 0)
	// State unknown/daemon unrechable
	{
		document.getElementById('layer_sessions').style.display='none';
		document.getElementById('layer_auth').style.display='none';
		document.getElementById('attach_button').style.display='none';
		document.getElementById('setup_button').style.display='none';
		document.getElementById('quit_button').style.display='none';
		document.getElementById('refresh_button').style.display='none';
		document.getElementById('collapse_button').style.display='none';
		document.getElementById('cleanall_button').style.display='none';
		document.getElementById('layer_glimit').style.display='none';
		document.getElementById('glimit_button').style.display='none';
		document.getElementById('sesname_button').style.display='none';

		document.getElementById('glimit_cancel_button').style.display='none';
		document.getElementById('glimit_set_button').style.display='none';
		document.getElementById('detach_button').style.display='none';
		document.getElementById('logout_button').style.display='none';
		document.getElementById('upload').style.display='none';
		document.getElementById('layer_contexts').style.display='none';
		document.getElementById('status_download').style.display='none';
		document.getElementById('status_upload').style.display='none';
		document.getElementById('layer_sesname').style.display='none';
		doAutoRefresh = 0;
	}
	else if(state == 1)
	// Not authorized, show authorize controls.
	{
		document.getElementById('layer_auth').style.display='block';
		document.getElementById('layer_sessions').style.display='none';
		document.getElementById('attach_button').style.display='none';
		document.getElementById('setup_button').style.display='none';
		document.getElementById('quit_button').style.display='none';
		document.getElementById('refresh_button').style.display='none';
		document.getElementById('collapse_button').style.display='none';
		document.getElementById('cleanall_button').style.display='none';
		document.getElementById('layer_glimit').style.display='none';
		document.getElementById('glimit_button').style.display='none';
		document.getElementById('sesname_button').style.display='none';
		document.getElementById('glimit_cancel_button').style.display='none';
		document.getElementById('glimit_set_button').style.display='none';
		document.getElementById('detach_button').style.display='none';
		document.getElementById('logout_button').style.display='none';
		document.getElementById('upload').style.display='none';
		document.getElementById('layer_contexts').style.display='none';
		document.getElementById('status_download').style.display='none';
		document.getElementById('status_upload').style.display='none';
		document.getElementById('layer_sesname').style.display='none';
		doAutoRefresh = 0;
	}
	else if(state == 2)
	// Authorized, not attached to any session. Show attach and setup controls.
	{
		document.getElementById('layer_auth').style.display='none';
		document.getElementById('quit_button').style.display='none';
		document.getElementById('refresh_button').style.display='none';
		document.getElementById('collapse_button').style.display='none';
		document.getElementById('cleanall_button').style.display='none';
		document.getElementById('layer_glimit').style.display='none';
		document.getElementById('glimit_button').style.display='none';
		document.getElementById('sesname_button').style.display='none';
		document.getElementById('glimit_cancel_button').style.display='none';
		document.getElementById('glimit_set_button').style.display='none';
		document.getElementById('detach_button').style.display='none';
		document.getElementById('upload').style.display='none';
		document.getElementById('layer_contexts').style.display='none';
		document.getElementById('status_download').style.display='none';
		document.getElementById('status_upload').style.display='none';

		document.getElementById('layer_sessions').style.display='block';
		document.getElementById('attach_button').style.display='inline';
		document.getElementById('setup_button').style.display='inline';
		document.getElementById('logout_button').style.display='inline';
		document.getElementById('layer_sesname').style.display='none';
		doAutoRefresh = 0;
	}
	else if(state == 3)
	// Authorized, attached to a session. Show context list and controls. 
	{
		document.getElementById('layer_sessions').style.display='none';
		document.getElementById('attach_button').style.display='none';
		document.getElementById('setup_button').style.display='none';
		document.getElementById('refresh_button').style.display='block';
		document.getElementById('quit_button').style.display='inline';
		document.getElementById('collapse_button').style.display='inline';
		document.getElementById('cleanall_button').style.display='inline';
		document.getElementById('layer_glimit').style.display='none';
		document.getElementById('glimit_button').style.display='inline';
		document.getElementById('sesname_button').style.display='inline';
		document.getElementById('glimit_cancel_button').style.display='none';
		document.getElementById('glimit_set_button').style.display='none';
		document.getElementById('detach_button').style.display='inline';
		document.getElementById('logout_button').style.display='inline';
		document.getElementById('upload').style.display='block';
		document.getElementById('layer_contexts').style.display='block';
		document.getElementById('status_download').style.display='inline';
		document.getElementById('status_upload').style.display='inline';
		document.getElementById('layer_sesname').style.display='none';
		doAutoRefresh = 1;
	}
	else if(state == 4)
	// Authorized, Set global limits.
	{
		document.getElementById('layer_sessions').style.display='none';
		document.getElementById('attach_button').style.display='none';
		document.getElementById('setup_button').style.display='none';
		document.getElementById('refresh_button').style.display='none';
		document.getElementById('quit_button').style.display='inline';
		document.getElementById('collapse_button').style.display='none';
		document.getElementById('cleanall_button').style.display='none';
		document.getElementById('layer_glimit').style.display='block';
		document.getElementById('glimit_button').style.display='none';
		document.getElementById('sesname_button').style.display='none';
		document.getElementById('glimit_cancel_button').style.display='inline';
		document.getElementById('glimit_set_button').style.display='inline';
		document.getElementById('detach_button').style.display='inline';
		document.getElementById('logout_button').style.display='inline';
		document.getElementById('upload').style.display='none';
		document.getElementById('layer_contexts').style.display='none';
		document.getElementById('status_download').style.display='none';
		document.getElementById('status_upload').style.display='none';
		document.getElementById('layer_sesname').style.display='none';
		doAutoRefresh = 0;
	}
	else if(state == 5)
	    {
		document.getElementById('layer_sessions').style.display='none';
		document.getElementById('attach_button').style.display='none';
		document.getElementById('setup_button').style.display='none';
		document.getElementById('refresh_button').style.display='none';
		document.getElementById('quit_button').style.display='inline';
		document.getElementById('collapse_button').style.display='none';
		document.getElementById('cleanall_button').style.display='none';
		document.getElementById('layer_glimit').style.display='none';
		document.getElementById('glimit_button').style.display='none';
		document.getElementById('sesname_button').style.display='none';
		document.getElementById('glimit_cancel_button').style.display='none';
		document.getElementById('glimit_set_button').style.display='none';
		document.getElementById('detach_button').style.display='inline';
		document.getElementById('logout_button').style.display='inline';
		document.getElementById('upload').style.display='none';
		document.getElementById('layer_contexts').style.display='none';
		document.getElementById('status_download').style.display='none';
		document.getElementById('status_upload').style.display='none';
		document.getElementById('layer_sesname').style.display='inline';
		doAutoRefresh = 0;
	    }
}

/* Called when the upload iframe has been loaded, either on first page load
 * or after an upload is complete. 
 */
function uploadDone()
{
	var file = document.getElementById('upload_input');
	var content = document.getElementById('upload_iframe').contentDocument;

	/* We only care about the contents if its a XMLDocument, not on default empty page load */
	if(content == null)
		return;
		
	/* This breaks on safari.... However, safari is fuckedup anyways since it doesnt run onLoad on the iframe
	when its reloaded... Opera does not know about XMLDocument either though... */
	if(!(content instanceof XMLDocument))
		return;

	/* Fetch the response object from the DOM */
	var response = content.getElementsByTagName('response')[0];
	if(!response)
	{
		alert("Invalid response from server, missing response. Upload failed.");
		return;
	}

	/* Check if any errors occured */
	var error = response.getElementsByTagName('error');
	if(error.length > 0)
	{
		var errStr = ""
		for(var i=0; i < error.length; i++)
			errStr+=error[i].childNodes[0].nodeValue+"\n";

		setStatus(errStr);
		file.value = '';
		return;
	}

	/* No errors occured, torrents should be uploaded and running. Erase file field and update the context list */
	file.value = '';
	refreshContextList();
}

/**
 * This function is called every second, is used for misc stuff like refresh, age counter etc.
 */
function timer()
{
	if(doAutoRefresh && isStatusIdle)
	{
		contextsAge++;

		var diff = refreshTimeout - contextsAge;

		if(isBlurred)
			document.getElementById('statusMessage').innerHTML = 'Window not on focus, wont update...';
		else if(diff <= 0)
		{
			if (updatesStopped == 0)
				refreshContextList();
		}
		else if(diff > 0)
		    {
			if (updatesStopped == 0)
				document.getElementById('statusMessage').innerHTML = 'Update in ' + (refreshTimeout - contextsAge)  + ' seconds.';
		    }
	}
	else
	{
		contextsAge = 0; 
	}

	clearTimeout(timerHandle);
	timerHandle = setTimeout(timer, 1000);
}

/**************************************************
 * Callback functions for the remote calls.
 **************************************************/

function cb_setSessionName(response)
{
    setStatus("Session name set.");
}

function cb_setSessionName_err(error, errStr)
{
	 setStatus("Session name not set: " + errStr);
}

function cb_globallimit(response)
{
	setStatus("Global limits set.");
	setUIState(3);
}

function cb_globallimit_err(error, errStr)
{
	setStatus("Unable to set global limits.");
	setUIState(3);
}

function cb_globallimitstatus(response)
{
	var r_ull  = parseInt(getFirstChildValue(response, 'uploadLimit'));
	if (r_ull != noLimit)
	{
		ri_ull = parseInt(r_ull / bytesPerKiB);
	}
	else
	{
		ri_ull = noLimit;
	}

	var r_dll  = parseInt(getFirstChildValue(response, 'downloadLimit'));
	if (r_dll != noLimit)
	{
		ri_dll = parseInt(r_dll / bytesPerKiB);
	}
	else
	{
		ri_dll = noLimit;
	}

	var r_mull = parseInt(getFirstChildValue(response, 'maxuploads'));
	var r_mc   = parseInt(getFirstChildValue(response, 'maxconnections'));

	// Update the limit table.
	var eUl = document.getElementById('gupload');
	var eDl = document.getElementById('gdownload');
	var eMu = document.getElementById('gmaxuploads');
	var eMc = document.getElementById('gmaxconn');

	eUl.value = ri_ull;
	eDl.value = ri_dll;
	eMu.value = r_mull;
	eMc.value = r_mc;
}

function cb_globallimitstatus_err(error, errStr)
{
	setStatus("Unable to get global limit status...");
}

/**
 * Callback for btg_auth.
 * Called when a user has been authed successfully.
 */
function cb_auth(response)
{
	setStatus("");
	setUIState(2);
	sessionList();
}

/**
 * Error Callback for btg_auth.
 * Called when the auth failed.
 */
function cb_auth_err(error, errStr)
{
	setError(error, 'Failed to login: ' + errStr);
	document.frm_auth.username.focus();
}

/**
 * Callback and error callback for btg_deauth.
 * Called when a user has been logged out, both success and failure...
 */
function cb_deauth(response)
{
	setStatus("Logged out.");
	setUIState(1);
}

/**
 * Callback for btg_sessionList.
 * Called when a successfull list of sessions was retrieved. Display these in the listbox.
 */
function cb_sessionList(response)
{
	var sessions = response.getElementsByTagName('sessions')[0].getElementsByTagName('session');
	var names    = response.getElementsByTagName('sessions')[0].getElementsByTagName('name');
	var list = document.getElementById('sessionlist');
	while(list.hasChildNodes())
		list.removeChild(list.childNodes[0]);
	for(var i=0; i < sessions.length; i++)
	{
		var session = sessions[i].childNodes[0].nodeValue;
		var name    = names[i].childNodes[0].nodeValue;
		list.options[i] = new Option(session + ":" + name, session);
	}
	setStatus("");
}

/**
 * Error Callback for btg_sessionList.
 * Called when we failed to get a list of sessions, for example, when 
 * there are no sessions (?) or when daemon is down.
 */
function cb_sessionList_err(error, errStr)
{
	var list = document.getElementById('sessionlist');
	while(list.hasChildNodes())
		list.removeChild(list.childNodes[0]);
	setError(error, 'Could not list sessions: ' + errStr);
}

/**
 * Callback for btg_sessionAttach.
 * Called when a session was successfully attached.
 */
function cb_sessionAttach(response)
{
	setUIState(3);
	refreshContextList();
	// Fix: calling this function right after or before updating
	// the contexts does not work. Hence the 1 second delay.
	setTimeout('updateSessionName();', 1000);
}

/**
 * Error Callback for btg_sessionAttach.
 * Called when we failed to attach to a session.
 */
function cb_sessionAttach_err(error, errStr)
{
	setError(error, 'Failed to attach to session: ' + errStr);
}

/**
 * Callback for btg_sessionSetup.
 * Called when a new session was successfully created.
 */
function cb_sessionSetup(response)
{
	setUIState(3);
	refreshContextList();
}

/**
 * Error Callback for btg_sessionSetup.
 * Called when we failed to create a session.
 */
function cb_sessionSetup_err(error, errStr)
{
	setError(error, 'Failed to create new session: ' + errStr);
}

/**
 * Callback for btg_sessionDetach.
 * Called when a session was successfully detached.
 */
function cb_sessionDetach(response)
{
	setUIState(2);
	clearContextList();
	sessionList();
}

/**
 * Error Callback for btg_sessionDetach.
 * Called when we failed to detach to a session.
 */
function cb_sessionDetach_err(error, errStr)
{
	setError(error, 'Failed to detach from session: ' + errStr);
}

/**
 * Callback for btg_sessionQuit.
 * Called when a session was successfully terminated.
 */
function cb_sessionQuit(response)
{
	setUIState(2);
	clearContextList();
	sessionList();
}

/**
 * Error Callback for btg_sessionQuit.
 * Called when we failed to terminate a session.
 */
function cb_sessionQuit_err(error, errStr)
{
	setError(error, 'Failed to terminate session: ' + errStr);
}

function cb_cleanAll(response)
{
	setStatus("Session cleaned...");
	refreshContextList();
}

function cb_cleanAll_err(error, errorStr)
{
	setError(error, 'Unable to clean session.');
}

/**
 * Callback for btg_contextStatus.
 * Called when a successfull status query was executed.
 */
function cb_contextStatus(response)
{
	setStatus("Parsing context response...");

	/* Fetch all contexts, create new Status objects for them (status.js) and let 
	 * updateContextTable do the work.
	 */
	var contexts = response.getElementsByTagName('contexts')[0];
	if(contexts == null)
	{
		setStatus("Server didnt provide any contexts!");
		return;
	}

	var newContextList = new Array();
	var strContexts = "";
	for(var i=0; i < contexts.getElementsByTagName('context').length; i++)
	{
		var s = new Status(contexts.getElementsByTagName('context')[i]);
		newContextList.push(s);
		if(strContexts == "")
			strContexts += s.contextID;
		else
			strContexts += ","+s.contextID;
	}

	updateContextTable(newContextList);
	contextsAge = 0;

	// Refresh limits
	if(strContexts != "")
		contextLimitStatus(strContexts);

	/* Updates done */
	setStatus("");
}

/**
 * Error Callback for btg_contextStatus.
 * Called when an error occured when context status was requested.
 */
function cb_contextStatus_err(error, errStr)
{
	setError(error, 'Failed to list contexts: '+errStr);
	canGetContexts = 0;
}

/*
function cb_contextTrackers(response)
{
	 var cid = getFirstChildValue(response, 'cid');
	 var trackername = getFirstChildValue(response, 'tracker');

	 var itemname = 'trackerfield_' + cid;
	 var item = document.getElementById(itemname);
	 item.innerHTML = "Tracker URL for " + cid;
	 item.innerHTML = trackername;

}

function cb_contextTrackers_err(error, errStr)
{
	 setStatus("Failed to get trackers for context: "+errStr);
}
*/

function cb_sessionName(response)
{
	 var sesnam = getFirstChildValue(response, 'sessionname');
	 var sesid  = getFirstChildValue(response, 'sessionid');
    document.title = "BTG (session " + sesid + ": " + sesnam + ")";
}

function cb_sessionName_err(error, errStr)
{
    document.title = "BTG";
}

function cb_updateSessionNameField(response)
{
	 document.getElementById('session_name').value = getFirstChildValue(response, 'sessionname');
}

function cb_updateSessionNameField_err(error, errStr)
{
	 document.getElementById('session_name').value = "Unknown";
}

/**
 * Callback for btg_contextLimitStatus.
 * Called when a successfull limit status query was executed.
 */
function cb_contextLimitStatus(response)
{
	var limits = response.getElementsByTagName('limits')[0];

	for(var i=0; i < limits.getElementsByTagName('limit').length; i++)
	{
		var l = new Limit(limits.getElementsByTagName('limit')[i]);
		var e = document.getElementById('txtUploadLimit_'+l.contextID);
		if(l.uploadLimit == noLimit)
			e.value = "";
		else
			e.value = l.uploadLimit/bytesPerKiB;
		e = document.getElementById('txtDownloadLimit_'+l.contextID);
		if(l.downloadLimit == noLimit)
			e.value = "";
		else
			e.value = l.downloadLimit/bytesPerKiB;
		e = document.getElementById('txtSeedLimit_'+l.contextID);
		if(l.seedLimit == noLimit)
			e.value = "";
		else
			e.value = l.seedLimit;
		e = document.getElementById('txtSeedTimeout_'+l.contextID);
		if(l.seedTimeout == noLimit)
			e.value = "";
		else
			e.value = l.seedTimeout/60;
	}
}

/**
 * Error Callback for btg_contextLimitStatus.
 * Called when an error occured when context status was requested.
 */
function cb_contextLimitStatus_err(error, errStr)
{
	setError(error, 'Failed to get limit status: '+errStr);
}

/**
 * Callback for btg_contextLimit.
 * Called when a successfull limit save was executed.
 */
function cb_contextLimit(response)
{
	setStatus("");
}

/**
 * Error Callback for btg_contextStatus.
 * Called when an error occured when context status was requested.
 */
function cb_contextLimit_err(error, errStr)
{
	setError(error, 'Failed to set limit: '+errStr);
}

/**
 * Callback for btg_contextStart.
 * Called when a torrent was successfully started.
 */
function cb_contextStart(response)
{
	refreshContextList();
}

/**
 * Error Callback for btg_contextStart.
 * Called when a start operation fails.
 */
function cb_contextStart_err(error, errStr)
{
	setError(error, "Failed to start torrent: "+ errStr);
}

/**
 * Callback for btg_contextStop.
 * Called when a torrent was successfully stopped.
 */
function cb_contextStop(response)
{
	refreshContextList();
}

/**
 * Error Callback for btg_contextStop.
 * Called when a stop operation fails.
 */
function cb_contextStop_err(error, errStr)
{
	setError(error, "Failed to stop torrent: "+ errStr);
}

/**
 * Callback for btg_contextPeers.
 * Called when a list of peers is sent.
 */
function cb_contextPeers(response)
{
	var peers = new Array();
	for(var i=0; i < contexts.getElementsByTagName('peers').length; i++)
	{
		var p = new Peer(contexts.getElementsByTagName('peer')[i]);
		peers.push(p);
	}
	if(peers.length == 0)
		setStatus("No peer information available.");
	else
		alert(peers);
}

/**
 * Error Callback for btg_contextPeers.
 * Called when a peer list request fails.
 */
function cb_contextPeers_err(error, errStr)
{
	setError(error, "Failed to get peers list: "+ errStr);
}

/**
 * Callback for btg_contextAbort.
 * Called when a torrent was successfully deleted.
 */
function cb_contextAbort(response)
{
	refreshContextList();
}

/**
 * Error Callback for btg_contextAbort.
 * Called when a abort operation fails.
 */
function cb_contextAbort_err(error, errStr)
{
	setError(error, "Failed to delete torrent: "+ errStr);
}

/**
 * Callback for btg_contextClean.
 * Called when a torrent was successfully deleted.
 */
function cb_contextClean(response)
{
	refreshContextList();
}

/**
 * Error Callback for btg_contextClean.
 * Called when a clean operation fails.
 */
function cb_contextClean_err(error, errStr)
{
	setError(error, "Failed to move torrent: "+ errStr);
}

/**************************************************
 * Misc shortcut functions.
 **************************************************/
/* Set the status message */
function setStatus(msg)
{
	isStatusIdle = 0;
	if(msg == '' || msg == null || msg == undefined)
	{
		isStatusIdle = 1;
		msg = "Idle.";
	}

	document.getElementById('statusMessage').innerHTML = msg;
}

/* Set an error message.
 * error should be error DOM from cb_._err. 
 * msg should be error to display.
 */
function setError(error, msg)
{
	for(var i=0; i < error.length; i++)
	{
		var code = error[i].getAttribute('code');
		if(code != null)
		{
			if(code == '1')
			{
				/* Failed to connect to BTG, or failed to init session.. This is fatal  */
				setUIState(0);
				document.getElementById('layer_fatal').innerHTML = "<h2>Fatal error</h2><p>" + error[i].childNodes[0].nodeValue + "</p>";
				document.getElementById('layer_fatal').style.display="block";
				setStatus('Cannot connect to BTG. Please refresh this page and try later.');
				return;
			}
			else if(code == '2')
			{
				/* Failed to authorize */
				setUIState(1);
				setStatus('Failed to authorize.');
				return;
			}
			else if(code == '3')
			{
				/* Failed to attach session, let user select new session */
				sessionList();
				setUIState(2);
				setStatus(error[i].childNodes[0].nodeValue);
				return;
			}
			else if(code == '4')
			{
				/* Failed to setup session */
				setUIState(2);
				setStatus(error[i].childNodes[0].nodeValue);
				return;
			}
		}
	}

	setStatus(msg);
}

/** Retreive a specific field value in DOM node */
function getFirstChildValue(node, field)
{
	var elem = node.getElementsByTagName(field);
	if(elem.length == 0)
		return "";
	if(elem[0].childNodes.length == 0)
		return "";
	else
		return elem[0].childNodes[0].nodeValue;
}

/** Round a number X to N decimals */
function RoundToNdp(X, N) {
	var T = Number("1e" + N);
	return Math.round(X * T) / T;
}

/** Shortcut to rounding val with 1 decimal */
function round1(val)
{
	return RoundToNdp(val,1);
}

/** Shortcut to rounding val with 2 decimals */
function round2(val)
{
	return RoundToNdp(val,2);
}

/** Converts size (in bytes) to a human readble figure */
function humanizeSize(size, precision)
{
	if(size < bytesPerKiB)
		return size + "b";

	size = size / bytesPerKiB;
	if(size < bytesPerKiB)
		return RoundToNdp(size, precision) + " kB";

	size = size / bytesPerKiB;
	if(size < bytesPerKiB)
		return RoundToNdp(size, precision) + " MB";
		
	size = size / bytesPerKiB;
	if(size < bytesPerKiB)
		return RoundToNdp(size, precision) + " GB";
}

/** Converts speed (in byte/s) to a human readble figure */
function humanizeSpeed(size, precision)
{
	return humanizeSize(size, precision) + "/s";
}

/* Refresh the context list */
function refreshContextList()
{
	if (updatesStopped == 1)
	{
		changeUpdateMode();
	}

	setStatus("Updating torrent list...");
	btg_contextStatus(cb_contextStatus, cb_contextStatus_err, -1, true);
}

function changeUpdateMode()
{
	if (updatesStopped == 0)
	{
	    updatesStopped = 1;
	    setStatus("Stopped automatic updates...");
	    document.getElementById('stop_refresh').value='Start updates';
	}
    else
	{
	    contextsAge    = 0;
	    updatesStopped = 0;
	    document.getElementById('stop_refresh').value='Stop updates';
	    setStatus();
	}
}

/* Update the name of the current session. */
function updateSessionName()
{
    btg_sessionName(cb_sessionName, cb_sessionName_err);
}

/** Handles updating of the visuals.
 * This is called from cb_contextStatus, the newList will be a list of fresh Status objects. Here
 * how it works:
 *
 * 1. Iteration over all items:
 * 	1. Item is added to a temporary list.
 * 	2. If the item exist in the previously initiated global contextList, it will be removed from there.
 * 	3. If a table row with this context ID exists (a previously known context), it will be updated.
 * 		Else a new row is added (a new context).
 *	2. When all items has been processed, any items left in contextList will be removed (those items 
 * 		did not exist in the new list, in other words they have been removed from the session) and their
 * 		row in the context table will be removed.
 * 3. The (now empty) contextList will be replaced with the newly created temporary list for use the next
 * 	this function is called.
 */
function updateContextTable(newList)
{
	var tempList = new Array();

	var totalUpRate = 0;
	var totalDownRate = 0;
	
	var table = document.getElementById('torrent_table');
	var evenOrUneven = 'uneven';
	/* Iterate over all new items */
	for(var i=0; i < newList.length; i++)
	{
		var s = newList[i];

		/* Add it to the new list */
		tempList.push(s);

		/* Remove it from the current list (isn't there any better way to do this???) */
		for(var y=0; y < contextList.length; y++)
		{
			if(s.compare(contextList[y]))
			{
				contextList.splice(y, 1);
				break;
			}
		}

		/* Determine the state of the torrent */
		if(s.status == ts_queued)
		{
			/* Some other torrent is checking its data and this torrent is in the wait queue */
			s.statusMessage = "Queued for check"
		}
		else if(s.status == ts_checking)
		{
			/* This torrent is currently checking its data */
			s.statusMessage = "Checking data (" + s.done + "%)"
		}
		else if(s.status == ts_connecting)
		{
			/* Connecting to tracker or peers  */
			s.statusMessage = "Connecting"
		}
		else if(s.status == ts_downloading)
		{
			/* This torrent is currently downloading data, not finished */
			s.statusMessage = "Downloading (" + humanizeSpeed(s.downloadrate,1) + "): "+ round1(s.done) +"% (" + s.timeleft + " left)"
		}
		else if(s.status == ts_seeding)
		{
			/* This torrent is fully downloaded and is seeding */
			s.statusMessage = "Seeding (" + humanizeSpeed(s.uploadrate,1) + "): "+ round1(s.done) + "%";
		}
		else if(s.status == ts_stopped)
		{
			/* User has stopped torrent, not finished */
			s.statusMessage = "Stopped ("+ round1(s.done) +"%)"
		}
		else if(s.status == ts_finished)
		{
			/* Torrent is fully downloaded and is stopped */
			s.statusMessage = "Finished ("+ round1(s.done)+"%)"
		}
		else
		{
			/* Undefined...? Use the specified statustext... */
			s.statusMessage = s.statustext
		}

		if(s.trackerstatus != 200 && s.trackerstatus != -1)
			s.statusMessage+='. Error '+s.trackerstatus + ': '+s.trackerstatustext;

		if(s.trackerstatusmessage != "")
		{
			s.statusMessage+='. '+s.trackerstatusmessage;
		}

		/* Check if we can find the row already (existing torrent) */
		var row = document.getElementById('context_'+s.contextID+'_row');
		var detailsTable = document.getElementById('context_'+s.contextID+'_details_table');
		var detailsRow = document.getElementById('context_'+s.contextID+'_details_row');
		var details = document.getElementById('context_'+s.contextID+'_details');

		/* Not found, create it */
		if(!row)
		{
			/* This torrent is new! Create a new row at index 0 */
			row = createTorrentTableRow(table, s);

			/* Create a hidden details row. Insert this as THIRD row! (headers, main row, info row..) */
			detailsRow = table.insertRow(2);
			detailsRow.className = "ctx_details";
			detailsRow.style.display = 'none'; 
			detailsRow.id = 'context_'+s.contextID+'_details_row';
			detailsRow.contextID = s.contextID;
			detailsRow.onclick = function(){ toogleContextDetails(this.contextID); }

			/* Create the details cell and insert a detailsTable in it */
			detailsCell = detailsRow.insertCell(-1);
			detailsCell.colSpan = 3;
			detailsCell.className = "ctx_details";
			
			details = createTorrentDetails();
			details.id = 'context_'+s.contextID+'_details';
			detailsCell.appendChild(details);

			var controlRow = table.insertRow(3);
			controlRow.className = "ctx_control";
			controlRow.style.display = 'none';
			controlRow.id = 'context_'+s.contextID+'_control_row';
			controlRow.contextID = s.contextID;
			var controlCell = controlRow.insertCell(-1);
			controlCell.colSpan = 3;
			controlCell.className = "ctx_control";

			var control = createTorrentExtendedControls(s.contextID);
			control.id = 'context_'+s.contextID+'_control';
			controlCell.appendChild(control);
			
		}

		if(evenOrUneven == 'even')
			evenOrUneven = 'uneven'
		else
			evenOrUneven = 'even'

		/* Update the data */
		updateTorrentTableRow(row, s);
		row.className=evenOrUneven+'_row';
		//detailsRow.className='extra_'+evenOrUneven+'_row';
		updateTorrentDetails(details, s)

		totalUpRate+= s.uploadrate;
		totalDownRate+= s.downloadrate;
	}

	/* Okay. All available items has been processed and removed from contextList if they
	 * where there before. Any contexts still there has been removed from the session.
	 * Delete all those rows.. */
	for(var i=0; i < contextList.length; i++)
	{
		var s = contextList[i];
		var row = document.getElementById('context_'+s.contextID+'_row');
		if(!row)
		{
			alert('Error! '+s.contextID+' was found in contextList on cleanup, but no table row is found!');
			continue;
		}

		table.deleteRow(row.rowIndex);
		var detailsRow = document.getElementById('context_'+s.contextID+'_details_row');
		table.deleteRow(detailsRow.rowIndex);
		var controlRow = document.getElementById('context_'+s.contextID+'_control_row');
		table.deleteRow(controlRow.rowIndex);
		delete s;
		delete contextList[i];
	}

	/* Done! Replace the contextList with the new list we created. */
	contextList = tempList;

	/* Refresh total counters */
	document.getElementById('status_download').innerHTML = 'Total download speed is: ' + humanizeSpeed(totalDownRate, 2);
	document.getElementById('status_upload').innerHTML = 'Total upload speed is: ' + humanizeSpeed(totalUpRate, 2);

	/* Call IE specific crap to get mouseover working */
	if(parseStylesheets)
		parseStylesheets();
}

/**
 * Clear the context list from all entrys. Done on detach.
 */
function clearContextList()
{
	var totalUpRate = 0;
	var totalDownRate = 0;
	
	var table = document.getElementById('torrent_table');
	/* Iterate over all items */
	for(var i=0; i < contextList.length; i++)
	{
		var s = contextList[i];
		var row = document.getElementById('context_'+s.contextID+'_row');
		if(!row)
		{
			alert('Error! '+s.contextID+' was found in contextList on cleanup, but no table row is found!');
			continue;
		}

		table.deleteRow(row.rowIndex);
		var detailsRow = document.getElementById('context_'+s.contextID+'_details_row');
		table.deleteRow(detailsRow.rowIndex);
		var controlRow = document.getElementById('context_'+s.contextID+'_control_row');
		table.deleteRow(controlRow.rowIndex);
		delete s;
		delete contextList[i];
	}
	contextList = new Array();

	/* Refresh total counters */
	document.getElementById('status_download').innerHTML = 'Total download speed is: ' + humanizeSpeed(totalDownRate, 2);
	document.getElementById('status_upload').innerHTML = 'Total upload speed is: ' + humanizeSpeed(totalUpRate, 2);
}

/**
 * Create a table row for a torrent.
 * @param t The table in which to create the row.
 * @param s The status object for this torrent.
 * @return A HTMLTableRowElement.
 */
function createTorrentTableRow(t, s)
{
	// Insert AFTER headers...
	var r = t.insertRow(1);
	r.contextID = s.contextID;
	r.id = 'context_'+s.contextID+'_row';

	/* Caller should change this */
	r.className='even_row';

	/* Filename cell */
	var c = r.insertCell(-1);
	c.contextID = r.contextID;
	c.onclick = function(){ toogleContextDetails(this.contextID); }
	c.className = 'column_torrent';
	
	/* Status bar cell */
	c = r.insertCell(-1);
	c.contextID = r.contextID;
	c.onclick = function(){ toogleContextDetails(this.contextID); }
	c.className = 'column_status';

	var d1 = document.createElement('div');
	d1.className = 'status_bar';

	var d2 = document.createElement('div');
	d2.style.width = '0%';

	var p = document.createElement('pre');

	d2.appendChild(p);
	d1.appendChild(d2);
	c.appendChild(d1);

	/* Controls cell */
	c = r.insertCell(-1);
	c.className = 'column_control';

	return r;
}

/**
 * Update a torrent info row created by createTorrentTableRow().
 * @param r The HTMLTableRowElement to be updated.
 * @param s The Status object containing the information to put in the table.
 */
function updateTorrentTableRow(r, s)
{
	// Strip .torrent extension
	var f = s.filename
	var rexp = f.match(/^(.*)\.torrent$/)
	if(rexp)
		f = rexp[1]

	// and then cut it if to long
	if(f.length > 60)
		f = f.substr(0,60) + '...';

	r.cells[0].innerHTML = f;
	r.cells[0].title = s.filename;

	if(s.done > 100)
		r.cells[1].childNodes[0].childNodes[0].style.width = '100%';
	else
		r.cells[1].childNodes[0].childNodes[0].style.width = ''+s.done+'%';

	if(s.status == ts_downloading)
		r.cells[1].childNodes[0].childNodes[0].className = 'downloading_bar';
	else if(s.status == ts_seeding)
		r.cells[1].childNodes[0].childNodes[0].className = 'seeding_bar';
	else if(s.status == ts_finished)
		r.cells[1].childNodes[0].childNodes[0].className = 'finished_bar';
	
	var stxt=r.cells[1].childNodes[0].childNodes[0].childNodes[0];
	if(s.statusMessage.length > 50)
		stxt.innerHTML = s.statusMessage.substr(0, 50)+"...";
	else
		stxt.innerHTML = s.statusMessage;

	// erase all controls
	var ctrl = r.cells[2];
	while(ctrl.childNodes.length > 0)
		ctrl.removeChild(ctrl.childNodes[0]);

	// and create new
	createTorrentControls(s, r.cells[2]);
}

/**
 * Creates a "button".
 */
function createButton(title)
{
	var b = document.createElement('span');
	b.className = 'button';
	b.innerHTML = title;
	return b;
}

/**
 * Create control buttons for a torrent.
 * @param s Status object with torrent info.
 * @param d A DOM object in which the controls will be placed.
 * @return A list of HTMLSpanElement's
 */
function createTorrentControls(s, d)
{
	/* Start/Stop button */
	if(s.status == ts_checking || s.status == ts_connecting || s.status == ts_downloading || s.status == ts_seeding)
	{
		// Add a stop button
		var b = createButton('Stop');
		b.contextID = s.contextID;
		b.onclick = function(){contextStop(this.contextID);}
		d.appendChild(b);
	}
	else if(s.status == ts_stopped || s.status == ts_finished)
	{
		// Add a start button
		var b = createButton('Start');
		b.contextID = s.contextID;
		b.onclick = function(){contextStart(this.contextID);}
		d.appendChild(b);
	}

	/* Abort/Clean button */
	if(s.status == ts_checking || s.status == ts_connecting || s.status == ts_downloading || s.status == ts_stopped)
	{
		// Add a abort button
		var b = createButton('Abort');
		b.contextID = s.contextID;
		b.onclick = function(){contextAbort(this.contextID);}
		d.appendChild(b);
	}
	else if(s.status == ts_seeding || s.status == ts_finished)
	{
		// Add a abort button
		var b = createButton('Abort');
		b.contextID = s.contextID;
		b.onclick = function(){contextAbort(this.contextID);}
		d.appendChild(b);
		// Add a clean button
		b = createButton('Clean');
		b.contextID = s.contextID;
		b.onclick = function(){contextClean(this.contextID);}
		d.appendChild(b);
	}
}

/**
 * Create a table for details.
 * @return A HTMLTableElement.
 */
function createTorrentDetails()
{
	var tbl = document.createElement('table');
	tbl.className='extrainfo';

	/* First row , Torrent filename, filesize and seeders */
	var r = tbl.insertRow(-1);
	r.className='extra_uneven_row';

	var c = r.insertCell(-1);
	c.className = 'extrainfo_type';
	c.innerHTML = "Status:";

	c = r.insertCell(-1);
	c.className = 'extrainfo_value';
	c.colSpan = 5;

   /* */
   r = tbl.insertRow(-1);
	r.className='extra_even_row';

   var c = r.insertCell(-1);
	c.className = 'extrainfo_type';
	c.innerHTML = "";

   c = r.insertCell(-1);
	c.className = 'extrainfo_value';
	c.colSpan = 5;
	c.innerHTML = "";

   /* */

	r = tbl.insertRow(-1);
	r.className='extra_uneven_row';

	c = r.insertCell(-1);
	c.className = 'extrainfo_type';
	c.innerHTML='Torrent Filename:';

	c = r.insertCell(-1);
	c.className = 'extrainfo_value';

	c = r.insertCell(-1);
	c.className = 'extrainfo_type extrainfo_divider';
	c.innerHTML='Filesize:';

	c = r.insertCell(-1);
	c.className = 'extrainfo_value';

	c = r.insertCell(-1);
	c.className = 'extrainfo_type extrainfo_divider';
	c.innerHTML='Seeders:';

	c = r.insertCell(-1);
	c.className = 'extrainfo_value';

	/* Second row , Uploaded, Upload speed, Leechers */
	r = tbl.insertRow(-1);
	r.className='extra_even_row';

	c = r.insertCell(-1);
	c.className = 'extrainfo_type';
	c.innerHTML='Upload Speed:';

	c = r.insertCell(-1);
	c.className = 'extrainfo_value';

	c = r.insertCell(-1);
	c.className = 'extrainfo_type extrainfo_divider';
	c.innerHTML='Uploaded:';

	c = r.insertCell(-1);
	c.className = 'extrainfo_value';

	c = r.insertCell(-1);
	c.className = 'extrainfo_type extrainfo_divider';
	c.innerHTML='Leechers:';

	c = r.insertCell(-1);
	c.className = 'extrainfo_value';

	/* Third row , Downloaded, download speed, total progress */
	r = tbl.insertRow(-1);
	r.className='extra_uneven_row';

	c = r.insertCell(-1);
	c.className = 'extrainfo_type';
	c.innerHTML='Download Speed:';

	c = r.insertCell(-1);
	c.className = 'extrainfo_value';

	c = r.insertCell(-1);
	c.className = 'extrainfo_type extrainfo_divider';
	c.innerHTML='Downloaded:';

	c = r.insertCell(-1);
	c.className = 'extrainfo_value';

	c = r.insertCell(-1);
	c.className = 'extrainfo_type extrainfo_divider';
	c.innerHTML='Total progress:';

	c = r.insertCell(-1);
	c.className = 'extrainfo_value';

	/* */

	r = tbl.insertRow(-1);
	r.className='extra_even_row';

	c = r.insertCell(-1);

	// Add a tracker URL.
	c.className = 'extrainfo_type';
	c.innerHTML='Tracker:';

	c = r.insertCell(-1);
	c.className = 'extrainfo_value';
	c.innerHTML='';
	c.colSpan = 3;

	return tbl;
}

/**
 * Create a table for extended controls.
 * @return A HTMLTableElement.
 */
function createTorrentExtendedControls(contextID)
{
	var ctrl = document.createElement("table");
	ctrl.className='extendedcontrol';
	var cr = ctrl.insertRow(-1);
	var cc = cr.insertCell(-1);
	cc.innerHTML = "Upload Limit (KB/s):";

	cc = cr.insertCell(-1);
	var ct = document.createElement('input');
	ct.className='textinput';
	ct.id = "txtUploadLimit_"+contextID;
	ct.type="text";
	cc.appendChild(ct);

	cc = cr.insertCell(-1);
	cc.innerHTML = "Seed Limit (%):"

	cc = cr.insertCell(-1);
	ct = document.createElement('input');
	ct.className='textinput';
	ct.id = "txtSeedLimit_"+contextID;
	ct.type="text";
	cc.appendChild(ct);

	cc = cr.insertCell(-1);

	cr = ctrl.insertRow(-1);
	cc = cr.insertCell(-1);
	cc.innerHTML = "Download Limit (KB/s):";

	cc = cr.insertCell(-1);
	ct = document.createElement('input');
	ct.className='textinput';
	ct.id = "txtDownloadLimit_"+contextID;
	ct.type="text";
	cc.appendChild(ct);

	cc = cr.insertCell(-1);
	cc.innerHTML = "Seed Timeout (minutes):";

	cc = cr.insertCell(-1);
	ct = document.createElement('input');
	ct.className='textinput';
	ct.id = "txtSeedTimeout_"+contextID;
	ct.type="text";
	cc.appendChild(ct);

	cc = cr.insertCell(-1);
	ct = createButton('Save');

	ct.contextID = contextID;
	ct.onclick = function(){saveContextLimit(this.contextID);};
	cc.appendChild(ct);

	return ctrl;
}

/**
 * Update a details table created by createTorrentDetailsTable().
 * @param t The HTMLTableElement to be updated.
 * @param s The Status object containing the information to put in the table.
 */
function updateTorrentDetails(t, s)
{
	var f = s.filename;
	if(f.length > 60)
		f = f.substr(0,60) + '...';

	t.rows[0].cells[1].innerHTML = s.statusMessage;

   /* Amount of time this torrent has been downloading or seeding. */
   /* TODO: convert to days:hours:minutes. */
   if (s.status == ts_downloading)
	{
		t.rows[1].cells[0].innerHTML = "Download time";
		t.rows[1].cells[1].innerHTML = s.activitycounter + " minutes";
	}
   else if (s.status == ts_seeding)
	{
		t.rows[1].cells[0].innerHTML = "Seed time";
		t.rows[1].cells[1].innerHTML = s.activitycounter + " minutes";
	}
   else
	{
		t.rows[1].cells[0].innerHTML = "";
		t.rows[1].cells[1].innerHTML = "";
	}

	t.rows[2].cells[1].innerHTML = f;
	t.rows[2].cells[1].title = s.filename;
	t.rows[2].cells[3].innerHTML = humanizeSize(s.filesize,2);
	t.rows[2].cells[5].innerHTML = s.seeders;

	t.rows[3].cells[1].innerHTML = humanizeSpeed(s.uploadrate,1);
	t.rows[3].cells[3].innerHTML = humanizeSize(s.uploadtotal,2);
	t.rows[3].cells[5].innerHTML = s.leechers;

	t.rows[4].cells[1].innerHTML = humanizeSpeed(s.downloadrate ,1);
	t.rows[4].cells[3].innerHTML = humanizeSize(s.downloadtotal ,2);

	t.rows[4].cells[5].innerHTML = RoundToNdp(s.done,1)+'%';
	// Tracker url.
	t.rows[5].cells[1].innerHTML = s.tracker;

}

/**************************************************
 * Status object.
 * This object represents a context (torrent) with 
 * some data about it.
 **************************************************/

ts_undefined = 0;	
ts_queued = 1;	
ts_checking	= 2;
ts_connecting = 3;
ts_downloading = 4;
ts_seeding = 5;	
ts_stopped = 6;
ts_finished = 7;

function Status(dom)
{
	this.contextID = -1;
	this.filename = "";
	this.status = 0;
	this.statustext = "";
	this.downloadTotal = 0;
	this.uploadTotal = 0;
	this.failedBytes = 0;
	this.downloadRate = 0;
	this.uploadRate = 0;
	this.done = 0;
	this.filesize = 0;
	this.leechers = 0;
	this.seeders = 0;
	this.timeleft = "";
	this.trackerstatus = "";
	this.trackerstatustext = "";
	this.trackerstatusmessage = "";
	this.activitycounter = 0;
	this.tracker = "";
	if(dom)
	{
	    <!-- Defaults set, try to parse dom -->
	    this.contextID = parseInt(getFirstChildValue(dom, 'id'));
	    this.filename = getFirstChildValue(dom, 'filename');
	    this.status = parseInt(getFirstChildValue(dom, 'status'));
	    this.statustext = parseInt(getFirstChildValue(dom, 'statustext'));
	    this.downloadtotal = parseInt(getFirstChildValue(dom, 'downloadtotal'));
	    this.uploadtotal = parseInt(getFirstChildValue(dom, 'uploadtotal'));
	    this.failedbytes = parseInt(getFirstChildValue(dom, 'failedbytes'));
	    this.uploadrate = parseInt(getFirstChildValue(dom, 'uploadrate'));
	    this.downloadrate = parseInt(getFirstChildValue(dom, 'downloadrate'));
	    this.done = parseInt(getFirstChildValue(dom, 'done'));

	    this.filesize = parseInt(getFirstChildValue(dom, 'filesize'));
	    this.leechers = parseInt(getFirstChildValue(dom, 'leechers'));
	    this.seeders = parseInt(getFirstChildValue(dom, 'seeders'));
	    this.timeleft = getFirstChildValue(dom, 'timeleft');

	    this.trackerstatus = getFirstChildValue(dom, 'trackerstatus');
	    this.trackerstatustext = getFirstChildValue(dom, 'trackerstatustext');
	    this.trackerstatusmessage = getFirstChildValue(dom, 'trackerstatusmessage');
	    this.activitycounter = parseInt(getFirstChildValue(dom, 'activitycounter'));
	    this.tracker = getFirstChildValue(dom, 'tracker');

		/* Recalculate done */
		if(this.status == ts_seeding || this.status == ts_finished)
		{
			if(this.downloadtotal < this.filesize)
				// User has probably added this torrent with data currently available (readded it), use filesize instead of downloadtotal..
				this.done = 100 * (this.uploadtotal / this.filesize)
			else
				this.done = 100 * (this.uploadtotal / this.downloadtotal)
		}
	}

	this.toString = function()
	{
		<!-- Printable representation -->
		ret = "Status object: ";
		ret+= "context id: "+ this.contextID + "\n";
		ret+= "filename: "+ this.filename + "\n";
		ret+= "status: "+ this.status + "\n";
		ret+= "dn_total: "+ this.downloadTotal + "\n";
		ret+= "ul_total: "+ this.uploadTotal + "\n";
		ret+= "failed_bytes: "+ this.failedBytes + "\n";
		ret+= "dn_rate: "+ this.downloadRate + "\n";
		ret+= "ul_rate: "+ this.uploadRate + "\n";
		ret+= "done: "+ this.done + "\n";
		ret+= "filesize: "+ this.filesize + "\n";
		ret+= "leechers: "+ this.leechers+ "\n";
		ret+= "seeders: "+ this.seeders + "\n";
		ret+= "timeleft: "+ this.timeleft+ "\n";
		ret+= "trackerstatus: "+ this.trackerstatus+ "\n";
		ret+= "trackerstatustext: "+ this.trackerstatustext+ "\n";
		ret+= "trackerstatusmessage: "+ this.trackerstatusmessage+ "\n";
		ret+= "activitycounter: "+ this.activitycounter+ "\n";
		ret+= "activitycounter: "+ this.tracker+ "\n";

		return ret;
	}
	
	<!-- Test if this status object represents the same context as another status object s -->
	this.compare = function(s)
	{
		if(!(s instanceof Status))
			return false;
		if(s.contextID != this.contextID)	return false;
		if(s.filename != this.filename)	return false;
		return true;
	}

	<!-- Test if the other Status object passed in s is newer than this (updated data) -->
	this.isOld = function(s)
	{
		if(this.status != s.status)	return true;
		if(this.dn_total != s.dn_total)	return true;
		if(this.ul_total != s.ul_total)	return true;
		if(this.failed_bytes != s.failed_bytes)	return true;
		if(this.dn_rate != s.dn_rate)	return true;
		if(this.ul_rate != s.ul_rate)	return true;
		if(this.done != s.done)	return true;
		if(this.filesize != s.filesize)	return true;
		if(this.leechers != s.leechers)	return true;
		if(this.seeders != s.seeders)	return true;
		if(this.timeleft != s.timeleft)	return true;
		if(this.trackerstatus != s.trackerstatus)	return true;
		if(this.trackerstatustext != s.trackerstatustext)	return true;
		if(this.trackerstatusmessage != s.trackerstatusmessage)	return true;
		if(this.activitycounter != s.activitycounter) return true;
		if(this.tracker != s.tracker) return true;
		return false;
	}
}

function Peer(dom)
{
	this.ip = "0.0.0.0";
	this.seeder = false;

	if(dom)
	{
		<!-- Defaults set, try to parse dom -->
		this.ip = getFirstChildValue(dom, 'ip')
		this.seeder = parseBool(getFirstChildValue(dom, 'seeder'))
	}

	this.toString = function()
	{
		<!-- Printable representation -->
		ret = "Peer object: ";
		ret+= "IP: "+ this.ip+ "\n";
		ret+= "seeder: "+ this.seeder + "\n";

		return ret;
	}
	
	<!-- Test if this status object represents the same context as another status object s -->
	this.compare = function(s)
	{
		if(s.ip != this.ip)	return false;
		return true;
	}
}

function Limit(dom)
{
	this.contextID = -1;
	this.uploadLimit = 0;
	this.downloadLimit = 0;
	this.seedLimit = 0;
	this.seedTimeout = 0;

	if(dom)
	{
		<!-- Defaults set, try to parse dom -->
		this.contextID = parseInt(getFirstChildValue(dom, 'id'))
		this.uploadLimit = parseInt(getFirstChildValue(dom, 'uploadLimit'))
		this.downloadLimit = parseInt(getFirstChildValue(dom, 'downloadLimit'))
		this.seedLimit = parseInt(getFirstChildValue(dom, 'seedLimit'))
		this.seedTimeout = parseInt(getFirstChildValue(dom, 'seedTimeout'))
	}

	this.toString = function()
	{
		<!-- Printable representation -->
		ret = "Limit object: ";
		ret+= "context id: "+ this.contextID + "\n";
		ret+= "uploadLimit: "+ this.uploadLimit+ "\n";
		ret+= "downloadLimit: "+ this.downloadLimit+ "\n";
		ret+= "seedLimit "+ this.seedLimit+ "\n";
		ret+= "seedTimeout: "+ this.seedTimeout;

		return ret;
	}
	
	<!-- Test if this limit object represents the same context as another liimt object l -->
	this.compare = function(l)
	{
		if(!(l instanceof Limit))
			return false;
		if(l.contextID != this.contextID)	return false;
		return true;
	}

	<!-- Test if the other Limit object passed in l is newer than this (updated data) -->
	this.isOld = function(l)
	{
		if(this.uploadLimit != l.uploadLimit)	return true;
		if(this.downloadLimit != l.downloadLimit)	return true;
		if(this.seedLimit != l.seedLimit)	return true;
		if(this.seedTimeout != l.seedTimeout)	return true;
		return false;
	}
}
-->

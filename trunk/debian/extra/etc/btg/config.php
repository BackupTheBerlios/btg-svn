<?php

/*
 * This file is included from /usr/share/wwwbtg/htdocs/config.php
 */

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
 * $Id$
 */

/* This file contains various configuration variables 
 * for the BTG web interface.
 */

/* URL to the btgdaemon to connect to.
 * Should have protocol, host and port number. Example:
 * http://127.0.0.1:16000
 * https://127.0.0.1:16000
 */
#$btg_config_url = "http://127.0.0.1:16000/";

/**
 * This is the CA certificate.
 */
#$btg_ca_cert = "stromnetCA.crt";

/**
 * This is the clients certificate in combined PEM format.
 * This contains BOTH certificate and private key.
 * Can be created from btgclient.crt and btgclient.key with these commands:
 * openssl x509 -out btgclient.crt.pem -in btgclient.crt -outform pem
 * 
 * cat btgclient.key > btgclient.pem
 * cat btgclient.crt.pem >> btgclient.pem
 * rm btgclient.crt.pem
 * 
 * This btgclient.pem can now be used below.
 */
#$btg_client_cert = "btgclient.pem";

/* Autostart
 * Default for a new torrent is to start automatically. 
 * Setting autostart to false will change this so 
 * the user has to start the torrent manually.
 */
#$btg_config_autostart = true;

/* Debug Mode.
 * Valid values:
 * 
 * 0:
 * No debugging.
 * 
 * 1:
 * Debugging output is generated on server, 
 * but not shown on client side unless activated in UI.
 * The syslog messages produced are also written to syslog.
 * 
 * 2:
 * Debugging output is generated on server, 
 * and is shown to the client by default.
 * The syslog messages produced are also written to syslog.
 */
#$btg_config_debug = 0;
?>
